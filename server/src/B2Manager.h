#pragma once
#include <memory>

class b2World;

#ifdef WIN32
namespace Debug
{
class Debugger;
}
#endif

namespace Physics
{
class B2Manager
{
  public:
    B2Manager();
    ~B2Manager() = default;

    void Update(float deltaTime);

    std::weak_ptr<b2World> GetWorld();

#ifdef WIN32
    void DbgRender(std::weak_ptr<Debug::Debugger> debugger);
#endif

  private:
    std::shared_ptr<b2World> m_world;
    float m_accumelatedTime;
};

} // namespace Physics
