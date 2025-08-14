#include "Player.h"
#include "Avatar.h"
#include "GameManager.h"
#include "PlayerManager.h"

using namespace Gameplay;

constexpr float RESPAWN_HOLD_SENTINEL = -1.0;

Player::Player(const GameManager &gameManager, const PlayerManager &playerManager, std::weak_ptr<b2World> world,
               unsigned int tint, unsigned int teamTint)
    : m_gameManager(gameManager), m_playerManager(playerManager), m_world(world), m_sledgeInput(0.0f),
      m_moveInput(0.0f), m_jumpInput(0.0f), m_pendingRemove(false), m_respawnTimer(RESPAWN_HOLD_SENTINEL), m_tint(tint),
      m_teamTint(teamTint), m_score(0), m_winner(false), m_usingTeamColors(false), m_gameModeWish(GameModeWish::None)
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

    m_gameModeWish = GameModeWish::None;
    if (m_avatar != nullptr)
    {
        m_avatar->Update(deltaTime, m_sledgeInput, m_jumpInput, m_moveInput);

        if (m_avatar->GetPosition().y < -50.0f)
        {
            m_avatar->Kill();
        }

        m_gameModeWish = (m_avatar->GetWeaponRot() < -1.5f && m_avatar->GetWeaponRot() > -1.7f) ? GameModeWish::Solo
                                                                                                : GameModeWish::None;
        m_gameModeWish = (m_avatar->GetWeaponRot() > 1.5f && m_avatar->GetWeaponRot() < 1.7f) ? GameModeWish::Team
                                                                                              : GameModeWish::None;
    }
}

void Player::SetGameModeConfiguration(const GameModeConfiguration &configuration)
{
    m_gameModeConfiguration = configuration;
    SetTeamColors(m_gameModeConfiguration.teams);
    if (m_avatar != nullptr)
    {
        m_avatar->UpdateSettings(m_gameModeConfiguration);
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

std::vector<std::weak_ptr<Asset>> Player::GetAssets() const
{
    std::vector<std::weak_ptr<Asset>> assets;
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

void Player::SetWinner()
{
    m_winner = true;
}

GameModeWish Player::GetGameModeWish() const
{
    return m_gameModeWish;
}

bool Player::IsDead() const
{
    return m_avatar == nullptr || m_avatar->IsDead();
}

signed int Player::CollectKiller()
{
    if (m_avatar != nullptr)
    {
        auto id = m_avatar->GetKillerId();
        m_avatar->SetKillerId(-1);
        return id;
    }
    return -1;
}

Team Player::GetTeam() const
{
    return m_teamTint == 0xAA0000 ? Team::Red : Team::Blue;
}

unsigned int Player::GetTeamTint() const
{
    return m_teamTint;
}

void Player::SetTeamColors(bool useTeamColors)
{
    m_usingTeamColors = useTeamColors;
}

void Player::SpawnAvatar(std::weak_ptr<b2World> world)
{
    auto spawn = m_gameManager.GetOptimalSpawn(this);
    b2Vec2 spawnVec;
    spawnVec.x = 2.0f * static_cast<float>(spawn.first);
    spawnVec.y = 2.0f * static_cast<float>(spawn.second);

    m_avatar = std::make_unique<Avatar>(world, spawnVec, m_tint, m_usingTeamColors == true ? m_teamTint : 0, m_winner);
    m_avatar->UpdateSettings(m_gameModeConfiguration);

    m_winner = false;
}
