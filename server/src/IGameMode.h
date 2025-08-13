#pragma once
#include <unordered_map>

namespace Gameplay
{

enum class GameModeType;

class IGameMode
{
  public:
    virtual ~IGameMode() = default;

    virtual void Update(float deltaTime) = 0;
    virtual bool Finished() const = 0;
    virtual bool IsValid() const = 0;
    virtual GameModeType GetType() const = 0;
    virtual signed int GetPoints() const = 0;
    virtual std::unordered_map<int, float> GetPointsMap() const = 0;
};

} // namespace Gameplay
