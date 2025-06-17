#!/bin/bash

# ----------- Parse Inputs ----------------
INPUT_FILE=""
EVENTS=""
OUTPUT_FILE="function_metrics.csv"  # default fallback

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -i|--input)
            INPUT_FILE="$2"
            shift 2
            ;;
        -e|--events)
            EVENTS="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        *)
            echo "❌ Unknown parameter: $1"
            echo "Usage: $0 -i <input_file.c> -e <PAPI_EVENTS> [-o output.csv]"
            exit 1
            ;;
    esac
done

# ----------- Validate Inputs ----------------
if [[ -z "$INPUT_FILE" || -z "$EVENTS" ]]; then
    echo "❌ Missing input file or events."
    echo "Usage: $0 -i <input_file.c> -e <PAPI_EVENTS> [-o output.csv]"
    exit 1
fi

# ----------- File Naming ----------------
BASENAME=$(basename "$INPUT_FILE" .c)
INSTRUMENTED="instrumented_${BASENAME}.c"
EXECUTABLE="exec_${BASENAME}"

# ----------- Step 1: Instrument Source ----------------
echo "📌 Instrumenting $INPUT_FILE with events: $EVENTS"
./build/tool/cd_lab_instrumentor -trace-papi-events="$EVENTS" "$INPUT_FILE" -- -I/usr/include > "$INSTRUMENTED"

if [[ $? -ne 0 ]]; then
    echo "❌ Instrumentation failed."
    exit 1
fi

# ----------- Step 2: Compile Instrumented Code ----------------
echo "⚙️  Compiling $INSTRUMENTED into $EXECUTABLE"
gcc -O0 -o "$EXECUTABLE" "$INSTRUMENTED" runtime/runtime.c -Iruntime -lpapi -lrt -g -pthread

if [[ $? -ne 0 ]]; then
    echo "❌ Compilation failed."
    exit 1
fi

# ----------- Step 3: Run Executable ----------------
echo "🚀 Running $EXECUTABLE..."
sudo ./"$EXECUTABLE"

if [[ $? -ne 0 ]]; then
    echo "❌ Execution failed."
    exit 1
fi

# ----------- Step 4: Handle Output ----------------
if [[ -f "function_metrics.csv" ]]; then
    mv function_metrics.csv "$OUTPUT_FILE"
    echo "✅ CSV output written to: $OUTPUT_FILE"
else
    echo "⚠️ No CSV output file generated."
fi
