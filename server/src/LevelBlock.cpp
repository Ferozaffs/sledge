#include "LevelBlock.h"
#include "Asset.h"
#include "B2Filters.h"

#include <box2d/box2d.h>

using namespace Gameplay;

LevelBlock::LevelBlock(b2World *world, int x, int y, std::string alias)
{
    if (alias.find("decor") != std::string::npos)
    {
        m_type = BlockType::Decor;
        m_health = 0.0f;
    }
    else if (alias.find("weak") != std::string::npos)
    {
        m_type = BlockType::Weak;
        m_health = 0.2f;
    }
    else if (alias.find("tough") != std::string::npos)
    {
        m_type = BlockType::Tough;
        m_health = 1.0f;
    }
    else if (alias.find("static") != std::string::npos)
    {
        m_type = BlockType::Static;
        m_health = -1.0f;
    }

    b2BodyDef blockDef;
    blockDef.position.Set(2.0f * static_cast<float>(x), 2.0f * static_cast<float>(y));
    m_asset = std::make_shared<Asset>(world->CreateBody(&blockDef), alias);
    b2PolygonShape block;
    block.SetAsBox(1.0f, 1.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &block;

    if (m_type == BlockType::Decor)
    {
        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Block_Decor);
        fixtureDef.filter.maskBits = 0xFFFF;
        fixtureDef.filter.maskBits &= ~(static_cast<unsigned int>(CollisionFilter::Avatar_Body) |
                                        static_cast<unsigned int>(CollisionFilter::Avatar_Legs));
    }

    m_asset->GetBody()->CreateFixture(&fixtureDef);
    m_asset->UpdateSize();
}

LevelBlock::~LevelBlock()
{
}

bool LevelBlock::Update(float deltaTime)
{
    if (m_asset->GetBody()->GetType() != b2_dynamicBody && m_asset->GetBody()->GetContactList() != nullptr)
    {
        for (auto c = m_asset->GetBody()->GetContactList(); c; c = c->next)
        {
            if (c->contact != nullptr && c->contact->IsTouching())
            {
                b2Fixture *contactFixture = nullptr;
                if (c->contact->GetFixtureA()->GetBody() == m_asset->GetBody())
                {
                    contactFixture = c->contact->GetFixtureB();
                }
                else
                {
                    contactFixture = c->contact->GetFixtureA();
                }

                auto impact = 0.0f;
                if (contactFixture->GetFilterData().categoryBits !=
                        static_cast<unsigned int>(CollisionFilter::Avatar_Body) &&
                    contactFixture->GetFilterData().categoryBits !=
                        static_cast<unsigned int>(CollisionFilter::Avatar_Legs))
                {
                    impact = contactFixture->GetBody()->GetLinearVelocity().Length() *
                             contactFixture->GetBody()->GetMass() * deltaTime;

                    m_health -= impact;
                }

                if (m_type != BlockType::Static && m_health <= 0.0f)
                {
                    if (m_type == BlockType::Tough)
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
    }

    if (m_asset->GetY() < -50.0f)
    {
        return false;
    }

    return true;
}

const std::shared_ptr<Asset> &LevelBlock::GetAsset() const
{
    return m_asset;
}

bool LevelBlock::InMotion()
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
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.1f;

    m_asset->GetBody()->CreateFixture(&fixtureDef);
    m_asset->UpdateSize();
}
