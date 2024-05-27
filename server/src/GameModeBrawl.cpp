#include "GameModeBrawl.h"
#include "GameManager.h"
#include "Player.h"
#include "PlayerManager.h"

using namespace Gameplay;

GameModeBrawl::GameModeBrawl(PlayerManager *playerManager)
    : m_playerManager(playerManager), m_valid(true), m_countDown(5.0f)
{
    m_numPlayers = m_playerManager->GetNumPlayers();
    if (m_numPlayers <= 1)
    {
        m_valid = false;
    }

    auto players = m_playerManager->GetPlayers();
    for (const auto &player : players)
    {
        player->SetTeamColors(false);
        player->Respawn(0.0);
    }
}

GameModeBrawl::~GameModeBrawl()
{
}

void GameModeBrawl::Update(float deltaTime)
{
    auto players = m_playerManager->GetPlayersAlive();
    int numPlayers = players.size();
    if (numPlayers < m_numPlayers)
    {
        for (const auto &player : players)
        {
            player->Score(1);
        }

        m_numPlayers = numPlayers;
    }

    if (m_numPlayers <= 1)
    {
        if (m_numPlayers == 1)
        {
            players[0]->SetWinner();
        }
        m_countDown -= deltaTime;
    }
}

GameModeType GameModeBrawl::GetType() const
{
    return GameModeType::Brawl;
}

bool GameModeBrawl::Finished() const
{
    return m_countDown <= 0.0f;
}

bool GameModeBrawl::IsValid() const
{
    return m_valid;
}
