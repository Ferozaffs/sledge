#pragma once
#include <memory>
#include <string>

class b2World;

namespace Gameplay
{
class Asset;

class LevelBlock
{
    enum class BlockType
    {
        Static,
        Tough,
        Weak,
        Decor
    };

  public:
    LevelBlock(b2World *world, int x, int y, std::string alias);
    ~LevelBlock();

    bool Update(float deltaTime);

    const std::shared_ptr<Asset> &GetAsset() const;
    bool InMotion();

  private:
    void ConvertToDynamic();

    std::shared_ptr<Asset> m_asset;
    BlockType m_type;
    float m_health;
};

} // namespace Gameplay
