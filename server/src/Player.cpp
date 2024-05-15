#include "Player.h"
#include "Avatar.h"

using namespace Gameplay;

Player::Player(b2World* world)
	: m_sledgeInput(0.0f)
	, m_moveInput(0.0f)
	, m_jumpInput(0.0f)
	, m_pendingRemove(false)
	, m_respawnTimer(0.0f)
{
	m_avatar = std::make_unique<Gameplay::Avatar>(world, b2Vec2(0.0f, 4.0f));
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	m_avatar->Update(deltaTime, m_sledgeInput, m_jumpInput, m_moveInput);

	if (m_avatar->IsDead())
	{
		m_respawnTimer += deltaTime;
	}
	else
	{
		m_respawnTimer = 0.0f;
	}

	if (m_avatar->GetPosition().y < -100.0f || m_respawnTimer > 10.0f)
	{
		auto world = m_avatar->GetBody()->GetWorld();
		m_avatar = std::make_unique<Gameplay::Avatar>(world, b2Vec2(0.0f, 4.0f));
	}
}

void Player::SetInputs(float sledgeInput, float moveInput, float jumpInput)
{
	m_sledgeInput = sledgeInput;
	m_moveInput = moveInput;
	m_jumpInput = jumpInput;
}

std::vector<std::shared_ptr<Asset>> Player::GetAssets() const
{
	std::vector<std::shared_ptr<Asset>> assets;
	if (m_avatar != nullptr)
	{
		assets = m_avatar->GetAssets();
	}

	return assets;
}
