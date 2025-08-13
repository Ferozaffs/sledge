#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace Gameplay
{
class GameManager;
class Asset;
} // namespace Gameplay

namespace Network
{
class ConnectionManager
{
  public:
    ConnectionManager(Gameplay::GameManager &gameManager);
    ~ConnectionManager();

    void Update(float deltaTime);

    static void RemoveAsset(int id);

  private:
    static std::mutex removalMutex;
    static std::vector<int> m_assetsToRemove;

  private:
    void SendAssets(std::vector<std::weak_ptr<Gameplay::Asset>> assets, bool playerJoined);
    void SendScore() const;
    void SendPoints() const;

    void RemoveAssets();

    Gameplay::GameManager &m_gameManager;

    float m_tickCounter;
    signed int m_cachedScore;
    signed int m_cachedPoints;
};

} // namespace Network
