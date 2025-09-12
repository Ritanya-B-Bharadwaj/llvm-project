#!/bin/bash

set -e

echo "=========================================="
echo "OpenMP Test Generator Dependency Installer"
echo "=========================================="

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if [ -f /etc/debian_version ]; then
            echo "debian"
        elif [ -f /etc/redhat-release ]; then
            echo "centos"
        else
            echo "linux"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    else
        echo "unknown"
    fi
}

OS=$(detect_os)
echo "Detected OS: $OS"

install_debian_deps() {
    echo "Installing dependencies for Debian/Ubuntu..."
    
    # Update package list
    sudo apt update
    
    # Core development tools
    sudo apt install -y \
        build-essential \
        cmake \
        git \
        pkg-config \
        wget \
        curl
    
    # Required libraries for the project (no LLVM)
    sudo apt install -y \
        libcurl4-openssl-dev \
        libsqlite3-dev \
        nlohmann-json3-dev \
        g++ \
        libedit-dev \
        zlib1g-dev \
        libzstd-dev
    
    # Optional: Install libclang for pattern extraction (lightweight)
    sudo apt install -y \
        libclang-dev \
        clang
    
    echo "✓ All dependencies installed successfully"
}

install_centos_deps() {
    echo "Installing dependencies for CentOS/RHEL..."
    
    # Enable EPEL repository
    sudo yum install -y epel-release
    
    # Development tools
    sudo yum groupinstall -y "Development Tools"
    sudo yum install -y cmake git pkg-config wget curl
    
    # Required libraries
    sudo yum install -y libcurl-devel sqlite-devel
    
    # Install nlohmann-json manually if not available
    if ! yum list nlohmann-json3-devel &>/dev/null; then
        echo "Installing nlohmann-json manually..."
        cd /tmp
        git clone https://github.com/nlohmann/json.git
        cd json
        mkdir build && cd build
        cmake .. -DJSON_BuildTests=OFF
        make -j$(nproc)
        sudo make install
        cd ../../..
    else
        sudo yum install -y nlohmann-json3-devel
    fi
    
    # Optional: Install clang for pattern extraction
    sudo yum install -y clang-devel clang
}

install_macos_deps() {
    echo "Installing dependencies for macOS..."
    
    # Install Xcode command line tools
    if ! xcode-select -p &>/dev/null; then
        echo "Installing Xcode command line tools..."
        xcode-select --install
        echo "Please complete Xcode installation and re-run this script"
        exit 1
    fi
    
    # Install Homebrew if not present
    if ! command -v brew &>/dev/null; then
        echo "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # Add Homebrew to PATH for current session
        if [[ -f "/opt/homebrew/bin/brew" ]]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        elif [[ -f "/usr/local/bin/brew" ]]; then
            eval "$(/usr/local/bin/brew shellenv)"
        fi
    fi
    
    # Install dependencies (no LLVM)
    brew install cmake curl sqlite3 nlohmann-json pkg-config
    
    # Optional: Install clang for pattern extraction
    brew install llvm
}

# Main installation
case $OS in
    "debian")
        install_debian_deps
        ;;
    "centos")
        install_centos_deps
        ;;
    "macos")
        install_macos_deps
        ;;
    *)
        echo "Unsupported OS: $OS"
        exit 1
        ;;
esac

echo "=========================================="
echo "Dependencies installed successfully!"
echo "=========================================="
echo ""
echo "Installed components:"
echo "  ✓ Build tools (cmake, gcc/g++, git)"
echo "  ✓ Core libraries (libcurl, sqlite3, nlohmann-json)"
echo "  ✓ Optional: Basic clang for pattern extraction"
echo ""
echo "Next steps:"
echo "1. Run: source scripts/setup.sh"
echo "2. Set your API keys in .env file"
echo "3. Build the project: mkdir build && cd build && cmake .. && make"
echo ""
