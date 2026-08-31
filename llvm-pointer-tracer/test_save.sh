#!/bin/bash
# Enhanced LLVM 21 Specific Build Script for Pointer Tracer Pass
# This script handles the specific requirements for LLVM 21 in WSL
# Usage: ./build_and_run.sh [test_folder] [output_csv]

# Function to display usage
usage() {
    echo "Usage: $0 [input] [output_csv]"
    echo "  input:       C file or directory containing test files (default: ./test)"
    echo "  output_csv:  Output CSV file path (default: ./results/pointer_trace.csv)"
    echo ""
    echo "Examples:"
    echo "  $0                              # Use default ./test directory and ./results/pointer_trace.csv"
    echo "  $0 ./test.c                     # Process single file ./test.c with default CSV output"
    echo "  $0 ./my_tests                   # Use ./my_tests directory, default CSV output"
    echo "  $0 ./test.c ./output.csv        # Process single file with custom CSV output"
    echo "  $0 ./my_tests ./output.csv      # Use ./my_tests directory with custom CSV output"
    exit 1
}

# Parse command line arguments
INPUT="${1:-./test}"
OUTPUT_CSV="${2:-./results/pointer_trace.csv}"

# Check if input is a file or directory
if [ -f "$INPUT" ]; then
    # Single file mode
    if [[ "$INPUT" != *.c ]]; then
        echo "Error: Input file '$INPUT' is not a C file (.c extension required)"
        echo ""
        usage
    fi
    INPUT_MODE="file"
    TEST_FILE="$INPUT"
    echo "=== Single File Mode ==="
    echo "Input file: $TEST_FILE"
elif [ -d "$INPUT" ]; then
    # Directory mode
    INPUT_MODE="directory"
    TEST_DIR="$INPUT"
    echo "=== Directory Mode ==="
    echo "Test directory: $TEST_DIR"
else
    echo "Error: Input '$INPUT' is neither a file nor a directory"
    echo ""
    usage
fi

# Create output directory if it doesn't exist
OUTPUT_DIR=$(dirname "$OUTPUT_CSV")
mkdir -p "$OUTPUT_DIR"

echo "Output CSV: $OUTPUT_CSV"
echo ""

# Configuration - Update these paths according to your setup
LLVM_BUILD_DIR=~/llvm-project/build
LLVM_SRC_DIR=~/llvm-project/llvm

# Derived paths
CLANG=$LLVM_BUILD_DIR/bin/clang
CLANGXX=$LLVM_BUILD_DIR/bin/clang++
OPT=$LLVM_BUILD_DIR/bin/opt

# Verify LLVM tools exist
if [ ! -x "$CLANG" ]; then
    echo "Error: Clang not found at $CLANG"
    echo "Please update LLVM_BUILD_DIR in the script"
    exit 1
fi

if [ ! -x "$OPT" ]; then
    echo "Error: opt not found at $OPT"
    echo "Please update LLVM_BUILD_DIR in the script"
    exit 1
fi

# Initialize CSV file with header
echo "filename,function_name,pointer_addresses" > "$OUTPUT_CSV"
echo "CSV file initialized: $OUTPUT_CSV"
echo ""

# Function to process a single C file
process_file() {
    local c_file="$1"
    local base_name=$(basename "$c_file" .c)
    local file_dir=$(dirname "$c_file")
    
    echo "=== Processing: $c_file ==="
    
    # Generate LLVM IR
    echo "Generating LLVM IR..."
    $CLANG -S -emit-llvm -O0 "$c_file" -o "$file_dir/${base_name}.ll"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to generate LLVM IR for $c_file"
        return 1
    fi
    
    # Apply the pass
    echo "Applying pointer tracer pass..."
    $OPT -load-pass-plugin=./pass/PointerTracerPass.so \
         -passes="pointer-tracer" \
         -S "$file_dir/${base_name}.ll" -o "$file_dir/${base_name}_instrumented.ll"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to apply the pass to $c_file"
        return 1
    fi
    
    # Compile instrumented code
    echo "Compiling instrumented code..."
    $CLANG "$file_dir/${base_name}_instrumented.ll" -o "$file_dir/${base_name}_instrumented"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to compile instrumented code for $c_file"
        return 1
    fi
    
    # Run and capture output
    echo "Running instrumented program..."
    local raw_output="$file_dir/${base_name}_output.txt"
    if timeout 30s "$file_dir/${base_name}_instrumented" > "$raw_output" 2>&1; then
        echo "Program executed successfully"
        
        # Process output and convert to CSV format
        process_output_to_csv "$raw_output" "$base_name" "$OUTPUT_CSV"
    else
        local exit_code=$?
        if [ $exit_code -eq 124 ]; then
            echo "Warning: Program timed out after 30 seconds"
            echo "$base_name,TIMEOUT,N/A" >> "$OUTPUT_CSV"
        else
            echo "Warning: Program exited with code $exit_code"
            echo "$base_name,ERROR,Exit_code_$exit_code" >> "$OUTPUT_CSV"
        fi
    fi
    
    echo "Completed processing: $c_file"
    echo ""
}

# Function to convert program output to CSV format
process_output_to_csv() {
    local output_file="$1"
    local filename="$2"
    local csv_file="$3"
    
    # Process each line of output
    while IFS= read -r line; do
        # Look for lines that match the pattern: function_name(), 0xaddr1, 0xaddr2, ...
        if [[ "$line" =~ ^[[:space:]]*([^,()]+)\(\)[[:space:]]*,[[:space:]]*(.*) ]]; then
            local func_name="${BASH_REMATCH[1]}"
            local addresses="${BASH_REMATCH[2]}"
            
            # Clean up addresses - remove extra spaces and trailing commas
            addresses=$(echo "$addresses" | sed 's/,[[:space:]]*$//' | sed 's/[[:space:]]*,[[:space:]]*/,/g')
            
            # Add to CSV
            echo "\"$filename\",\"$func_name\",\"$addresses\"" >> "$csv_file"
        elif [[ "$line" =~ ^[[:space:]]*([^,()]+)\(\)[[:space:]]*$ ]]; then
            # Function with no pointer accesses
            local func_name="${BASH_REMATCH[1]}"
            echo "\"$filename\",\"$func_name\",\"NO_POINTERS\"" >> "$csv_file"
        fi
    done < "$output_file"
}

# Find and process files based on input mode
if [ "$INPUT_MODE" = "file" ]; then
    echo "=== Processing Single File ==="
    echo "File: $TEST_FILE"
    echo ""
    
    # Process the single file
    total_files=1
    if process_file "$TEST_FILE"; then
        successful_files=1
    else
        successful_files=0
    fi
else
    # Directory mode - find and process all C files
    echo "=== Searching for C files in $TEST_DIR ==="
    c_files=$(find "$TEST_DIR" -name "*.c" -type f)

    if [ -z "$c_files" ]; then
        echo "No C files found in $TEST_DIR"
        exit 1
    fi

    echo "Found C files:"
    echo "$c_files"
    echo ""

    # Process each C file
    total_files=0
    successful_files=0

    for c_file in $c_files; do
        total_files=$((total_files + 1))
        if process_file "$c_file"; then
            successful_files=$((successful_files + 1))
        fi
    done
fi

echo "=== Summary ==="
echo "Total files processed: $total_files"
echo "Successful: $successful_files"
echo "Failed: $((total_files - successful_files))"
echo ""
echo "Results saved to: $OUTPUT_CSV"
echo ""

# Display CSV preview
if [ -f "$OUTPUT_CSV" ]; then
    echo "=== CSV Preview (first 10 rows) ==="
    head -10 "$OUTPUT_CSV"
    echo ""
    
    # Count entries
    total_entries=$(($(wc -l < "$OUTPUT_CSV") - 1))  # Subtract header
    echo "Total entries in CSV: $total_entries"
    
    # Show unique functions
    unique_functions=$(tail -n +2 "$OUTPUT_CSV" | cut -d',' -f2 | sort -u | wc -l)
    echo "Unique functions traced: $unique_functions"
fi

echo ""
echo "=== CSV Format Information ==="
echo "Columns: filename, function_name, pointer_addresses"
echo "- filename: Name of the source C file (without extension)"
echo "- function_name: Name of the function being traced" 
echo "- pointer_addresses: Comma-separated list of pointer addresses accessed"
echo "Special values:"
echo "  - NO_POINTERS: Function has no pointer accesses"
echo "  - TIMEOUT: Program execution timed out"
echo "  - ERROR: Program failed to execute"

# Optional: Open CSV file if requested
if command -v xdg-open > /dev/null 2>&1; then
    echo ""
    read -p "Open CSV file with default application? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        xdg-open "$OUTPUT_CSV"
    fi
fi
