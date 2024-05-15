#pragma once
#include <memory>
#include <vector>

class b2Body;
class b2Joint;

namespace Gameplay
{
	class Avatar;
	class Asset;
	
	class Weapon
	{
	public:
		Weapon(const Avatar* avatar);
		virtual ~Weapon();

		virtual void Update(const float& /*deltaTime*/) {}

		b2Body* GetShaft() const;
		const float& GetSpeed() const;
		const float& GetTorque() const;
		const Avatar* GetAvatar() const;

		std::vector<std::shared_ptr<Asset>> GetAssets() const;

		void BreakJoints();

	protected:
		std::vector<std::shared_ptr<Asset>> m_assets;
		std::vector<b2Joint*> m_joints;
		std::shared_ptr<Asset> m_shaftAsset;
		float m_speed;
		float m_torque;

	private:
		const Avatar* m_avatar;
	};

}

