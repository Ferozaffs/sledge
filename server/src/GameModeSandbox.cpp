#include "GameModeSandbox.h"
#include "Player.h"
#include "PlayerManager.h"

using namespace Gameplay;

GameModeSandbox::GameModeSandbox(PlayerManager *playerManager) : m_playerManager(playerManager)
{
    auto players = m_playerManager->GetPlayersDead();
    for (const auto &player : players)
    {
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

bool GameModeSandbox::Finished()
{
    return false;
}

bool GameModeSandbox::IsValid()
{
    return true;
}
