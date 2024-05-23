#pragma once
#include "IGameMode.h"

namespace Gameplay
{
class PlayerManager;

class GameModeSandbox : public IGameMode
{
  public:
    GameModeSandbox(PlayerManager *playerManager);
    ~GameModeSandbox();

    void Update(float deltaTime) override;
    bool Finished() override;

    bool IsValid() override;

  private:
    PlayerManager *m_playerManager;
};

} // namespace Gameplay
