@echo off
setlocal

REM ============================================================
REM VCPilot - Visual Studio Code Development Environment
REM ============================================================

cd /d "%~dp0"

REM ------------------------------------------------------------
REM Locate Visual Studio
REM ------------------------------------------------------------

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found.
    pause
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`
    "%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
`) do (
    set "VS_PATH=%%i"
)

if not defined VS_PATH (
    echo [ERROR] Visual Studio with C++ tools not found.
    pause
    exit /b 1
)

REM ------------------------------------------------------------
REM Initialize MSVC x64 environment
REM ------------------------------------------------------------

call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"

if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    pause
    exit /b 1
)

REM ------------------------------------------------------------
REM Verify development tools
REM ------------------------------------------------------------

echo.
echo ============================================================
echo VCPilot Development Environment
echo ============================================================
echo.
echo Project:
echo   %CD%
echo.

echo MSVC:
where cl
echo.

echo CMake:
where cmake
echo.

echo Ninja:
where ninja
echo.

echo Conan:
where conan
echo.

echo VS Code:
where code
echo.

REM ------------------------------------------------------------
REM Open VS Code
REM ------------------------------------------------------------

code .

endlocal
exit