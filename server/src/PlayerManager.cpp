#include "PlayerManager.h"
#include "GameManager.h"
#include "LevelManager.h"
#include "Player.h"

#include <cmath>

using namespace Gameplay;

std::vector<unsigned int> tintList = {
    0xAA0000, 0x0000AA, 0x00AA00, 0xAAAA00, 0x00AAAA, 0xAA00AA, 0xFF0000, 0x0000FF, 0x00FF00,
    0xFFFF00, 0x00FFFF, 0xFF00FF, 0x880000, 0x000088, 0x008800, 0x888800, 0x008888, 0x880088,
};

PlayerManager::PlayerManager(b2World *world)
    : m_world(world), m_levelManager(nullptr), m_playersSpawned(0), m_restartTimer(5.0f), m_totalScore(0),
      m_redScore(0), m_blueScore(0)
{
}

PlayerManager::~PlayerManager()
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
        if (m_restartTimer <= 0.0f && m_levelManager != nullptr)
        {
            ClearScore();
            if (wishingBrawl >= halfPlayers)
            {
                m_levelManager->NextLevel(GameModeType::Brawl);
            }
            else if (wishingTeamBrawl >= halfPlayers)
            {
                m_levelManager->NextLevel(GameModeType::TeamBrawl);
            }
            m_restartTimer = 10.0f;
        }
    }
    else
    {
        m_restartTimer = 10.0f;
    }
}

const std::shared_ptr<Player> &PlayerManager::CreatePlayer()
{
    unsigned int team = 0;
    if (GetRedPlayers().size() > GetBluePlayers().size())
    {
        team = 1;
    }

    m_players.emplace_back(
        std::make_shared<Player>(this, m_world, tintList[m_playersSpawned++ % tintList.size()], tintList[team]));

    return m_players.back();
}

std::shared_ptr<Player> PlayerManager::GetPlayer(size_t index)
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

std::vector<std::shared_ptr<Player>> PlayerManager::GetPlayers()
{
    return m_players;
}

std::vector<std::shared_ptr<Player>> PlayerManager::GetPlayersAlive()
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

std::vector<std::shared_ptr<Player>> PlayerManager::GetPlayersDead()
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

std::vector<std::shared_ptr<Player>> PlayerManager::GetRedPlayers()
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

std::vector<std::shared_ptr<Player>> PlayerManager::GetBluePlayers()
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

std::vector<std::shared_ptr<Asset>> PlayerManager::GetAssets()
{
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto &player : m_players)
    {
        auto playerAssets = player->GetAssets();
        assets.insert(assets.end(), playerAssets.begin(), playerAssets.end());
    }

    return assets;
}

std::pair<int, int> PlayerManager::GetOptimalSpawn() const
{
    if (m_levelManager != nullptr)
    {
        auto spawns = m_levelManager->GetSpawns();

        std::map<std::pair<int, int>, float> spawnDistances;
        for (const auto &s : spawns)
        {
            auto spawnX = 2.0f * static_cast<float>(s.first);
            auto spawnY = 2.0f * static_cast<float>(s.second);

            float spawnDistance = 100000.0f;
            for (const auto &player : m_players)
            {
                if (player->IsDead() == false)
                {
                    spawnDistance = std::min(
                        spawnDistance, std::max(std::abs(spawnX - player->GetX()), std::abs(spawnY - player->GetY())));
                }
            }

            spawnDistances.insert(std::make_pair(s, spawnDistance));
        }

        std::pair<int, int> spawn = {0, 0};
        float distance = 0.0f;
        for (const auto &s : spawnDistances)
        {
            if (distance <= s.second)
            {
                spawn = s.first;
                distance = s.second;
            }
        }

        return spawn;
    }

    return std::make_pair(50, 50);
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

void PlayerManager::SetLevelManager(LevelManager *levelManager)
{
    m_levelManager = levelManager;
}
