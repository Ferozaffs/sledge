#include "B2Manager.h"
#include "Renderer.h"

#include <box2d/box2d.h>
#include <linmath.h>

using namespace Physics;

constexpr float timeStep = 1.0f / 60.0f;
constexpr int32 velocityIterations = 6;
constexpr int32 positionIterations = 2;

static Debug::Renderer::ShapeType GetRenderType(b2Shape::Type type)
{
	switch (type)
	{
	case b2Shape::e_circle:
		return Debug::Renderer::ShapeType::Box;
	case b2Shape::e_polygon:
		return Debug::Renderer::ShapeType::Box;
	default:
		return Debug::Renderer::ShapeType::Box;
	}
}


B2Manager::B2Manager()
	: m_accumelatedTime(0.0f)
{
	b2Vec2 gravity(0.0f, -9.81f);
	m_world = std::make_shared<b2World>(gravity);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	b2Body* groundBody = m_world->CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);
	
	//b2BodyDef bodyDef;
	//bodyDef.type = b2_dynamicBody;
	//bodyDef.position.Set(0.0f, 4.0f);
	//b2Body* body = m_world->CreateBody(&bodyDef);
	//b2PolygonShape dynamicBox;
	//dynamicBox.SetAsBox(1.0f, 1.0f);
	//b2FixtureDef fixtureDef;
	//fixtureDef.shape = &dynamicBox;
	//fixtureDef.density = 1.0f;
	//fixtureDef.friction = 0.3f;
	//body->CreateFixture(&fixtureDef);
	//
	//body->ApplyAngularImpulse(100.0f, true);
}

B2Manager::~B2Manager()
{
	
}

void B2Manager::Update(const float& deltaTime)
{
	m_accumelatedTime += deltaTime;
	while (m_accumelatedTime > timeStep)
	{
		m_accumelatedTime -= timeStep;

		m_world->Step(timeStep, velocityIterations, positionIterations);
	}
}

std::shared_ptr<b2World> Physics::B2Manager::GetWorld()
{
	return m_world;
}

void B2Manager::DbgRender(Debug::Renderer* renderer)
{
	for (auto b = m_world->GetBodyList(); b; b = b->GetNext())
	{
		auto transform = b->GetTransform();
		for (auto f = b->GetFixtureList(); f; f = f->GetNext())
		{
			auto shape = static_cast<b2PolygonShape*>(f->GetShape());
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
			mat4x4_scale_aniso(pose, pose, max.x-min.x, max.y-min.y, 1.0f);

			renderer->AddShape(pose, GetRenderType(f->GetType()));
		}
	}
}
