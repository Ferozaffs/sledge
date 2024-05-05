#pragma once
#include <map>
#include <vector>
#include <array>
#include <linmath.h>

struct GLFWwindow;

namespace Debug
{
	class Renderer
	{
	public:
		enum class ShapeType
		{
			Box,
			Triangle,
		};

		Renderer();
		~Renderer();

		void AddShape(const mat4x4& pose, ShapeType type);
		void Render();

	private:
		void UpdateShapes();
		void CreateDefaultProgram();
		void CreateTriangleVertices();
		void CreateBoxVertices();


		GLFWwindow* m_window;

		unsigned int m_defaultProgram;
		unsigned int m_vpUniform;
		unsigned int m_colorUniform;

		unsigned int m_triangles;
		unsigned int m_triangleInstances;

		unsigned int m_boxes;
		unsigned int m_boxInstances;

		std::map<ShapeType, std::vector<std::array<float, 16>>> m_shapes;
	};

}

