#include "Sword.h"
#include "Asset.h"
#include "Avatar.h"

using namespace Gameplay;

Sword::Sword(const Avatar *avatar) : Weapon(avatar)
{
    m_speed = 10.0f;
    m_torque = 20000.0f;

    static const b2Vec2 shaftSize(5.0f, 0.25f);
    static const b2Vec2 hiltSize(0.25f, 1.0f);
    constexpr float shaftDensity = 0.1f;
    constexpr float shaftFriction = 0.3f;
    constexpr float hiltDensity = 0.1f;
    constexpr float hiltFriction = 0.5f;

    auto world = avatar->GetBody()->GetWorld();

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = avatar->GetPosition();
    bodyDef.position.x += shaftSize.x;
    bodyDef.fixedRotation = false;
    m_shaftAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef));

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(shaftSize.x, shaftSize.y);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = shaftDensity;
    fixtureDef.friction = shaftFriction;

    fixtureDef.filter.categoryBits = 0x4000;
    fixtureDef.filter.maskBits = 0xFFFF;
    fixtureDef.filter.maskBits &= ~0x8000;

    GetShaft()->CreateFixture(&fixtureDef);
    m_shaftAsset->UpdateSize();

    bodyDef.type = b2_dynamicBody;
    bodyDef.position.x += shaftSize.x * 0.1f;
    m_hiltAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef));

    dynamicBox.SetAsBox(hiltSize.x, hiltSize.y);

    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = hiltDensity;
    fixtureDef.friction = hiltFriction;
    m_hiltAsset->GetBody()->CreateFixture(&fixtureDef);
    m_hiltAsset->UpdateSize();

    b2WeldJointDef joint;
    joint.Initialize(GetShaft(), m_hiltAsset->GetBody(), bodyDef.position);
    m_joints.emplace_back(world->CreateJoint(&joint));

    m_assets.emplace_back(m_shaftAsset);
    m_assets.emplace_back(m_hiltAsset);
}

Sword::~Sword()
{
    BreakJoints();

    if (m_hiltAsset->GetBody() != nullptr)
    {
        m_hiltAsset->GetBody()->GetWorld()->DestroyBody(m_hiltAsset->GetBody());
    }
}
