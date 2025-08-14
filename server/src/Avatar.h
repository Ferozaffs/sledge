#pragma once
#include "B2Filters.h"
#include "GameSettings.h"
#include <boost/uuid/uuid.hpp>
#include <box2d/box2d.h>
#include <memory>
#include <vector>

namespace Gameplay
{
struct GameSettings;
class Weapon;
class Asset;

class Avatar
{
  public:
    Avatar(std::weak_ptr<b2World> world, const b2Vec2 &spawnPos, unsigned int tint, unsigned int teamTint,
           bool winner = false);
    ~Avatar();

    void Update(const float &deltaTime, const float &sledgeInput, const float &jumpInput, const float &moveInput);
    void UpdateSettings(const GameModeConfiguration &configuration);

    unsigned int GetBodyId() const;
    signed int GetKillerId() const;
    const b2Vec2 &GetPosition() const;
    const float GetX() const;
    const float GetY() const;
    const float GetWeaponRot() const;

    b2Body *GetShaft() const;
    float GetShaftLength() const;

    std::vector<std::weak_ptr<Asset>> GetAssets() const;

    void Kill();
    bool IsDead() const;

    void SetKillerId(signed int opponentId);

  private:
    b2Body *GetBody() const;
    b2Body *GetHead() const;
    b2Body *GetLegs() const;
    void BreakJoints();
    void BreakCrown();
    void BreakHelm();
    void AssignWeapon();

    signed int m_killerId;

    bool m_invincibility;
    float m_invincibilityTimer;
    unsigned int m_health;
    bool m_dead;

    std::weak_ptr<b2World> m_world;
    std::shared_ptr<Asset> m_bodyAsset;
    std::shared_ptr<Asset> m_headAsset[3];
    std::shared_ptr<Asset> m_crownAsset;
    std::shared_ptr<Asset> m_legsAsset;
    std::shared_ptr<Asset> m_shaftAsset;
    b2Joint *m_headJoint;
    b2Joint *m_crownJoint;
    b2Joint *m_legsJoint;
    b2RevoluteJoint *m_shaftJoint;
    std::unique_ptr<Weapon> m_weapon;

    PlayerControl m_groundControl;
    PlayerControl m_airControl;
    float m_controlModifier;

    PhysicsObjectUserData m_userData;
};

} // namespace Gameplay
