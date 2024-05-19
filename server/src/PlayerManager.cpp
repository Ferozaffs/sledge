#include "PlayerManager.h"
#include "LevelManager.h"
#include "Player.h"

#include <math.h>

using namespace Gameplay;

std::vector<unsigned int> tintList = {
    0xAA0000, 0x0000AA, 0x00AA00, 0xAAAA00, 0x00AAAA, 0xAA00AA, 0xFF0000, 0x0000FF, 0x00FF00,
    0xFFFF00, 0x00FFFF, 0xFF00FF, 0x880000, 0x000088, 0x008800, 0x888800, 0x008888, 0x880088,
};

PlayerManager::PlayerManager(LevelManager *levelManager, const std::shared_ptr<b2World> &b2World)
    : m_levelManager(levelManager), m_b2World(b2World), m_restartTimer(5.0f)
{
}

PlayerManager::~PlayerManager()
{
}

void PlayerManager::Update(float deltaTime)
{
    bool wishingToRestart = true;
    for (auto it = m_players.begin(); it != m_players.end();)
    {
        if ((*it)->m_pendingRemove)
        {
            it = m_players.erase(it);
        }
        else
        {
            (*it)->Update(deltaTime);

            if ((*it)->IsWishingToRestart() == false)
            {
                wishingToRestart = false;
            }

            it++;
        }
    }

    if (wishingToRestart == true)
    {
        m_restartTimer -= deltaTime;
        if (m_restartTimer <= 0.0f)
        {
            m_levelManager->ReloadLevel();
            for (auto it = m_players.begin(); it != m_players.end(); ++it)
            {
                (*it)->Respawn();
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
    m_players.emplace_back(
        std::make_shared<Player>(this, m_b2World.get(), tintList[m_players.size() % tintList.size()]));
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

std::vector<std::shared_ptr<Asset>> PlayerManager::GetDynamicAssets() const
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
    auto spawns = m_levelManager->GetSpawns();

    std::map<std::pair<int, int>, float> spawnDistances;
    for (const auto &s : spawns)
    {
        auto spawnX = 2.0f * static_cast<float>(s.first);
        auto spawnY = 2.0f * static_cast<float>(s.second);

        float spawnDistance = 100000.0f;
        for (const auto &player : m_players)
        {
            spawnDistance = std::min(spawnDistance, abs(spawnX - player->GetX()));
            spawnDistance = std::min(spawnDistance, abs(spawnY - player->GetY()));
        }

        spawnDistances.insert(std::make_pair(s, spawnDistance));
    }

    std::pair<int, int> spawn = {0, 0};
    float distance = 0.0f;
    for (const auto &s : spawnDistances)
    {
        if (distance < s.second)
        {
            spawn = s.first;
            distance = s.second;
        }
    }

    return spawn;
}
