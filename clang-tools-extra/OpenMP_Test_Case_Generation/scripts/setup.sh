#!/bin/bash

# OpenMP Test Generator Environment Setup (Non-LLVM Version)
echo "Setting up OpenMP Test Generator environment..."

# Set project root
export PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
echo "Project root: $PROJECT_ROOT"

# Load .env file if it exists
if [ -f "$PROJECT_ROOT/.env" ]; then
    echo "Loading environment variables from .env file..."
    source "$PROJECT_ROOT/.env"
fi

# API Keys validation
if [ -z "$GROQ_API_KEY" ]; then
    echo "⚠️  GROQ_API_KEY not set. Please set it in .env file or export it manually."
    echo "    Get your key from: https://console.groq.com/"
else
    echo "✓ GROQ_API_KEY is set"
fi

if [ -z "$GITHUB_TOKEN" ]; then
    echo "ℹ️  GITHUB_TOKEN not set (optional, but recommended for higher API rate limits)"
    echo "    Get your token from: https://github.com/settings/tokens"
else
    echo "✓ GITHUB_TOKEN is set"
fi

# Create necessary directories
mkdir -p "$PROJECT_ROOT/outputs"
mkdir -p "$PROJECT_ROOT/build"

# Check for required tools
echo ""
echo "Checking system requirements..."

# Check C++ compiler
if command -v g++ &>/dev/null; then
    echo "✓ g++ compiler: $(g++ --version | head -1)"
else
    echo "✗ g++ compiler not found. Please install build-essential."
fi

# Check CMake
if command -v cmake &>/dev/null; then
    echo "✓ CMake: $(cmake --version | head -1)"
else
    echo "✗ CMake not found. Please install cmake."
fi

# Check pkg-config
if command -v pkg-config &>/dev/null; then
    echo "✓ pkg-config available"
else
    echo "✗ pkg-config not found. Please install pkg-config."
fi

# Check required libraries
echo ""
echo "Checking required libraries..."

if pkg-config --exists libcurl; then
    echo "✓ libcurl: $(pkg-config --modversion libcurl)"
else
    echo "✗ libcurl not found. Please install libcurl4-openssl-dev."
fi

if pkg-config --exists sqlite3; then
    echo "✓ sqlite3: $(pkg-config --modversion sqlite3)"
else
    echo "✗ sqlite3 not found. Please install libsqlite3-dev."
fi

# Check for nlohmann-json
if [ -f "/usr/include/nlohmann/json.hpp" ] || [ -f "/usr/local/include/nlohmann/json.hpp" ] || [ -f "/opt/homebrew/include/nlohmann/json.hpp" ]; then
    echo "✓ nlohmann-json library found"
else
    echo "⚠️  nlohmann-json library not found. Please install nlohmann-json3-dev."
fi

# Optional: Check for clang (for pattern extraction)
if command -v clang &>/dev/null; then
    echo "✓ clang available for pattern extraction: $(clang --version | head -1)"
else
    echo "ℹ️  clang not found (optional for pattern extraction)"
fi

echo ""
echo "Environment setup complete!"
echo ""
echo "Configuration summary:"
echo "  Project root: $PROJECT_ROOT"
echo "  Outputs directory: $PROJECT_ROOT/outputs"
echo "  Build directory: $PROJECT_ROOT/build"
echo ""
echo "Available commands:"
echo "  Build project:     cd build && cmake .. && make"
echo "  Run tool:          ./build/openmp-test-gen --help"
echo "  Clean build:       rm -rf build/*"
echo "  Clean outputs:     rm -rf outputs/*"
echo ""
echo "Example usage:"
echo "  export GROQ_API_KEY='your_api_key_here'"
echo "  ./build/openmp-test-gen --pr 67890 --stage codegen --num-tests 2"
echo ""

# Create sample .env file if it doesn't exist
if [ ! -f "$PROJECT_ROOT/.env" ]; then
    echo "Creating sample .env file..."
    cat > "$PROJECT_ROOT/.env" << 'EOF'
# OpenMP Test Generator Environment Configuration
# Fill in your actual API keys below

# Required: Groq API Key
# Get from: https://console.groq.com/
GROQ_API_KEY=your_groq_api_key_here

# Optional: GitHub Token (recommended for higher API rate limits)
# Get from: https://github.com/settings/tokens
# Only needs 'public_repo' scope for reading public repositories
GITHUB_TOKEN=your_github_token_here
EOF
    echo "✓ Created .env template file. Please edit it with your API keys."
fi

# Function to validate setup
validate_setup() {
    echo ""
    echo "Running setup validation..."
    
    local errors=0
    
    # Check essential tools
    if ! command -v g++ &>/dev/null; then
        echo "✗ Missing g++ compiler"
        ((errors++))
    fi
    
    if ! command -v cmake &>/dev/null; then
        echo "✗ Missing CMake"
        ((errors++))
    fi
    
    # Check essential libraries
    if ! pkg-config --exists libcurl; then
        echo "✗ Missing libcurl development library"
        ((errors++))
    fi
    
    if ! pkg-config --exists sqlite3; then
        echo "✗ Missing sqlite3 development library"
        ((errors++))
    fi
    
    if [ $errors -eq 0 ]; then
        echo "✓ All essential components are available"
        echo "✓ Ready to build the project!"
        return 0
    else
        echo "✗ Found $errors missing components"
        echo "Please run: ./scripts/install_deps.sh"
        return 1
    fi
}

# Run validation
validate_setup

echo ""
echo "Setup script completed. You can now build and run the OpenMP Test Generator!"
