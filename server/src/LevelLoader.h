#pragma once
#include <memory>
#include <string>
#include <vector>

class b2World;

namespace Gameplay
{
	class LevelBlock;
	class Asset;

	class LevelLoader 
	{
	public:
		LevelLoader(const std::shared_ptr<b2World>& world);
		~LevelLoader();

		bool LoadLevel(const std::string& filename);

		std::vector< std::shared_ptr<Asset>> GetAssets() const;
		std::vector< std::shared_ptr<Asset>> GetDynamicAssets() const;

	private:
		bool BuildLevel(std::vector<std::vector<uint8_t>> rows);
		bool CreateStaticBlock(int x, int y);

		std::shared_ptr<b2World> m_world;

		std::vector<std::shared_ptr<LevelBlock>> m_blocks;
	};

}

