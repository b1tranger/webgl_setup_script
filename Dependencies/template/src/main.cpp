#include "Common.h"

// -----------------------------------------------------------------------------
// Global Window & State
// -----------------------------------------------------------------------------
static GLFWwindow* g_window = nullptr;

// -----------------------------------------------------------------------------
// Modern WebGL 2.0 Shaders (OpenGL ES 3.0 / #version 300 es)
// -----------------------------------------------------------------------------
#ifdef __EMSCRIPTEN__
const char* vertexShaderSource = R"(#version 300 es
precision highp float;
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

const char* fragmentShaderSource = R"(#version 300 es
precision highp float;
in vec3 vColor;
out vec4 FragColor;

uniform float uTime;

void main() {
    // Subtle pulsating glow
    float glow = 0.85 + 0.15 * sin(uTime * 3.0);
    FragColor = vec4(vColor * glow, 1.0);
}
)";
#else
const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vColor;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

const char* fragmentShaderSource = R"(#version 330 core
in vec3 vColor;
out vec4 FragColor;
uniform float uTime;
void main() {
    float glow = 0.85 + 0.15 * sin(uTime * 3.0);
    FragColor = vec4(vColor * glow, 1.0);
}
)";
#endif

// -----------------------------------------------------------------------------
// Geometry & Shader State
// -----------------------------------------------------------------------------
static GLuint g_shaderProgram = 0;
static GLuint g_VAO = 0;
static GLuint g_VBO = 0;
static GLint  g_uTimeLoc = -1;

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[Shader Compile Error]: " << infoLog << std::endl;
    }
    return shader;
}

void initGeometry() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    g_shaderProgram = glCreateProgram();
    glAttachShader(g_shaderProgram, vs);
    glAttachShader(g_shaderProgram, fs);
    glLinkProgram(g_shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    g_uTimeLoc = glGetUniformLocation(g_shaderProgram, "uTime");

    // Triangle vertices with positions and RGB colors
    float vertices[] = {
        // Position     // Color (RGB)
         0.0f,  0.55f,  0.0f, 0.90f, 1.0f, // Top (Cyan)
        -0.5f, -0.45f,  1.0f, 0.20f, 0.5f, // Bottom-Left (Magenta)
         0.5f, -0.45f,  0.4f, 1.0f, 0.3f  // Bottom-Right (Lime)
    };

    glGenVertexArrays(1, &g_VAO);
    glGenBuffers(1, &g_VBO);

    glBindVertexArray(g_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// -----------------------------------------------------------------------------
// Per-Frame Loop Callback (Called every requestAnimationFrame)
// -----------------------------------------------------------------------------
void mainLoopTick() {
    float timeVal = static_cast<float>(glfwGetTime());

    // Dynamically cycle background clear color
    float bgR = 0.08f + 0.04f * std::sin(timeVal * 0.7f);
    float bgG = 0.09f + 0.04f * std::cos(timeVal * 0.9f);
    float bgB = 0.14f + 0.05f * std::sin(timeVal * 1.1f);

    int w, h;
    glfwGetFramebufferSize(g_window, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(bgR, bgG, bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render Shader Triangle
    glUseProgram(g_shaderProgram);
    if (g_uTimeLoc >= 0) {
        glUniform1f(g_uTimeLoc, timeVal);
    }

    glBindVertexArray(g_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glfwSwapBuffers(g_window);
    glfwPollEvents();

#ifdef __EMSCRIPTEN__
    if (glfwWindowShouldClose(g_window)) {
        emscripten_cancel_main_loop();
    }
#endif
}

// -----------------------------------------------------------------------------
// Entry Point
// -----------------------------------------------------------------------------
int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    g_window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                                DEFAULT_WINDOW_TITLE, nullptr, nullptr);
    if (!g_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1);

#ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#endif

    initGeometry();

#ifdef __EMSCRIPTEN__
    // Asynchronous non-blocking browser loop (0 = requestAnimationFrame, 1 = infinite loop simulation)
    emscripten_set_main_loop(mainLoopTick, 0, 1);
#else
    while (!glfwWindowShouldClose(g_window)) {
        mainLoopTick();
    }
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteProgram(g_shaderProgram);
    glfwDestroyWindow(g_window);
    glfwTerminate();
#endif

    return 0;
}

