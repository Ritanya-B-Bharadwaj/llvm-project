 @echo off
REM Run the Clang AST Line Mapper Web Application

echo Installing required dependencies...
pip install -r web_app/requirements.txt

echo Starting Clang AST Line Mapper Web Application...
python web_app/app.py

if %ERRORLEVEL% NEQ 0 (
    echo Error starting the application. Please check that all dependencies are installed.
    echo Run: pip install -r web_app/requirements.txt
    pause
)
