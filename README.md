# WebGL 2.0 & WebAssembly (C++ / Emscripten) 1-Click Setup & Automation

An automated setup package and comprehensive technical toolkit for developing **WebGL 2.0 and WebAssembly applications in C++** on Windows environments (Windows 10 / 11 64-Bit). Automates Emscripten SDK (`emsdk`) deployment, LLVM & Clang WebAssembly compiler toolchain installation, safe Windows System `PATH` configuration, local CORS-bypassing HTTP servers, boilerplate project generators, and turnkey sample labs.

Build your C++ graphics applications and games once, and publish directly to the web, browser canvases, or **itch.io**!

---

> [!IMPORTANT]
> 📖 **Full Technical & Troubleshooting Guide Available**  
> For in-depth explanations of WebGL 2.0 architecture, WebAssembly memory pipelines, Web Audio JavaScript bridges, itch.io packaging, and CORS security:  
> 👉 **[Read the complete technical guide in guide.md](guide.md)**

---

## ⚡ Quick Start (Automated Setup)

### 1. Clone or Download the Repository

- **Clone with Git:**
  ```bash
  git clone <YOUR_REPOSITORY_URL>/webgl_setup_script.git
  cd webgl_setup_script
  ```

---

### 2. Run the Automated Installer

1. Right-click **`install_webgl_admin.bat`** and select **Run as Administrator** (or double-click to run with automatic UAC self-elevation).
2. The script executes the automated 4-step setup workflow:
   - **[1/4] Emscripten SDK Detection & Download**: Clones or downloads `emsdk` directly into `C:\emsdk`.
   - **[2/4] Toolchain Provisioning**: Installs the latest Clang/LLVM WebAssembly compiler backend, Binaryen optimizer, and Node.js runtime.
   - **[3/4] Toolchain Activation**: Permanently configures SDK profiles.
   - **[4/4] Environment & PATH Registration**: Registers `C:\emsdk`, `C:\emsdk\upstream\emscripten`, and localizes `EM_CACHE` to `C:\emsdk\cache` to prevent Windows cross-drive python path bugs.

---

### 3. Verify the Environment

Run **`test_webgl_env.bat`** (no administrator privileges needed):
- Tests Python, Node.js, and Git.
- Validates `em++` compiler availability.
- Performs an end-to-end compilation test of a live WebAssembly bundle to ensure LLVM and Binaryen function without issues.

---

### 4. Create a New WebGL Project or Run Demos

- **Create a New Project:**
  Double-click `PROJECTS\create_webgl_project.bat`. Enter a project name (e.g. `MyWebGLGame`). The scaffolder creates a complete project folder with pre-configured `src/main.cpp`, `Common.h`, responsive `shell_minimal.html`, `Makefile`, and `build.bat`.

- **Run Sample Demos:**
  Navigate into any directory inside `sample_projects/` and double-click `build.bat`:
  - **`Lab0_WebGL_ClearScreen`**: Minimal WebGL 2.0 clear screen and dynamic color cycle.
  - **`Lab1_WebGL_Triangle`**: Modern WebGL 2.0 RGB triangle using GLSL ES 3.00, VAO, and VBO.
  - **`Lab2_WebGL_2D_Game_Sprite`**: 2D interactive player physics, keyboard controls, Web Audio tone synthesis via `EM_JS`, and `--preload-file assets` packaging.

---

## 📁 Repository Directory Structure

```plaintext
webgl_setup_script/
├── install_webgl_admin.bat        # 1-Click self-elevating setup script
├── uninstall_webgl_admin.bat      # Clean uninstaller (removes PATH & toolchain)
├── test_webgl_env.bat             # Diagnostic tool for compiler & environment
├── README.md                      # Overview & Quick Start guide
├── guide.md                       # Comprehensive WebGL & Emscripten manual
│
├── Dependencies/
│   ├── server/
│   │   └── run_local_server.bat   # 1-Click local HTTP preview server (CORS bypass)
│   └── template/
│       ├── include/
│       │   └── Common.h           # Unified header for GLES3, GLFW3, and WebAssembly
│       ├── src/
│       │   └── main.cpp           # Standard boilerplate WebGL 2.0 entry point
│       ├── shell_minimal.html     # Responsive HTML5 canvas & loader template
│       ├── Makefile               # GNU Make build script
│       └── build.bat              # Turnkey Windows build and preview script
│
├── PROJECTS/
│   └── create_webgl_project.bat   # Interactive project scaffolder
│
└── sample_projects/
    ├── Lab0_WebGL_ClearScreen/    # Minimal clear screen demo
    ├── Lab1_WebGL_Triangle/       # Modern RGB triangle (VAO/VBO/GLSL 3.00 ES)
    └── Lab2_WebGL_2D_Game_Sprite/ # 2D game sprite, Web Audio, asset packaging
```

---

## 🌐 Deploying to itch.io (Web Playable Game)

To make your game playable directly in web browsers on itch.io:

1. In your project folder, compile your game:
   ```cmd
   build.bat
   ```
2. Your build artifacts will be created in `dist/`:
   - `index.html`
   - `index.js`
   - `index.wasm`
   - `index.data` (if assets are bundled)
3. Select all files **inside** `dist/` and compress them directly into a `.zip` archive (ensure `index.html` is at the root of the zip, not nested in a subfolder).
4. On itch.io:
   - Create a new project.
   - Set **Kind of project** to **HTML**.
   - Upload your `.zip` and check **"This file will be played in the browser"**.
   - Recommended Viewport dimensions: **800 x 600** (or your canvas dimensions). Check **"Enable fullscreen button"**.

---

## 🧹 Clean Uninstallation

If you ever wish to remove the WebGL toolchain:
1. Right-click **`uninstall_webgl_admin.bat`** and select **Run as Administrator**.
2. The uninstaller removes `EMSDK` environment variables and cleanly purges Emscripten entries from Windows System `PATH`.
3. Confirms before optionally deleting `C:\emsdk` from disk.

