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
    : m_world(world), m_gameManager(gameManager), m_playersSpawned(0), m_restartTimer(5.0f), m_totalScore(0)
{
}

void PlayerManager::Update(float deltaTime)
{
    unsigned int wishingNewSolo = 0;
    unsigned int wishingNewTeams = 0;

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

            if ((*it)->GetGameModeWish() == GameModeWish::Solo)
            {
                ++wishingNewSolo;
            }
            else if ((*it)->GetGameModeWish() == GameModeWish::Team)
            {
                ++wishingNewTeams;
            }

            it++;
        }
    }

    unsigned int halfPlayers = std::ceil(m_players.size() / 2.0f);
    if (wishingNewSolo >= halfPlayers || wishingNewTeams >= halfPlayers)
    {
        m_restartTimer -= deltaTime;
        if (m_restartTimer <= 0.0f)
        {
            ClearScore();
            if (wishingNewSolo >= halfPlayers)
            {
                m_gameManager.NextLevel(GameModeWish::Solo);
            }
            else if (wishingNewTeams >= halfPlayers)
            {
                m_gameManager.NextLevel(GameModeWish::Team);
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
    m_players.back()->UpdateSettings(m_levelSettings);

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

void PlayerManager::SetSettings(const GameSettings &settings)
{
    m_levelSettings = settings;
    for (const auto &player : m_players)
    {
        player->UpdateSettings(settings);
    }
}

signed int PlayerManager::GetScore() const
{
    return m_totalScore;
}

void PlayerManager::ClearScore()
{
    for (const auto &player : m_players)
    {
        player->ClearScore();
    }
}
