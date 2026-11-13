@echo off
setlocal enabledelayedexpansion
title WebGL 2.0 Project Scaffolder

echo ==============================================================================
echo                      WebGL 2.0 Project Scaffolder
echo ==============================================================================
echo.

set /p PROJ_NAME="Enter new project name (e.g. SpaceInvaders_Web): "
if "%PROJ_NAME%"=="" (
    echo [ERROR] Project name cannot be empty.
    pause
    exit /b 1
)

:: Sanitize spaces
set "PROJ_NAME=%PROJ_NAME: =_%"

set "SCRIPT_DIR=%~dp0"
set "TARGET_DIR=%SCRIPT_DIR%%PROJ_NAME%"
set "TEMPLATE_DIR=%SCRIPT_DIR%..\Dependencies\template"

if exist "%TARGET_DIR%" (
    echo [ERROR] Directory '%TARGET_DIR%' already exists!
    pause
    exit /b 1
)

if not exist "%TEMPLATE_DIR%" (
    echo [ERROR] Template folder not found at '%TEMPLATE_DIR%'!
    pause
    exit /b 1
)

echo.
echo [1/3] Creating directory structure for '%PROJ_NAME%'...
mkdir "%TARGET_DIR%"
mkdir "%TARGET_DIR%\include"
mkdir "%TARGET_DIR%\src"
mkdir "%TARGET_DIR%\assets"

echo [2/3] Copying boilerplate template files...
copy /y "%TEMPLATE_DIR%\include\Common.h" "%TARGET_DIR%\include\" >nul
copy /y "%TEMPLATE_DIR%\src\main.cpp" "%TARGET_DIR%\src\" >nul
copy /y "%TEMPLATE_DIR%\shell_minimal.html" "%TARGET_DIR%\" >nul
copy /y "%TEMPLATE_DIR%\Makefile" "%TARGET_DIR%\" >nul
copy /y "%TEMPLATE_DIR%\build.bat" "%TARGET_DIR%\" >nul

:: Create sample asset readme
echo Place your game textures, sounds, and assets here. > "%TARGET_DIR%\assets\readme.txt"

echo [3/3] Customizing shell title for '%PROJ_NAME%'...
powershell -NoProfile -Command "(Get-Content -Path '%TARGET_DIR%\shell_minimal.html' -Raw) -replace 'WebGL 2.0 Application', '%PROJ_NAME%' | Set-Content -Path '%TARGET_DIR%\shell_minimal.html'"

echo.
echo ==============================================================================
echo [SUCCESS] Project created at: %TARGET_DIR%
echo.
echo Quick Start:
echo   1. Navigate to: cd %TARGET_DIR%
echo   2. Edit your C++ code in src\main.cpp
echo   3. Run build.bat to compile and launch your browser!
echo ==============================================================================
echo.

set /p OPEN_NOW="Open project folder in File Explorer? (y/n) [default: y]: "
if "%OPEN_NOW%"=="" set OPEN_NOW=y
if /i "%OPEN_NOW%"=="y" (
    explorer "%TARGET_DIR%"
)

pause

