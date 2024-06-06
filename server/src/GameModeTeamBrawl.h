#pragma once
#include "IGameMode.h"

namespace Gameplay
{
class PlayerManager;

class GameModeTeamBrawl : public IGameMode
{
  public:
    GameModeTeamBrawl(PlayerManager &playerManager);
    ~GameModeTeamBrawl();

    void Update(float deltaTime) override;
    bool Finished() const override;
    bool IsValid() const override;
    GameModeType GetType() const override;

  private:
    PlayerManager &m_playerManager;
    int m_numPlayersTeamRed;
    int m_numPlayersTeamBlue;
    bool m_valid;

    float m_countDown;
};

} // namespace Gameplay
