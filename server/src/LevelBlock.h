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
    LevelBlock(std::weak_ptr<b2World> world, int x, int y, const std::string &alias);
    ~LevelBlock() = default;

    bool Update(float deltaTime);

    std::weak_ptr<Asset> GetAsset() const;
    bool InMotion() const;

  private:
    void ConvertToDynamic();

    std::shared_ptr<Asset> m_asset;
    BlockType m_type;
    float m_health;
};

} // namespace Gameplay
