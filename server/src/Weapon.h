#pragma once
#include <memory>
#include <string>
#include <vector>

class b2Body;
class b2Joint;
class b2World;
class b2Vec2;

namespace Gameplay
{
class Avatar;
class Asset;

class Weapon
{
    struct WeaponShape
    {
        unsigned int links;
        float sizeX;
        float sizeY;
        float offsetX;
        float offsetY;
        float anchorX;
        float anchorY;
        float anchorLimit;
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
        int rarity;
        std::vector<WeaponShape> shapes;
    };

  public:
    Weapon(const WeaponData &weaponData, std::weak_ptr<b2World> world, const Avatar &avatar);
    ~Weapon();

    static std::unique_ptr<Weapon> GetWeaponFromPool(std::weak_ptr<b2World> world, const Avatar &avatar);

    void Update(float deltaTime);

    float GetSpeed() const;
    float GetTorque() const;

    std::vector<std::shared_ptr<Asset>> GetAssets() const;

    void BreakJoints();

  private:
    static void LoadWeapons();
    static WeaponData LoadWeaponFromJson(const std::string &jsonFile);
    static bool SortRarity(std::pair<std::string, Weapon::WeaponData> &a,
                           std::pair<std::string, Weapon::WeaponData> &b);

    void CreateWeapon(const WeaponData &wpnData, std::weak_ptr<b2World> world, const Avatar &avatar);
    void CreateShape(const WeaponShape &shape, std::weak_ptr<b2World> world, const Avatar &avatar);
    std::shared_ptr<Asset> CreateBody(const WeaponShape &shape, const b2Vec2 &pos, std::shared_ptr<b2World> world);
    void CreateAnchor(const WeaponShape &shape, const b2Vec2 &pos, std::shared_ptr<b2World> world, b2Body *connection,
                      const std::shared_ptr<Asset> &asset);

    std::vector<std::shared_ptr<Asset>> m_assets;
    std::vector<b2Joint *> m_joints;
    float m_speed;
    float m_torque;

    static std::vector<std::pair<std::string, WeaponData>> WeaponCache;
};

} // namespace Gameplay
