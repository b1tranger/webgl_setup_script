#include <cmath>
#include <iostream>

#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

GLFWwindow *g_Window = nullptr;

void mainLoopIteration() {
  glfwPollEvents();

  // Compute dynamic oscillating background color using time
  float time = (float)glfwGetTime();
  float r = (std::sin(time * 1.5f) + 1.0f) * 0.5f * 0.4f;
  float g = (std::sin(time * 2.0f + 1.0f) + 1.0f) * 0.5f * 0.5f;
  float b = (std::sin(time * 1.0f + 2.0f) + 1.0f) * 0.5f * 0.8f;

  glClearColor(r, g, b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glfwSwapBuffers(g_Window);
}

int main() {
  std::cout << "[Lab0] Initializing WebGL 2.0 Clear Screen...\n";

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  g_Window =
      glfwCreateWindow(800, 600, "Lab 0: WebGL Clear Screen", nullptr, nullptr);
  if (!g_Window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(g_Window);

#if defined(__EMSCRIPTEN__)
  // Emscripten non-blocking render loop: 0 fps matches browser
  // requestAnimationFrame
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
