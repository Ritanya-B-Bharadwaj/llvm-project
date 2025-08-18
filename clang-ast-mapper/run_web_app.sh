#!/bin/bash
# Run the Clang AST Line Mapper Web Application

echo "Starting Clang AST Line Mapper Web Application..."
python3 main.py

if [ $? -ne 0 ]; then
    echo "Error starting the application. Please check that all dependencies are installed."
    echo "Run: pip install -r web_app/requirements.txt"
    read -p "Press Enter to continue..."
fi
