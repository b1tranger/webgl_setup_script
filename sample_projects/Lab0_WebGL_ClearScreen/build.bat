@echo off
setlocal enabledelayedexpansion
title Lab 0: WebGL Clear Screen

echo Building Lab 0: WebGL Clear Screen...

where em++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    if exist "C:\emsdk\emsdk_env.bat" (
        call "C:\emsdk\emsdk_env.bat" >nul 2>nul
    ) else (
        echo [ERROR] Emscripten compiler (em++) not found. Run install_webgl_admin.bat first.
        pause
        exit /b 1
    )
)

if not exist "dist" mkdir "dist"

em++ src\main.cpp ^
    -std=c++17 -O2 ^
    -s USE_GLFW=3 ^
    -s FULL_ES3=1 ^
    -s USE_WEBGL2=1 ^
    -s WASM=1 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    --shell-file shell_minimal.html ^
    -o dist\index.html

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo [OK] Build successful: dist\index.html
start http://localhost:8080/index.html
python -m http.server 8080 --directory dist

