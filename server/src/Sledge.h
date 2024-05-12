#pragma once
#include "Weapon.h"

class b2Body;
class b2Joint;

namespace Gameplay
{
	class Avatar;

	class Sledge : public Weapon
	{
	public:
		Sledge(const Avatar* avatar);
		virtual ~Sledge();

	private:
		b2Body* m_sledgeHead;
		b2Joint* m_weld;
	};

}

