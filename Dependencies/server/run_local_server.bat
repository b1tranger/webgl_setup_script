@echo off
setlocal EnableDelayedExpansion

:: ------------------------------------------------------------
:: 1-Click Local HTTP Server Runner for WebGL & WebAssembly
:: Bypasses browser file:// CORS restrictions and opens localhost
:: ------------------------------------------------------------

set "TARGET_DIR=%~1"
if "%TARGET_DIR%"=="" set "TARGET_DIR=%CD%"
set "PORT=%~2"
if "%PORT%"=="" set "PORT=8080"

echo ============================================================
echo   WebGL Local HTTP Web Server
echo ============================================================
echo   Serving Directory: %TARGET_DIR%
echo   Local Address:     http://localhost:%PORT%
echo ============================================================
echo.

:: Check for Python
where python >nul 2>&1
if %errorLevel% equ 0 (
    echo [*] Starting Python HTTP Server on port %PORT%...
    start "" "http://localhost:%PORT%"
    python -m http.server %PORT% --directory "%TARGET_DIR%"
    goto END
)

:: Check for Node.js / npx
where npx >nul 2>&1
if %errorLevel% equ 0 (
    echo [*] Starting Node.js HTTP Server via npx serve on port %PORT%...
    start "" "http://localhost:%PORT%"
    npx serve -l %PORT% "%TARGET_DIR%"
    goto END
)

:: Fallback check for emrun
where emrun >nul 2>&1
if %errorLevel% equ 0 (
    echo [*] Starting Emscripten emrun web server...
    emrun --port %PORT% "%TARGET_DIR%\index.html"
    goto END
)

echo [ERROR] Neither Python, Node.js (npx), nor emrun was found on PATH.
echo Please install Python 3 or Node.js, or run install_webgl_admin.bat.
pause

:END

