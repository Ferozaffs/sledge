#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

class b2World;

namespace Gameplay
{
class LevelBlock;
class Asset;

class LevelManager
{
  public:
    LevelManager(b2World *world);
    ~LevelManager();

    bool LoadPlaylist(const std::string &path);
    bool NextLevel();

    void Update(float deltaTime);

    std::vector<std::shared_ptr<Asset>> GetAssets(bool allAssets = false);

    const std::vector<std::pair<int, int>> &GetSpawns() const;

  private:
    bool LoadLevel(const std::string &filename);
    bool BuildLevel(std::vector<std::vector<uint8_t>> rows);
    bool CreateBlock(int x, int y, std::string alias);
    bool Decorate();
    bool CreateFloorDecor(std::pair<int, int> coord);
    bool CreateRoofDecor(std::pair<int, int> coord);

    b2World *m_world;

    std::map<std::pair<int, int>, std::shared_ptr<LevelBlock>> m_blocks;
    std::vector<std::pair<int, int>> m_spawns;

    std::vector<std::string> m_playlist;
    unsigned int m_currentLevelIndex;
    bool m_reloaded;
};

} // namespace Gameplay
