#pragma once
#include <memory>
#include <vector>

class b2World;

namespace Gameplay
{
class Avatar;
class Asset;

class Player
{
  public:
    Player(b2World *world);
    ~Player();

    void Update(float deltaTime);

    void SetInputs(float sledgeInput, float moveInput, float jumpInput);

    std::vector<std::shared_ptr<Asset>> GetAssets() const;

    std::atomic<bool> m_pendingRemove;

  private:
    std::shared_ptr<Avatar> m_avatar;
    float m_sledgeInput;
    float m_moveInput;
    float m_jumpInput;
    float m_respawnTimer;
};

} // namespace Gameplay
