#include "GameManager.h"
#include "GameModeBrawl.h"
#include "GameModeSandbox.h"
#include "GameModeTeamBrawl.h"
#include "Player.h"

using namespace Gameplay;

GameManager::GameManager(std::weak_ptr<b2World> world) : m_levelManager(world), m_playerManager(*this, world)
{
    m_levelManager.LoadPlaylist("data/levels");
}

GameManager::~GameManager()
{
}

void GameManager::Update(float deltaTime)
{
    m_levelManager.Update(deltaTime);
    m_playerManager.Update(deltaTime);

    if (m_currentGameMode != nullptr)
    {
        m_currentGameMode->Update(deltaTime);
    }
}

bool GameManager::Finished() const
{
    return m_currentGameMode == nullptr || m_currentGameMode->Finished();
}

std::pair<int, int> GameManager::GetOptimalSpawn() const
{
    auto spawns = m_levelManager.GetSpawns();

    std::map<std::pair<int, int>, float> spawnDistances;
    for (const auto &s : spawns)
    {
        auto spawnX = 2.0f * static_cast<float>(s.first);
        auto spawnY = 2.0f * static_cast<float>(s.second);

        float spawnDistance = 100000.0f;
        for (const auto &player : m_playerManager.GetPlayers())
        {
            if (player->IsDead() == false)
            {
                spawnDistance = std::min(
                    spawnDistance, std::max(std::abs(spawnX - player->GetX()), std::abs(spawnY - player->GetY())));
            }
        }

        spawnDistances.insert(std::make_pair(s, spawnDistance));
    }

    std::pair<int, int> spawn = {50, 50};
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

std::vector<std::weak_ptr<Asset>> GameManager::GetAssets(bool allAssets)
{
    auto assets = m_levelManager.GetAssets(allAssets);
    auto playerAssets = m_playerManager.GetAssets();

    assets.insert(assets.end(), playerAssets.begin(), playerAssets.end());

    return assets;
}

int GameManager::GetScore() const
{
    int score = 0;
    for (const auto &player : m_playerManager.GetPlayers())
    {
        score += player->GetScore();
    }
    return score;
}

std::unordered_map<int, int> GameManager::GetScoreMap() const
{
    std::unordered_map<int, int> scoreMap;
    for (const auto &player : m_playerManager.GetPlayers())
    {
        scoreMap[player->GetMainAssetId()] = player->GetScore();
    }

    return scoreMap;
}

GameModeType GameManager::GetCurrentGameMode() const
{
    return m_currentGameMode != nullptr ? m_currentGameMode->GetType() : GameModeType::None;
}

void GameManager::SetGameMode(GameModeType type)
{
    if (type == GameModeType::Brawl)
    {
        m_currentGameMode = std::make_unique<GameModeBrawl>(m_playerManager);
        if (m_currentGameMode->IsValid() == false)
        {
            m_currentGameMode = std::make_unique<GameModeSandbox>(m_playerManager);
        }
    }
    else if (type == GameModeType::TeamBrawl)
    {
        m_currentGameMode = std::make_unique<GameModeTeamBrawl>(m_playerManager);
        if (m_currentGameMode->IsValid() == false)
        {
            m_currentGameMode = std::make_unique<GameModeSandbox>(m_playerManager);
        }
    }
    else
    {
        m_currentGameMode = std::make_unique<GameModeSandbox>(m_playerManager);
    }
}

std::weak_ptr<Player> GameManager::AddPlayer()
{
    auto player = m_playerManager.CreatePlayer();

    if (m_playerManager.GetNumPlayers() <= 2)
    {
        m_playerManager.ClearScore();
        SetGameMode(GameModeType::Brawl);
        m_levelManager.NextLevel(GetCurrentGameMode());
    }

    return player;
}

void GameManager::NextLevelWish(GameModeType mode)
{
    m_levelManager.NextLevel(mode);
}
