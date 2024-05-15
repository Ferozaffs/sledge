#include "Avatar.h"
#include "Asset.h"
#include "Sledge.h"
#include "Sword.h"

using namespace Gameplay;

Avatar::Avatar(const std::shared_ptr<b2World>& world, const b2Vec2& spawnPos)
	: m_weaponJoint(nullptr)
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
	m_bodyAsset = std::make_shared<Asset>(world->CreateBody(&bodyDef));

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(bodySize.x, bodySize.y);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = AvatarDensity;
	fixtureDef.friction = AvatarFriction;
	GetBody()->CreateFixture(&fixtureDef);
	m_bodyAsset->UpdateSize();

	AssignWeapon(WeaponType::Sledge);
}

Avatar::~Avatar()
{
	if (m_weaponJoint != nullptr)
	{
		GetBody()->GetWorld()->DestroyJoint(m_weaponJoint);
	}
	if (GetBody() != nullptr)
	{
		GetBody()->GetWorld()->DestroyBody(GetBody());
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
	jd.Initialize(GetBody(), m_weapon->GetShaft(), GetPosition());
	jd.motorSpeed = m_weapon->GetSpeed();
	jd.maxMotorTorque = m_weapon->GetTorque();
	jd.enableMotor = true;
	jd.enableLimit = false;

	m_weaponJoint = (b2RevoluteJoint*)GetBody()->GetWorld()->CreateJoint(&jd);
}

void Avatar::Update(const float& deltaTime, const float& debugSledgeInput, const float& debugJumpInput, const float& debugMoveInput)
{
	m_weaponJoint->SetMotorSpeed(m_weapon->GetSpeed() * debugSledgeInput);
	
	bool hasContact = false;
	for (auto c = GetBody()->GetContactList(); c; c = c->next) {
		if (c->contact->IsTouching()) {
			hasContact = true;
			break;
		}
	}
	
	//Air control movement
	auto movement = 30000.0f * std::max(0.0f, (1.0f - abs(GetBody()->GetLinearVelocity().x * 0.05f)));
	if (abs(GetBody()->GetLinearVelocity().y) < 10.0f && hasContact == true)
	{
		if (debugJumpInput > 0.0f) {
			GetBody()->ApplyLinearImpulse(b2Vec2(0.0f, 300.0f), GetBody()->GetTransform().p, true);
		}	
		
		//Ground control movement
		movement = 180000.0f * std::max(0.0f, (1.0f - abs(GetBody()->GetLinearVelocity().x * 0.05f)));
	}

	GetBody()->ApplyForce(b2Vec2(movement * debugMoveInput * deltaTime, 0.0f), GetBody()->GetTransform().p, true);

}

b2Body* Avatar::GetBody() const
{
	return m_bodyAsset->GetBody();
}

const b2Vec2& Avatar::GetPosition() const
{
	return GetBody()->GetTransform().p;
}

std::vector<std::shared_ptr<Asset>> Avatar::GetAssets() const
{
	std::vector<std::shared_ptr<Asset>> assets;
	assets.emplace_back(m_bodyAsset);

	auto weaponAssets = m_weapon->GetAssets();
	assets.insert(assets.end(), weaponAssets.begin(), weaponAssets.end());

	return assets;
}
