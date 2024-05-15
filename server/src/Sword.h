#pragma once
#include "Weapon.h"
#include <memory>

class b2Body;
class b2Joint;

namespace Gameplay
{
	class Avatar;
	class Asset;

	class Sword : public Weapon
	{
	public:
		Sword(const Avatar* player);
		virtual ~Sword();

	private:
		std::shared_ptr<Asset> m_hiltAsset;
		b2Joint* m_weld;
	};

}

