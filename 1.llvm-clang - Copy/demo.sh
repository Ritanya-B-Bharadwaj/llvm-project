#!/usr/bin/env bash
set -e

# Build not needed for python script

# Create output directory if it doesn't exist
mkdir -p output

# echo "🔍 Discovering OpenMP test cases..."
# python3 discover_tests.py

# echo "🚀 Running enhanced demo with OpenMP test suite..."
# python3 demo_enhanced.py

echo "📋 Running demo on test.cpp..."
# Run the CLI tool on test.cpp
python3 ompir_map.py test.cpp \
  --annotated output/annotated.ll \
  --json output/mapping.json \
  --explain output/explanations.md

# Display outputs
echo "Annotated IR:" && head -n 20 output/annotated.ll

echo "Mapping JSON:" && head -n 20 output/mapping.json

echo "Explanations:" && head -n 20 output/explanations.md

# commands to run

# cmake -B build -DSOURCE_FILE=test.cpp -DARGS="--annotated;output/annotated.ll;--json;output/mapping.json;--explain;output/explanations.md;"

# cmake --build build --target run