#!/bin/bash

# LLVM Source Mapper with AI Summaries
# A wrapper script that combines llvm-source-mapper with optional AI summaries

set -e

# Default values
FORMAT="html"
OUTPUT=""
SOURCE_FILE=""
ENABLE_AI=false
TEMP_MD=""

# Parse arguments
show_help() {
    echo "LLVM Source Mapper - Generate mappings between C++ source and LLVM IR"
    echo ""
    echo "Usage: $0 <source_file> [options]"
    echo ""
    echo "Arguments:"
    echo "  <source_file>      C++ source file (.cpp, .cc, .cxx)"
    echo ""
    echo "Options:"
    echo "  --format=FORMAT    Output format: ll, md, html (default: html)"
    echo "  --output=FILE      Output file (default: auto-generated)"
    echo "  --ai-summaries     Add AI-generated summaries (requires API key)"
    echo "  --help             Show this help message"
    echo ""
    echo "Environment Variables:"
    echo "  GROQ_API_KEY       API key for AI summaries (optional)"
    echo "  GROQ_MODEL         AI model to use (default: llama-3.3-70b-versatile)"
    echo ""
    echo "Examples:"
    echo "  $0 example.cpp"
    echo "  $0 example.cpp --format=md --output=mapping.md"
    echo "  $0 example.cpp --ai-summaries --format=html"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --format=*)
            FORMAT="${1#*=}"
            shift
            ;;
        --output=*)
            OUTPUT="${1#*=}"
            shift
            ;;
        --ai-summaries)
            ENABLE_AI=true
            shift
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        -*)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
        *)
            if [[ -z "$SOURCE_FILE" ]]; then
                SOURCE_FILE="$1"
            else
                echo "Error: Multiple source files specified"
                exit 1
            fi
            shift
            ;;
    esac
done

# Validate inputs
if [[ -z "$SOURCE_FILE" ]]; then
    echo "Error: No source file specified"
    echo "Use --help for usage information"
    exit 1
fi

if [[ ! -f "$SOURCE_FILE" ]]; then
    echo "Error: Source file '$SOURCE_FILE' does not exist"
    exit 1
fi

if [[ ! "$SOURCE_FILE" =~ \.(cpp|cc|cxx)$ ]]; then
    echo "Error: Only C++ files (.cpp, .cc, .cxx) are supported"
    exit 1
fi

if [[ "$FORMAT" != "ll" && "$FORMAT" != "md" && "$FORMAT" != "html" ]]; then
    echo "Error: Invalid format '$FORMAT'. Supported formats: ll, md, html"
    exit 1
fi

# Generate output filename if not provided
if [[ -z "$OUTPUT" ]]; then
    BASENAME=$(basename "$SOURCE_FILE" .cpp)
    BASENAME=$(basename "$BASENAME" .cc)
    BASENAME=$(basename "$BASENAME" .cxx)
    case "$FORMAT" in
        "md")
            OUTPUT="${BASENAME}_mapping.md"
            ;;
        "html")
            OUTPUT="${BASENAME}_mapping.html"
            ;;
        *)
            OUTPUT="${BASENAME}_mapping.ll"
            ;;
    esac
fi

# Find llvm-source-mapper executable
LLVM_SOURCE_MAPPER=""
if command -v llvm-source-mapper &> /dev/null; then
    LLVM_SOURCE_MAPPER="llvm-source-mapper"
elif [[ -f "./llvm-source-mapper" ]]; then
    LLVM_SOURCE_MAPPER="./llvm-source-mapper"
elif [[ -f "../../../build/bin/llvm-source-mapper" ]]; then
    LLVM_SOURCE_MAPPER="../../../build/bin/llvm-source-mapper"
else
    echo "Error: llvm-source-mapper not found in PATH or expected locations"
    echo "Make sure LLVM is built with llvm-source-mapper enabled"
    exit 1
fi

echo "🔍 Generating LLVM IR mapping..."

# If AI summaries are requested and format supports it
if [[ "$ENABLE_AI" == true && ("$FORMAT" == "md" || "$FORMAT" == "html") ]]; then
    # Generate temporary markdown first
    TEMP_MD=$(mktemp /tmp/llvm-source-mapper.XXXXXX.md)
    
    # Generate basic markdown mapping
    if ! "$LLVM_SOURCE_MAPPER" "$SOURCE_FILE" --format=md -o "$TEMP_MD"; then
        echo "Error: Failed to generate LLVM IR mapping"
        rm -f "$TEMP_MD"
        exit 1
    fi
    
    echo "🤖 Adding AI summaries..."
    
    # Check if Python dependencies are available
    if ! python3 -c "import requests" 2>/dev/null; then
        echo "Warning: 'requests' package not found. Installing..."
        if command -v pip3 &> /dev/null; then
            pip3 install --user requests python-dotenv 2>/dev/null || {
                echo "Warning: Could not install Python packages. AI summaries disabled."
                ENABLE_AI=false
            }
        else
            echo "Warning: pip3 not found. AI summaries disabled."
            ENABLE_AI=false
        fi
    fi
    
    if [[ "$ENABLE_AI" == true ]]; then
        # Find the AI script
        AI_SCRIPT=""
        SCRIPT_DIR="$(dirname "$0")"
        if [[ -f "$SCRIPT_DIR/llvm-source-mapper-ai.py" ]]; then
            AI_SCRIPT="$SCRIPT_DIR/llvm-source-mapper-ai.py"
        elif [[ -f "./llvm-source-mapper-ai.py" ]]; then
            AI_SCRIPT="./llvm-source-mapper-ai.py"
        else
            echo "Warning: llvm-source-mapper-ai.py not found. Generating basic mapping without AI summaries."
            ENABLE_AI=false
        fi
        
        if [[ "$ENABLE_AI" == true ]]; then
            # Generate AI summaries
            if [[ "$FORMAT" == "html" ]]; then
                python3 "$AI_SCRIPT" "$TEMP_MD" "$OUTPUT" --html
            else
                python3 "$AI_SCRIPT" "$TEMP_MD" "$OUTPUT"
            fi
            
            if [[ $? -eq 0 ]]; then
                echo "✅ Successfully generated $OUTPUT with AI summaries!"
            else
                echo "❌ Failed to generate AI summaries. Falling back to basic mapping."
                cp "$TEMP_MD" "$OUTPUT"
            fi
        else
            cp "$TEMP_MD" "$OUTPUT"
        fi
    else
        cp "$TEMP_MD" "$OUTPUT"
    fi
    
    # Clean up
    rm -f "$TEMP_MD"
else
    # Generate mapping directly
    if ! "$LLVM_SOURCE_MAPPER" "$SOURCE_FILE" --format="$FORMAT" -o "$OUTPUT"; then
        echo "Error: Failed to generate LLVM IR mapping"
        exit 1
    fi
    
    echo "✅ Successfully generated $OUTPUT!"
fi

# Open HTML files in browser if possible
if [[ "$FORMAT" == "html" ]]; then
    if command -v open &> /dev/null; then
        echo "🌐 Opening in browser..."
        open "$OUTPUT"
    elif command -v xdg-open &> /dev/null; then
        echo "🌐 Opening in browser..."
        xdg-open "$OUTPUT"
    else
        echo "📄 Open $OUTPUT in your browser to view the results"
    fi
fi

echo "📄 Output written to: $OUTPUT"
