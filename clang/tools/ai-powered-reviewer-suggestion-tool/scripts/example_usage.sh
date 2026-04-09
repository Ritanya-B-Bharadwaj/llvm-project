#!/bin/bash

# Example Usage Script for AI-Powered Reviewer Suggestion Tool
# This script demonstrates various ways to use the tool

echo "🔍 AI-Powered Reviewer Suggestion Tool - Example Usage"
echo "====================================================="

# Check if the tool is built
if [ ! -f "./bin/reviewer-suggester" ]; then
    echo "❌ Tool not found. Please build the project first:"
    echo "   mkdir build && cd build"
    echo "   cmake .. && make"
    exit 1
fi

# Check if token is provided
if [ -z "$GITHUB_TOKEN" ]; then
    echo "❌ GitHub token not found. Please set GITHUB_TOKEN environment variable:"
    echo "   export GITHUB_TOKEN='your_token_here'"
    exit 1
fi

echo "✅ Tool found and token configured"
echo ""

# Example 1: Basic usage
echo "📝 Example 1: Basic usage with a recent OpenMP PR"
echo "./bin/reviewer-suggester --pr 12345 --token \$GITHUB_TOKEN"
echo ""

# Example 2: Custom parameters
echo "📝 Example 2: Custom parameters for thorough analysis"
echo "./bin/reviewer-suggester \"
echo "  --pr 12345 \"
echo "  --token \$GITHUB_TOKEN \"
echo "  --max-pages 10 \"
echo "  --top-reviewers 10"
echo ""

# Example 3: Quick analysis
echo "📝 Example 3: Quick analysis for testing"
echo "./bin/reviewer-suggester \"
echo "  --pr 12345 \"
echo "  --token \$GITHUB_TOKEN \"
echo "  --max-pages 2 \"
echo "  --top-reviewers 3"
echo ""

# Example 4: Custom model paths
echo "📝 Example 4: Using custom model paths"
echo "./bin/reviewer-suggester \"
echo "  --pr 12345 \"
echo "  --token \$GITHUB_TOKEN \"
echo "  --model-path ./models/all-MiniLM-L6-v2.onnx \"
echo "  --vocab-path ./models/vocab.txt"
echo ""

echo "💡 Tips:"
echo "  - Start with --max-pages 2 for quick testing"
echo "  - Use --max-pages 5-10 for production usage"
echo "  - Higher --max-pages values give more accurate results but take longer"
echo "  - The tool analyzes historical OpenMP PRs to make suggestions"
echo ""

echo "🚀 Run any of the examples above to get reviewer suggestions!"
