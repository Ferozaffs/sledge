#include "GameMode.h"
#include "GameManager.h"
#include "Player.h"
#include "PlayerManager.h"

using namespace Gameplay;

GameMode::GameMode(PlayerManager &playerManager, const GameModeConfiguration &configuration)
    : m_playerManager(playerManager), m_configuration(configuration), m_redPoints(0), m_bluePoints(0),
      m_pointsReached(false), m_countDown(5.0f)
{
    if (m_configuration.teams)
    {
        m_previousNumPlayersTeamRedAlive = m_playerManager.GetRedPlayers().size();
        m_previousNumPlayersTeamBlueAlive = m_playerManager.GetBluePlayers().size();
        if (m_previousNumPlayersTeamRedAlive + m_previousNumPlayersTeamBlueAlive <= 1)
        {
            m_valid = false;
        }
    }
    else
    {
        m_previousNumPlayersAlive = m_playerManager.GetRedPlayers().size();
        if (m_previousNumPlayersAlive <= 1)
        {
            m_valid = false;
        }
    }

    auto players = m_playerManager.GetPlayersDead();
    for (const auto &player : players)
    {
        player->SetTeamColors(m_configuration.teams);
        player->Respawn(0.0);
    }
}

GameMode::~GameMode()
{
}

void GameMode::Update(float deltaTime)
{
    UpdateRespawn(deltaTime);
    UpdatePoints(deltaTime);
    UpdateWinCondition(deltaTime);
}

void GameMode::UpdateRespawn(float deltaTime)
{
    if (m_configuration.respawnsEnabled)
    {
        auto players = m_playerManager.GetPlayersDead();
        for (const auto &player : players)
        {
            player->Respawn(m_configuration.respawnTime);
        }
    }
}

void GameMode::UpdatePoints(float deltaTime)
{
    if (m_configuration.scoringType == ScoringType::Deaths)
    {
        UpdateDeathPoints(deltaTime);
    }
    else if (m_configuration.scoringType == ScoringType::Objectives)
    {
        UpdateZoneObjectivePoints(deltaTime);
        UpdateDestructionObjectivePoints(deltaTime);
    }
}

void GameMode::UpdateDeathPoints(float deltaTime)
{
    if (m_configuration.teams)
    {
        size_t numRedPlayersAlive = 0;
        for (auto &player : m_playerManager.GetRedPlayers())
        {
            if (player->IsDead() == false)
            {
                ++numRedPlayersAlive;
            }
        }
        size_t numBluePlayersAlive = 0;
        for (auto &player : m_playerManager.GetBluePlayers())
        {
            if (player->IsDead() == false)
            {
                ++numBluePlayersAlive;
            }
        }

        if (numRedPlayersAlive < m_previousNumPlayersTeamRedAlive)
        {
            m_bluePoints++;
        }
        if (numBluePlayersAlive < m_previousNumPlayersTeamBlueAlive)
        {
            m_redPoints++;
        }

        m_previousNumPlayersTeamRedAlive = numRedPlayersAlive;
        m_previousNumPlayersTeamBlueAlive = numBluePlayersAlive;
    }
    else
    {
        const auto &playersAlive = m_playerManager.GetPlayersAlive();

        if (playersAlive.size() < m_previousNumPlayersAlive)
        {
            for (const auto &player : playersAlive)
            {
                m_playerPoints[player]++;
            }
        }

        m_previousNumPlayersAlive = playersAlive.size();
    }
}

void GameMode::UpdateZoneObjectivePoints(float deltaTime)
{
}

void GameMode::UpdateDestructionObjectivePoints(float deltaTime)
{
}

void GameMode::UpdateWinCondition(float deltaTime)
{
    if (m_pointsReached == false)
    {
        int numRedPlayersAlive = 0;
        for (auto &player : m_playerManager.GetRedPlayers())
        {
            if (player->IsDead() == false)
            {
                ++numRedPlayersAlive;
            }
        }
        int numBluePlayersAlive = 0;
        for (auto &player : m_playerManager.GetBluePlayers())
        {
            if (player->IsDead() == false)
            {
                ++numBluePlayersAlive;
            }
        }

        if (m_configuration.teams)
        {
            if (m_redPoints >= m_configuration.pointsToWin ||
                (m_configuration.scoringType == ScoringType::LastStanding && numBluePlayersAlive == 0))
            {
                for (auto &player : m_playerManager.GetRedPlayers())
                {
                    player->Score(1);
                    player->SetWinner();
                }

                m_pointsReached = true;
            }
            else if (m_bluePoints >= m_configuration.pointsToWin ||
                     (m_configuration.scoringType == ScoringType::LastStanding && numRedPlayersAlive == 0))
            {
                for (auto &player : m_playerManager.GetBluePlayers())
                {
                    player->Score(1);
                    player->SetWinner();
                }
                m_pointsReached = true;
            }
        }
        else
        {
            for (const auto &player : m_playerManager.GetPlayers())
            {
                if (m_playerPoints[player] > m_configuration.pointsToWin ||
                    (m_configuration.scoringType == ScoringType::LastStanding &&
                     m_playerManager.GetPlayersAlive().size() == 1))
                {
                    player->Score(1);
                    player->SetWinner();
                    m_pointsReached = true;
                }
            }
        }
    }
    else
    {
        m_countDown -= deltaTime;
    }
}

GameModeType GameMode::GetType() const
{
    return GameModeType::Custom;
}

bool GameMode::Finished() const
{
    return m_countDown <= 0.0f;
}

bool GameMode::IsValid() const
{
    return m_valid;
}
