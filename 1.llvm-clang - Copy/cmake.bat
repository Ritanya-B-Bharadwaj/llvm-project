@echo off
setlocal enabledelayedexpansion

REM Simple OpenMP IR Mapper Runner
REM Usage: run.bat <source_file> [additional_args...]

if "%~1"=="" (
    echo.
    echo OpenMP IR Mapper Runner
    echo =======================
    echo.
    echo Usage: run.bat ^<source_file^> [additional_args...]
    echo.
    echo Examples:
    echo   run.bat test.cpp
    echo   run.bat test.cpp --annotated output/annotated.ll
    echo   run.bat test.cpp --annotated output/annotated.ll --json output/mapping.json
    echo   run.bat test.cpp --annotated output/annotated.ll --json output/mapping.json --explain output/explanations.md
    echo.
    echo Full argument support:
    echo   --annotated ^<file^>    Generate annotated LLVM IR
    echo   --json ^<file^>         Generate JSON mapping file
    echo   --explain ^<file^>      Generate explanations file
    echo   --verbose              Enable verbose output
    echo   --help                 Show help
    echo.
    exit /b 1
)

set "SOURCE_FILE=%~1"
shift

REM Collect remaining arguments
set "PYTHON_ARGS="
:collect_args
if "%~1"=="" goto :end_collect
set "PYTHON_ARGS=%PYTHON_ARGS% %~1"
shift
goto :collect_args
:end_collect

REM Trim leading space
set "PYTHON_ARGS=%PYTHON_ARGS:~1%"

echo.
echo ========================================
echo OpenMP IR Mapper
echo ========================================
echo Source File: %SOURCE_FILE%
if not "%PYTHON_ARGS%"=="" (
    echo Arguments: %PYTHON_ARGS%
)
echo ========================================
echo.

REM Check if source file exists
if not exist "%SOURCE_FILE%" (
    echo Error: Source file '%SOURCE_FILE%' not found
    exit /b 1
)

REM Create output directory
if not exist "output" mkdir output

REM Check for Python
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Python not found. Please install Python and ensure it's in your PATH
    exit /b 1
)

REM Run the Python script
echo Running: python ompir_map.py %SOURCE_FILE% %PYTHON_ARGS%
echo.

python ompir_map.py %SOURCE_FILE% %PYTHON_ARGS%
set "EXIT_CODE=%errorlevel%"

echo.
if %EXIT_CODE% equ 0 (
    echo ========================================
    echo SUCCESS: OpenMP IR mapping completed
    echo ========================================
    
    REM Show output files if they exist
    if exist "output\annotated.ll" (
        echo.
        echo Generated annotated IR:
        echo   output\annotated.ll
    )
    if exist "output\mapping.json" (
        echo Generated JSON mapping:
        echo   output\mapping.json
    )
    if exist "output\explanations.md" (
        echo Generated explanations:
        echo   output\explanations.md
    )
) else (
    echo ========================================
    echo FAILED: OpenMP IR mapping failed with exit code %EXIT_CODE%
    echo ========================================
)

exit /b %EXIT_CODE%