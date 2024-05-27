#include "Sword.h"
#include "Asset.h"
#include "Avatar.h"
#include "B2Filters.h"

using namespace Gameplay;

Sword::Sword(const Avatar *avatar) : Weapon(avatar)
{
    m_speed = 10.0f;
    m_torque = 20000.0f;

    static const b2Vec2 shaftSize(4.9f, 0.25f);
    static const b2Vec2 hiltSize(0.25f, 1.0f);
    static const b2Vec2 edgeSize(0.1f, 0.25f);
    constexpr float shaftDensity = 0.025f;
    constexpr float shaftFriction = 0.3f;
    constexpr float hiltDensity = 0.025f;
    constexpr float hiltFriction = 0.5f;

    auto world = avatar->GetBody()->GetWorld();

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = avatar->GetPosition();
    bodyDef.position.x += shaftSize.x;
    bodyDef.fixedRotation = false;
    m_shaftAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef), "weapon_head");

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(shaftSize.x, shaftSize.y);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = shaftDensity;
    fixtureDef.friction = shaftFriction;

    fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Weapon_Shaft);
    fixtureDef.filter.maskBits = 0xFFFF;
    fixtureDef.filter.maskBits &= ~(static_cast<unsigned int>(CollisionFilter::Avatar_Head) |
                                    static_cast<unsigned int>(CollisionFilter::Avatar_Legs));

    GetShaft()->CreateFixture(&fixtureDef);
    m_shaftAsset->UpdateSize();

    m_hiltAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef), "weapon_head");

    dynamicBox.SetAsBox(hiltSize.x, hiltSize.y);

    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = hiltDensity;
    fixtureDef.friction = hiltFriction;

    fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Weapon_Head);
    fixtureDef.filter.maskBits = 0xFFFF;

    m_hiltAsset->GetBody()->CreateFixture(&fixtureDef);
    m_hiltAsset->UpdateSize();

    b2WeldJointDef joint;
    joint.Initialize(GetShaft(), m_hiltAsset->GetBody(), bodyDef.position);
    m_joints.emplace_back(world->CreateJoint(&joint));

    bodyDef.position.x += shaftSize.x;
    m_edgeAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef), "weapon_head");

    dynamicBox.SetAsBox(edgeSize.x, edgeSize.y);

    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = shaftDensity;
    fixtureDef.friction = shaftFriction;

    m_edgeAsset->GetBody()->CreateFixture(&fixtureDef);
    m_edgeAsset->UpdateSize();

    joint.Initialize(GetShaft(), m_edgeAsset->GetBody(), bodyDef.position);
    m_joints.emplace_back(world->CreateJoint(&joint));

    m_assets.emplace_back(m_shaftAsset);
    m_assets.emplace_back(m_hiltAsset);
    m_assets.emplace_back(m_edgeAsset);
}

Sword::~Sword()
{
    BreakJoints();
}
