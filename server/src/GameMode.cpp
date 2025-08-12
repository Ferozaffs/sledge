#include "GameMode.h"
#include "Asset.h"
#include "GameManager.h"
#include "Level.h"
#include "LevelBlock.h"
#include "Player.h"
#include "PlayerManager.h"

using namespace Gameplay;

GameMode::GameMode(PlayerManager &playerManager, const GameModeConfiguration &configuration,
                   const std::weak_ptr<Level> level)
    : m_playerManager(playerManager), m_configuration(configuration), m_level(level), m_redPoints(0.0f),
      m_bluePoints(0.0f), m_pointsReached(false), m_countDown(5.0f), m_valid(true), m_previousNumPlayersAlive(0),
      m_previousNumPlayersTeamBlueAlive(0), m_previousNumPlayersTeamRedAlive(0)
{
    if (m_configuration.scoringType == ScoringType::LastStanding)
    {
        m_redPoints = 0.0f;
        m_bluePoints = 0.0f;
    }

    m_configuration.pointsToWin = m_configuration.pointsToWin * m_playerManager.GetNumPlayers();

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
        m_previousNumPlayersAlive = m_playerManager.GetNumPlayers();
        if (m_previousNumPlayersAlive <= 1)
        {
            m_valid = false;
        }
    }

    auto players = m_playerManager.GetPlayers();
    for (const auto &player : players)
    {
        player->Respawn(0.0);
        if (m_configuration.scoringType == ScoringType::LastStanding)
        {
            m_playerPoints[player] = -1.0f;
        }
    }

    if (auto l = m_level.lock())
    {
        for (const auto &objective : m_configuration.scoreObjectives)
        {
            if (objective.objectiveType == ObjectiveType::Destruction)
            {
                m_destructionObjectives.emplace_back(objective);
                for (auto &block : l->GetBlocks(objective.objectCode))
                {
                    block->SetDestoryCallback([this](LevelBlock *block) { this->OnObjectiveDestroyed(block); });
                }
            }
            else
            {
                ZoneData zoneData;
                zoneData.pointTimer = objective.scoreTickRate;
                for (auto &block : l->GetBlocks(objective.objectCode))
                {
                    block->SetVisibility(true);
                    if (auto a = block->GetAsset().lock())
                    {
                        zoneData.minX = std::min(zoneData.minX, a->GetX());
                        zoneData.minY = std::min(zoneData.minY, a->GetY());
                        zoneData.maxX = std::max(zoneData.maxX, a->GetX());
                        zoneData.maxY = std::max(zoneData.maxY, a->GetY());
                    }
                }
                m_zoneObjectives.emplace_back(objective, zoneData);
            }
        }
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
            m_bluePoints += 1.0f;
        }
        if (numBluePlayersAlive < m_previousNumPlayersTeamBlueAlive)
        {
            m_redPoints += 1.0f;
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
                m_playerPoints[player] += 1.0f;
            }
        }

        m_previousNumPlayersAlive = playersAlive.size();
    }
}

void GameMode::UpdateZoneObjectivePoints(float deltaTime)
{
    for (auto &[objective, zone] : m_zoneObjectives)
    {
        bool zoneOccupied = false;

        bool scoreRed = false;
        bool scoreBlue = false;
        std::vector<std::shared_ptr<Player>> playersToScore;
        if (objective.triggerType == TriggerType::Player)
        {
            if (m_configuration.teams)
            {
                for (auto &player : m_playerManager.GetRedPlayers())
                {
                    if (player->IsDead() == false && zone.IsWithin(player->GetX(), player->GetY()))
                    {
                        scoreRed = true;
                        zoneOccupied = true;
                    }
                }
                for (auto &player : m_playerManager.GetBluePlayers())
                {
                    if (player->IsDead() == false && zone.IsWithin(player->GetX(), player->GetY()))
                    {
                        scoreBlue = true;
                        zoneOccupied = true;
                    }
                }
            }
            else
            {
                for (const auto &player : m_playerManager.GetPlayers())
                {
                    if (player->IsDead() == false && zone.IsWithin(player->GetX(), player->GetY()))
                    {
                        playersToScore.emplace_back(player);
                        zoneOccupied = true;
                    }
                }
            }
        }
        else
        {
            if (auto l = m_level.lock())
            {
                for (auto &block : l->GetBlocks(objective.triggerCode))
                {
                    if (auto a = block->GetAsset().lock())
                    {
                        if (zone.IsWithin(a->GetX(), a->GetY()))
                        {
                            zoneOccupied = true;
                            if (objective.affectedPlayers == AffectedPlayers::Red)
                            {
                                scoreRed = true;
                            }
                            else if (objective.affectedPlayers == AffectedPlayers::Blue)
                            {
                                scoreBlue = true;
                            }

                            if (objective.resetObject)
                            {
                                block->Reset();
                            }
                        }
                    }
                }
            }
        }

        if (zoneOccupied)
        {
            zone.pointTimer -= deltaTime;
            if (zone.pointTimer < 0.0f)
            {
                zone.pointTimer = objective.scoreTickRate;

                if (scoreRed)
                {
                    m_redPoints += 1.0f;
                }
                if (scoreBlue)
                {
                    m_bluePoints += 1.0f;
                }

                for (const auto &player : playersToScore)
                {
                    m_playerPoints[player] += 1.0f;
                }
            }
        }
        else
        {
            zone.pointTimer = objective.scoreTickRate;
        }
    }
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

void GameMode::OnObjectiveDestroyed(LevelBlock *block)
{
    for (const auto &objective : m_destructionObjectives)
    {
        if (objective.objectCode == block->GetCode())
        {
            if (objective.affectedPlayers == AffectedPlayers::Blue)
            {
                m_bluePoints++;
            }
            if (objective.affectedPlayers == AffectedPlayers::Red)
            {
                m_redPoints++;
            }
        }
    }
}

std::unordered_map<int, float> GameMode::GetPointsMap() const
{

    std::unordered_map<int, float> pointsMap;

    if (m_configuration.scoringType == ScoringType::LastStanding)
    {
        for (const auto &player : m_playerManager.GetPlayers())
        {
            pointsMap[player->GetMainAssetId()] = -1.0f;
        }
    }
    else
    {
        if (m_configuration.teams)
        {
            for (const auto &player : m_playerManager.GetPlayers())
            {
                pointsMap[player->GetMainAssetId()] =
                    (player->GetTeam() == Team::Red ? m_redPoints : m_bluePoints) / m_configuration.pointsToWin;
            }
        }
        else
        {
            for (const auto &player : m_playerManager.GetPlayers())
            {
                if (m_playerPoints.find(player) != m_playerPoints.end())
                {
                    pointsMap[player->GetMainAssetId()] = m_playerPoints.at(player) / m_configuration.pointsToWin;
                }
            }
        }
    }

    return pointsMap;
}

GameModeType GameMode::GetType() const
{
    return GameModeType::Custom;
}

signed int GameMode::GetPoints() const
{
    float playerPoints = 0.0f;
    for (const auto &points : m_playerPoints)
    {
        playerPoints += points.second;
    }

    return static_cast<unsigned int>(m_redPoints + m_bluePoints + playerPoints);
}

bool GameMode::Finished() const
{
    return m_countDown <= 0.0f;
}

bool GameMode::IsValid() const
{
    return m_valid;
}
