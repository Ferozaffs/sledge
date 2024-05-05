#pragma once
#include "Weapon.h"

class b2Body;
class b2Joint;

namespace Gameplay
{
	class Player;

	class Sledge : public Weapon
	{
	public:
		Sledge(const Player* player);
		virtual ~Sledge();

	private:
		b2Body* m_sledgeHead;
		b2Joint* m_weld;
	};

}

