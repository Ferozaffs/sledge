#pragma once
#include "GameSettings.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class b2World;

namespace Gameplay
{
class Asset;
class Level;
class GameManager;

class LevelManager
{
  public:
    LevelManager(std::weak_ptr<b2World> world);
    ~LevelManager();

    bool LoadPlaylist(const std::string &path);
    bool NextLevel();

    void Update(float deltaTime);

    std::vector<std::weak_ptr<Asset>> GetAssets(bool allAssets = false);

    const std::weak_ptr<Level> GetCurrentLevel() const;
    std::vector<std::pair<int, int>> GetSpawns() const;
    std::vector<std::pair<int, int>> GetRedSpawns() const;
    std::vector<std::pair<int, int>> GetBlueSpawns() const;
    GameSettings GetSettings() const;

    void SetGameModeConfiguration(const GameModeConfiguration &configuration);

  private:
    bool LoadLevel(const std::pair<std::string, std::string> &files);

    std::weak_ptr<b2World> m_world;

    std::shared_ptr<Level> m_currentLevel;

    std::vector<std::pair<std::string, std::string>> m_playlist;
    unsigned int m_currentLevelIndex;
    bool m_reloaded;
};

} // namespace Gameplay
