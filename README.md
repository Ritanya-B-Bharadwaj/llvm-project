# 🧠 OpenMP PR Summarizer

A **Clang-compatible command-line tool** that automatically generates **structured and informative summaries** for LLVM/OpenMP pull requests by combining code diffs with contextual information extracted from the OpenMP specification.

---

## ✨ Features

- 🔍 **GitHub PR Parsing** – Fetches title, body, and changed files from a given pull request.
- 📘 **OpenMP Spec Awareness** – Retrieves relevant context from the OpenMP specification.
- 🤖 **AI-Powered Summaries** – Uses a local LLM (via Ollama) or OpenAI API to generate detailed markdown summaries.
- 🛠️ **Clang-style CLI** – Built using LLVM infrastructure (`llvm::cl::opt`) and fits seamlessly into Clang-style tooling.
- 📄 **Markdown Output** – Outputs summaries as clean, GitHub-ready markdown in `summary.md`.

---

## 📦 Example Output

```bash
$ ./bin/omp-pr-summary --pr 82715 --repo llvm/llvm-project
```

```markdown
### 📝 PR Summary

**Title**: [Driver] Remove duplicate `-r` flag usage when linking  
**Bug**: #82010

---

### 📁 Files Changed
- `clang/lib/Driver/ToolChains/Darwin.cpp`
- `clang/lib/Driver/ToolChains/DragonFly.cpp`
- `clang/lib/Driver/ToolChains/FreeBSD.cpp`
...

---

### ✅ Description

This PR removes redundant usage of the `-r` linker flag across multiple platform-specific toolchains in Clang.  
It addresses inconsistencies and simplifies the driver logic related to partial linking behaviors.
```

---

## 🧰 Prerequisites

### ✅ Required Tools

- `cmake` (>= 3.13)
- `make` or `ninja`
- `gh` (GitHub CLI)
- `curl`
- [`ollama`](https://ollama.com) (for local LLM support)
- `clang` or AppleClang

### 🔑 Optional: OpenAI API Key

If you prefer using OpenAI:
```bash
export OPENAI_API_KEY=sk-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

---

## 🚀 Getting Started

### 1. Clone LLVM Monorepo

```bash
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
```

### 2. Add Tool

Create a new directory for the tool:

```bash
mkdir -p llvm/tools/omp-pr-summary
```

Add the following files to that directory:
- `main.cpp`
- `DiffFetcher.cpp/h`
- `SpecRetriever.cpp/h`
- `SummaryGenerator.cpp/h`
- `CMakeLists.txt`

### 3. Register Tool

In `llvm/tools/CMakeLists.txt`, add:

```cmake
add_subdirectory(omp-pr-summary)
```

### 4. Build

```bash
mkdir build && cd build
cmake ../llvm -DCMAKE_BUILD_TYPE=Release
make omp-pr-summary
```

---

## 🧪 Usage

### 🔁 Run the tool
First line lists out PR ids.
```bash
gh pr list --repo llvm/llvm-project --limit 10
./bin/omp-pr-summary --pr <PR_NUMBER> --repo llvm/llvm-project
```

This generates:
- Console output (summary)
- A Markdown file: `summary.md`

---

## 🧠 Local Model Setup (Ollama)

To use a local LLM:

### 1. Install Ollama

```bash
brew install ollama
```

### 2. Start the server

```bash
ollama serve
```

### 3. Pull a tiny model (e.g. TinyLLaMA)

```bash
ollama pull tinyllama
ollama run tinyllama
```

The tool will automatically send the prompt to your running model via:

```
POST http://localhost:11434/api/generate
```

---

## 🧼 Sample Prompt Format

```text
Summarize this PR in Markdown format:

### Title:
<PR Title>

### Body:
<PR Description>

### Changed Files:
- file1.cpp
- file2.cpp

### Relevant OpenMP Spec Snippet:
<clause or section content>
```

---

## 📚 OpenMP Spec Integration

You must extract the OpenMP 6.0 spec:

```bash
wget https://www.openmp.org/wp-content/uploads/OpenMP-API-Specification-6-0.pdf
brew install poppler  # if not installed
pdftotext OpenMP-API-Specification-6-0.pdf
mv OpenMP-API-Specification-6-0.txt llvm/tools/omp-pr-summary/data/openmp_spec.txt
```

---

---

## 📝 License

This tool inherits the LLVM Project License.  
Refer to the [LICENSE.TXT](https://github.com/llvm/llvm-project/blob/main/LICENSE.TXT) in the root LLVM repo.

---

## 🙌 Acknowledgments

- [LLVM Project](https://llvm.org/)
- [OpenMP](https://www.openmp.org/)
- [Ollama](https://ollama.com/)
- [OpenAI API](https://platform.openai.com/)
