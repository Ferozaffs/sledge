#include "Player.h"
#include "Avatar.h"

using namespace Gameplay;

Player::Player(const std::shared_ptr<b2World>& world)
	: m_sledgeInput(0.0f)
	, m_moveInput(0.0f)
	, m_jumpInput(0.0f)
	, m_pendingRemove(false)
{
	m_avatar = std::make_unique<Gameplay::Avatar>(world, b2Vec2(0.0f, 4.0f));
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	m_avatar->Update(deltaTime, m_sledgeInput, m_jumpInput, m_moveInput);
}

void Player::SetInputs(float sledgeInput, float moveInput, float jumpInput)
{
	m_sledgeInput = sledgeInput;
	m_moveInput = moveInput;
	m_jumpInput = jumpInput;
}
