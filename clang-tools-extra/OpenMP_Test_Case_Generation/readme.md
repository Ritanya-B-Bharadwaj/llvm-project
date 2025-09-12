# OpenMP Test Generator

An automated system that extracts patterns from existing OpenMP test cases and generates new test skeletons using AI-powered analysis. The tool analyzes GitHub pull requests and leverages extracted patterns to create contextually relevant OpenMP compiler tests.


## Prerequisites

### System Requirements
- **OS**: Debian 12+, Ubuntu 20.04+, macOS 10.15+
- **Compiler**: C++17 compatible compiler (GCC 8+, Clang 10+)
- **CMake**: 3.13.4 or higher

### Required Libraries
- libcurl development headers
- SQLite3 development headers
- nlohmann/json library
- pkg-config

### API Requirements
- **Groq API Key** (required) - Get from [console.groq.com](https://console.groq.com/)
- **GitHub Token** (optional but recommended) - Get from [github.com/settings/tokens](https://github.com/settings/tokens)

##  Installation

### Automatic Installation
- Install dependencies
```
chmod +x scripts/install_deps.sh
./scripts/install_deps.sh
```

- Setup environment
```
source scripts/setup.sh
```
- Setup api in .env file
```
GROQ_API_KEY=your_groq_api_key_here
GITHUB_TOKEN=your_github_token_here
```
- Load Env
```
source .env
```
### Manual Installation
- **Debian/Ubuntu:**
```
sudo apt update
sudo apt install build-essential cmake git pkg-config
sudo apt install libcurl4-openssl-dev libsqlite3-dev nlohmann-json3-dev
```
- **macOS:**
```
brew install cmake curl sqlite3 nlohmann-json pkg-config
```
- Add Environment variables manually
```
export GROQ_API_KEY="your_key_here"
export GITHUB_TOKEN="your_token_here"
```
### Building
```
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 📝 Supported Commands

### Command Line Options

| Option | Description | Required | Default | Example |
|--------|-------------|----------|---------|---------|
| `--pr NUMBER` | GitHub PR number to analyze | ✅ | - | `--pr 67890` |
| `--stage STAGE` | Test stage (parse/sema/codegen) | ❌ | `sema` | `--stage codegen` |
| `--db PATH` | Pattern database path | ❌ | `openmp_patterns.db` | `--db custom.db` |
| `--groq-key KEY` | Groq API key | ❌ | From env var | `--groq-key gsk_...` |
| `--repo REPO` | GitHub repository | ❌ | `llvm/llvm-project` | `--repo my-org/llvm` |
| `--num-tests N` | Number of tests (1-4) | ❌ | `1` | `--num-tests 3` |
| `-h, --help` | Show help message | ❌ | - | `--help` |

### Usage Examples

**Basic Test Generation:**
```
./openmp-test-gen --pr 67890 --stage codegen --num-tests 2
./openmp-test-gen --pr 67890 --stage codegen --num-tests 3

```
## Help and Troubleshooting
- Show all available options
```
./openmp-test-gen --help
```
- Verify environment setup
```
source scripts/setup.sh
```
- Check API key is set
```
echo $GROQ_API_KEY
```


Outputs are stored in build/outputs
