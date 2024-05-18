#include "Sledge.h"
#include "Asset.h"
#include "Avatar.h"

using namespace Gameplay;

Sledge::Sledge(const Avatar *avatar) : Weapon(avatar)
{
    m_speed = 5.0f;
    m_torque = 10000.0f;

    static const b2Vec2 shaftSize(5.0f, 0.25f);
    static const b2Vec2 headSize(0.75f, 2.0f);
    constexpr float shaftDensity = 0.1f;
    constexpr float shaftFriction = 0.3f;
    constexpr float headDensity = 0.1f;
    constexpr float headFriction = 0.5f;

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
    bodyDef.position.x += shaftSize.x;
    m_sledgeHeadAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef));

    dynamicBox.SetAsBox(headSize.x, headSize.y);

    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = headDensity;
    fixtureDef.friction = headFriction;

    fixtureDef.filter.categoryBits = 0x0001;
    fixtureDef.filter.maskBits = 0xFFFF;

    m_sledgeHeadAsset->GetBody()->CreateFixture(&fixtureDef);
    m_sledgeHeadAsset->UpdateSize();

    b2WeldJointDef joint;
    joint.Initialize(GetShaft(), m_sledgeHeadAsset->GetBody(), bodyDef.position);
    m_joints.emplace_back(world->CreateJoint(&joint));

    m_assets.emplace_back(m_shaftAsset);
    m_assets.emplace_back(m_sledgeHeadAsset);
}

Sledge::~Sledge()
{
    BreakJoints();

    if (m_sledgeHeadAsset->GetBody() != nullptr)
    {
        m_sledgeHeadAsset->GetBody()->GetWorld()->DestroyBody(m_sledgeHeadAsset->GetBody());
    }
}
