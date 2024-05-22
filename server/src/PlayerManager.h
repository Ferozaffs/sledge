#pragma once
#include <memory>
#include <vector>

class b2World;

namespace Gameplay
{
class Player;
class Asset;
class LevelManager;

class PlayerManager
{
  public:
    PlayerManager(LevelManager *levelManager, b2World *world);
    ~PlayerManager();

    void Update(float deltaTime);

    const std::shared_ptr<Player> &CreatePlayer();
    std::shared_ptr<Player> GetPlayer(size_t index);

    std::vector<std::shared_ptr<Asset>> GetAssets();

    std::pair<int, int> GetOptimalSpawn() const;

  private:
    LevelManager *m_levelManager;
    b2World *m_world;
    std::vector<std::shared_ptr<Player>> m_players;
    unsigned int m_playersSpawned;
    float m_restartTimer;
};

} // namespace Gameplay
