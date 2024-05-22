#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Gameplay
{
class PlayerManager;
class Player;
class Asset;
class LevelManager;
} // namespace Gameplay

namespace Network
{
class Impl;

class ConnectionManager
{
  public:
    ConnectionManager(Gameplay::PlayerManager *playerManager, Gameplay::LevelManager *levelManager);
    ~ConnectionManager();

    void Update(float deltaTime);

    static void RemoveAsset(int id);

  private:
    friend class Impl;

    static std::string CreateStatusMessage(std::string message);
    static std::string CreateErrorMessage(std::string message);

    void SendAssets(std::vector<std::shared_ptr<Gameplay::Asset>> assets, bool playerJoined);

    void RemoveAssets();

    static std::unique_ptr<Impl> m_impl;
    Gameplay::PlayerManager *m_playerManager;
    Gameplay::LevelManager *m_levelManager;

    float m_tickCounter;

    static std::vector<int> m_assetsToRemove;
};

} // namespace Network
