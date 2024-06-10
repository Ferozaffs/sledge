#include "Sword.h"
#include "Asset.h"
#include "Avatar.h"
#include "B2Filters.h"

using namespace Gameplay;

Sword::Sword(std::weak_ptr<b2World> world, const Avatar &avatar) : Weapon()
{
    static const b2Vec2 hiltSize(0.25f, 1.0f);
    static const b2Vec2 edgeSize(2.5f, 0.25f);
    constexpr float shaftDensity = 0.05f;
    constexpr float shaftFriction = 0.3f;
    constexpr float hiltDensity = 0.05f;
    constexpr float hiltFriction = 0.5f;

    if (auto w = world.lock())
    {
        m_speed = 7.5f;
        m_torque = 15000.0f;

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = avatar.GetPosition();
        bodyDef.position.x += avatar.GetShaftLength() * 2.0f + hiltSize.x * 0.9f;
        bodyDef.fixedRotation = false;
        m_hiltAsset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "weapon_head");

        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(hiltSize.x, hiltSize.y);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = hiltDensity;
        fixtureDef.friction = hiltFriction;

        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Weapon_Head);
        fixtureDef.filter.maskBits = 0xFFFF;

        m_hiltAsset->GetBody()->CreateFixture(&fixtureDef);
        m_hiltAsset->UpdateSize();

        b2WeldJointDef joint;
        joint.Initialize(avatar.GetShaft(), m_hiltAsset->GetBody(), bodyDef.position);
        m_joints.emplace_back(w->CreateJoint(&joint));

        bodyDef.position.x += edgeSize.x * 0.9f;
        m_edgeAsset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "weapon_head");

        dynamicBox.SetAsBox(edgeSize.x, edgeSize.y);

        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = shaftDensity;
        fixtureDef.friction = shaftFriction;

        m_edgeAsset->GetBody()->CreateFixture(&fixtureDef);
        m_edgeAsset->UpdateSize();

        joint.Initialize(avatar.GetShaft(), m_edgeAsset->GetBody(), bodyDef.position);
        m_joints.emplace_back(w->CreateJoint(&joint));

        m_assets.emplace_back(m_hiltAsset);
        m_assets.emplace_back(m_edgeAsset);
    }
}
