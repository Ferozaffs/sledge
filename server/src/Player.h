#pragma once
#include <memory>
#include <vector>

class b2World;

namespace Gameplay
{
class Avatar;
class Asset;
class PlayerManager;

class Player
{
  public:
    Player(PlayerManager *playerManager, b2World *world, unsigned int tint);
    ~Player();

    const float GetX() const;
    const float GetY() const;

    void Update(float deltaTime);
    void Respawn();

    void SetInputs(float sledgeInput, float moveInput, float jumpInput);

    std::vector<std::shared_ptr<Asset>> GetAssets() const;

    std::atomic<bool> m_pendingRemove;

    bool IsWishingToRestart() const;

  private:
    void SpawnAvatar(b2World* world);

    PlayerManager *m_playerManager;
    std::shared_ptr<Avatar> m_avatar;
    float m_sledgeInput;
    float m_moveInput;
    float m_jumpInput;
    float m_respawnTimer;
    unsigned int m_tint;

    bool m_wishToRestart;
};

} // namespace Gameplay
