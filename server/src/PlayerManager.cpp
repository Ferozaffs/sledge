#include "PlayerManager.h"
#include "Player.h"

using namespace Gameplay;

PlayerManager::PlayerManager(const std::shared_ptr<b2World>& b2World)
	: m_b2World(b2World)
{
}

PlayerManager::~PlayerManager()
{
}

void PlayerManager::Update(float deltaTime)
{
	for (auto it = m_players.begin(); it != m_players.end();) {
		if ((*it)->m_pendingRemove)
		{
			it = m_players.erase(it);
		}
		else 
		{
			(*it)->Update(deltaTime);
			it++;
		}
	}
}

const std::shared_ptr<Player>& PlayerManager::CreatePlayer()
{
	m_players.emplace_back(std::make_shared<Player>(m_b2World));
	return m_players.back();
}

const std::shared_ptr<Player>& Gameplay::PlayerManager::GetPlayer(size_t index)
{
	if (m_players.size() > index)
	{
		return m_players[index];
	}

	return nullptr;
}

std::vector<std::shared_ptr<Asset>> Gameplay::PlayerManager::GetDynamicAssets() const
{
	std::vector<std::shared_ptr<Asset>> assets;
	for (const auto& player : m_players)
	{
		auto playerAssets = player->GetAssets();
		assets.insert(assets.end(), playerAssets.begin(), playerAssets.end());
	}

	return assets;
}
