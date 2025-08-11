#pragma once
#include "GameSettings.h"
#include <functional>
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
    ~LevelBlock();

    bool Update(float deltaTime);
    void Reset();

    std::weak_ptr<Asset> GetAsset() const;
    bool InMotion() const;

    void OnContact(b2Body *otherBody, b2Fixture *otherFixture, bool contact);

    unsigned int GetCode() const;
    bool HasCollision();

    void SetDestoryCallback(std::function<void(LevelBlock *)> callback)
    {
        m_destroyCallback = std::move(callback);
    }

  private:
    void ConvertToDynamic();

    int m_originalX;
    int m_originalY;

    std::unordered_set<b2Body *> m_contacts;
    std::shared_ptr<Asset> m_asset;
    BlockConfiguration m_configuration;
    float m_health;

    std::function<void(LevelBlock *)> m_destroyCallback;
};

} // namespace Gameplay
