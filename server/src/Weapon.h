#pragma once
class b2Body;

namespace Gameplay
{
	class Avatar;
	
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

	protected:
		b2Body* m_shaft;
		float m_speed;
		float m_torque;

	private:
		const Avatar* m_avatar;
	};

}

