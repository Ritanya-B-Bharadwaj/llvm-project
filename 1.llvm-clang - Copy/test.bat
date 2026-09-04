@echo off
setlocal enabledelayedexpansion

echo Detecting available CMake generators...
echo.

REM Get available generators
cmake --help > cmake_help.tmp 2>&1

REM Check for Visual Studio generators
findstr /C:"Visual Studio 17 2022" cmake_help.tmp >nul
if !errorlevel! equ 0 (
    echo Found: Visual Studio 17 2022
    set GENERATOR=Visual Studio 17 2022
    goto :found
)

findstr /C:"Visual Studio 16 2019" cmake_help.tmp >nul
if !errorlevel! equ 0 (
    echo Found: Visual Studio 16 2019
    set GENERATOR=Visual Studio 16 2019
    goto :found
)

findstr /C:"Visual Studio 15 2017" cmake_help.tmp >nul
if !errorlevel! equ 0 (
    echo Found: Visual Studio 15 2017
    set GENERATOR=Visual Studio 15 2017
    goto :found
)

REM Check for MinGW
findstr /C:"MinGW Makefiles" cmake_help.tmp >nul
if !errorlevel! equ 0 (
    echo Found: MinGW Makefiles
    set GENERATOR=MinGW Makefiles
    goto :found
)

REM Check for Ninja
findstr /C:"Ninja" cmake_help.tmp >nul
if !errorlevel! equ 0 (
    echo Found: Ninja
    set GENERATOR=Ninja
    goto :found
)

REM Fallback to NMake if available
findstr /C:"NMake Makefiles" cmake_help.tmp >nul
if !errorlevel! equ 0 (
    echo Found: NMake Makefiles
    set GENERATOR=NMake Makefiles
    goto :found
)

echo No suitable generator found. Using default.
set GENERATOR=
goto :end

:found
echo Using generator: !GENERATOR!
echo.
echo Running CMake with detected generator...
cmake -G "!GENERATOR!" -B build -DSOURCE_FILE=test.cpp -DARGS="--annotated;output/annotated.ll;--json;output/mapping.json;--explain;output/explanations.md;"

if !errorlevel! equ 0 (
    echo.
    echo Configuration successful! Now run:
    echo cmake --build build --target run
) else (
    echo.
    echo Configuration failed. Available generators:
    echo.
    cmake --help | findstr /C:"Generators"
)

:end
del cmake_help.tmp 2>nul
pause
 