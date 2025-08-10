#include "LevelBlock.h"
#include "Asset.h"
#include "B2Filters.h"
#include "ContactListener.h"

#include <box2d/box2d.h>

using namespace Gameplay;

LevelBlock::LevelBlock(std::weak_ptr<b2World> world, int x, int y, const BlockConfiguration &configuration)
    : m_configuration(configuration)
{
    m_health = std::max(0.000001f, m_configuration.toughness);

    if (auto w = world.lock())
    {
        b2BodyDef blockDef;
        blockDef.position.Set(2.0f * static_cast<float>(x), 2.0f * static_cast<float>(y));
        blockDef.enabled = m_configuration.collision;
        m_asset = std::make_shared<Asset>(w->CreateBody(&blockDef), m_configuration.assetName);
        Physics::PhysicsObjectUserData *data = new Physics::PhysicsObjectUserData{Physics::BodyType::LevelBlock, this};
        m_asset->GetBody()->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);

        b2PolygonShape block;
        block.SetAsBox(1.0f, 1.0f);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &block;
        fixtureDef.friction = configuration.friction;
        fixtureDef.density = configuration.density;

        m_asset->GetBody()->CreateFixture(&fixtureDef);
        m_asset->UpdateSize();
    }
}

bool LevelBlock::Update(float deltaTime)
{
    if (m_configuration.type == BlockType::Static)
    {
        return true;
    }

    if (m_contacts.empty() == false)
    {
        for (auto body : m_contacts)
        {
            if (body == false)
                continue;

            auto velocity = body->GetLinearVelocity();
            float impact = velocity.Length() * body->GetMass() * deltaTime;

            auto fixture = body->GetFixtureList();
            if (fixture == false)
                continue;

            auto category = fixture->GetFilterData().categoryBits;

            if (category != static_cast<uint16_t>(CollisionFilter::Avatar_Body) &&
                category != static_cast<uint16_t>(CollisionFilter::Avatar_Legs))
            {
                m_health -= impact;
            }
            else if (m_configuration.destructable == false)
            {
                m_health -= 0.1f * deltaTime;
            }

            if (m_health <= 0.0f)
            {
                if (m_configuration.destructable == false)
                {
                    ConvertToDynamic();
                    break;
                }
                else
                {
                    return false;
                }
            }
        }
    }

    if (m_asset->GetY() < -50.0f)
    {
        return false;
    }

    return true;
}

std::weak_ptr<Asset> LevelBlock::GetAsset() const
{
    return m_asset;
}

bool LevelBlock::InMotion() const
{
    return m_asset->GetBody()->GetLinearVelocity().Length() > 0.0f;
}

void LevelBlock::ConvertToDynamic()
{
    auto alias = m_asset->GetAlias();
    auto x = m_asset->GetX();
    auto y = m_asset->GetY();
    auto world = m_asset->GetBody()->GetWorld();

    b2BodyDef blockDef;
    blockDef.type = b2_dynamicBody;
    blockDef.gravityScale = 1.0f;
    blockDef.position.Set(x, y);
    m_asset = std::make_shared<Asset>(world->CreateBody(&blockDef), alias);
    b2PolygonShape block;
    block.SetAsBox(1.0f, 1.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &block;
    fixtureDef.density = m_configuration.density;
    fixtureDef.friction = m_configuration.friction;

    m_asset->GetBody()->CreateFixture(&fixtureDef);
    m_asset->UpdateSize();
}

void LevelBlock::OnContact(b2Body *otherBody, b2Fixture *otherFixture, bool contact)
{
    if (contact)
    {
        m_contacts.insert(otherBody);
    }
    else
    {
        m_contacts.erase(otherBody);
    }
}
