#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Gameplay 
{
	class PlayerManager;
	class Player;
	class Asset;
	class LevelLoader;
}

namespace Network
{
	class Impl;

	class ConnectionManager
	{
	public:
		ConnectionManager(Gameplay::PlayerManager* playerManager, Gameplay::LevelLoader* levelLoader);
		~ConnectionManager();

		void Update(float deltaTime);

	private:
		friend class Impl;

		static std::string CreateStatusMessage(std::string message);
		static std::string CreateErrorMessage(std::string message);

		void SendAssets(const std::shared_ptr<Gameplay::Player>& player, std::vector<std::shared_ptr<Gameplay::Asset>> assets);
		
		static std::unique_ptr<Impl> m_impl;
		Gameplay::PlayerManager* m_playerManager;
		Gameplay::LevelLoader* m_levelLoader;

		float m_tickCounter;
	};

}

