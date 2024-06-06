#include "GameModeTeamBrawl.h"
#include "GameManager.h"
#include "Player.h"
#include "PlayerManager.h"

using namespace Gameplay;

GameModeTeamBrawl::GameModeTeamBrawl(PlayerManager &playerManager)
    : m_playerManager(playerManager), m_valid(true), m_countDown(5.0f)
{
    m_numPlayersTeamRed = m_playerManager.GetRedPlayers().size();
    m_numPlayersTeamBlue = m_playerManager.GetBluePlayers().size();
    if (m_numPlayersTeamRed + m_numPlayersTeamBlue <= 1)
    {
        m_valid = false;
    }

    auto players = m_playerManager.GetPlayers();
    for (const auto &player : players)
    {
        player->SetTeamColors(true);
        player->Respawn(0.0);
    }
}

GameModeTeamBrawl::~GameModeTeamBrawl()
{
}

void GameModeTeamBrawl::Update(float deltaTime)
{
    auto redPlayers = m_playerManager.GetRedPlayers();
    auto bluePlayers = m_playerManager.GetBluePlayers();

    int numRedPlayersAlive = 0;
    for (auto &player : redPlayers)
    {
        if (player->IsDead() == false)
        {
            ++numRedPlayersAlive;
        }
    }

    if (numRedPlayersAlive < m_numPlayersTeamRed)
    {
        m_playerManager.ScoreBlue(1);

        m_numPlayersTeamRed = numRedPlayersAlive;
    }

    int numBluePlayersAlive = 0;
    for (auto &player : bluePlayers)
    {
        if (player->IsDead() == false)
        {
            ++numBluePlayersAlive;
        }
    }

    if (numBluePlayersAlive < m_numPlayersTeamBlue)
    {
        m_playerManager.ScoreRed(1);

        m_numPlayersTeamBlue = numBluePlayersAlive;
    }

    if (numRedPlayersAlive == 0)
    {
        for (auto &player : bluePlayers)
        {
            player->SetWinner();
        }
        m_countDown -= deltaTime;
    }
    else if (numBluePlayersAlive == 0)
    {
        for (auto &player : redPlayers)
        {
            player->SetWinner();
        }
        m_countDown -= deltaTime;
    }
}

GameModeType GameModeTeamBrawl::GetType() const
{
    return GameModeType::TeamBrawl;
}

bool GameModeTeamBrawl::Finished() const
{
    return m_countDown <= 0.0f;
}

bool GameModeTeamBrawl::IsValid() const
{
    return m_valid;
}
