#include "GameModeSandbox.h"
#include "GameManager.h"
#include "Player.h"
#include "PlayerManager.h"

using namespace Gameplay;

GameModeSandbox::GameModeSandbox(PlayerManager *playerManager) : m_playerManager(playerManager)
{
    auto players = m_playerManager->GetPlayersDead();
    for (const auto &player : players)
    {
        player->SetTeamColors(false);
        player->Respawn(0.0);
    }
}

GameModeSandbox::~GameModeSandbox()
{
}

void GameModeSandbox::Update(float deltaTime)
{
    auto players = m_playerManager->GetPlayersDead();
    for (const auto &player : players)
    {
        player->Respawn(3.0f);
    }
}

GameModeType GameModeSandbox::GetType() const
{
    return GameModeType::Sandbox;
}

bool GameModeSandbox::Finished() const
{
    return false;
}

bool GameModeSandbox::IsValid() const
{
    return true;
}
