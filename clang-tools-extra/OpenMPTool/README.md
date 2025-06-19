# AI-powered OpenMP Test Case Generator

This tool scans the diff between a clean and a codegen OpenMP file, 
detects new OpenMP pragmas, and uses a GenAI-powered script to generate 
matching OpenMP test cases.

## ✅ Features
- Diff-based pragma detection
- Prompt generation for test synthesis
- Simple GenAI integration (mock script)

## 📦 How to Build

```bash
cd ~/llvm-build
ninja OpenMPTool
