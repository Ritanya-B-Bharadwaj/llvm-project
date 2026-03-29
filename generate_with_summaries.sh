#!/bin/bash

# LLVM Source Mapper with Groq Summaries - Generate C++ to LLVM IR mapping with AI summaries
# Usage: ./generate_with_summaries.sh <source_file> [--format=html|md] [--output=filename]

# Default values
FORMAT="html"
OUTPUT=""
SOURCE_FILE=""
TEMP_MD=""

# Parse arguments
for arg in "$@"; do
    if [[ $arg == --format=* ]]; then
        FORMAT="${arg#*=}"
    elif [[ $arg == --output=* ]]; then
        OUTPUT="${arg#*=}"
    elif [[ $arg == --help ]]; then
        echo "LLVM Source Mapper with Groq Summaries"
        echo "Usage: ./generate_with_summaries.sh <source_file> [--format=html|md] [--output=filename]"
        echo "  <source_file>      C++ source file (.cpp, .cc, .cxx)"
        echo "  --format=html|md   Output format (html: Beautiful HTML, md: Markdown table)"
        echo "  --output=filename  Output file (default: auto-generated)"
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
    echo "Usage: ./generate_with_summaries.sh <source_file> [--format=html|md] [--output=filename]"
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
if [[ "$FORMAT" != "html" && "$FORMAT" != "md" ]]; then
    echo "Error: Invalid format '$FORMAT'. Supported formats: html, md"
    exit 1
fi

# Generate auto output filename if not provided
if [ -z "$OUTPUT" ]; then
    BASENAME=$(basename "$SOURCE_FILE" .cpp)
    BASENAME=$(basename "$BASENAME" .cc)
    BASENAME=$(basename "$BASENAME" .cxx)
    if [[ "$FORMAT" == "html" ]]; then
        OUTPUT="${BASENAME}_mapping_with_summaries.html"
    else
        OUTPUT="${BASENAME}_mapping_with_summaries.md"
    fi
fi

# Generate temporary markdown file
TEMP_MD="temp_mapping.md"

echo "🔍 Generating LLVM IR mapping..."
./llvm-source-mapper.sh "$SOURCE_FILE" --format=md --output="$TEMP_MD"

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate LLVM IR mapping."
    exit 1
fi

echo "🤖 Adding Groq AI summaries..."

# Check if python3 is available
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is required but not installed."
    exit 1
fi

# Check if virtual environment exists, if not create it
if [ ! -d "venv" ]; then
    echo "Creating Python virtual environment..."
    python3 -m venv venv
fi

# Activate virtual environment
source venv/bin/activate

# Install required Python packages if not available
python3 -c "import requests, dotenv" 2>/dev/null || {
    echo "Installing required Python packages..."
    pip install requests python-dotenv
}

# Generate summaries using Python script with appropriate format
if [[ "$FORMAT" == "html" ]]; then
    python3 add_groq_summaries.py "$TEMP_MD" "$OUTPUT" --html --model="${GROQ_MODEL:-llama-3.3-70b-versatile}"
else
    python3 add_groq_summaries.py "$TEMP_MD" "$OUTPUT" --model="${GROQ_MODEL:-llama-3.3-70b-versatile}"
fi

if [ $? -eq 0 ]; then
    echo "✅ Successfully generated $OUTPUT with AI summaries!"
    
    # Clean up temporary file
    rm -f "$TEMP_MD"
    
    # Deactivate virtual environment
    deactivate
    
    # Open in browser if HTML format
    if [[ "$FORMAT" == "html" ]]; then
        echo "🌐 Opening in browser..."
        if command -v open &> /dev/null; then
            open "$OUTPUT"
        elif command -v xdg-open &> /dev/null; then
            xdg-open "$OUTPUT"
        else
            echo "Open $OUTPUT in your browser to view the results."
        fi
    fi
else
    echo "❌ Failed to generate AI summaries. Check your GROQ_API_KEY in .env file."
    echo "📄 Basic mapping saved as $TEMP_MD"
    deactivate
    exit 1
fi
