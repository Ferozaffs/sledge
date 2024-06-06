#include "B2Manager.h"
#ifdef WIN32
#include "Debugger.h"
#endif

#include <box2d/box2d.h>
#include <linmath.h>

using namespace Physics;

constexpr float timeStep = 1.0f / 60.0f;
constexpr int32 velocityIterations = 6;
constexpr int32 positionIterations = 2;

#ifdef WIN32
static Debug::Debugger::ShapeType GetRenderType(b2Shape::Type type)
{
    switch (type)
    {
    case b2Shape::e_circle:
        return Debug::Debugger::ShapeType::Box;
    case b2Shape::e_polygon:
        return Debug::Debugger::ShapeType::Box;
    default:
        return Debug::Debugger::ShapeType::Box;
    }
}
#endif

B2Manager::B2Manager() : m_accumelatedTime(0.0f)
{
    b2Vec2 gravity(0.0f, -9.81f);
    m_world = std::make_shared<b2World>(gravity);
}

void B2Manager::Update(float deltaTime)
{
    m_accumelatedTime += deltaTime;
    while (m_accumelatedTime > timeStep)
    {
        m_accumelatedTime -= timeStep;

        m_world->Step(timeStep, velocityIterations, positionIterations);
    }
}

std::weak_ptr<b2World> Physics::B2Manager::GetWorld()
{
    return m_world;
}

#ifdef WIN32
void B2Manager::DbgRender(std::weak_ptr<Debug::Debugger> debugger)
{
    if (auto dbg = debugger.lock())
    {
        for (auto b = m_world->GetBodyList(); b; b = b->GetNext())
        {
            auto transform = b->GetTransform();
            for (auto f = b->GetFixtureList(); f; f = f->GetNext())
            {
                auto shape = static_cast<b2PolygonShape *>(f->GetShape());
                b2Vec2 min;
                b2Vec2 max;
                min = max = shape->m_vertices[0];
                for (int i = 1; i < shape->m_count; i++)
                {
                    min.x = std::min(min.x, shape->m_vertices[i].x);
                    min.y = std::min(min.y, shape->m_vertices[i].y);
                    max.x = std::max(max.x, shape->m_vertices[i].x);
                    max.y = std::max(max.y, shape->m_vertices[i].y);
                }

                mat4x4 pose;
                mat4x4_identity(pose);

                mat4x4_translate(pose, transform.p.x, transform.p.y, 0.0f);
                mat4x4_rotate_Z(pose, pose, transform.q.GetAngle());
                mat4x4_scale_aniso(pose, pose, max.x - min.x, max.y - min.y, 1.0f);

                dbg->AddShape(pose, GetRenderType(f->GetType()));
            }
        }
    }
}
#endif
