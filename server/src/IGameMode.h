#pragma once
namespace Gameplay
{

enum class GameModeType;

class IGameMode
{
  public:
    virtual void Update(float deltaTime) = 0;
    virtual bool Finished() const = 0;
    virtual bool IsValid() const = 0;
    virtual GameModeType GetType() const = 0;
};

} // namespace Gameplay
