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
    bool Finished() override;

    bool IsValid() override;

  private:
    PlayerManager *m_playerManager;
    size_t m_numPlayers;
    bool m_valid;

    float m_countDown;
};

} // namespace Gameplay
