#pragma once
#include "GameSettings.h"
#include <memory>
#include <string>
#include <unordered_set>

class b2World;
class b2Body;
class b2Fixture;

namespace Gameplay
{
class Asset;

class LevelBlock
{
  public:
    LevelBlock(std::weak_ptr<b2World> world, int x, int y, const BlockConfiguration &configuration);
    ~LevelBlock() = default;

    bool Update(float deltaTime);

    std::weak_ptr<Asset> GetAsset() const;
    bool InMotion() const;

    void OnContact(b2Body *otherBody, b2Fixture *otherFixture, bool contact);

  private:
    void ConvertToDynamic();

    std::unordered_set<b2Body *> m_contacts;
    std::shared_ptr<Asset> m_asset;
    BlockConfiguration m_configuration;
    float m_health;
};

} // namespace Gameplay
