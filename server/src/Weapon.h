#pragma once
#include <memory>
#include <vector>

class b2Body;
class b2Joint;

namespace Gameplay
{
class Avatar;
class Asset;

class Weapon
{
  public:
    Weapon();
    virtual ~Weapon();

    virtual void Update(float /*deltaTime*/)
    {
    }

    float GetSpeed() const;
    float GetTorque() const;

    std::vector<std::shared_ptr<Asset>> GetAssets() const;

    void BreakJoints();

  protected:
    std::vector<std::shared_ptr<Asset>> m_assets;
    std::vector<b2Joint *> m_joints;
    float m_speed;
    float m_torque;
};

} // namespace Gameplay
