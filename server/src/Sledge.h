#pragma once
#include "Weapon.h"
#include <memory>

class b2World;

namespace Gameplay
{
class Avatar;
class Asset;

class Sledge : public Weapon
{
  public:
    Sledge(std::weak_ptr<b2World> world, const Avatar& avatar);

  private:
    std::shared_ptr<Asset> m_sledgeHeadAsset;
};

} // namespace Gameplay
