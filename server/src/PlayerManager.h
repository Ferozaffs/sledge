#pragma once
#include <memory>
#include <vector>

class b2World;

namespace Gameplay
{
class Player;
class Asset;
class GameManager;

class PlayerManager
{
  public:
    PlayerManager(GameManager &gameManager, std::weak_ptr<b2World> world);
    ~PlayerManager() = default;

    void Update(float deltaTime);

    std::weak_ptr<Player> CreatePlayer();
    std::shared_ptr<Player> GetPlayer(size_t index) const;

    size_t GetNumPlayers() const;
    const std::vector<std::shared_ptr<Player>> &GetPlayers() const;
    std::vector<std::shared_ptr<Player>> GetPlayersAlive() const;
    std::vector<std::shared_ptr<Player>> GetPlayersDead() const;

    std::vector<std::shared_ptr<Player>> GetRedPlayers() const;
    std::vector<std::shared_ptr<Player>> GetBluePlayers() const;

    std::vector<std::weak_ptr<Asset>> GetAssets() const;

    signed int GetScore() const;
    signed int GetTeamScore(unsigned int teamTint) const;
    void ScoreRed(signed int score);
    void ScoreBlue(signed int score);
    void ClearScore();

  private:
    GameManager &m_gameManager;
    std::weak_ptr<b2World> m_world;

    std::vector<std::shared_ptr<Player>> m_players;
    unsigned int m_playersSpawned;
    float m_restartTimer;
    signed int m_totalScore;
    signed int m_redScore;
    signed int m_blueScore;
};

} // namespace Gameplay
