@echo off
setlocal enabledelayedexpansion
title WebGL / Emscripten Uninstaller (Admin)

:: Self-Elevation to Administrator
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if '%errorlevel%' NEQ '0' (
    echo [INFO] Requesting Administrative Privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin_uninst.vbs"
    set params=%*
    echo UAC.ShellExecute "cmd.exe", "/c """"%~s0"" %params%""", "", "runas", 1 >> "%temp%\getadmin_uninst.vbs"
    "%temp%\getadmin_uninst.vbs"
    del "%temp%\getadmin_uninst.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"

echo ==============================================================================
echo               WebGL / Emscripten Toolchain Uninstaller
echo ==============================================================================
echo.

set /p CONFIRM="Are you sure you want to uninstall Emscripten and remove it from PATH? (y/n): "
if /i not "%CONFIRM%"=="y" (
    echo Operation cancelled by user.
    pause
    exit /b 0
)

echo.
echo [1/2] Removing Emscripten paths from System and User PATH...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$cleanTarget = { param($target) " ^
    "  $oldPath = [Environment]::GetEnvironmentVariable('Path', $target); " ^
    "  if ($oldPath) { " ^
    "    $paths = $oldPath -split ';' | Where-Object { $_ -ne '' -and $_ -notlike '*emsdk*' }; " ^
    "    $newPath = ($paths -join ';'); " ^
    "    [Environment]::SetEnvironmentVariable('Path', $newPath, $target); " ^
    "  } " ^
    "}; " ^
    "& $cleanTarget 'Machine'; " ^
    "& $cleanTarget 'User'; " ^
    "[Environment]::SetEnvironmentVariable('EMSDK', $null, 'Machine'); " ^
    "[Environment]::SetEnvironmentVariable('EM_CACHE', $null, 'Machine'); " ^
    "[Environment]::SetEnvironmentVariable('EMSDK', $null, 'User'); " ^
    "[Environment]::SetEnvironmentVariable('EM_CACHE', $null, 'User'); " ^
    "Write-Host '[OK] Removed EMSDK variables and PATH entries.'"

echo.
echo [2/2] Removing Emscripten files from disk...
if exist "C:\emsdk" (
    set /p DEL_FILES="Delete toolchain files at 'C:\emsdk'? (y/n) [default: y]: "
    if "%DEL_FILES%"=="" set DEL_FILES=y
    if /i "%DEL_FILES%"=="y" (
        echo Deleting C:\emsdk (this may take a minute)...
        rmdir /s /q "C:\emsdk"
        if exist "C:\emsdk" (
            echo [WARN] Some locked files could not be deleted immediately.
        ) else (
            echo [OK] C:\emsdk successfully deleted.
        )
    ) else (
        echo Preserving C:\emsdk directory on disk.
    )
)

echo.
echo ==============================================================================
echo [SUCCESS] WebGL / Emscripten environment uninstalled cleanly.
echo ==============================================================================
echo.
pause

