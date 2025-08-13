#include "GameManager.h"
#include "B2Manager.h"
#include "GameMode.h"
#include "GameModeSandbox.h"
#include "Player.h"
#include <random>

using namespace Gameplay;

GameManager::GameManager(std::weak_ptr<b2World> world)
    : m_levelManager(world), m_playerManager(*this, world), m_currentGameModeWish(GameModeWish::None)
{
    if (m_levelManager.LoadPlaylist("data/levels"))
    {
        NextLevel(GetCurrentGameModeWish());
    }
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

        if (m_currentGameMode->Finished())
        {
            NextLevel(GetCurrentGameModeWish());
        }
    }
}

bool GameManager::Finished() const
{
    return m_currentGameMode == nullptr || m_currentGameMode->Finished();
}

std::pair<int, int> GameManager::GetOptimalSpawn(const Player *player) const
{
    auto spawns = m_levelManager.GetSpawns();
    auto redSpawns = m_levelManager.GetRedSpawns();
    auto blueSpawns = m_levelManager.GetBlueSpawns();

    if (m_currentGameModeConfiguration.teams && redSpawns.empty() == false && blueSpawns.empty() == false)
    {
        if (player->GetTeam() == Team::Red)
        {
            spawns = redSpawns;
        }
        else
        {
            spawns = blueSpawns;
        }
    }

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

signed int GameManager::GetPoints() const
{
    return m_currentGameMode->GetPoints();
}

std::unordered_map<int, float> GameManager::GetPointsMap() const
{
    return m_currentGameMode->GetPointsMap();
}

GameModeWish GameManager::GetCurrentGameModeWish() const
{
    return m_currentGameModeWish;
}

void GameManager::SetGameMode(GameModeType type, const GameModeConfiguration &configuration,
                              const std::weak_ptr<Level> level)
{
    if (type == GameModeType::Custom)
    {
        m_currentGameMode = std::make_unique<GameMode>(m_playerManager, configuration, level);
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
        NextLevel(GetCurrentGameModeWish());
    }

    return player;
}

void GameManager::NextLevel(GameModeWish wish)
{
    if (m_levelManager.NextLevel())
    {
        std::vector<GameModeConfiguration> configurations;
        if (wish == GameModeWish::Team)
        {
            for (const auto &mode : m_levelManager.GetSettings().gameModeConfigurations)
            {
                if (mode.teams)
                {
                    configurations.emplace_back(mode);
                }
            }
        }
        else if (wish == GameModeWish::Solo)
        {
            for (const auto &mode : m_levelManager.GetSettings().gameModeConfigurations)
            {
                if (mode.teams == false)
                {
                    configurations.emplace_back(mode);
                }
            }
        }

        if (configurations.empty())
        {
            configurations = m_levelManager.GetSettings().gameModeConfigurations;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, configurations.size() - 1);

        m_currentGameModeConfiguration = configurations[dist(gen)];

        m_playerManager.SetGameModeConfiguration(m_currentGameModeConfiguration);
        m_levelManager.SetGameModeConfiguration(m_currentGameModeConfiguration);
        SetGameMode(GameModeType::Custom, m_currentGameModeConfiguration, m_levelManager.GetCurrentLevel());
    }
}
