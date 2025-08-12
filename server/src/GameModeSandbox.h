#pragma once
#include "IGameMode.h"

namespace Gameplay
{
class PlayerManager;

class GameModeSandbox : public IGameMode
{
  public:
    GameModeSandbox(PlayerManager &playerManager);
    ~GameModeSandbox() = default;

    void Update(float deltaTime) override;
    bool Finished() const override;
    bool IsValid() const override;
    GameModeType GetType() const override;
    signed int GetPoints() const override;
    std::unordered_map<int, float> GetPointsMap() const override;

  private:
    PlayerManager &m_playerManager;
};

} // namespace Gameplay
