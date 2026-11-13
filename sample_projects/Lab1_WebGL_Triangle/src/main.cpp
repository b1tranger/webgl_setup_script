#include <iostream>
#include <cmath>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

GLFWwindow* g_Window = nullptr;
GLuint g_Program = 0;
GLuint g_VAO = 0;
GLuint g_VBO = 0;

// GLSL 3.00 ES Vertex Shader
const char* vertexShaderSource = R"(#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    vColor = aColor;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
)";

// GLSL 3.00 ES Fragment Shader
const char* fragmentShaderSource = R"(#version 300 es
precision mediump float;

in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Error:\n" << infoLog << "\n";
    }
    return shader;
}

void initGL() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    g_Program = glCreateProgram();
    glAttachShader(g_Program, vs);
    glAttachShader(g_Program, fs);
    glLinkProgram(g_Program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Triangle vertices: Position (x, y) + Color (r, g, b)
    float vertices[] = {
        // Position      // Color (RGB)
         0.0f,  0.6f,    1.0f, 0.0f, 0.0f,  // Top: Red
        -0.6f, -0.6f,    0.0f, 1.0f, 0.0f,  // Bottom-Left: Green
         0.6f, -0.6f,    0.0f, 0.0f, 1.0f   // Bottom-Right: Blue
    };

    glGenVertexArrays(1, &g_VAO);
    glGenBuffers(1, &g_VBO);

    glBindVertexArray(g_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 0: Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute 1: Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void mainLoopIteration() {
    glfwPollEvents();

    glClearColor(0.1f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_Program);
    glBindVertexArray(g_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glfwSwapBuffers(g_Window);
}

int main() {
    std::cout << "[Lab1] Initializing WebGL 2.0 RGB Triangle...\n";

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    g_Window = glfwCreateWindow(800, 600, "Lab 1: WebGL 2.0 RGB Triangle", nullptr, nullptr);
    if (!g_Window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_Window);
    initGL();

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(mainLoopIteration, 0, 1);
#else
    while (!glfwWindowShouldClose(g_Window)) {
        mainLoopIteration();
    }
    glfwDestroyWindow(g_Window);
    glfwTerminate();
#endif

    return 0;
}

