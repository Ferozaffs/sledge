#include "Sledge.h"
#include "Asset.h"
#include "Avatar.h"
#include "B2Filters.h"

using namespace Gameplay;

Sledge::Sledge(std::weak_ptr<b2World> world, const Avatar &avatar) : Weapon()
{
    static const b2Vec2 shaftSize(5.0f, 0.25f);
    static const b2Vec2 headSize(0.75f, 2.0f);
    constexpr float shaftDensity = 0.1f;
    constexpr float shaftFriction = 0.3f;
    constexpr float headDensity = 0.1f;
    constexpr float headFriction = 0.5f;

    if (auto w = world.lock())
    {
        m_speed = 5.0f;
        m_torque = 10000.0f;

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = avatar.GetPosition();
        bodyDef.position.x += shaftSize.x;
        bodyDef.fixedRotation = false;
        m_shaftAsset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "weapon_shaft");

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

        bodyDef.type = b2_dynamicBody;
        bodyDef.position.x += shaftSize.x;
        m_sledgeHeadAsset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "weapon_head");

        dynamicBox.SetAsBox(headSize.x, headSize.y);

        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = headDensity;
        fixtureDef.friction = headFriction;

        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Weapon_Head);
        fixtureDef.filter.maskBits = 0xFFFF;

        m_sledgeHeadAsset->GetBody()->CreateFixture(&fixtureDef);
        m_sledgeHeadAsset->UpdateSize();

        b2WeldJointDef joint;
        joint.Initialize(GetShaft(), m_sledgeHeadAsset->GetBody(), bodyDef.position);
        m_joints.emplace_back(w->CreateJoint(&joint));

        m_assets.emplace_back(m_shaftAsset);
        m_assets.emplace_back(m_sledgeHeadAsset);
    }
}
