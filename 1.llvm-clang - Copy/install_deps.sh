#!/usr/bin/env bash
set -e

# Create Python virtual environment and install python dependencies
if [ ! -d venv ]; then
    python3 -m venv venv
fi
source venv/bin/activate
pip install --upgrade pip
pip install google-genai

# Install clang and LLVM with OpenMP support
if command -v apt-get >/dev/null; then
    sudo apt-get update && sudo apt-get install -y clang llvm libomp-dev cmake
else
    echo "Package manager not supported. Please install clang/llvm manually."
fi
