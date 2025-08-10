#pragma once
#include "GameSettings.h"
#include "IGameMode.h"
#include <map>
#include <memory>

namespace Gameplay
{
class PlayerManager;
class Player;

class GameMode : public IGameMode
{
  public:
    GameMode(PlayerManager &playerManager, const GameModeConfiguration &configuration);
    ~GameMode();

    void Update(float deltaTime) override;
    bool Finished() const override;
    bool IsValid() const override;
    GameModeType GetType() const override;

  private:
    void UpdateRespawn(float deltaTime);
    void UpdatePoints(float deltaTime);
    void UpdateDeathPoints(float deltaTime);
    void UpdateZoneObjectivePoints(float deltaTime);
    void UpdateDestructionObjectivePoints(float deltaTime);
    void UpdateWinCondition(float deltaTime);

    PlayerManager &m_playerManager;
    GameModeConfiguration m_configuration;

    bool m_valid;

    size_t m_previousNumPlayersAlive;
    size_t m_previousNumPlayersTeamRedAlive;
    size_t m_previousNumPlayersTeamBlueAlive;

    bool m_pointsReached;
    std::map<const std::shared_ptr<Player>, unsigned int> m_playerPoints;
    unsigned int m_redPoints;
    unsigned int m_bluePoints;

    float m_countDown;
};

} // namespace Gameplay
