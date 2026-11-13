# Lab 0: WebGL 2.0 Clear Screen

## Overview
This is the minimal "Hello World" of WebGL 2.0 and WebAssembly using C++ and Emscripten. It demonstrates:
- GLFW3 window and context creation in a browser canvas.
- Using `emscripten_set_main_loop()` to bind C++ render ticks to the browser's `requestAnimationFrame` loop.
- Dynamic color animation clearing the framebuffer every frame.

## How to Build & Run
Double-click `build.bat` or run:
```cmd
build.bat
```
or with Make:
```bash
make run
```
This compiles `src/main.cpp` into `dist/index.html` and launches a local web server at `http://localhost:8080`.

