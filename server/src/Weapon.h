#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class b2Body;
class b2Joint;
class b2World;

namespace Gameplay
{
class Avatar;
class Asset;

class Weapon
{
    struct WeaponShape
    {
        float sizeX;
        float sizeY;
        float offsetX;
        float offsetY;
        float anchorX;
        float anchorY;
        std::string anchorType;
        float density;
        float friction;
        bool canDamage;
        std::string texture;
        unsigned int tint;
    };
    struct WeaponData
    {
        float speed;
        float torque;
        std::vector<WeaponShape> shapes;
    };

  public:
    Weapon(const std::string &jsonFile, std::weak_ptr<b2World> world, const Avatar &avatar);
    ~Weapon();

    void Update(float deltaTime);

    float GetSpeed() const;
    float GetTorque() const;

    std::vector<std::shared_ptr<Asset>> GetAssets() const;

    void BreakJoints();

  private:
    WeaponData LoadWeaponFromJson(const std::string &jsonFile);
    void CreateWeapon(const WeaponData &wpnData, std::weak_ptr<b2World> world, const Avatar &avatar);

    std::vector<std::shared_ptr<Asset>> m_assets;
    std::vector<b2Joint *> m_joints;
    float m_speed;
    float m_torque;

    static std::unordered_map<std::string, WeaponData> WeaponCache;
};

} // namespace Gameplay
