#pragma once
#ifdef WIN32
#include <array>
#include <linmath.h>
#include <map>
#include <vector>

struct GLFWwindow;

namespace Debug
{
class Debugger
{
  public:
    enum class ShapeType
    {
        Box,
        Triangle,
    };

    Debugger();
    ~Debugger();

    void AddShape(const mat4x4 &pose, ShapeType type);
    void Update(const float &deltaTime);
    void Render();

    static float DbgSledgeInput;
    static float DbgJumpInput;
    static float DbgMoveInput;

  private:
    void UpdateShapes();
    void CreateDefaultProgram();
    void CreateTriangleVertices();
    void CreateBoxVertices();

    GLFWwindow *m_window;

    unsigned int m_defaultProgram;
    unsigned int m_vpUniform;
    unsigned int m_colorUniform;

    unsigned int m_triangles;
    unsigned int m_triangleInstances;

    unsigned int m_boxes;
    unsigned int m_boxInstances;

    std::map<ShapeType, std::vector<std::array<float, 16>>> m_shapes;
};

} // namespace Debug
#endif
