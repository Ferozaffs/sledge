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
    Avatar(std::weak_ptr<b2World> world, const b2Vec2 &spawnPos, unsigned int tint = 0xFFFFFF, bool winner = false);
    ~Avatar();

    void AssignWeapon(WeaponType type);

    void Update(const float &deltaTime, const float &sledgeInput, const float &jumpInput, const float &moveInput);

    unsigned int GetBodyId() const;
    const b2Vec2 &GetPosition() const;
    const float GetX() const;
    const float GetY() const;
    const float GetWeaponRot() const;

    std::vector<std::weak_ptr<Asset>> GetAssets() const;

    void Kill();
    bool IsDead() const;

  private:
    b2Body *GetBody() const;
    b2Body *GetHead() const;
    b2Body *GetLegs() const;
    void BreakJoints();
    void BreakCrown();
    void BreakHelm();

    float m_invincibilityTimer;
    unsigned int m_health;
    bool m_dead;

    std::weak_ptr<b2World> m_world;
    std::shared_ptr<Asset> m_bodyAsset;
    std::shared_ptr<Asset> m_headAsset[3];
    std::shared_ptr<Asset> m_crownAsset;
    std::shared_ptr<Asset> m_legsAsset;
    b2Joint *m_headJoint;
    b2Joint *m_crownJoint;
    b2Joint *m_legsJoint;
    std::unique_ptr<Weapon> m_weapon;
    b2RevoluteJoint *m_weaponJoint;
};

} // namespace Gameplay
