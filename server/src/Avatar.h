#pragma once
#include "WeaponTypes.h"

#include <memory>
#include <vector>
#include <box2d/box2d.h>

namespace Gameplay
{
	class Weapon;
	class Asset;

	class Avatar
	{
	public:
		Avatar(b2World* world, const b2Vec2& spawnPos);
		~Avatar();

		void AssignWeapon(WeaponType type);

		void Update(const float& deltaTime, const float& debugSledgeInput, const float& debugJumpInput, const float& debugMoveInput);

		b2Body* GetBody() const;
		const b2Vec2& GetPosition() const;

		std::vector<std::shared_ptr<Asset>> GetAssets() const;

		void Kill();
		bool IsDead() const;

	private:
		b2Body* GetHead() const;
		void BreakJoints();

		bool m_dead;

		std::shared_ptr<Asset> m_bodyAsset;
		std::shared_ptr<Asset> m_headAsset;
		b2Joint* m_headJoint;
		std::shared_ptr<Weapon> m_weapon;
		b2RevoluteJoint* m_weaponJoint;
	};

}

