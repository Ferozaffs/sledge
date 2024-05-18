#include "LevelBlock.h"
#include "Asset.h"

#include <box2d/box2d.h>

using namespace Gameplay;

LevelBlock::LevelBlock(b2World *world, int x, int y)
{
    b2BodyDef blockDef;
    blockDef.position.Set(2.0f * static_cast<float>(x), 2.0f * static_cast<float>(y));
    m_asset = std::make_shared<Asset>(world->CreateBody(&blockDef));
    b2PolygonShape block;
    block.SetAsBox(1.0f, 1.0f);
    m_asset->GetBody()->CreateFixture(&block, 0.0f);
    m_asset->UpdateSize();
}

LevelBlock::~LevelBlock()
{
}

const std::shared_ptr<Asset> &Gameplay::LevelBlock::GetAsset() const
{
    return m_asset;
}

bool Gameplay::LevelBlock::InMotion()
{
    return m_asset->GetBody()->GetLinearVelocity().Length() > 0.0f;
}
