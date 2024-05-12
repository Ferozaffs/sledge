#include "Avatar.h"
#include "Sledge.h"
#include "Sword.h"

using namespace Gameplay;

Avatar::Avatar(const std::shared_ptr<b2World>& world, const b2Vec2& spawnPos)
	: m_body(nullptr)
	, m_weaponJoint(nullptr)
{
	static const b2Vec2 bodySize(2.0f,2.0f);
	constexpr float AvatarDensity = 1.0f;
	constexpr float AvatarFriction = 0.3f;
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
	fixtureDef.density = AvatarDensity;
	fixtureDef.friction = AvatarFriction;
	m_body->CreateFixture(&fixtureDef);

	AssignWeapon(WeaponType::Sledge);
}

Avatar::~Avatar()
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

void Avatar::AssignWeapon(WeaponType type)
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

void Avatar::Update(const float& /*deltaTime*/, const float& debugSledgeInput, const float& debugJumpInput, const float& debugMoveInput)
{
	m_weaponJoint->SetMotorSpeed(m_weapon->GetSpeed() * debugSledgeInput);
	
	bool hasContact = false;
	for (auto c = m_body->GetContactList(); c; c = c->next) {
		if (c->contact->IsTouching()) {
			hasContact = true;
			break;
		}
	}
	
	//Air control movement
	auto movement = 500.0f * std::max(0.0f, (1.0f - abs(m_body->GetLinearVelocity().x * 0.05f)));
	if (abs(m_body->GetLinearVelocity().y) < 10.0f && hasContact == true)
	{
		if (debugJumpInput > 0.0f) {
			m_body->ApplyLinearImpulse(b2Vec2(0.0f, 300.0f), m_body->GetTransform().p, true);
		}	
		
		//Ground control movement
		movement = 3000.0f * std::max(0.0f, (1.0f - abs(m_body->GetLinearVelocity().x * 0.05f)));
	}

	m_body->ApplyForce(b2Vec2(movement * debugMoveInput, 0.0f), m_body->GetTransform().p, true);

}

b2Body* Avatar::GetBody() const
{
	return m_body;
}

const b2Vec2& Avatar::GetPosition() const
{
	return m_body->GetTransform().p;
}
