#include "Asset.h"
#include "ConnectionManager.h"

#include <boost/uuid/uuid_generators.hpp>
#include <box2d/box2d.h>
#include <combaseapi.h>

using namespace Gameplay;

Asset::Asset(b2Body *body, const std::string &alias)
    : m_body(body), m_alias(alias), m_sizeX(0.0f), m_sizeY(0.0f), m_tint(0xFFFFFF)
{
    m_id = boost::uuids::random_generator()();
}

Asset::~Asset()
{
    Network::ConnectionManager::RemoveAsset(m_id);

    if (m_body != nullptr)
    {
        m_body->GetWorld()->DestroyBody(m_body);
    }
}

const boost::uuids::uuid Asset::GetId() const
{
    return m_id;
}

const std::string &Asset::GetAlias() const
{
    return m_alias;
}

const float Asset::GetX() const
{
    return m_body->GetTransform().p.x;
}

const float Asset::GetY() const
{
    return m_body->GetTransform().p.y;
}

const float Asset::GetSizeX() const
{
    assert(m_sizeX > 0 && "Asset size not set");

    return m_sizeX;
}

const float Asset::GetSizeY() const
{
    assert(m_sizeY > 0 && "Asset size not set");

    return m_sizeY;
}

const float Asset::GetRot() const
{
    return m_body->GetTransform().q.GetAngle();
}

const unsigned int Asset::GetTint() const
{
    return m_tint;
}

b2Body *Asset::GetBody()
{
    return m_body;
}

void Asset::SetTint(unsigned int tint)
{
    m_tint = tint;
}

void Asset::UpdateSize()
{
    for (auto f = m_body->GetFixtureList(); f; f = f->GetNext())
    {
        auto shape = static_cast<b2PolygonShape *>(f->GetShape());
        b2Vec2 min;
        b2Vec2 max;
        min = max = shape->m_vertices[0];
        for (int i = 1; i < shape->m_count; i++)
        {
            min.x = min(min.x, shape->m_vertices[i].x);
            min.y = min(min.y, shape->m_vertices[i].y);
            max.x = max(max.x, shape->m_vertices[i].x);
            max.y = max(max.y, shape->m_vertices[i].y);
        }

        m_sizeX = max.x - min.x;
        m_sizeY = max.y - min.y;
    }
}
