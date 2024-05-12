#pragma once
#include "WeaponTypes.h"

#include <memory>
#include <box2d/box2d.h>

namespace Gameplay
{
	class Weapon;

	class Avatar
	{
	public:
		Avatar(const std::shared_ptr<b2World>& world, const b2Vec2& spawnPos);
		~Avatar();

		void AssignWeapon(WeaponType type);

		void Update(const float& deltaTime, const float& debugSledgeInput, const float& debugJumpInput, const float& debugMoveInput);

		b2Body* GetBody() const;
		const b2Vec2& GetPosition() const;

	private:
		b2Body* m_body;

		std::shared_ptr<Weapon> m_weapon;
		b2RevoluteJoint* m_weaponJoint;
	};

}

