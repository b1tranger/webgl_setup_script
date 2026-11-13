@echo off
setlocal enabledelayedexpansion
title WebGL / Emscripten Environment Diagnostic

echo ==============================================================================
echo              WebGL / Emscripten Development Environment Diagnostic
echo ==============================================================================
echo.

set "FAILURES=0"

:: 1. Check Git
where git >nul 2>nul
if %ERRORLEVEL% equ 0 (
    for /f "tokens=*" %%i in ('git --version 2^>nul') do echo [PASS] Git: %%i
) else (
    echo [WARN] Git is not found in PATH (recommended for managing emsdk updates).
)

:: 2. Check Python
where python >nul 2>nul
if %ERRORLEVEL% equ 0 (
    for /f "tokens=*" %%i in ('python --version 2^>nul') do echo [PASS] Python: %%i
) else (
    echo [FAIL] Python is NOT found in PATH.
    set /a FAILURES+=1
)

:: 3. Check Node.js
where node >nul 2>nul
if %ERRORLEVEL% equ 0 (
    for /f "tokens=*" %%i in ('node --version 2^>nul') do echo [PASS] Node.js: %%i
) else (
    echo [WARN] Node.js is NOT found in PATH (emsdk provides its own internal node if needed).
)

:: 4. Check emsdk folder
if exist "C:\emsdk\emsdk.bat" (
    echo [PASS] Emscripten SDK directory found at C:\emsdk
) else (
    echo [FAIL] Emscripten SDK not found at C:\emsdk
    set /a FAILURES+=1
)

:: 5. Check em++
where em++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [INFO] em++ not detected directly in active PATH. Testing C:\emsdk\emsdk_env.bat...
    if exist "C:\emsdk\emsdk_env.bat" (
        call "C:\emsdk\emsdk_env.bat" >nul 2>nul
    )
)

where em++ >nul 2>nul
if %ERRORLEVEL% equ 0 (
    for /f "tokens=*" %%i in ('em++ --version 2^>nul ^| findstr /i "emcc"') do echo [PASS] Emscripten Compiler: %%i
) else (
    echo [FAIL] Emscripten compiler (em++) could not be found or loaded!
    set /a FAILURES+=1
)

echo.
echo ------------------------------------------------------------------------------
echo Testing Live WebAssembly Compilation...
echo ------------------------------------------------------------------------------

set "TEST_DIR=%TEMP%\webgl_test_%RANDOM%"
mkdir "%TEST_DIR%" 2>nul
echo #include ^<emscripten.h^> > "%TEST_DIR%\test.cpp"
echo int main() { return 0; } >> "%TEST_DIR%\test.cpp"

em++ "%TEST_DIR%\test.cpp" -s WASM=1 -o "%TEST_DIR%\test.html" >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [PASS] Wasm compilation succeeded! (LLVM + Clang + Binaryen are working).
) else (
    echo [FAIL] Test WebAssembly compilation failed. Check your compiler installation.
    set /a FAILURES+=1
)

rmdir /s /q "%TEST_DIR%" 2>nul

echo.
echo ==============================================================================
if %FAILURES% equ 0 (
    echo [ALL CHECKS PASSED] Your system is fully ready for WebGL 2.0 / C++ development!
) else (
    echo [ATTENTION] %FAILURES% check(s) failed or require attention.
    echo Please run 'install_webgl_admin.bat' with Administrator privileges.
)
echo ==============================================================================
echo.
pause

