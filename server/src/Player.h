#pragma once
#include <atomic>
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
    void Respawn(float time);

    void SetInputs(float sledgeInput, float moveInput, float jumpInput);

    int GetMainAssetId() const;
    std::vector<std::shared_ptr<Asset>> GetAssets() const;

    signed int GetScore() const;
    void Score(signed int score);
    void ClearScore();

    bool IsWishingToRestart() const;
    bool IsDead() const;

    std::atomic<bool> m_pendingRemove;

  private:
    void Respawn();
    void SpawnAvatar(b2World *world);

    PlayerManager *m_playerManager;
    b2World *m_world;
    std::shared_ptr<Avatar> m_avatar;
    float m_sledgeInput;
    float m_moveInput;
    float m_jumpInput;
    float m_respawnTimer;
    unsigned int m_tint;
    signed int m_score;

    bool m_wishToRestart;
};

} // namespace Gameplay
