#pragma once
#include "GameSettings.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class b2World;

namespace Gameplay
{
class LevelBlock;

class Level
{
  public:
    Level(std::weak_ptr<b2World> world, const std::pair<std::string, std::string> &files);
    ~Level();

    void Update(float deltaTime);

    bool IsValid();

    const GameSettings &GetSettings() const;
    const std::map<std::pair<int, int>, std::shared_ptr<LevelBlock>> &GetBlocks() const;
    std::vector<std::shared_ptr<LevelBlock>> GetBlocks(unsigned int code);
    unsigned int GetNumBlocks() const;
    unsigned int GetNumBlocks(unsigned int code) const;
    const std::vector<std::pair<int, int>> &GetSpawns() const;
    const std::vector<std::pair<int, int>> &GetRedSpawns() const;
    const std::vector<std::pair<int, int>> &GetBlueSpawns() const;

  private:
    void SetDefault();

    bool LoadLevel(const std::string &levelFilename);
    bool BuildLevel(std::vector<std::vector<uint8_t>> rows);
    bool CreateBlock(int x, int y, const BlockConfiguration &configuration);
    bool Decorate();
    bool CreateFloorDecor(std::pair<int, int> coord);
    bool CreateRoofDecor(std::pair<int, int> coord);

    bool LoadSettings(const std::string &settingsFilename);

    std::weak_ptr<b2World> m_world;

    GameSettings m_settings;
    std::map<unsigned int, BlockConfiguration> m_blockConfigurations;
    std::map<std::pair<int, int>, std::shared_ptr<LevelBlock>> m_blocks;
    std::vector<std::pair<int, int>> m_spawns;
    std::vector<std::pair<int, int>> m_redSpawns;
    std::vector<std::pair<int, int>> m_blueSpawns;
};

} // namespace Gameplay
