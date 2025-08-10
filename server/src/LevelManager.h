#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

class b2World;

namespace Gameplay
{
class Asset;
class Level;
struct GameSettings;
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

    std::vector<std::pair<int, int>> GetSpawns() const;
    GameSettings GetSettings() const;

  private:
    bool LoadLevel(const std::pair<std::string, std::string> &files);

    std::weak_ptr<b2World> m_world;

    std::unique_ptr<Level> m_currentLevel;

    std::vector<std::pair<std::string, std::string>> m_playlist;
    unsigned int m_currentLevelIndex;
    bool m_reloaded;
};

} // namespace Gameplay
