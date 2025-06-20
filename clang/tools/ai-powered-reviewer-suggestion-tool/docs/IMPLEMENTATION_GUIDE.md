# Step-by-Step Implementation Guide

## AI-Powered Reviewer Suggestion Tool for OpenMP PRs

This guide walks you through setting up and running the AI-powered reviewer suggestion tool on macOS.

### Prerequisites

Before starting, ensure you have:
- macOS 10.15+ (Catalina or later)
- Admin access to install software
- Stable internet connection for downloading dependencies
- A GitHub account with access to LLVM repository

### Step 1: System Preparation

#### 1.1 Install Xcode Command Line Tools
```bash
xcode-select --install
```
- This installs essential development tools including the C++ compiler
- Follow the on-screen prompts to complete installation
- Verify installation: `gcc --version`

#### 1.2 Install Homebrew Package Manager
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
- Follow the installation prompts
- For Apple Silicon Macs, add Homebrew to your PATH:
```bash
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
```

### Step 2: Install Dependencies

#### 2.1 Install Core Dependencies
```bash
brew update
brew install cmake llvm curl nlohmann-json onnxruntime
```

**What each dependency does:**
- `cmake`: Build system generator
- `llvm`: LLVM/Clang development libraries for CLI integration
- `curl`: HTTP client for GitHub API requests
- `nlohmann-json`: JSON parsing library
- `onnxruntime`: Machine learning inference runtime

#### 2.2 Verify Installations
```bash
cmake --version          # Should show 3.20+
llvm-config --version    # Should show LLVM version
curl --version          # Should show curl version
pkg-config --exists onnxruntime && echo "ONNX Runtime found"
```

### Step 3: Obtain GitHub API Token

#### 3.1 Create Personal Access Token
1. Go to https://github.com/settings/tokens
2. Click "Generate new token (classic)"
3. Give it a descriptive name: "OpenMP Reviewer Tool"
4. Select scopes: `repo` (Full control of private repositories)
5. Click "Generate token"
6. **Important**: Copy the token immediately (you won't see it again)

#### 3.2 Store Token Securely
```bash
# Option 1: Environment variable (recommended)
echo 'export GITHUB_TOKEN="your_token_here"' >> ~/.zprofile
source ~/.zprofile

# Option 2: Use it directly in commands (less secure)
# We'll show this in the usage examples
```

### Step 4: Download and Prepare Model Files

#### 4.1 Create Models Directory
```bash
mkdir -p models
cd models
```

#### 4.2 Download Model Files

**Method 1: Using wget**
```bash
# Download ONNX model (~80MB)
wget -O all-MiniLM-L6-v2.onnx https://huggingface.co/onnx-models/all-MiniLM-L6-v2-onnx/resolve/main/model.onnx

# Download vocabulary file (~230KB)
wget -O vocab.txt https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2/resolve/main/vocab.txt
```

**Method 2: Using curl**
```bash
# Download ONNX model
curl -L -o all-MiniLM-L6-v2.onnx https://huggingface.co/onnx-models/all-MiniLM-L6-v2-onnx/resolve/main/model.onnx

# Download vocabulary file  
curl -L -o vocab.txt https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2/resolve/main/vocab.txt
```

**Method 3: Manual Download**
1. Visit: https://huggingface.co/onnx-models/all-MiniLM-L6-v2-onnx
2. Download `model.onnx` and save as `all-MiniLM-L6-v2.onnx`
3. Visit: https://huggingface.co/sentence-transformers/all-MiniLM-L6-v2
4. Download `vocab.txt`

#### 4.3 Verify Model Files
```bash
ls -la models/
# Should show:
# all-MiniLM-L6-v2.onnx (~80MB)
# vocab.txt (~230KB)
```

### Step 5: Build the Project

#### 5.1 Create Build Directory
```bash
mkdir build
cd build
```

#### 5.2 Configure with CMake
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Troubleshooting CMake Issues:**
- If LLVM not found: `export LLVM_DIR=/opt/homebrew/opt/llvm`
- If ONNX Runtime not found: `export ONNXRUNTIME_ROOT=/opt/homebrew`

#### 5.3 Compile the Project
```bash
make -j$(sysctl -n hw.ncpu)
```
- This uses all available CPU cores for faster compilation
- Compilation should take 2-5 minutes depending on your system

#### 5.4 Verify Build
```bash
ls -la bin/
# Should show: reviewer-suggester (executable)

# Test the executable
./bin/reviewer-suggester --help
```

### Step 6: Test the Tool

#### 6.1 Quick Test Run
```bash
# Test with a known OpenMP PR (replace with actual token)
./bin/reviewer-suggester --pr 12345 --token "your_github_token_here"
```

#### 6.2 Understanding the Output
The tool will:
1. Fetch historical OpenMP PRs (may take 2-3 minutes)
2. Process PR content and generate embeddings
3. Calculate similarity scores
4. Output ranked reviewer suggestions

**Example Output:**
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

### Step 7: Advanced Usage

#### 7.1 Customize Analysis Parameters
```bash
# Analyze more historical data
./bin/reviewer-suggester --pr 12345 --token "$GITHUB_TOKEN" --max-pages 10

# Get more reviewer suggestions
./bin/reviewer-suggester --pr 12345 --token "$GITHUB_TOKEN" --top-reviewers 10

# Use custom model paths
./bin/reviewer-suggester \
  --pr 12345 \
  --token "$GITHUB_TOKEN" \
  --model-path ../models/all-MiniLM-L6-v2.onnx \
  --vocab-path ../models/vocab.txt
```

#### 7.2 Performance Optimization
For faster repeated runs:
- The tool fetches historical data each time (for accuracy)
- Consider caching if running frequently on the same dataset
- Use `--max-pages 3` for faster testing with less historical data

### Step 8: Troubleshooting Common Issues

#### 8.1 Build Issues

**Error: "LLVM not found"**
```bash
export LLVM_DIR=/opt/homebrew/opt/llvm
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Error: "ONNX Runtime not found"**
```bash
brew reinstall onnxruntime
export DYLD_LIBRARY_PATH=/opt/homebrew/lib:$DYLD_LIBRARY_PATH
```

**Error: "nlohmann/json not found"**
```bash
brew reinstall nlohmann-json
```

#### 8.2 Runtime Issues

**Error: "curl_easy_perform() failed"**
- Check internet connection
- Verify GitHub token is valid
- Check if GitHub API rate limit is reached

**Error: "Failed to load ONNX model"**
- Verify model file exists and is not corrupted
- Re-download the model file
- Check file permissions

**Error: "Could not open vocabulary file"**
- Verify vocab.txt exists in models/ directory
- Check file permissions
- Re-download if necessary

#### 8.3 GitHub API Issues

**Rate Limiting:**
- GitHub API has rate limits (5000 requests/hour for authenticated users)
- Use `--max-pages` to reduce API calls during testing
- Wait if you hit the rate limit

**Authentication Issues:**
- Ensure token has `repo` scope
- Token should start with `ghp_` (classic tokens)
- Check token hasn't expired

### Step 9: Understanding the Algorithm

#### 9.1 How It Works
1. **Data Collection**: Fetches historical OpenMP PRs from LLVM repository
2. **Text Processing**: Tokenizes PR titles and descriptions using BERT tokenizer
3. **Embedding Generation**: Creates 384-dimensional vectors using all-MiniLM-L6-v2
4. **Similarity Calculation**: 
   - Semantic similarity (cosine similarity on embeddings): 70% weight
   - File similarity (Jaccard similarity on changed files): 30% weight
5. **Reviewer Ranking**: Aggregates similarity scores for each potential reviewer

#### 9.2 Model Details
- **Model**: all-MiniLM-L6-v2 (sentence transformer)
- **Input**: Text up to 256 tokens
- **Output**: 384-dimensional normalized embeddings
- **Inference Time**: ~10ms per PR on modern hardware
- **Memory Usage**: ~200MB for model + embeddings

#### 9.3 Customization Opportunities
- Adjust similarity weights in `ReviewerSuggester::calculateSimilarity()`
- Modify tokenization in `BertTokenizer` class
- Add recency weighting based on PR creation dates
- Include additional features (PR size, commit patterns, etc.)

### Step 10: Production Usage Tips

#### 10.1 Best Practices
- Use environment variables for GitHub tokens
- Run with `--max-pages 5-10` for good balance of accuracy and speed
- Monitor GitHub API rate limits
- Keep model files updated

#### 10.2 Integration Ideas
- Integrate with GitHub webhooks for automatic suggestions
- Add to CI/CD pipeline for PR analysis
- Create web interface for easier access
- Export suggestions to JSON for further processing

#### 10.3 Maintenance
- Update ONNX Runtime periodically: `brew upgrade onnxruntime`
- Update LLVM: `brew upgrade llvm`
- Monitor HuggingFace for model updates
- Regularly test with new OpenMP PRs

### Conclusion

You now have a fully functional AI-powered reviewer suggestion tool! The system uses state-of-the-art NLP techniques to analyze PR content and suggest the most relevant reviewers based on historical patterns in the LLVM OpenMP project.

For questions or issues, refer to the README.md or create an issue in the project repository.
