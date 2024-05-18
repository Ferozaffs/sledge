#pragma once
#include <memory>

class b2World;

namespace Gameplay
{
class Asset;

class LevelBlock
{
  public:
    LevelBlock(b2World *world, int x, int y);
    ~LevelBlock();

    const std::shared_ptr<Asset> &GetAsset() const;
    bool InMotion();

  private:
    std::shared_ptr<Asset> m_asset;
};

} // namespace Gameplay
