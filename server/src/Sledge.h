#pragma once
#include "Weapon.h"
#include <memory>

class b2Body;
class b2Joint;

namespace Gameplay
{
	class Avatar;
	class Asset;

	class Sledge : public Weapon
	{
	public:
		Sledge(const Avatar* avatar);
		virtual ~Sledge();

	private:
		std::shared_ptr<Asset> m_sledgeHeadAsset;
		b2Joint* m_weld;
	};

}

