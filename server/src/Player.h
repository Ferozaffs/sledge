#pragma once
#include "GameSettings.h"
#include <atomic>
#include <memory>
#include <vector>

class b2World;

namespace Gameplay
{
class Avatar;
class Asset;
class GameManager;
class PlayerManager;
enum class GameModeWish;

class Player
{
  public:
    Player(const GameManager &gameManager, const PlayerManager &playerManager, std::weak_ptr<b2World> world,
           unsigned int tint, unsigned int teamTint);
    ~Player();

    const float GetX() const;
    const float GetY() const;

    void Update(float deltaTime);
    void SetGameModeConfiguration(const GameModeConfiguration &configuration);
    void Respawn(float time);

    void SetInputs(float sledgeInput, float moveInput, float jumpInput);

    int GetMainAssetId() const;
    std::vector<std::weak_ptr<Asset>> GetAssets() const;

    signed int GetScore() const;
    void Score(signed int score);
    void ClearScore();
    void SetWinner();

    GameModeWish GetGameModeWish() const;
    bool IsDead() const;
    signed int CollectKiller();

    Team GetTeam() const;
    unsigned int GetTeamTint() const;
    void SetTeamColors(bool useTeamColors);

    std::atomic<bool> m_pendingRemove;

  private:
    void Respawn();
    void SpawnAvatar(std::weak_ptr<b2World> world);

    const GameManager &m_gameManager;
    const PlayerManager &m_playerManager;
    std::weak_ptr<b2World> m_world;

    std::unique_ptr<Avatar> m_avatar;
    float m_sledgeInput;
    float m_moveInput;
    float m_jumpInput;
    float m_respawnTimer;
    unsigned int m_tint;
    unsigned int m_teamTint;
    signed int m_score;
    bool m_winner;
    bool m_usingTeamColors;
    GameModeWish m_gameModeWish;
    GameModeConfiguration m_gameModeConfiguration;
};

} // namespace Gameplay
