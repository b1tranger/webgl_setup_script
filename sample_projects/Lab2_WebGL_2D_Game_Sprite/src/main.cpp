#include <iostream>
#include <vector>
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
GLuint g_Shader = 0;
GLuint g_VAO = 0;
GLuint g_VBO = 0;

// Player state
float g_PosX = 0.0f;
float g_PosY = 0.0f;
float g_VelY = 0.0f;
bool g_IsGrounded = true;
float g_PlayerScale = 0.15f;

// GLSL 3.00 ES Shader Sources
const char* vsSrc = R"(#version 300 es
layout (location = 0) in vec2 aPos;
uniform vec2 uOffset;
uniform vec2 uScale;
void main() {
    vec2 pos = aPos * uScale + uOffset;
    gl_Position = vec4(pos, 0.0, 1.0);
}
)";

const char* fsSrc = R"(#version 300 es
precision mediump float;
uniform vec3 uColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

#if defined(__EMSCRIPTEN__)
// Web Audio synthesizer bridge: Generates an instantaneous retro synth tone
EM_JS(void, playWebAudioTone, (float frequency, float duration), {
    try {
        var AudioCtx = window.AudioContext || window.webkitAudioContext;
        if (!window._labAudioContext) {
            window._labAudioContext = new AudioCtx();
        }
        var ctx = window._labAudioContext;
        if (ctx.state === 'suspended') {
            ctx.resume();
        }
        var osc = ctx.createOscillator();
        var gain = ctx.createGain();
        osc.type = 'triangle';
        osc.frequency.setValueAtTime(frequency, ctx.currentTime);
        gain.gain.setValueAtTime(0.3, ctx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.001, ctx.currentTime + duration);
        osc.connect(gain);
        gain.connect(ctx.destination);
        osc.start();
        osc.stop(ctx.currentTime + duration);
    } catch(e) {
        console.warn('Web Audio error:', e);
    }
});
#else
void playWebAudioTone(float frequency, float duration) {
    std::cout << "[Audio BEEP] Freq: " << frequency << "Hz\n";
}
#endif

void initQuad() {
    // Unit quad centered at (0, 0)
    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    glGenVertexArrays(1, &g_VAO);
    glGenBuffers(1, &g_VBO);

    glBindVertexArray(g_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint createProgram(const char* vs, const char* fs) {
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vs, nullptr);
    glCompileShader(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fs, nullptr);
    glCompileShader(fShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return program;
}

void processInput() {
    float moveSpeed = 0.015f;
    if (glfwGetKey(g_Window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(g_Window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        g_PosX -= moveSpeed;
    }
    if (glfwGetKey(g_Window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(g_Window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        g_PosX += moveSpeed;
    }
    if ((glfwGetKey(g_Window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(g_Window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(g_Window, GLFW_KEY_UP) == GLFW_PRESS) && g_IsGrounded) {
        g_VelY = 0.035f;
        g_IsGrounded = false;
        playWebAudioTone(523.25f, 0.15f); // High C tone for jump
    }

    // Keep within screen bounds
    if (g_PosX < -0.85f) g_PosX = -0.85f;
    if (g_PosX > 0.85f) g_PosX = 0.85f;
}

void updatePhysics() {
    // Gravity simulation
    if (!g_IsGrounded) {
        g_VelY -= 0.0018f;
        g_PosY += g_VelY;
        if (g_PosY <= -0.5f) {
            g_PosY = -0.5f;
            g_VelY = 0.0f;
            g_IsGrounded = true;
            playWebAudioTone(220.0f, 0.08f); // Thud tone on landing
        }
    }
}

void renderQuad(float x, float y, float scaleX, float scaleY, float r, float g, float b) {
    glUseProgram(g_Shader);
    GLint uOffset = glGetUniformLocation(g_Shader, "uOffset");
    GLint uScale  = glGetUniformLocation(g_Shader, "uScale");
    GLint uColor  = glGetUniformLocation(g_Shader, "uColor");

    glUniform2f(uOffset, x, y);
    glUniform2f(uScale, scaleX, scaleY);
    glUniform3f(uColor, r, g, b);

    glBindVertexArray(g_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void mainLoopIteration() {
    glfwPollEvents();
    processInput();
    updatePhysics();

    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Ground
    renderQuad(0.0f, -0.75f, 1.0f, 0.25f, 0.15f, 0.22f, 0.18f);

    // Draw Player Sprite / Quad
    renderQuad(g_PosX, g_PosY, g_PlayerScale, g_PlayerScale, 0.2f, 0.7f, 1.0f);

    glfwSwapBuffers(g_Window);
}

int main() {
    std::cout << "[Lab2] Initializing WebGL 2D Game Sprite with Web Audio...\n";

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    g_Window = glfwCreateWindow(800, 600, "Lab 2: WebGL 2D Game Sprite", nullptr, nullptr);
    if (!g_Window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_Window);
    initQuad();
    g_Shader = createProgram(vsSrc, fsSrc);
    g_PosY = -0.5f;

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

