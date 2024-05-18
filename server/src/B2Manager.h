#pragma once
#include <memory>

class b2World;
namespace Debug
{
class Debugger;
}

namespace Physics
{
class B2Manager
{
  public:
    B2Manager();
    ~B2Manager();

    void Update(const float &deltaTime);

    std::shared_ptr<b2World> GetWorld();

    void DbgRender(Debug::Debugger *debugger);

  private:
    std::shared_ptr<b2World> m_world;
    float m_accumelatedTime;
};

} // namespace Physics
