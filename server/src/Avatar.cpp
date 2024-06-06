#include "Avatar.h"
#include "Asset.h"
#include "B2Filters.h"
#include "Sledge.h"
#include "Sword.h"

using namespace Gameplay;

Avatar::Avatar(std::weak_ptr<b2World> world, const b2Vec2 &spawnPos, unsigned int tint, bool winner)
    : m_world(world), m_invincibilityTimer(2.0f), m_dead(false), m_weaponJoint(nullptr), m_health(3),
      m_crownJoint(nullptr)
{
    static const b2Vec2 bodySize(2.0f, 2.0f);
    static const b2Vec2 legsSize(1.5f, 0.5f);
    static const b2Vec2 headSize(1.0f, 1.0f);
    constexpr float AvatarDensity = 0.8f;
    constexpr float AvatarFriction = 0.01f;
    constexpr float LegsFriction = 0.3f;
    constexpr float gravityScale = 2.0f;

    if (auto w = m_world.lock())
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = spawnPos;
        bodyDef.fixedRotation = true;
        bodyDef.gravityScale = gravityScale;
        m_bodyAsset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "avatar_body");
        m_bodyAsset->SetTint(tint);

        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(bodySize.x, bodySize.y);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = AvatarDensity;
        fixtureDef.friction = AvatarFriction;

        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Avatar_Body);
        fixtureDef.filter.maskBits = 0xFFFF;
        fixtureDef.filter.maskBits &= ~static_cast<unsigned int>(CollisionFilter::Block_Decor);
        GetBody()->CreateFixture(&fixtureDef);
        m_bodyAsset->UpdateSize();

        bodyDef.position = spawnPos;
        bodyDef.position.y -= (bodySize.y + legsSize.y);
        m_legsAsset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "avatar_legs");

        dynamicBox.SetAsBox(legsSize.x, legsSize.y);
        fixtureDef.friction = LegsFriction;
        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Avatar_Legs);
        fixtureDef.filter.maskBits = 0xFFFF;
        fixtureDef.filter.maskBits &= ~(static_cast<unsigned int>(CollisionFilter::Block_Decor) |
                                        static_cast<unsigned int>(CollisionFilter::Weapon_Shaft));
        GetLegs()->CreateFixture(&fixtureDef);
        m_legsAsset->UpdateSize();

        b2WeldJointDef joint;
        joint.Initialize(GetBody(), GetLegs(), bodyDef.position);
        m_legsJoint = w->CreateJoint(&joint);

        bodyDef.type = b2_dynamicBody;
        bodyDef.position = spawnPos;
        bodyDef.position.y += bodySize.y + headSize.y;
        bodyDef.fixedRotation = false;
        bodyDef.gravityScale = 1.0f;
        m_headAsset[m_health - 1] =
            std::make_shared<Asset>(w->CreateBody(&bodyDef), "avatar_head_" + std::to_string(m_health));
        m_headAsset[m_health - 1]->SetTint(0x7F7F7F);

        dynamicBox.SetAsBox(headSize.x, headSize.y);
        fixtureDef.friction = AvatarFriction;

        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Avatar_Head);
        fixtureDef.filter.maskBits = 0xFFFF;
        fixtureDef.filter.maskBits &= ~static_cast<unsigned int>(CollisionFilter::Weapon_Shaft);

        GetHead()->CreateFixture(&fixtureDef);
        m_headAsset[m_health - 1]->UpdateSize();

        joint.Initialize(GetBody(), GetHead(), bodyDef.position);
        m_headJoint = w->CreateJoint(&joint);

        if (winner == true)
        {
            bodyDef.type = b2_dynamicBody;
            bodyDef.position.y += headSize.y + headSize.y;
            bodyDef.fixedRotation = false;
            bodyDef.gravityScale = 1.0f;
            m_crownAsset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "avatar_crown");

            dynamicBox.SetAsBox(headSize.x, headSize.y);
            fixtureDef.filter.maskBits = 0;

            m_crownAsset->GetBody()->CreateFixture(&fixtureDef);
            m_crownAsset->UpdateSize();

            joint.Initialize(GetHead(), m_crownAsset->GetBody(), bodyDef.position);
            m_crownJoint = w->CreateJoint(&joint);
        }

        int random = rand();
        if (random % 4 == 0)
        {
            AssignWeapon(WeaponType::Sword);
        }
        else
        {
            AssignWeapon(WeaponType::Sledge);
        }
    }
}

Avatar::~Avatar()
{
    BreakJoints();
}

void Avatar::AssignWeapon(WeaponType type)
{
    switch (type)
    {
    case WeaponType::Sledge:
        m_weapon = std::make_unique<Sledge>(m_world, *this);
        break;
    case WeaponType::Sword:
        m_weapon = std::make_unique<Sword>(m_world, *this);
        break;
    default:
        m_weapon = std::make_unique<Sledge>(m_world, *this);
        break;
    }

    b2RevoluteJointDef jd;
    jd.Initialize(GetBody(), m_weapon->GetShaft(), GetPosition());
    jd.motorSpeed = m_weapon->GetSpeed();
    jd.maxMotorTorque = m_weapon->GetTorque();
    jd.enableMotor = true;
    jd.enableLimit = false;

    m_weaponJoint = (b2RevoluteJoint *)GetBody()->GetWorld()->CreateJoint(&jd);
}

void Avatar::Update(const float &deltaTime, const float &sledgeInput, const float &jumpInput, const float &moveInput)
{
    m_invincibilityTimer = std::max(0.0f, m_invincibilityTimer - deltaTime);
    if (m_dead == false && m_invincibilityTimer == 0.0f && GetHead()->GetContactList() != nullptr &&
        GetHead()->GetContactList()->contact->IsTouching())
    {
        auto contact = GetHead()->GetContactList()->contact;
        auto vel = 0.0f;
        if (contact->GetFixtureA()->GetBody() == GetHead())
        {
            vel = contact->GetFixtureB()->GetBody()->GetLinearVelocity().Length();
        }
        else
        {
            vel = contact->GetFixtureA()->GetBody()->GetLinearVelocity().Length();
        }

        constexpr float killThreshold = 1.0f;
        if (vel > killThreshold)
        {
            if (m_crownAsset != nullptr && m_crownJoint != nullptr)
            {
                BreakCrown();
            }
            else
            {
                --m_health;
                if (m_health == 0)
                {
                    Kill();
                }
                else
                {
                    BreakHelm();
                }
            }
        }
    }

    if (m_dead == false)
    {
        if (m_weaponJoint != nullptr)
        {
            m_weaponJoint->SetMotorSpeed(m_weapon->GetSpeed() * -sledgeInput);
        }

        bool hasGroundContact = false;
        for (auto c = GetLegs()->GetContactList(); c; c = c->next)
        {
            if (c->contact->IsTouching())
            {
                hasGroundContact = true;
                break;
            }
        }

        // Air control movement
        auto movement = 30000.0f * std::max(0.0f, (1.0f - abs(GetBody()->GetLinearVelocity().x * 0.05f)));
        if (abs(GetBody()->GetLinearVelocity().y) < 10.0f && hasGroundContact == true)
        {
            if (jumpInput > 0.5f)
            {
                GetBody()->ApplyLinearImpulse(b2Vec2(0.0f, 300.0f), GetBody()->GetTransform().p, true);
            }

            // Ground control movement
            movement = 180000.0f * std::max(0.0f, (1.0f - abs(GetBody()->GetLinearVelocity().x * 0.05f)));
        }

        GetBody()->ApplyForce(b2Vec2(movement * moveInput * deltaTime, 0.0f), GetBody()->GetTransform().p, true);
    }
}

unsigned int Avatar::GetBodyId() const
{
    return m_bodyAsset->GetId();
}

b2Body *Avatar::GetBody() const
{
    return m_bodyAsset->GetBody();
}

b2Body *Avatar::GetHead() const
{
    return m_headAsset[m_health - 1]->GetBody();
}

b2Body *Gameplay::Avatar::GetLegs() const
{
    return m_legsAsset->GetBody();
}

const b2Vec2 &Avatar::GetPosition() const
{
    return GetBody()->GetTransform().p;
}

const float Avatar::GetX() const
{
    return GetPosition().x;
}

const float Avatar::GetY() const
{
    return GetPosition().y;
}

const float Avatar::GetWeaponRot() const
{
    return m_weapon != nullptr ? m_weapon->GetShaft()->GetTransform().q.GetAngle() : 0.0f;
}

std::vector<std::weak_ptr<Asset>> Avatar::GetAssets() const
{
    std::vector<std::weak_ptr<Asset>> assets;
    assets.emplace_back(m_bodyAsset);
    assets.emplace_back(m_legsAsset);
    if (m_headAsset[0] != nullptr)
    {
        assets.emplace_back(m_headAsset[0]);
    }
    if (m_headAsset[1] != nullptr)
    {
        assets.emplace_back(m_headAsset[1]);
    }
    if (m_headAsset[2] != nullptr)
    {
        assets.emplace_back(m_headAsset[2]);
    }
    if (m_crownAsset != nullptr)
    {
        assets.emplace_back(m_crownAsset);
    }

    auto weaponAssets = m_weapon->GetAssets();
    assets.insert(assets.end(), weaponAssets.begin(), weaponAssets.end());

    return assets;
}

void Avatar::Kill()
{
    BreakJoints();
    m_weapon->BreakJoints();

    m_dead = true;
}

bool Avatar::IsDead() const
{
    return m_dead;
}

void Avatar::BreakJoints()
{
    if (m_weaponJoint != nullptr)
    {
        GetBody()->GetWorld()->DestroyJoint(m_weaponJoint);
        m_weaponJoint = nullptr;
    }

    if (m_legsJoint != nullptr)
    {
        GetBody()->GetWorld()->DestroyJoint(m_legsJoint);
        m_legsJoint = nullptr;
    }

    if (m_headJoint != nullptr)
    {
        GetBody()->GetWorld()->DestroyJoint(m_headJoint);
        m_headJoint = nullptr;
    }

    BreakCrown();
}

void Avatar::BreakCrown()
{
    if (m_crownAsset != nullptr)
    {
        if (m_crownJoint != nullptr)
        {
            GetBody()->GetWorld()->DestroyJoint(m_crownJoint);
            m_crownJoint = nullptr;
        }

        m_crownAsset->GetBody()->ApplyLinearImpulse(b2Vec2((rand() % 2 - 1) * 50.0f, 50.0f),
                                                    m_crownAsset->GetBody()->GetTransform().p, true);

        m_crownAsset->GetBody()->GetFixtureList()->SetFilterData(b2Filter());

        m_invincibilityTimer = 1.0f;
    }
}

void Avatar::BreakHelm()
{
    if (m_headJoint != nullptr)
    {
        GetBody()->GetWorld()->DestroyJoint(m_headJoint);
        m_headJoint = nullptr;
    }
    m_headAsset[m_health]->GetBody()->ApplyLinearImpulse(b2Vec2((rand() % 2 - 1) * 50.0f, 50.0f),
                                                         m_headAsset[m_health]->GetBody()->GetTransform().p, true);

    m_headAsset[m_health]->GetBody()->GetFixtureList()->SetFilterData(b2Filter());

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = m_headAsset[m_health]->GetBody()->GetTransform().p;
    bodyDef.position.y -= 0.05f;
    bodyDef.fixedRotation = false;
    bodyDef.gravityScale = 1.0f;
    m_headAsset[m_health - 1] =
        std::make_shared<Asset>(GetBody()->GetWorld()->CreateBody(&bodyDef), "avatar_head_" + std::to_string(m_health));
    m_headAsset[m_health - 1]->SetTint(0x7F7F7F);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(m_headAsset[m_health]->GetSizeX() * 0.45f, m_headAsset[m_health]->GetSizeY() * 0.45f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.8f;
    fixtureDef.friction = 0.01f;

    fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Avatar_Head);
    fixtureDef.filter.maskBits = 0xFFFF;
    fixtureDef.filter.maskBits &= ~static_cast<unsigned int>(CollisionFilter::Weapon_Shaft);

    GetHead()->CreateFixture(&fixtureDef);
    m_headAsset[m_health - 1]->UpdateSize();

    b2WeldJointDef joint;
    joint.Initialize(GetBody(), GetHead(), bodyDef.position);
    m_headJoint = GetBody()->GetWorld()->CreateJoint(&joint);

    m_invincibilityTimer = 1.0f;
}
