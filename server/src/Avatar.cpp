#include "Avatar.h"
#include "Asset.h"
#include "Sledge.h"
#include "Sword.h"

using namespace Gameplay;

Avatar::Avatar(b2World *world, const b2Vec2 &spawnPos)
    : m_spawnInvincibility(2.0f), m_dead(false), m_weaponJoint(nullptr)
{
    static const b2Vec2 bodySize(2.0f, 2.0f);
    static const b2Vec2 headSize(0.5f, 0.5f);
    constexpr float AvatarDensity = 1.0f;
    constexpr float AvatarFriction = 0.3f;
    constexpr float gravityScale = 2.0f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = spawnPos;
    bodyDef.fixedRotation = true;
    bodyDef.gravityScale = gravityScale;
    m_bodyAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef));

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(bodySize.x, bodySize.y);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = AvatarDensity;
    fixtureDef.friction = AvatarFriction;
    GetBody()->CreateFixture(&fixtureDef);
    m_bodyAsset->UpdateSize();

    bodyDef.type = b2_dynamicBody;
    bodyDef.position.y += bodySize.y + headSize.y;
    bodyDef.fixedRotation = false;
    bodyDef.gravityScale = 1.0f;
    m_headAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef));

    dynamicBox.SetAsBox(headSize.x, headSize.y);

    fixtureDef.filter.categoryBits = 0x8000;
    fixtureDef.filter.maskBits = 0xFFFF;
    fixtureDef.filter.maskBits &= ~0x4000;

    GetHead()->CreateFixture(&fixtureDef);
    m_headAsset->UpdateSize();

    b2WeldJointDef joint;
    joint.Initialize(GetBody(), GetHead(), bodyDef.position);
    m_headJoint = world->CreateJoint(&joint);

    AssignWeapon(WeaponType::Sledge);
}

Avatar::~Avatar()
{
    BreakJoints();

    if (GetBody() != nullptr)
    {
        GetBody()->GetWorld()->DestroyBody(GetBody());
    }
}

void Avatar::AssignWeapon(WeaponType type)
{
    switch (type)
    {
    case WeaponType::Sledge:
        m_weapon = std::make_shared<Sledge>(this);
        break;
    case WeaponType::Sword:
        m_weapon = std::make_shared<Sword>(this);
        break;
    default:
        m_weapon = std::make_shared<Sledge>(this);
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
    m_spawnInvincibility = std::max(0.0f, m_spawnInvincibility - deltaTime);
    if (m_dead == false && m_spawnInvincibility == 0.0f && GetHead()->GetContactList() != nullptr &&
        GetHead()->GetContactList()->contact->IsTouching())
    {
        auto contact = GetHead()->GetContactList()->contact;
        auto velA = contact->GetFixtureA()->GetBody()->GetLinearVelocity().Length();
        auto velB = contact->GetFixtureB()->GetBody()->GetLinearVelocity().Length();

        constexpr float killThreshold = 1.0f;
        if (velA + velB > killThreshold)
        {
            Kill();
        }
    }

    if (m_dead == false)
    {
        if (m_weaponJoint != nullptr)
        {
            m_weaponJoint->SetMotorSpeed(m_weapon->GetSpeed() * -sledgeInput);
        }

        bool hasContact = false;
        for (auto c = GetBody()->GetContactList(); c; c = c->next)
        {
            if (c->contact->IsTouching())
            {
                hasContact = true;
                break;
            }
        }

        // Air control movement
        auto movement = 30000.0f * std::max(0.0f, (1.0f - abs(GetBody()->GetLinearVelocity().x * 0.05f)));
        if (abs(GetBody()->GetLinearVelocity().y) < 10.0f && hasContact == true)
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

b2Body *Avatar::GetBody() const
{
    return m_bodyAsset->GetBody();
}

b2Body *Avatar::GetHead() const
{
    return m_headAsset->GetBody();
}

const b2Vec2 &Avatar::GetPosition() const
{
    return GetBody()->GetTransform().p;
}

std::vector<std::shared_ptr<Asset>> Avatar::GetAssets() const
{
    std::vector<std::shared_ptr<Asset>> assets;
    assets.emplace_back(m_bodyAsset);
    assets.emplace_back(m_headAsset);

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

    if (m_headJoint != nullptr)
    {
        GetBody()->GetWorld()->DestroyJoint(m_headJoint);
        m_headJoint = nullptr;
    }
}
