#pragma once
#include "WeaponTypes.h"

#include <box2d/box2d.h>
#include <memory>
#include <vector>

namespace Gameplay
{
class Weapon;
class Asset;

class Avatar
{
  public:
    Avatar(b2World *world, const b2Vec2 &spawnPos, unsigned int tint = 0xFFFFFF);
    ~Avatar();

    void AssignWeapon(WeaponType type);

    void Update(const float &deltaTime, const float &sledgeInput, const float &jumpInput, const float &moveInput);

    b2Body *GetBody() const;
    const b2Vec2 &GetPosition() const;
    const float GetX() const;
    const float GetY() const;
    const float GetWeaponRot() const;

    std::vector<std::shared_ptr<Asset>> GetAssets() const;

    void Kill();
    bool IsDead() const;

  private:
    b2Body *GetHead() const;
    void BreakJoints();

    float m_spawnInvincibility;
    bool m_dead;

    std::shared_ptr<Asset> m_bodyAsset;
    std::shared_ptr<Asset> m_headAsset;
    b2Joint *m_headJoint;
    std::shared_ptr<Weapon> m_weapon;
    b2RevoluteJoint *m_weaponJoint;
};

} // namespace Gameplay
