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
    ~B2Manager();

    void Update(const float &deltaTime);

    b2World *GetWorld();

#ifdef WIN32
    void DbgRender(Debug::Debugger *debugger);
#endif

  private:
    std::unique_ptr<b2World> m_world;
    float m_accumelatedTime;
};

} // namespace Physics
