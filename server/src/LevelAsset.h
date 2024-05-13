#pragma once
#include <memory>
#include <guiddef.h>

class b2World;
class b2Body;

namespace Gameplay
{
	class LevelAsset 
	{
	public:
		LevelAsset(b2World* world, int x, int y);
		~LevelAsset();

		const GUID GetId() const;

		const float GetX() const;
		const float GetY() const;

	private:
		GUID m_id;
		b2Body* m_body;
	};

}

