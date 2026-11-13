# Lab 2: WebGL 2D Game Sprite with Web Audio

## Overview
Demonstrates practical 2D game engine concepts running on the web:
- Keyboard event handling via GLFW (`A`/`D`/Arrow keys to move, `Space`/`W`/Up Arrow to jump)
- Simple Euler integration physics (velocity and gravity)
- Instanced/Uniform transformations (`uOffset`, `uScale`, `uColor`) for rendering sprites and environment quads
- HTML5 Web Audio API synthesis using Emscripten's `EM_JS` bridge (instant retro sound effects without external audio libraries)
- Asset packaging using `--preload-file assets`, creating a virtual in-memory filesystem for WebAssembly

## How to Build & Run
Double-click `build.bat` or run:
```cmd
build.bat
```
or with Make:
```bash
make run
```
This builds into `dist/` and starts the local preview server at `http://localhost:8080`.

