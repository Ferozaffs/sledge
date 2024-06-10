#include "PlayerManager.h"
#include "GameManager.h"
#include "LevelManager.h"
#include "Player.h"

#include <cmath>

using namespace Gameplay;

std::vector<unsigned int> tintList = {
    0xAA0000, 0x0000AA, 0x00AA00, 0xAAAA00, 0x00AAAA, 0xAA00AA, 0x00FF00,
    0xFFFF00, 0x00FFFF, 0xFF00FF, 0x008800, 0x888800, 0x008888, 0x880088,
};

PlayerManager::PlayerManager(GameManager &gameManager, std::weak_ptr<b2World> world)
    : m_world(world), m_gameManager(gameManager), m_playersSpawned(0), m_restartTimer(5.0f), m_totalScore(0),
      m_redScore(0), m_blueScore(0)
{
}

void PlayerManager::Update(float deltaTime)
{
    unsigned int wishingBrawl = 0;
    unsigned int wishingTeamBrawl = 0;

    m_totalScore = m_redScore + m_blueScore;
    for (auto it = m_players.begin(); it != m_players.end();)
    {
        if ((*it)->m_pendingRemove)
        {
            it = m_players.erase(it);
        }
        else
        {
            (*it)->Update(deltaTime);
            m_totalScore += (*it)->GetScore();

            if ((*it)->GetGameModeWish() == GameModeType::Brawl)
            {
                ++wishingBrawl;
            }
            else if ((*it)->GetGameModeWish() == GameModeType::TeamBrawl)
            {
                ++wishingTeamBrawl;
            }

            it++;
        }
    }

    unsigned int halfPlayers = std::ceil(m_players.size() / 2.0f);
    if (wishingBrawl >= halfPlayers || wishingTeamBrawl >= halfPlayers)
    {
        m_restartTimer -= deltaTime;
        if (m_restartTimer <= 0.0f)
        {
            ClearScore();
            if (wishingBrawl >= halfPlayers)
            {
                m_gameManager.NextLevelWish(GameModeType::Brawl);
            }
            else if (wishingTeamBrawl >= halfPlayers)
            {
                m_gameManager.NextLevelWish(GameModeType::TeamBrawl);
            }
            m_restartTimer = 10.0f;
        }
    }
    else
    {
        m_restartTimer = 10.0f;
    }
}

std::weak_ptr<Player> PlayerManager::CreatePlayer()
{
    unsigned int team = 0;
    if (GetRedPlayers().size() > GetBluePlayers().size())
    {
        team = 1;
    }

    m_players.emplace_back(std::make_shared<Player>(
        m_gameManager, *this, m_world, tintList[(m_playersSpawned++ % (tintList.size() - 2)) + 2], tintList[team]));

    return m_players.back();
}

std::shared_ptr<Player> PlayerManager::GetPlayer(size_t index) const
{
    if (m_players.size() > index)
    {
        return m_players[index];
    }

    return nullptr;
}

size_t PlayerManager::GetNumPlayers() const
{
    return m_players.size();
}

const std::vector<std::shared_ptr<Player>> &PlayerManager::GetPlayers() const
{
    return m_players;
}

std::vector<std::shared_ptr<Player>> PlayerManager::GetPlayersAlive() const
{
    std::vector<std::shared_ptr<Player>> players;
    for (const auto &player : m_players)
    {
        if (player->IsDead() == false)
        {
            players.emplace_back(player);
        }
    }

    return players;
}

std::vector<std::shared_ptr<Player>> PlayerManager::GetPlayersDead() const
{
    std::vector<std::shared_ptr<Player>> players;
    for (const auto &player : m_players)
    {
        if (player->IsDead())
        {
            players.emplace_back(player);
        }
    }

    return players;
}

std::vector<std::shared_ptr<Player>> PlayerManager::GetRedPlayers() const
{
    std::vector<std::shared_ptr<Player>> players;
    for (const auto &player : m_players)
    {
        if (player->GetTeamTint() == tintList[0])
        {
            players.emplace_back(player);
        }
    }

    return players;
}

std::vector<std::shared_ptr<Player>> PlayerManager::GetBluePlayers() const
{
    std::vector<std::shared_ptr<Player>> players;
    for (const auto &player : m_players)
    {
        if (player->GetTeamTint() == tintList[1])
        {
            players.emplace_back(player);
        }
    }

    return players;
}

std::vector<std::weak_ptr<Asset>> PlayerManager::GetAssets() const
{
    std::vector<std::weak_ptr<Asset>> assets;
    for (const auto &player : m_players)
    {
        auto playerAssets = player->GetAssets();
        assets.insert(assets.end(), playerAssets.begin(), playerAssets.end());
    }

    return assets;
}

signed int PlayerManager::GetScore() const
{
    return m_totalScore;
}

signed int PlayerManager::GetTeamScore(unsigned int teamTint) const
{
    return tintList[0] == teamTint ? m_redScore : m_blueScore;
}

void PlayerManager::ScoreRed(signed int score)
{
    m_redScore += score;
}

void PlayerManager::ScoreBlue(signed int score)
{
    m_blueScore += score;
}

void PlayerManager::ClearScore()
{
    m_redScore = 0;
    m_blueScore = 0;

    for (const auto &player : m_players)
    {
        player->ClearScore();
    }
}
