#pragma once
#include <memory>

class b2World;

namespace Gameplay
{
	class Avatar;

	class Player
	{
	public:
		Player(const std::shared_ptr<b2World>& world);
		~Player();

		void Update(float deltaTime);

		void SetInputs(float sledgeInput, float moveInput, float jumpInput);
			
	private:
		std::shared_ptr<Avatar> m_avatar;
		float m_sledgeInput;
		float m_moveInput;
		float m_jumpInput;
	};

}

