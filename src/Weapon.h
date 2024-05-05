#pragma once
class b2Body;

namespace Gameplay
{
	class Player; 
	
	class Weapon
	{
	public:
		Weapon(const Player* player);
		virtual ~Weapon();

		virtual void Update(const float& /*deltaTime*/) {}

		b2Body* GetShaft() const;
		const float& GetSpeed() const;
		const float& GetTorque() const;
		const Player* GetPlayer() const;

	protected:
		b2Body* m_shaft;
		float m_speed;
		float m_torque;

	private:
		const Player* m_player;
	};

}

