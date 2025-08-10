#pragma once
#include "LevelManager.h"
#include "PlayerManager.h"
#include <memory>
#include <unordered_map>

namespace Physics
{
class B2Manager;
};

namespace Gameplay
{
class IGameMode;

enum class GameModeWish
{
    None,
    Solo,
    Team,
};

enum class GameModeType
{
    Sandbox,
    Custom,
};

class GameManager
{
  public:
    GameManager(std::weak_ptr<b2World> world);
    ~GameManager();

    void Update(float deltaTime);

    bool Finished() const;

    std::pair<int, int> GetOptimalSpawn() const;

    std::vector<std::weak_ptr<Asset>> GetAssets(bool allAssets = false);

    int GetScore() const;
    std::unordered_map<int, int> GetScoreMap() const;

    GameModeWish GetCurrentGameModeWish() const;
    void SetGameMode(GameModeType type, const GameModeConfiguration &configuration);

    std::weak_ptr<Player> AddPlayer();

    void NextLevel(GameModeWish wish);

  private:
    std::unique_ptr<IGameMode> m_currentGameMode;
    GameModeWish m_currentGameModeWish;
    PlayerManager m_playerManager;
    LevelManager m_levelManager;
};

} // namespace Gameplay
