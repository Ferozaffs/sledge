#include "GameModeSandbox.h"
#include "GameManager.h"
#include "Player.h"
#include "PlayerManager.h"

using namespace Gameplay;

GameModeSandbox::GameModeSandbox(PlayerManager &playerManager) : m_playerManager(playerManager)
{
    auto players = m_playerManager.GetPlayersDead();
    for (const auto &player : players)
    {
        player->SetTeamColors(false);
        player->Respawn(0.0);
    }
}

void GameModeSandbox::Update(float deltaTime)
{
    auto players = m_playerManager.GetPlayersDead();
    for (const auto &player : players)
    {
        player->Respawn(3.0f);
    }
}

GameModeType GameModeSandbox::GetType() const
{
    return GameModeType::Sandbox;
}

signed int GameModeSandbox::GetPoints() const
{
    return -1;
}

std::unordered_map<int, float> Gameplay::GameModeSandbox::GetPointsMap() const
{
    return std::unordered_map<int, float>();
}

bool GameModeSandbox::Finished() const
{
    return false;
}

bool GameModeSandbox::IsValid() const
{
    return true;
}
