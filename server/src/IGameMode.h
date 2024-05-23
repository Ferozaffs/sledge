#pragma once
namespace Gameplay
{

class IGameMode
{
  public:
    virtual void Update(float deltaTime) = 0;
    virtual bool Finished() = 0;
    virtual bool IsValid() = 0;
};

} // namespace Gameplay
