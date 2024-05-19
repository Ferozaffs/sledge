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
    PlayerManager(LevelManager *levelManager, const std::shared_ptr<b2World> &b2World);
    ~PlayerManager();

    void Update(float deltaTime);

    const std::shared_ptr<Player> &CreatePlayer();
    std::shared_ptr<Player> GetPlayer(size_t index);

    std::vector<std::shared_ptr<Asset>> GetDynamicAssets() const;

    std::pair<int, int> GetOptimalSpawn() const;

  private:
    LevelManager *m_levelManager;
    const std::shared_ptr<b2World> &m_b2World;
    std::vector<std::shared_ptr<Player>> m_players;
    float m_restartTimer;
};

} // namespace Gameplay
