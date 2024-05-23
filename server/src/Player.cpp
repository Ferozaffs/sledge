#include "Player.h"
#include "Avatar.h"
#include "PlayerManager.h"

using namespace Gameplay;

constexpr float RESPAWN_HOLD_SENTINEL = -1.0;

Player::Player(PlayerManager *playerManager, b2World *world, unsigned int tint)
    : m_playerManager(playerManager), m_world(world), m_sledgeInput(0.0f), m_moveInput(0.0f), m_jumpInput(0.0f),
      m_pendingRemove(false), m_respawnTimer(RESPAWN_HOLD_SENTINEL), m_tint(tint), m_score(0), m_wishToRestart(false)
{
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
    if (m_respawnTimer >= 0.0f)
    {
        m_respawnTimer = std::max(0.0f, m_respawnTimer - deltaTime);
        if (m_respawnTimer == 0.0f)
        {
            Respawn();
        }
    }

    if (m_avatar != nullptr)
    {
        m_avatar->Update(deltaTime, m_sledgeInput, m_jumpInput, m_moveInput);

        if (m_avatar->GetPosition().y < -50.0f)
        {
            m_avatar->Kill();
        }

        m_wishToRestart = m_avatar->GetWeaponRot() < -1.0f ? true : false;
    }
    else
    {
        m_wishToRestart = true;
    }
}

void Player::Respawn(float time)
{
    if (m_respawnTimer == RESPAWN_HOLD_SENTINEL)
    {
        m_respawnTimer = time;
    }
}

void Player::Respawn()
{
    SpawnAvatar(m_world);

    m_respawnTimer = RESPAWN_HOLD_SENTINEL;
}

void Player::SetInputs(float sledgeInput, float moveInput, float jumpInput)
{
    m_sledgeInput = sledgeInput;
    m_moveInput = moveInput;
    m_jumpInput = jumpInput;
}

int Player::GetMainAssetId() const
{
    if (m_avatar != nullptr)
    {
        return m_avatar->GetBodyId();
    }

    return -1;
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

signed int Player::GetScore() const
{
    return m_score;
}

void Player::Score(signed int score)
{
    m_score += score;
}

void Player::ClearScore()
{
    m_score = 0;
}

bool Player::IsWishingToRestart() const
{
    return m_wishToRestart;
}

bool Gameplay::Player::IsDead() const
{
    return m_avatar == nullptr || m_avatar->IsDead();
}

void Player::SpawnAvatar(b2World *world)
{
    auto spawn = m_playerManager->GetOptimalSpawn();
    b2Vec2 spawnVec;
    spawnVec.x = 2.0f * static_cast<float>(spawn.first);
    spawnVec.y = 2.0f * static_cast<float>(spawn.second);

    m_avatar = std::make_unique<Gameplay::Avatar>(world, spawnVec, m_tint);
}
