#include "GameManager.h"
#include "GameModeBrawl.h"
#include "GameModeSandbox.h"
#include "LevelManager.h"

using namespace Gameplay;

GameManager::GameManager(PlayerManager *playerManager) : m_playerManager(playerManager)
{
}

GameManager::~GameManager()
{
}

void GameManager::Update(float deltaTime)
{
    if (m_currentGameMode != nullptr)
    {
        m_currentGameMode->Update(deltaTime);
    }
}

bool Gameplay::GameManager::Finished() const
{
    return m_currentGameMode == nullptr || m_currentGameMode->Finished();
}

void GameManager::SetGameMode(GameModeType type)
{
    if (type == GameModeType::Brawl)
    {
        m_currentGameMode = std::make_unique<GameModeBrawl>(m_playerManager);
        if (m_currentGameMode->IsValid() == false)
        {
            m_currentGameMode = std::make_unique<GameModeSandbox>(m_playerManager);
        }
    }
}
