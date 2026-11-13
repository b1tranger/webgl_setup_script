@echo off
setlocal enabledelayedexpansion
title WebGL Project Builder

echo ==============================================================================
echo                      WebGL 2.0 / WebAssembly Project Build
echo ==============================================================================

:: Check if em++ is in PATH
where em++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [INFO] em++ not detected in active PATH. Attempting automatic environment load...
    if exist "C:\emsdk\emsdk_env.bat" (
        call "C:\emsdk\emsdk_env.bat" >nul 2>nul
        echo [OK] Loaded Emscripten environment from C:\emsdk.
    ) else (
        echo [ERROR] Emscripten compiler (em++) not found in PATH or C:\emsdk.
        echo Please run install_webgl_admin.bat first or load emsdk_env.bat.
        pause
        exit /b 1
    )
)

:: Prepare output folder
if not exist "dist" mkdir "dist"

echo.
echo [1/2] Compiling C++ to WebAssembly (WebGL 2.0)...
echo Sources: src\main.cpp
echo Target:  dist\index.html
echo.

em++ src\main.cpp ^
    -Iinclude ^
    -std=c++17 -O2 ^
    -s USE_GLFW=3 ^
    -s FULL_ES3=1 ^
    -s USE_WEBGL2=1 ^
    -s WASM=1 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    --shell-file shell_minimal.html ^
    -o dist\index.html

if %ERRORLEVEL% neq 0 (
    echo.
    echo [BUILD FAILED] An error occurred during compilation.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ==============================================================================
echo [BUILD SUCCESS] HTML5/WebGL bundle generated in dist\
echo   - dist\index.html
echo   - dist\index.js
echo   - dist\index.wasm
echo ==============================================================================
echo.

set /p RUN_SERVER="Start local HTTP preview server now? (y/n) [default: y]: "
if "%RUN_SERVER%"=="" set RUN_SERVER=y

if /i "%RUN_SERVER%"=="y" (
    echo.
    echo Starting local web server at http://localhost:8080 ...
    echo Opening browser automatically...
    start http://localhost:8080/index.html
    where python >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        python -m http.server 8080 --directory dist
    ) else (
        where npx >nul 2>nul
        if %ERRORLEVEL% equ 0 (
            npx serve dist -p 8080
        ) else (
            echo [WARN] Neither Python nor Node.js npx found to serve files.
            echo Please run Dependencies\server\run_local_server.bat
            pause
        )
    )
)

exit /b 0

