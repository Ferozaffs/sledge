#pragma once
#include "Weapon.h"
#include <memory>

class b2World;

namespace Gameplay
{
class Avatar;
class Asset;

class Sword : public Weapon
{
  public:
    Sword(std::weak_ptr<b2World> world, const Avatar &avatar);

  private:
    std::shared_ptr<Asset> m_hiltAsset;
    std::shared_ptr<Asset> m_edgeAsset;
};

} // namespace Gameplay
