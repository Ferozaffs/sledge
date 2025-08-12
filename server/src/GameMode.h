#pragma once
#include "GameSettings.h"
#include "IGameMode.h"
#include <map>
#include <memory>
#include <vector>

namespace Gameplay
{
class PlayerManager;
class Player;
class Level;
class LevelBlock;

class GameMode : public IGameMode
{
  public:
    GameMode(PlayerManager &playerManager, const GameModeConfiguration &configuration,
             const std::weak_ptr<Level> level);
    ~GameMode();

    void Update(float deltaTime) override;
    bool Finished() const override;
    bool IsValid() const override;
    GameModeType GetType() const override;
    signed int GetPoints() const override;
    std::unordered_map<int, float> GetPointsMap() const override;

    void OnObjectiveDestroyed(LevelBlock *block);

  private:
    struct ZoneData
    {
        float minX;
        float minY;
        float maxX;
        float maxY;

        float pointTimer;

        ZoneData() : minX(FLT_MAX), minY(FLT_MAX), maxX(FLT_MIN), maxY(FLT_MIN), pointTimer(0.0f)
        {
        }

        bool IsWithin(float x, float y) const
        {
            if (x >= minX && x <= maxX && y >= minY && y <= maxY)
            {
                return true;
            }

            return false;
        }
    };

    void UpdateRespawn(float deltaTime);
    void UpdatePoints(float deltaTime);
    void UpdateDeathPoints(float deltaTime);
    void UpdateZoneObjectivePoints(float deltaTime);
    void UpdateWinCondition(float deltaTime);

    PlayerManager &m_playerManager;
    GameModeConfiguration m_configuration;
    const std::weak_ptr<Level> m_level;

    bool m_valid;

    size_t m_previousNumPlayersAlive;
    size_t m_previousNumPlayersTeamRedAlive;
    size_t m_previousNumPlayersTeamBlueAlive;

    std::vector<std::pair<GameScoreObjective, ZoneData>> m_zoneObjectives;
    std::vector<GameScoreObjective> m_destructionObjectives;

    bool m_pointsReached;
    std::map<const std::shared_ptr<Player>, float> m_playerPoints;
    float m_redPoints;
    float m_bluePoints;

    float m_countDown;
};

} // namespace Gameplay
