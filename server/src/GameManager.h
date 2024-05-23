#pragma once
#include <memory>

namespace Gameplay
{
class LevelManager;
class PlayerManager;
class IGameMode;

class GameManager
{
  public:
    enum class GameModeType
    {
        Sandbox,
        Brawl,
        Race,
        KotH,
    };

    GameManager(PlayerManager *playerManager);
    ~GameManager();

    void Update(float deltaTime);

    bool Finished() const;

    void SetGameMode(GameModeType type);

  private:
    std::unique_ptr<IGameMode> m_currentGameMode;
    PlayerManager *m_playerManager;
};

} // namespace Gameplay
