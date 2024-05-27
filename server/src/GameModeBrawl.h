#pragma once
#include "IGameMode.h"

namespace Gameplay
{
class PlayerManager;

class GameModeBrawl : public IGameMode
{
  public:
    GameModeBrawl(PlayerManager *playerManager);
    ~GameModeBrawl();

    void Update(float deltaTime) override;
    bool Finished() const override;
    bool IsValid() const override;
    GameModeType GetType() const override;

  private:
    PlayerManager *m_playerManager;
    int m_numPlayers;
    bool m_valid;

    float m_countDown;
};

} // namespace Gameplay
