#pragma once
#include "WeaponTypes.h"

#include <memory>
#include <box2d/box2d.h>

namespace Gameplay
{
	class Weapon;

	class Player
	{
	public:
		Player(const std::shared_ptr<b2World>& world, const b2Vec2& spawnPos);
		~Player();

		void AssignWeapon(WeaponType type);

		void Update(const float& deltaTime);

		b2Body* GetBody() const;
		const b2Vec2& GetPosition() const;

	private:
		b2Body* m_body;

		std::shared_ptr<Weapon> m_weapon;
		b2RevoluteJoint* m_weaponJoint;
	};

}

