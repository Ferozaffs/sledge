#include "Sledge.h"
#include "Player.h"

using namespace Gameplay;

Sledge::Sledge(const Player* player) 
	: Weapon(player)
{
	m_speed = 3.0f;
	m_torque = 5000.0f;

	static const b2Vec2 shaftSize(5.0f, 0.25f);
	static const b2Vec2 headSize(0.75f, 2.0f);
	constexpr float shaftDensity = 0.1f;
	constexpr float shaftFriction = 0.3f;
	constexpr float headDensity = 0.1f;
	constexpr float headFriction = 0.5f;

	auto world = player->GetBody()->GetWorld();

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = player->GetPosition();
	bodyDef.position.x += shaftSize.x;
	bodyDef.fixedRotation = false;
	m_shaft = world->CreateBody(&bodyDef);
	
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(shaftSize.x, shaftSize.y);
	
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = shaftDensity;
	fixtureDef.friction = shaftFriction;
	m_shaft->CreateFixture(&fixtureDef);

	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x += shaftSize.x;
	m_sledgeHead = world->CreateBody(&bodyDef);

	dynamicBox.SetAsBox(headSize.x, headSize.y);

	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = headDensity;
	fixtureDef.friction = headFriction;
	m_sledgeHead->CreateFixture(&fixtureDef);

	b2WeldJointDef joint;
	joint.Initialize(m_shaft, m_sledgeHead, bodyDef.position);
	m_weld = world->CreateJoint(&joint);
}

Sledge::~Sledge()
{
	if (m_weld != nullptr)
	{
		m_sledgeHead->GetWorld()->DestroyJoint(m_weld);
	}
	if (m_sledgeHead != nullptr)
	{
		m_sledgeHead->GetWorld()->DestroyBody(m_sledgeHead);
	}
}
