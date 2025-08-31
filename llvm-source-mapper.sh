#!/bin/bash

# LLVM Source Mapper - A tool to map source code to LLVM IR
# Usage: ./llvm-source-mapper.sh <source_file> [--format=ll|md] [--output=filename]

# Default values
FORMAT="ll"
OUTPUT=""
SOURCE_FILE=""
COMPILE_DB_DIR=""
SUMMARIZE=false

# Parse arguments
for arg in "$@"; do
    if [[ $arg == --format=* ]]; then
        FORMAT="${arg#*=}"
    elif [[ $arg == --output=* ]]; then
        OUTPUT="${arg#*=}"
    elif [[ $arg == --db=* ]]; then
        COMPILE_DB_DIR="${arg#*=}"
    elif [[ $arg == --summarize ]]; then
        SUMMARIZE=true
    elif [[ $arg == --help ]]; then
        echo "LLVM Source Mapper - Map C++ source code to LLVM IR"
        echo "Usage: ./llvm-source-mapper.sh <source_file> [--format=ll|md|html] [--output=filename] [--summarize]"
        echo "  <source_file>      C++ source file (.cpp, .cc, .cxx)"
        echo "  --format=ll|md|html Output format (ll: annotated LLVM IR, md: Markdown table, html: Beautiful HTML)"
        echo "  --output=filename  Output file (default: stdout)"
        echo "  --summarize        Enable AI-powered LLVM analysis summaries"
        echo "  --db=path         Path to compilation database directory"
        echo "  --help             Show this help message"
        exit 0
    else
        # Assume it's the source file
        SOURCE_FILE="$arg"
    fi
done

# Check if source file is provided
if [ -z "$SOURCE_FILE" ]; then
    echo "Error: No source file specified."
    echo "Usage: ./llvm-source-mapper.sh <source_file> [--format=ll|md|html] [--output=filename]"
    exit 1
fi

# Check if source file exists
if [ ! -f "$SOURCE_FILE" ]; then
    echo "Error: Source file '$SOURCE_FILE' does not exist."
    exit 1
fi

# Check if source file is a C++ file
if [[ ! "$SOURCE_FILE" =~ \.(cpp|cc|cxx)$ ]]; then
    echo "Error: Only C++ files (.cpp, .cc, .cxx) are supported."
    exit 1
fi

# Validate format option
if [[ "$FORMAT" != "ll" && "$FORMAT" != "md" && "$FORMAT" != "html" ]]; then
    echo "Error: Invalid format '$FORMAT'. Supported formats: ll, md, html"
    exit 1
fi

# Construct the command
CMD="./build/source-mapper $SOURCE_FILE -format=$FORMAT"

# Add AI summaries if requested
if [ "$SUMMARIZE" = true ]; then
    CMD="$CMD -summarize"
fi

# Add output file if specified
if [ ! -z "$OUTPUT" ]; then
    CMD="$CMD -o=$OUTPUT"
fi

# Add compilation database if specified
if [ ! -z "$COMPILE_DB_DIR" ]; then
    CMD="$CMD -p=$COMPILE_DB_DIR"
fi

# Run the command
eval "$CMD"
