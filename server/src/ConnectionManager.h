#pragma once
#include <memory>

namespace Gameplay 
{
	class PlayerManager;
}

namespace Network
{
	class Impl;

	class ConnectionManager
	{
	public:
		ConnectionManager(Gameplay::PlayerManager* playerManager);
		~ConnectionManager();

	private:
		friend class Impl;
		static std::unique_ptr<Impl> m_impl;
	};

}

