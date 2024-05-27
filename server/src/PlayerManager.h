#pragma once
#include <memory>
#include <vector>

class b2World;

namespace Gameplay
{
class Player;
class Asset;
class LevelManager;

class PlayerManager
{
  public:
    PlayerManager(b2World *world);
    ~PlayerManager();

    void Update(float deltaTime);

    const std::shared_ptr<Player> &CreatePlayer();
    std::shared_ptr<Player> GetPlayer(size_t index);

    size_t GetNumPlayers() const;
    std::vector<std::shared_ptr<Player>> GetPlayers();
    std::vector<std::shared_ptr<Player>> GetPlayersAlive();
    std::vector<std::shared_ptr<Player>> GetPlayersDead();

    std::vector<std::shared_ptr<Player>> GetRedPlayers();
    std::vector<std::shared_ptr<Player>> GetBluePlayers();

    std::vector<std::shared_ptr<Asset>> GetAssets();

    std::pair<int, int> GetOptimalSpawn() const;

    signed int GetScore() const;
    signed int GetTeamScore(unsigned int teamTint) const;
    void ScoreRed(signed int score);
    void ScoreBlue(signed int score);
    void ClearScore();

    void SetLevelManager(LevelManager *levelManager);

  private:
    LevelManager *m_levelManager;
    b2World *m_world;
    std::vector<std::shared_ptr<Player>> m_players;
    unsigned int m_playersSpawned;
    float m_restartTimer;
    signed int m_totalScore;
    signed int m_redScore;
    signed int m_blueScore;
};

} // namespace Gameplay
