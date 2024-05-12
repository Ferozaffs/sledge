#include "Debugger.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

namespace Debug {

    typedef struct Vertex
    {
        vec2 pos;
        vec3 col;
    } Vertex;

    static const Vertex triangleVertices[3] =
    {
       { -0.6f, -0.4f },
       {  0.6f, -0.4f },
       {   0.f,  0.6f }
    };

    static const Vertex boxVertices[6] =
    {
       { 0.5f, 0.5f }, 
       { 0.5f, -0.5f }, 
       {  -0.5f,  0.5f },
       {   0.5f,  -0.5f },
       { -0.5f,  -0.5f }, 
       { -0.5f,  0.5f }
    };

    constexpr size_t MaxNumInstances = 500;

    static const char* vertexShaderSource =
        "#version 330\n"
        "layout (location = 0) in vec2 vPos;\n"
        "layout (location = 1) in mat4 vInstance;\n"
        "uniform mat4 uViewProj;\n"
        "void main()\n"
        "{\n"  
        "    gl_Position = uViewProj * vInstance * vec4(vPos, 0.0, 1.0);\n"
        "}\n";

    static const char* fragmentShaderSource =
        "#version 330\n"
        "out vec4 fragment;\n"
        "uniform vec4 uColor;\n"
        "void main()\n"
        "{\n"
        "    fragment = uColor;\n"
        "}\n";

    static void ErrorCallback(int /*error*/, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    static void GLAPIENTRY GLDebugMessageCallback(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei /*length*/,
        const GLchar* message,
        const void* /*userParam*/)
    {
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        {
            return;
        }

        static std::map<GLenum, const GLchar*> sources =
        {
            {GL_DEBUG_SOURCE_API, "API"},
            {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"},
            {GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"},
            {GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"},
            {GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"},
            {GL_DEBUG_SOURCE_OTHER, "OTHER"}
        };

        static std::map<GLenum, const GLchar*> severities =
        {
            {GL_DEBUG_SEVERITY_HIGH, "HIGH"},
            {GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM"},
            {GL_DEBUG_SEVERITY_LOW, "LOW"},
            {GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"}
        };

        static std::map<GLenum, const GLchar*> types =
        {
            {GL_DEBUG_TYPE_ERROR, "ERROR"},
            {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
            {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"},
            {GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
            {GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
            {GL_DEBUG_TYPE_MARKER, "MARKER"},
            {GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP"},
            {GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP"},
            {GL_DEBUG_TYPE_OTHER, "OTHER"}
        };

        printf("[OpenGL %s] - SEVERITY: %s, SOURCE: %s, ID: %d: %s\n", types[type], severities[severity], sources[source], id, message);
    }

    float Debugger::DbgSledgeInput = 0.0f;
    float Debugger::DbgJumpInput = 0.0f;
    float Debugger::DbgMoveInput = 0.0f;
    void key_callback(GLFWwindow* /*window*/, int key, int /*scancode*/ , int action, int /*mods*/) {
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            Debugger::DbgSledgeInput += 1.0f;
        }
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            Debugger::DbgSledgeInput += -1.0f;
        }
        if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
            Debugger::DbgSledgeInput -= 1.0f;
        }
        if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
            Debugger::DbgSledgeInput -= -1.0f;
        }

        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            Debugger::DbgJumpInput += 1.0f;
        }
        if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
            Debugger::DbgJumpInput -= 1.0f;
        }

        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            Debugger::DbgMoveInput += 1.0f;
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            Debugger::DbgMoveInput += -1.0f;
        }
        if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
            Debugger::DbgMoveInput -= 1.0f;
        }
        if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
            Debugger::DbgMoveInput -= -1.0f;
        }
    }

    Debugger::Debugger() {
        glfwSetErrorCallback(ErrorCallback);

        if (!glfwInit())
            exit(EXIT_FAILURE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(1280, 720, "Debug View", NULL, NULL);
        if (!m_window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(m_window, key_callback);

        glfwMakeContextCurrent(m_window);
        gladLoadGL(glfwGetProcAddress);
        glfwSwapInterval(1);

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLDebugMessageCallback, 0);

        CreateDefaultProgram();

        CreateTriangleVertices();
        CreateBoxVertices();
    }
    
    Debugger::~Debugger() {
        glfwDestroyWindow(m_window);

        glfwTerminate();
        exit(EXIT_SUCCESS);
    }

    void Debugger::AddShape(const mat4x4& pose, ShapeType type)
    {
        std::array<float, 16> clonedPose;
        memcpy(&clonedPose, &pose, sizeof(float) * 16);

        m_shapes[type].push_back(clonedPose);
    }

    void Debugger::Update(const float& /*deltaTime*/)
    {
    
    }

    void Debugger::UpdateShapes()
    {  
        if (m_shapes[ShapeType::Box].size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, m_boxInstances);
            void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            memcpy(ptr, &m_shapes[ShapeType::Box][0], sizeof(mat4x4) * m_shapes[ShapeType::Box].size());
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        if (m_shapes[ShapeType::Triangle].size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, m_triangleInstances);
            void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            memcpy(ptr, &m_shapes[ShapeType::Triangle][0], sizeof(mat4x4) * m_shapes[ShapeType::Triangle].size());
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

    }

    void Debugger::Render() {
        UpdateShapes();

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        const float ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        const float cameraZoom = 100.0f;

        mat4x4 v, p, vp;
        mat4x4_identity(v);
        mat4x4_ortho(p, -ratio * cameraZoom, ratio * cameraZoom, -cameraZoom, cameraZoom, 1.f, -1.f);
        mat4x4_mul(vp, p, v);

        vec4 colorBox = {1.0f, 0.5f, 0.0f, 1.0f};
        vec4 colorTriangle = { 1.0f, 1.0f, 0.0f, 1.0f };

        glUseProgram(m_defaultProgram);
        glUniformMatrix4fv(m_vpUniform, 1, GL_FALSE, (const GLfloat*)&vp);
        glUniform4fv(m_colorUniform, 1, (const GLfloat*)&colorBox);
        glBindVertexArray(m_boxes);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(m_shapes[ShapeType::Box].size()));

        glUniform4fv(m_colorUniform, 1, (const GLfloat*)&colorTriangle);
        glBindVertexArray(m_triangles);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(m_shapes[ShapeType::Triangle].size()));

        glfwSwapBuffers(m_window);
        glfwPollEvents();

        m_shapes.clear();
    }
    void Debugger::CreateDefaultProgram() {
        const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertex_shader);

        GLint status;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            char error[1024];
            glGetShaderInfoLog(vertex_shader, 1024, nullptr, error);
            printf_s("%s\n", error);

            glDeleteShader(vertex_shader);
            return;
        }

        const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            char error[1024];
            glGetShaderInfoLog(fragment_shader, 1024, nullptr, error);
            printf_s("%s\n", error);

            glDeleteShader(fragment_shader);
            return;
        }

        m_defaultProgram = glCreateProgram();
        glAttachShader(m_defaultProgram, vertex_shader);
        glAttachShader(m_defaultProgram, fragment_shader);
        glLinkProgram(m_defaultProgram);

        glGetProgramiv(m_defaultProgram, GL_LINK_STATUS, &status);
        if (status == GL_FALSE){
            char error[1024];
            glGetProgramInfoLog(fragment_shader, 1024, nullptr, error);
            printf_s("%s\n", error);

            glDeleteShader(fragment_shader);
        }

        m_vpUniform = glGetUniformLocation(m_defaultProgram, "uViewProj");
        m_colorUniform = glGetUniformLocation(m_defaultProgram, "uColor");
    }

    void Debugger::CreateTriangleVertices() {
        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &m_triangleInstances);

        glGenVertexArrays(1, &m_triangles);
        glBindVertexArray(m_triangles);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, m_triangleInstances);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mat4x4) * MaxNumInstances, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(1 * sizeof(vec4)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(2 * sizeof(vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(3 * sizeof(vec4)));

        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Debugger::CreateBoxVertices() {
        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &m_boxInstances);

        glGenVertexArrays(1, &m_boxes);
        glBindVertexArray(m_boxes);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, m_boxInstances);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mat4x4) * MaxNumInstances, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(1 * sizeof(vec4)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(2 * sizeof(vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(3 * sizeof(vec4)));

        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}
