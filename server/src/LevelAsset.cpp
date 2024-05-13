#include "LevelAsset.h"

#include <box2d/box2d.h>
#include <combaseapi.h>

using namespace Gameplay;

LevelAsset::LevelAsset(b2World* world, int x, int y)
{
    CoCreateGuid(&m_id);

    b2BodyDef blockDef;
    blockDef.position.Set(2.0f * static_cast<float>(x), 2.0f * static_cast<float>(y));
    m_body = world->CreateBody(&blockDef);
    b2PolygonShape block;
    block.SetAsBox(1.0f, 1.0f);
    m_body->CreateFixture(&block, 0.0f);
}

LevelAsset::~LevelAsset()
{
}

const GUID LevelAsset::GetId() const
{
    return m_id;
}

const float LevelAsset::GetX() const
{
    return m_body->GetTransform().p.x;
}

const float LevelAsset::GetY() const
{
    return m_body->GetTransform().p.y;
}
