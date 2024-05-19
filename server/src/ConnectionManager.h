#pragma once
#include <guiddef.h>
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

    static void RemoveAsset(GUID id);

  private:
    friend class Impl;

    static std::string CreateStatusMessage(std::string message);
    static std::string CreateErrorMessage(std::string message);

    void SendAssets(const std::shared_ptr<Gameplay::Player> &player,
                    std::vector<std::shared_ptr<Gameplay::Asset>> assets);

    void RemoveAssets();

    static std::unique_ptr<Impl> m_impl;
    Gameplay::PlayerManager *m_playerManager;
    Gameplay::LevelManager *m_levelManager;

    float m_tickCounter;

    static std::vector<GUID> m_assetsToRemove;
};

} // namespace Network
