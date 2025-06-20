@echo off
setlocal enabledelayedexpansion

echo Setting up dependencies for Windows...

REM Check if Python is available
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python from https://python.org/downloads/
    exit /b 1
)

REM Create Python virtual environment and install dependencies
if not exist venv (
    echo Creating Python virtual environment...
    python -m venv venv
)

echo Activating virtual environment...
call venv\Scripts\activate.bat

echo Upgrading pip...
python -m pip install --upgrade pip

echo Installing Python dependencies...
pip install google-genai

REM Check for package managers and install LLVM/Clang
echo Checking for package managers...

REM Check for Chocolatey
choco --version >nul 2>&1
if %errorlevel% equ 0 (
    echo Found Chocolatey. Installing LLVM and CMake...
    choco install -y llvm cmake
    goto :installed
)

REM Check for Scoop
scoop --version >nul 2>&1
if %errorlevel% equ 0 (
    echo Found Scoop. Installing LLVM and CMake...
    scoop install llvm cmake
    goto :installed
)

REM Check for winget (Windows Package Manager)
winget --version >nul 2>&1
if %errorlevel% equ 0 (
    echo Found winget. Installing LLVM and CMake...
    winget install --id=LLVM.LLVM -e
    winget install --id=Kitware.CMake -e
    goto :installed
)

REM If no package manager found, provide manual installation instructions
echo No supported package manager found (chocolatey, scoop, or winget).
echo.
echo Please install the following manually:
echo.
echo 1. LLVM/Clang:
echo    - Download from: https://releases.llvm.org/download.html
echo    - Or from GitHub: https://github.com/llvm/llvm-project/releases
echo    - Make sure to add LLVM/bin to your PATH
echo.
echo 2. CMake:
echo    - Download from: https://cmake.org/download/
echo    - Or install via Visual Studio Installer
echo.
echo 3. Visual Studio Build Tools (if not already installed):
echo    - Download from: https://visualstudio.microsoft.com/downloads/
echo    - Install "C++ build tools" workload
echo.
echo Alternative package manager installation options:
echo.
echo Using Chocolatey (run as administrator):
echo   Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
echo   choco install -y llvm cmake
echo.
echo Using Scoop:
echo   Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
echo   irm get.scoop.sh ^| iex
echo   scoop install llvm cmake
echo.
goto :end

:installed
echo.
echo Dependencies installed successfully!
echo.
echo To verify installation:
echo   clang --version
echo   cmake --version
echo.

:end
echo.
echo Setup complete! You can now run the OpenMP IR mapping tool.
echo.
pause
