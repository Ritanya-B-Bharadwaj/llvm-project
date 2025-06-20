# AI-Powered Reviewer Suggestion Tool for OpenMP PRs

A Clang-based CLI tool that automatically suggests the most relevant OpenMP reviewers for a given Pull Request using semantic analysis and machine learning.

## Features

- **Semantic Analysis**: Uses all-MiniLM-L6-v2 transformer model for deep understanding of PR content
- **File Similarity**: Analyzes changed files to find reviewers with relevant expertise
- **Historical Learning**: Learns from past PR review patterns in the LLVM OpenMP project
- **Fast Inference**: ONNX Runtime integration for efficient model execution
- **Clang Integration**: Built with LLVM/Clang infrastructure for robust C++ processing


## Requirements

### System Requirements

- Ubuntu 20.04 or later (or any modern Linux distribution)
- CMake 3.20+
- C++17 compatible compiler (e.g., g++ 9+ or clang 10+)
- Python 3.7+ (for model/vocab download)


### Dependencies

- LLVM/Clang development libraries
- ONNX Runtime
- libcurl for GitHub API requests
- nlohmann/json for JSON processing

---

## Installation

### 1. Install Dependencies

```bash
# Update package lists
sudo apt update

# Install build essentials and required packages
sudo apt install -y build-essential cmake curl wget git

# Install LLVM and Clang (choose your preferred version, e.g., 18)
sudo apt install -y llvm-18 llvm-18-dev clang-18 libclang-18-dev

# Install ONNX Runtime (CPU version)
sudo apt install -y libonnxruntime-dev

# Install nlohmann-json
sudo apt install -y nlohmann-json-dev

# Install libcurl
sudo apt install -y libcurl4-openssl-dev
```
---

### 2. Download Model Files

```bash
# Create models directory
mkdir -p models

# Download the ONNX model (from HuggingFace)
wget -O models/all-MiniLM-L6-v2.onnx https://huggingface.co/onnx-models/all-MiniLM-L6-v2-onnx/resolve/main/model.onnx

# Download vocabulary file
wget -O models/vocab.txt https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2/resolve/main/vocab.txt
```


---

### 3. Build the Project

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)
```

---

## Usage

### Basic Usage

```bash
# Get reviewer suggestions for PR #12345
./bin/reviewer-suggester --pr 12345 --token YOUR_GITHUB_TOKEN

# Specify custom model paths
./bin/reviewer-suggester \
  --pr 12345 \
  --token YOUR_GITHUB_TOKEN \
  --model-path ../models/all-MiniLM-L6-v2.onnx \
  --vocab-path ../models/vocab.txt
```

![Demo SS](https://github.com/AnanyaBhatblr/llvm-project/blob/main/clang/tools/ai-powered-reviewer-suggestion-tool/demo_ss.png)

![Demo GIF]("/ai-powered-reviewer-suggestion-tool/media/demo.gif")


### Advanced Options

```bash
# Customize the analysis
./bin/reviewer-suggester \
  --pr 12345 \
  --token YOUR_GITHUB_TOKEN \
  --max-pages 10 \
  --top-reviewers 10
```


### Command Line Options

- `--pr <number>`: Pull Request number to analyze (required)
- `--token <token>`: GitHub API token (required)
- `--model-path <path>`: Path to ONNX model file (default: models/all-MiniLM-L6-v2.onnx)
- `--vocab-path <path>`: Path to vocabulary file (default: models/vocab.txt)
- `--max-pages <number>`: Maximum pages of historical PRs to fetch (default: 5)
- `--top-reviewers <number>`: Number of top reviewers to suggest (default: 5)

---

## Getting a GitHub Token

1. Go to GitHub Settings → Developer settings → Personal access tokens
2. Generate a new token with `repo` permissions
3. Copy the token and use it with the `--token` parameter

---

## Example Output

```
=== Reviewer Suggestions for PR #12345 ===
Title: Add support for OpenMP 5.1 task reductions
Files changed: 8

Top 5 Suggested Reviewers:
---------------------------------------
1. jdenny-ornl (Similarity Score: 0.847)
2. AlexeySachkov (Similarity Score: 0.823)
3. jplehr (Similarity Score: 0.791)
4. tianshilei1992 (Similarity Score: 0.756)
5. dreachem (Similarity Score: 0.734)
```


---

## Architecture

### Components

1. **GitHubFetcher**: Handles GitHub API communication
2. **BertTokenizer**: Custom C++ tokenizer for BERT-compatible text processing
3. **PRParser**: Processes PR data and generates embeddings using ONNX Runtime
4. **ReviewerSuggester**: Calculates similarity and ranks potential reviewers

### Machine Learning Pipeline

1. **Text Preprocessing**: Clean and tokenize PR titles and descriptions
2. **Embedding Generation**: Use all-MiniLM-L6-v2 to create 384-dimensional embeddings
3. **Similarity Calculation**: Combine semantic (cosine) and structural (Jaccard) similarity
4. **Ranking**: Score and rank reviewers based on historical review patterns

---

## Model Details

- **Model**: all-MiniLM-L6-v2 (ONNX format)
- **Embedding Dimension**: 384
- **Max Sequence Length**: 256 tokens
- **Model Size**: ~80MB
- **Inference Speed**: ~10ms per PR on modern hardware

---

## Troubleshooting

### Common Issues

1. **ONNX Runtime not found**:

```bash
sudo apt install libonnxruntime-dev
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
```

2. **LLVM libraries not found**:

```bash
sudo apt install llvm-18 llvm-18-dev clang-18 libclang-18-dev
export LLVM_DIR=/usr/lib/llvm-18
```

3. **Model files missing**:
    - Ensure model files are downloaded to the `models/` directory
    - Check file permissions and paths

### Debug Build

For debugging issues:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```


---

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

---

## License

This project is licensed under the Apache License 2.0 - see the LICENSE file for details.

---

## Acknowledgments

- LLVM OpenMP community for providing the dataset
- HuggingFace for the pre-trained all-MiniLM-L6-v2 model
- Microsoft for ONNX Runtime
- The sentence-transformers project for model architecture insights

---

