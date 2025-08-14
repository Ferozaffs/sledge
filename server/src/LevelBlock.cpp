#include "LevelBlock.h"
#include "Asset.h"
#include "B2Filters.h"
#include "ContactListener.h"

#include <box2d/box2d.h>

using namespace Gameplay;

LevelBlock::LevelBlock(std::weak_ptr<b2World> world, int x, int y, const BlockConfiguration &configuration)
    : m_configuration(configuration), m_visbility(true)
{
    m_health = std::max(0.000001f, m_configuration.toughness);

    if (auto w = world.lock())
    {
        b2BodyDef blockDef;
        blockDef.position.Set(2.0f * static_cast<float>(x), 2.0f * static_cast<float>(y));
        m_originalX = blockDef.position.x;
        m_originalY = blockDef.position.y;
        blockDef.enabled = m_configuration.collision;
        blockDef.gravityScale = 1.0f;
        m_asset = std::make_shared<Asset>(w->CreateBody(&blockDef), m_configuration.assetName);
        PhysicsObjectUserData *data = new PhysicsObjectUserData{BodyType::LevelBlock, this};
        m_asset->GetBody()->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);

        b2PolygonShape block;
        block.SetAsBox(1.0f, 1.0f);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &block;
        fixtureDef.friction = configuration.friction;
        fixtureDef.density = configuration.density;
        fixtureDef.restitution = configuration.restitution;
        if (m_configuration.type == BlockType::Dynamic)
        {
            fixtureDef.restitutionThreshold = 0.0001f;
        }

        if (m_configuration.type == BlockType::Decor)
        {
            fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Block_Decor);
            fixtureDef.filter.maskBits = 0xFFFF;
            fixtureDef.filter.maskBits &= ~(static_cast<unsigned int>(CollisionFilter::Avatar_Body) |
                                            static_cast<unsigned int>(CollisionFilter::Avatar_Legs));
        }
        else if (m_configuration.type == BlockType::Zone)
        {
            m_visbility = false;
        }

        m_asset->GetBody()->CreateFixture(&fixtureDef);
        m_asset->UpdateSize();
    }
}

LevelBlock::~LevelBlock()
{
    if (m_destroyCallback)
    {
        m_destroyCallback(this);
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
            if (body == nullptr)
                continue;

            auto velocity = body->GetLinearVelocity();
            float impact = velocity.Length() * body->GetMass() * deltaTime;

            auto fixture = body->GetFixtureList();
            if (fixture == nullptr)
                continue;

            auto category = fixture->GetFilterData().categoryBits;

            if (impact > 0.0f && category != static_cast<uint16_t>(CollisionFilter::Avatar_Body) &&
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

void LevelBlock::Reset()
{
    m_asset->GetBody()->SetTransform(b2Vec2(m_originalX, m_originalY), 0.0f);
    auto vel = m_asset->GetBody()->GetLinearVelocity();
    vel *= 0.001f;
    m_asset->GetBody()->SetLinearVelocity(vel);
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
    blockDef.gravityScale = m_gameModeConfiguration.gravityModifier;
    blockDef.linearDamping = m_gameModeConfiguration.dampingModifier;
    blockDef.position.Set(x, y);
    m_asset = std::make_shared<Asset>(world->CreateBody(&blockDef), alias);
    b2PolygonShape block;
    block.SetAsBox(1.0f, 1.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &block;
    fixtureDef.density = m_configuration.density;
    fixtureDef.friction = m_configuration.friction;
    fixtureDef.restitution = m_configuration.restitution;

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

unsigned int LevelBlock::GetCode() const
{
    return m_configuration.blockCode;
}

bool LevelBlock::IsVisible() const
{
    return m_visbility;
}

bool LevelBlock::HasCollision() const
{
    return m_configuration.collision;
}

void LevelBlock::SetGameModeConfiguration(const GameModeConfiguration &configuration)
{
    m_gameModeConfiguration = configuration;
    m_asset->GetBody()->SetGravityScale(configuration.gravityModifier);
    m_asset->GetBody()->SetLinearDamping(configuration.dampingModifier);
}

void LevelBlock::SetVisibility(bool visibility)
{
    m_visbility = visibility;
}
