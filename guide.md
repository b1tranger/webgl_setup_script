# Comprehensive WebGL 2.0 & WebAssembly Development Guide

A complete technical manual for compiling, running, and publishing modern C++ graphics applications and games to the web using **WebGL 2.0**, **WebAssembly (WASM)**, and the **Emscripten SDK**.

---

## Table of Contents
1. [Desktop OpenGL vs. WebGL 2.0 & WebAssembly](#1-desktop-opengl-vs-webgl-20--webassembly)
2. [The Browser Execution Model & Non-Blocking Loops](#2-the-browser-execution-model--non-blocking-loops)
3. [The Emscripten Toolchain Architecture](#3-the-emscripten-toolchain-architecture)
4. [Shader Language Differences (Desktop GLSL vs. GLSL ES 3.00)](#4-shader-language-differences-desktop-glsl-vs-glsl-es-300)
5. [Asset Packaging & Virtual File Systems (MEMFS)](#5-asset-packaging--virtual-file-systems-memfs)
6. [Web Audio & Browser Autoplay Policies](#6-web-audio--browser-autoplay-policies)
7. [Local Testing & The CORS Security Constraint](#7-local-testing--the-cors-security-constraint)
8. [Publishing to itch.io (Step-by-Step)](#8-publishing-to-itchio-step-by-step)
9. [Windows Pitfalls & Troubleshooting Matrix](#9-windows-pitfalls--troubleshooting-matrix)

---

## 1. Desktop OpenGL vs. WebGL 2.0 & WebAssembly

When developing in C++, desktop graphics and web graphics share the conceptual state machine of OpenGL, but differ substantially in their platform architectures:

| Architectural Component | Desktop Modern OpenGL (3.3 Core) | WebGL 2.0 / WebAssembly |
| :--- | :--- | :--- |
| **Binary Output** | Native x86_64 `.exe` machine code | Portable `.wasm` bytecode + `.js` runtime glue |
| **Graphics API Specification** | OpenGL 3.3 Core Profile | OpenGL ES 3.0 (WebGL 2.0) |
| **Display Surface** | OS Window (Win32 / X11 / Cocoa via GLFW) | HTML5 `<canvas>` element |
| **Shader Directive** | `#version 330 core` | `#version 300 es` |
| **Float Precision Qualifiers** | Optional / Ignored by desktop drivers | **Mandatory** in Fragment Shaders (`precision mediump float;`) |
| **Main Loop Model** | Blocking `while (!glfwWindowShouldClose())` | Non-blocking callback via `emscripten_set_main_loop()` |
| **File I/O** | Native OS filesystem (`fopen`, `std::ifstream`) | In-memory virtual filesystem (MEMFS) via `--preload-file` |
| **Audio Pipeline** | DirectSound, XAudio2, ALSA, PulseAudio | Web Audio API (`AudioContext`) |

---

## 2. The Browser Execution Model & Non-Blocking Loops

On desktop systems, your application thread owns the CPU core and can block indefinitely in a loop:
```cpp
// Desktop OpenGL Pattern (DO NOT USE IN BROWSER)
while (!glfwWindowShouldClose(window)) {
    processInput();
    render();
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

In a web browser, JavaScript and the DOM run on a **single unified UI thread**. If a C++ WebAssembly module enters an infinite loop, it completely locks the browser tab, preventing user input, window resizing, rendering, and garbage collection. The browser will flag the page as "Unresponsive" and terminate the tab.

### The Solution: `emscripten_set_main_loop`
Emscripten bridges the C++ main loop with the browser's native `window.requestAnimationFrame()` mechanism:

```cpp
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

void mainLoopIteration() {
    processInput();
    render();
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    initWindowAndGL();
    // 0 fps = bind to requestAnimationFrame (matches monitor refresh rate, e.g. 60Hz/144Hz)
    // 1 simulate_infinite_loop = safely unwind stack
    emscripten_set_main_loop(mainLoopIteration, 0, 1);
    return 0;
}
#endif
```

---

## 3. The Emscripten Toolchain Architecture

Emscripten operates as a complete cross-compiler pipeline:
1. **Frontend (`clang++`)**: Parses standard C++17/20 code and headers into LLVM Intermediate Representation (LLVM IR).
2. **Optimizer & Codegen (`llc` / `lld`)**: Transforms LLVM IR into WebAssembly binary modules (`.wasm`).
3. **Binaryen (`wasm-opt`)**: Optimizes the WebAssembly binary, performing dead-code stripping, function inlining, and memory layout optimization.
4. **JavaScript Glue (`index.js`)**: Provides POSIX emulation, memory management (`Module.HEAPU8`), WebGL context bridging, and canvas event listeners.
5. **HTML Shell (`index.html`)**: Embeds the canvas, status indicators, and boots the runtime.

```plaintext
[ C++ Source Files (.cpp) ]
            │
            ▼
    [ Clang++ / LLVM ]  ───►  [ LLVM IR Bitcode ]
            │
            ▼
     [ LLD Linker ]     ───►  [ Unoptimized .wasm ]
            │
            ▼
  [ Binaryen (wasm-opt) ] ───►  [ Optimized index.wasm ]
            │
            ▼
   +─────────────────────────────────────────────+
   │ Final Web Bundle:                           │
   │  - index.html   (DOM & Canvas Shell)        │
   │  - index.js     (Emscripten Runtime Glue)   │
   │  - index.wasm   (Compiled WebAssembly Code) │
   │  - index.data   (Packaged Asset Filesystem) │
   +─────────────────────────────────────────────+
```

---

## 4. Shader Language Differences (Desktop GLSL vs. GLSL ES 3.00)

WebGL 2.0 implements the **OpenGL ES 3.0** specification. Shaders written for desktop OpenGL 3.3 Core must be updated as follows:

### 1. Version Header
- Desktop: `#version 330 core`
- WebGL 2.0: `#version 300 es`

### 2. Mandatory Precision Qualifiers
In desktop GLSL, floating point precision defaults to 32-bit `highp`. In OpenGL ES / WebGL 2.0 fragment shaders, **there is no default float precision**. Omitting the precision qualifier causes an immediate compile error:

```glsl
#version 300 es
// MANDATORY for WebGL 2.0 Fragment Shaders:
precision mediump float; // or precision highp float;

in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
```

---

## 5. Asset Packaging & Virtual File Systems (MEMFS)

WebAssembly runs inside an isolated browser sandbox with no direct access to the user's hard drive. Standard C runtime calls like `fopen("assets/player.png", "rb")` fail unless the files are mapped into Emscripten's virtual filesystem (MEMFS).

### Preloading Assets
Add `--preload-file assets` to your `em++` compiler command:
```bash
em++ src/main.cpp -O2 -s USE_GLFW=3 -s FULL_ES3=1 -s USE_WEBGL2=1 \
     --preload-file assets \
     --shell-file shell_minimal.html \
     -o dist/index.html
```

This generates `dist/index.data` containing your assets. When the page loads, Emscripten automatically downloads `index.data`, populates the virtual in-memory filesystem, and allows standard C/C++ file routines (`fopen`, `std::ifstream`, `stbi_load`) to open `"assets/player.png"` transparently.

---

## 6. Web Audio & Browser Autoplay Policies

Desktop games initialize sound drivers on startup. Web browsers, however, enforce a strict **User Gesture Autoplay Policy**: an `AudioContext` cannot produce audio until the user has interacted with the webpage (e.g. by clicking, pressing a key, or tapping the screen).

### The Lightweight Web Audio Bridge (`EM_JS`)
Instead of linking bulky audio libraries, you can directly synthesize or play audio via HTML5 Web Audio using Emscripten's `EM_JS` macro:

```cpp
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

EM_JS(void, playRetroTone, (float frequency, float duration), {
    try {
        var AudioCtx = window.AudioContext || window.webkitAudioContext;
        if (!window._gameAudioContext) {
            window._gameAudioContext = new AudioCtx();
        }
        var ctx = window._gameAudioContext;
        if (ctx.state === 'suspended') {
            ctx.resume(); // Automatically unlocks on first player input
        }
        var osc = ctx.createOscillator();
        var gain = ctx.createGain();
        osc.frequency.setValueAtTime(frequency, ctx.currentTime);
        gain.gain.setValueAtTime(0.3, ctx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.001, ctx.currentTime + duration);
        osc.connect(gain);
        gain.connect(ctx.destination);
        osc.start();
        osc.stop(ctx.currentTime + duration);
    } catch(e) {
        console.warn('Audio playback error:', e);
    }
});
#endif
```

---

## 7. Local Testing & The CORS Security Constraint

If you double-click `dist/index.html` directly in Windows Explorer, your browser opens it using the `file://` protocol (`file:///C:/path/dist/index.html`).

Modern browsers block WebAssembly loading and `fetch()` requests on the `file://` protocol due to **CORS (Cross-Origin Resource Sharing)** security restrictions. The browser console will display:
```plaintext
Access to fetch at 'file:///.../index.wasm' from origin 'null' has been blocked by CORS policy.
Failed to load resource: net::ERR_FAILED
```

### How to Preview Locally:
Always serve the files through a local HTTP server. You can:
1. Double-click **`Dependencies\server\run_local_server.bat`**, or
2. Run from your project directory:
   ```cmd
   python -m http.server 8080 --directory dist
   ```
3. Open `http://localhost:8080` in Chrome, Firefox, or Edge.

---

## 8. Publishing to itch.io (Step-by-Step)

Publishing a WebGL/Wasm game to **itch.io** takes under 2 minutes:

1. **Build Your Game**:
   In your project root, run `build.bat` to produce the `dist/` directory.
2. **Package the Archive**:
   - Open `dist/`.
   - Select all files (`index.html`, `index.js`, `index.wasm`, and optionally `index.data`).
   - Right-click ➔ **Compress to ZIP file**.
   - **Crucial Rule**: `index.html` must sit directly in the root of the `.zip` archive, NOT inside a subfolder!
3. **Upload to itch.io**:
   - Go to your itch.io dashboard and click **Create new project**.
   - Under **Classification**, select **Games**.
   - Under **Kind of project**, choose **HTML** (*You have a ZIP or HTML file that will be played in the browser*).
   - Scroll down to **Uploads** and click **Upload files**. Select your `.zip`.
   - Check the box **"This file will be played in the browser"**.
   - Under **Embed options**:
     - Set viewport dimensions (e.g. `800` width, `600` height, or your custom resolution).
     - Check **"Enable fullscreen button"**.
     - Check **"Automatically start on page load"**.
   - Save and set visibility to **Public** or **Restricted** (for playtesting).

---

## 9. Windows Pitfalls & Troubleshooting Matrix

### 1. Cross-Drive Python Path Bug in Emscripten
- **Symptom**: `ValueError: path is on mount 'C:', start on mount 'D:'` during compilation.
- **Root Cause**: Emscripten's internal cache defaults to the user directory (`C:\Users\...`). When compiling a project on drive `D:`, Python's `os.path.relpath()` fails between different Windows drive letters.
- **Fix**: The installer automatically sets `EM_CACHE=C:\emsdk\cache` as a System Environment Variable so all internal cache paths share drive `C:`.

### 2. Out of Memory During Execution
- **Symptom**: `RuntimeError: memory access out of bounds` or `Cannot enlarge memory arrays`.
- **Root Cause**: By default, Emscripten allocates a fixed 16MB WebAssembly heap.
- **Fix**: Always include `-s ALLOW_MEMORY_GROWTH=1` in your compiler flags so WebAssembly can dynamically expand its heap to accommodate larger textures and game levels.

### 3. Missing `make` Command
- **Symptom**: `'make' is not recognized as an internal or external command`.
- **Fix**: You do not need `make`. Simply run `build.bat`, which invokes `em++` directly. If you prefer `make`, install `mingw32-make` or run within an MSYS2 shell using `emmake make`.

