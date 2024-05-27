#pragma once
#include <memory>

namespace Gameplay
{
class LevelManager;
class PlayerManager;
class IGameMode;

enum class GameModeType
{
    Sandbox,
    Brawl,
    TeamBrawl,
    Race,
    KotH,

    None,
};

class GameManager
{
  public:
    GameManager(PlayerManager *playerManager);
    ~GameManager();

    void Update(float deltaTime);

    bool Finished() const;

    GameModeType GetCurrentGameMode() const;
    void SetGameMode(GameModeType type);

  private:
    std::unique_ptr<IGameMode> m_currentGameMode;
    PlayerManager *m_playerManager;
};

} // namespace Gameplay
