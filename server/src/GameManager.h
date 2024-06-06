#pragma once
#include "LevelManager.h"
#include "PlayerManager.h"
#include <memory>
#include <unordered_map>

namespace Gameplay
{
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
    GameManager(std::weak_ptr<b2World> world);
    ~GameManager();

    void Update(float deltaTime);

    bool Finished() const;

    std::pair<int, int> GetOptimalSpawn() const;

    std::vector<std::weak_ptr<Asset>> GetAssets(bool allAssets = false);

    int GetScore() const;
    std::unordered_map<int, int> GetScoreMap() const;

    GameModeType GetCurrentGameMode() const;
    void SetGameMode(GameModeType type);

    std::weak_ptr<Player> AddPlayer();

    void NextLevelWish(GameModeType mode);

  private:
    std::unique_ptr<IGameMode> m_currentGameMode;
    PlayerManager m_playerManager;
    LevelManager m_levelManager;
};

} // namespace Gameplay
