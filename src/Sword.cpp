#include "Sword.h"
#include "Avatar.h"

using namespace Gameplay;

Sword::Sword(const Avatar* avatar)
	: Weapon(avatar)
{
	m_speed = 10.0f;
	m_torque = 20000.0f;

	static const b2Vec2 shaftSize(5.0f, 0.25f);
	static const b2Vec2 hiltSize(0.25f, 1.0f);
	constexpr float shaftDensity = 0.1f;
	constexpr float shaftFriction = 0.3f;
	constexpr float hiltDensity = 0.1f;
	constexpr float hiltFriction = 0.5f;

	auto world = avatar->GetBody()->GetWorld();

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = avatar->GetPosition();
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
	bodyDef.position.x += shaftSize.x * 0.1f;
	m_hilt = world->CreateBody(&bodyDef);

	dynamicBox.SetAsBox(hiltSize.x, hiltSize.y);

	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = hiltDensity;
	fixtureDef.friction = hiltFriction;
	m_hilt->CreateFixture(&fixtureDef);

	b2WeldJointDef joint;
	joint.Initialize(m_shaft, m_hilt, bodyDef.position);
	m_weld = world->CreateJoint(&joint);
}

Sword::~Sword()
{
	if (m_weld != nullptr)
	{
		m_hilt->GetWorld()->DestroyJoint(m_weld);
	}
	if (m_hilt != nullptr)
	{
		m_hilt->GetWorld()->DestroyBody(m_hilt);
	}
}
