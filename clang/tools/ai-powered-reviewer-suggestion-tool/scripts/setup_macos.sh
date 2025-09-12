#!/bin/bash

# AI-Powered Reviewer Suggestion Tool Setup Script for macOS
# This script automates the installation and setup process

set -e  # Exit on any error

echo "🚀 Setting up AI-Powered Reviewer Suggestion Tool for OpenMP PRs"
echo "================================================================"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    print_error "This script is designed for macOS only"
    exit 1
fi

# Check if Xcode Command Line Tools are installed
if ! command -v gcc &> /dev/null; then
    print_warning "Xcode Command Line Tools not found. Installing..."
    xcode-select --install
    print_status "Please complete the Xcode Command Line Tools installation and run this script again"
    exit 1
fi

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    print_warning "Homebrew not found. Installing..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

    # Add Homebrew to PATH for M1 Macs
    if [[ $(uname -m) == "arm64" ]]; then
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
fi

print_status "Installing dependencies via Homebrew..."

# Install required packages
brew update
brew install cmake llvm curl nlohmann-json onnxruntime

print_status "Creating project directories..."
mkdir -p models
mkdir -p build

print_status "Downloading model files..."

# Download ONNX model
if [ ! -f "models/all-MiniLM-L6-v2.onnx" ]; then
    print_status "Downloading all-MiniLM-L6-v2 ONNX model..."
    if command -v wget &> /dev/null; then
        wget -O models/all-MiniLM-L6-v2.onnx https://huggingface.co/onnx-models/all-MiniLM-L6-v2-onnx/resolve/main/model.onnx
    elif command -v curl &> /dev/null; then
        curl -L -o models/all-MiniLM-L6-v2.onnx https://huggingface.co/onnx-models/all-MiniLM-L6-v2-onnx/resolve/main/model.onnx
    else
        print_error "Neither wget nor curl found. Please download the model manually."
        exit 1
    fi
else
    print_status "ONNX model already exists"
fi

# Download vocabulary file
if [ ! -f "models/vocab.txt" ]; then
    print_status "Downloading vocabulary file..."
    if command -v wget &> /dev/null; then
        wget -O models/vocab.txt https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2/resolve/main/vocab.txt
    elif command -v curl &> /dev/null; then
        curl -L -o models/vocab.txt https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2/resolve/main/vocab.txt
    else
        print_error "Neither wget nor curl found. Please download the vocabulary manually."
        exit 1
    fi
else
    print_status "Vocabulary file already exists"
fi

print_status "Building the project..."

cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(sysctl -n hw.ncpu)

print_status "✅ Setup completed successfully!"
echo ""
echo "📋 Next steps:"
echo "1. Get a GitHub API token from: https://github.com/settings/tokens"
echo "2. Run the tool with: ./build/bin/reviewer-suggester --pr <PR_NUMBER> --token <YOUR_TOKEN>"
echo ""
echo "📚 Example usage:"
echo "   ./build/bin/reviewer-suggester --pr 12345 --token ghp_xxxxxxxxxxxx"
echo ""
print_warning "Make sure to keep your GitHub token secure and never commit it to version control!"
