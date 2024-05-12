#pragma once
#include "Weapon.h"

class b2Body;
class b2Joint;

namespace Gameplay
{
	class Avatar;

	class Sword : public Weapon
	{
	public:
		Sword(const Avatar* player);
		virtual ~Sword();

	private:
		b2Body* m_hilt;
		b2Joint* m_weld;
	};

}

