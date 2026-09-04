#!/bin/bash

LLVM_BIN=llvm-project/build/bin
CXX=clang++
BUILTIN_LIB_PATH="llvm-project/build/lib/clang/21/lib/x86_64-unknown-linux-gnu"
RT_FLAGS="-L${BUILTIN_LIB_PATH} -Wl,-Bstatic -l:libclang_rt.builtins.a -Wl,-Bdynamic -lm"

run_test() {
    SRC=$1
    BASE=${SRC%.*}

    echo "======================================"
    echo "        Testing: $SRC"
    echo "======================================"

    "$LLVM_BIN/fp16AccuracyChecker" --float-type=float    "$SRC" > "${BASE}_float.cpp"
    "$LLVM_BIN/fp16AccuracyChecker" --float-type=__fp16   "$SRC" > "${BASE}_f16.cpp"
    "$LLVM_BIN/fp16AccuracyChecker" --float-type=__bf16   "$SRC" > "${BASE}_bf16.cpp"

    # Compile
    $CXX "${BASE}_float.cpp" -o "${BASE}_float" $RT_FLAGS || { echo "float compile failed"; return; }
    $CXX "${BASE}_f16.cpp" -o "${BASE}_f16" $RT_FLAGS 2>/dev/null || echo "__fp16 compile failed"

    if $CXX "${BASE}_bf16.cpp" -o "${BASE}_bf16" $RT_FLAGS 2>/dev/null; then
        bf16_exec="${BASE}_bf16"
    else
        cp "${BASE}_bf16.cpp" "${BASE}_bf16_emulated.cpp"
        sed -i 's/__bf16/float/g' "${BASE}_bf16_emulated.cpp"
        if $CXX "${BASE}_bf16_emulated.cpp" -o "${BASE}_bf16_emulated" $RT_FLAGS 2>/dev/null; then
            bf16_exec="${BASE}_bf16_emulated"
        else
            bf16_exec=""
        fi
    fi

    # Run and capture output
    float_out=$(./"${BASE}_float")
    f16_out=$([ -x "${BASE}_f16" ] && ./"${BASE}_f16")
    bf16_out=$([ -n "$bf16_exec" ] && [ -x "$bf16_exec" ] && ./"$bf16_exec")

    echo
    echo "--- float ---"
    echo "$float_out"
    echo "--- __fp16 ---"
    echo "$f16_out"
    echo "--- __bf16 ---"
    echo "$bf16_out"

    echo
    echo "==============================="
    echo "      Accuracy Comparison"
    echo "==============================="
    printf "%-10s %-12s %-12s %-12s %-12s %-12s\n" "Op#" "float" "__fp16" "fp16_err" "__bf16" "bf16_err"

    # Extract float values from operations
    float_vals=($(echo "$float_out" | grep -oP '\[.*?\] float: \K[-0-9.eE]+'))
    f16_vals=($(echo "$f16_out" | grep -oP '\[.*?\] __fp16: \K[-0-9.eE]+'))
    bf16_vals=($(echo "$bf16_out" | grep -oP '\[.*?\] __bf16: \K[-0-9.eE]+'))

    count=${#float_vals[@]}
    for ((i=0; i<count; i++)); do
        f=${float_vals[$i]}
        f16=${f16_vals[$i]:-N/A}
        bf16=${bf16_vals[$i]:-N/A}

        f16_err="N/A"
        bf16_err="N/A"

        if [[ "$f" =~ ^[0-9.eE+-]+$ && "$f16" =~ ^[0-9.eE+-]+$ ]]; then
            f16_err=$(echo "scale=8; $f - $f16" | bc -l)
        fi
        if [[ "$f" =~ ^[0-9.eE+-]+$ && "$bf16" =~ ^[0-9.eE+-]+$ ]]; then
            bf16_err=$(echo "scale=8; $f - $bf16" | bc -l)
        fi

        printf "%-10s %-12s %-12s %-12s %-12s %-12s\n" "Op#$(($i+1))" "$f" "$f16" "$f16_err" "$bf16" "$bf16_err"
    done

    echo

    # Optional cleanup
    rm -f "${BASE}_float" "${BASE}_f16" "${BASE}_bf16" "${BASE}_bf16_emulated"
}

# Run on all provided files
for file in "$@"; do
    if [[ -f "$file" ]]; then
        run_test "$file"
    else
        echo "File not found: $file"
    fi
done

