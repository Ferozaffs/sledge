#include "Player.h"
#include "Avatar.h"
#include "PlayerManager.h"

using namespace Gameplay;

Player::Player(PlayerManager *playerManager, b2World *world, unsigned int tint)
    : m_playerManager(playerManager), m_sledgeInput(0.0f), m_moveInput(0.0f), m_jumpInput(0.0f), m_pendingRemove(false),
      m_respawnTimer(0.0f), m_tint(tint), m_wishToRestart(false)
{
    SpawnAvatar(world);
}

Player::~Player()
{
}

const float Player::GetX() const
{
    return m_avatar->GetX();
}

const float Player::GetY() const
{
    return m_avatar->GetY();
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

    if (m_avatar->GetPosition().y < -50.0f || m_respawnTimer > 5.0f)
    {
        Respawn();
    }

    m_wishToRestart = m_avatar->GetWeaponRot() < -1.0f ? true : false;
}

void Player::Respawn()
{
    auto world = m_avatar->GetBody()->GetWorld();
    SpawnAvatar(world);
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

bool Player::IsWishingToRestart() const
{
    return m_wishToRestart;
}

void Player::SpawnAvatar(b2World *world)
{
    auto spawn = m_playerManager->GetOptimalSpawn();
    b2Vec2 spawnVec;
    spawnVec.x = 2.0f * static_cast<float>(spawn.first);
    spawnVec.y = 2.0f * static_cast<float>(spawn.second);

    m_avatar = std::make_unique<Gameplay::Avatar>(world, spawnVec, m_tint);
}
