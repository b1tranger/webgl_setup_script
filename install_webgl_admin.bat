@echo off
setlocal enabledelayedexpansion
title WebGL 2.0 / Emscripten One-Click Setup (Admin)

:: ==============================================================================
:: Self-Elevation to Administrator
:: ==============================================================================
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if '%errorlevel%' NEQ '0' (
    echo [INFO] Requesting Administrative Privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin_webgl.vbs"
    set params=%*
    echo UAC.ShellExecute "cmd.exe", "/c """"%~s0"" %params%""", "", "runas", 1 >> "%temp%\getadmin_webgl.vbs"
    "%temp%\getadmin_webgl.vbs"
    del "%temp%\getadmin_webgl.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"

echo ==============================================================================
echo                 WebGL 2.0 / WebAssembly Toolchain Installer
echo                 (Emscripten SDK + LLVM Wasm + Node + Python)
echo ==============================================================================
echo.
echo Target Installation Directory: C:\emsdk
echo.

set "EMSDK_DIR=C:\emsdk"

:: Step 1: Check or Clone emsdk
if exist "%EMSDK_DIR%\emsdk.bat" (
    echo [1/4] Emscripten SDK repository detected at %EMSDK_DIR%. Updating...
    pushd "%EMSDK_DIR%"
    call git pull origin main >nul 2>nul
    popd
) else (
    echo [1/4] Cloning Emscripten SDK repository to %EMSDK_DIR%...
    where git >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        git clone https://github.com/emscripten-core/emsdk.git "%EMSDK_DIR%"
    ) else (
        echo [INFO] Git not found. Downloading Emscripten SDK archive via PowerShell...
        powershell -NoProfile -ExecutionPolicy Bypass -Command ^
            "$zip = Join-Path $env:TEMP 'emsdk-main.zip'; [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/emscripten-core/emsdk/archive/refs/heads/main.zip' -OutFile $zip; Expand-Archive -Path $zip -DestinationPath $env:TEMP -Force; Move-Item -Path (Join-Path $env:TEMP 'emsdk-main') -Destination 'C:\emsdk' -Force; Remove-Item $zip -Force"
    )
)

if not exist "%EMSDK_DIR%\emsdk.bat" (
    echo.
    echo [ERROR] Failed to obtain Emscripten SDK at %EMSDK_DIR%.
    echo Please ensure you have internet access and try again.
    pause
    exit /b 1
)

:: Step 2: Install latest tools (LLVM backend, clang, binaryen, node)
echo.
echo [2/4] Installing latest Emscripten toolchain (this may take a few minutes)...
pushd "%EMSDK_DIR%"
call emsdk.bat install latest
if %ERRORLEVEL% neq 0 (
    echo [ERROR] emsdk install failed!
    popd
    pause
    exit /b %ERRORLEVEL%
)

:: Step 3: Activate latest tools permanently
echo.
echo [3/4] Activating latest Emscripten SDK and writing environment...
call emsdk.bat activate latest --permanent
popd

:: Step 4: Configure Machine Environment Variables & System PATH
echo.
echo [4/4] Registering Emscripten to System Environment & Setting Cache...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$emsdk = 'C:\emsdk'; " ^
    "$upstream = 'C:\emsdk\upstream\emscripten'; " ^
    "[Environment]::SetEnvironmentVariable('EMSDK', $emsdk, 'Machine'); " ^
    "[Environment]::SetEnvironmentVariable('EM_CACHE', 'C:\emsdk\cache', 'Machine'); " ^
    "$oldPath = [Environment]::GetEnvironmentVariable('Path', 'Machine'); " ^
    "$paths = $oldPath -split ';' | Where-Object { $_ -ne '' }; " ^
    "if ($paths -notcontains $emsdk) { $paths += $emsdk }; " ^
    "if ($paths -notcontains $upstream) { $paths += $upstream }; " ^
    "$newPath = ($paths -join ';'); " ^
    "[Environment]::SetEnvironmentVariable('Path', $newPath, 'Machine'); " ^
    "Write-Host '[OK] System environment variables and PATH configured successfully.'"

echo.
echo ==============================================================================
echo [SUCCESS] WebGL 2.0 / WebAssembly Environment Successfully Installed!
echo.
echo Installed Components:
echo   - Emscripten Compiler (emcc, em++)
echo   - LLVM & Clang WebAssembly Backend
echo   - Binaryen WebAssembly Optimizer
echo   - Embedded Node.js runtime
echo   - Local HTTP CORS bypass tools
echo.
echo Next Steps:
echo   1. Run 'test_webgl_env.bat' to verify your compiler.
echo   2. Open 'PROJECTS\create_webgl_project.bat' to start a new project.
echo   3. Explore ready-to-run demos in 'sample_projects\'.
echo ==============================================================================
echo.
pause

