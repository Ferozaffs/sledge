#pragma once
#include <memory>
#include <vector>

class b2World;

namespace Gameplay
{
	class Player;

	class PlayerManager
	{
	public:
		PlayerManager(const std::shared_ptr<b2World>& b2World);
		~PlayerManager();

		void Update(float deltaTime);

		const std::shared_ptr<Player>& CreatePlayer();
		const std::shared_ptr<Player>& GetPlayer(size_t index);

	private:
		const std::shared_ptr<b2World>& m_b2World;
		std::vector<std::shared_ptr<Player>> m_players;
	};

}

