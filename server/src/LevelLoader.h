#pragma once
#include <memory>
#include <string>
#include <vector>

class b2World;

namespace Gameplay
{
	class LevelAsset;

	class LevelLoader 
	{
	public:
		LevelLoader(const std::shared_ptr<b2World>& world);
		~LevelLoader();

		bool LoadLevel(const std::string& filename);

		const std::vector< std::shared_ptr<LevelAsset>>& GetLevelAssets();

	private:
		bool BuildLevel(std::vector<std::vector<uint8_t>> rows);
		bool CreateStaticBlock(int x, int y);

		std::shared_ptr<b2World> m_world;

		std::vector<std::shared_ptr<LevelAsset>> m_assets;
	};

}

