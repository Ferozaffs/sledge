#include "Player.h"
#include "Sledge.h"
#include "Sword.h"

using namespace Gameplay;

Player::Player(const std::shared_ptr<b2World>& world, const b2Vec2& spawnPos)
	: m_body(nullptr)
	, m_weaponJoint(nullptr)
{
	static const b2Vec2 bodySize(2.0f,2.0f);
	constexpr float playerDensity = 1.0f;
	constexpr float playerFriction = 0.3f;
	constexpr float gravityScale = 2.0f;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = spawnPos;
	bodyDef.fixedRotation = true;
	bodyDef.gravityScale = gravityScale;
	m_body = world->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(bodySize.x, bodySize.y);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = playerDensity;
	fixtureDef.friction = playerFriction;
	m_body->CreateFixture(&fixtureDef);

	AssignWeapon(WeaponType::Sword);
}

Player::~Player()
{
	if (m_weaponJoint != nullptr)
	{
		m_body->GetWorld()->DestroyJoint(m_weaponJoint);
	}
	if (m_body != nullptr)
	{
		m_body->GetWorld()->DestroyBody(m_body);
	}	
}

void Player::AssignWeapon(WeaponType type)
{
	switch (type)
	{
	case WeaponType::Sledge:
		m_weapon = std::make_shared<Sledge>(this);
		break;
	case WeaponType::Sword:
		m_weapon = std::make_shared<Sword>(this);
		break;
	default:
		m_weapon = std::make_shared<Sledge>(this);
		break;
	}
	

	b2RevoluteJointDef jd;
	jd.Initialize(m_body, m_weapon->GetShaft(), GetPosition());
	jd.motorSpeed = m_weapon->GetSpeed();
	jd.maxMotorTorque = m_weapon->GetTorque();
	jd.enableMotor = true;
	jd.enableLimit = false;

	m_weaponJoint = (b2RevoluteJoint*)m_body->GetWorld()->CreateJoint(&jd);
}

void Player::Update(const float& /*deltaTime*/)
{
}

b2Body* Gameplay::Player::GetBody() const
{
	return m_body;
}

const b2Vec2& Gameplay::Player::GetPosition() const
{
	return m_body->GetTransform().p;
}
