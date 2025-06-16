#!/bin/bash

# LLVM 21 Specific Build Script for Pointer Tracer Pass
# This script handles the specific requirements for LLVM 21 in WSL

# Configuration - Update these paths according to your setup
LLVM_BUILD_DIR=~/llvm-project/build
LLVM_SRC_DIR=~/llvm-project/llvm

# Derived paths
CLANG=$LLVM_BUILD_DIR/bin/clang
CLANGXX=$LLVM_BUILD_DIR/bin/clang++
OPT=$LLVM_BUILD_DIR/bin/opt

echo "=== LLVM 21 Pointer Tracer Pass Build and Test ==="
echo "LLVM Build Dir: $LLVM_BUILD_DIR"
echo "LLVM Source Dir: $LLVM_SRC_DIR"

# Check LLVM version
if [ -f "$LLVM_BUILD_DIR/bin/llvm-config" ]; then
    LLVM_VERSION=$($LLVM_BUILD_DIR/bin/llvm-config --version)
    echo "LLVM Version: $LLVM_VERSION"
    
    # Check if this is actually LLVM 21
    if [[ $LLVM_VERSION == 21.* ]]; then
        echo "✓ LLVM 21 detected"
    else
        echo "⚠ Warning: This script is optimized for LLVM 21, but found version $LLVM_VERSION"
    fi
fi

# Check if required tools exist
if [ ! -f "$CLANG" ]; then
    echo "Error: clang not found at $CLANG"
    echo "Please update LLVM_BUILD_DIR variable"
    exit 1
fi

if [ ! -f "$OPT" ]; then
    echo "Error: opt not found at $OPT"
    echo "Please update LLVM_BUILD_DIR variable"
    exit 1
fi

echo ""
echo "=== Step 1: Building the Pass ==="

# Build the pass with LLVM 21 specific flags
$CLANGXX -shared -fPIC \
    -I$LLVM_BUILD_DIR/include \
    -I$LLVM_SRC_DIR/include \
    -std=c++17 \
    -fno-rtti \
    -D_GNU_SOURCE \
    -DLLVM_ENABLE_NEW_PASS_MANAGER=1 \
    ./pass/PointerTracer.cpp \
    -o ./pass/PointerTracerPass.so

if [ $? -ne 0 ]; then
    echo "Error: Failed to build the pass"
    echo ""
    echo "Debugging information:"
    echo "- Check if the LLVM include directories exist:"
    echo "  ls -la $LLVM_BUILD_DIR/include/llvm/"
    echo "  ls -la $LLVM_SRC_DIR/include/llvm/"
    echo ""
    echo "- Try building with verbose output:"
    echo "  $CLANGXX -v -shared -fPIC -I$LLVM_BUILD_DIR/include -I$LLVM_SRC_DIR/include -std=c++17 -fno-rtti ./pass/PointerTracer.cpp -o ./pass/PointerTracerPass.so"
    exit 1
fi

echo "✓ Pass built successfully: PointerTracerPass.so"

echo ""
echo "=== Step 2: Generating LLVM IR ==="

# Generate LLVM IR with opaque pointers (default in LLVM 21)
$CLANG -S -emit-llvm -O0 ./test/test.c -o ./test/test.ll

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate LLVM IR"
    exit 1
fi

echo "✓ LLVM IR generated: test.ll"

# Show first few lines of IR to verify opaque pointers
echo ""
echo "IR Preview (first 10 lines):"
head -n 10 ./test/test.ll

echo ""
echo "=== Step 3: Applying the Pass ==="

# Apply the pass using new pass manager (default in LLVM 21)
$OPT -load-pass-plugin=./pass/PointerTracerPass.so \
     -passes="pointer-tracer" \
     -S ./test/test.ll -o ./test/test_instrumented.ll

if [ $? -ne 0 ]; then
    echo "Error: Failed to apply the pass"
    echo ""
    echo "Debugging steps:"
    echo "1. Check if the pass is loaded correctly:"
    echo "   $OPT -load-pass-plugin=./pass/PointerTracerPass.so -print-passes 2>&1 | grep pointer"
    echo ""
    echo "2. Try with verbose output:"
    echo "   $OPT -load-pass-plugin=./pass/PointerTracerPass.so -passes=\"pointer-tracer\" -debug -S ./test/test.ll -o ./test/test_instrumented.ll"
    echo ""
    echo "3. Check plugin symbols:"
    echo "   nm -D PointerTracerPass.so | grep llvm"
    exit 1
fi

echo "✓ Pass applied successfully: test_instrumented.ll"

# Show instrumentation diff
echo ""
echo "Instrumentation added (looking for printf calls):"
PRINTF_COUNT=$(grep -c "call.*printf" ./test/test_instrumented.ll 2>/dev/null || echo "0")
echo "Found $PRINTF_COUNT printf calls in instrumented IR"

if [ "$PRINTF_COUNT" -eq "0" ]; then
    echo "⚠ Warning: No printf calls found. The instrumentation might not have been applied correctly."
    echo "Check the instrumented IR manually:"
    echo "   grep -n \"call\" ./test/test_instrumented.ll"
fi

echo ""
echo "=== Step 4: Compiling Instrumented Code ==="

# Compile the instrumented IR
$CLANG ./test/test_instrumented.ll -o ./test/test_instrumented

if [ $? -ne 0 ]; then
    echo "Error: Failed to compile instrumented code"
    echo "Try linking with libc explicitly:"
    echo "   $CLANG -lc ./test/test_instrumented.ll -o ./test/test_instrumented"
    exit 1
fi

echo "✓ Instrumented program compiled: test_instrumented"

echo ""
echo "=== Step 5: Running the Instrumented Program ==="
echo "Expected output format: function_name(), 0xaddress1, 0xaddress2, ..."
echo ""

# Run the instrumented program
./test/test_instrumented

RETVAL=$?
echo ""
echo "Program exit code: $RETVAL"

if [ $RETVAL -eq 0 ]; then
    echo "✓ Program executed successfully"
else
    echo "⚠ Program execution failed"
fi

echo ""
echo "=== Additional Debugging Information ==="

# Check if the executable has the required symbols
echo "Checking for printf symbol in executable:"
nm ./test/test_instrumented 2>/dev/null | grep printf || echo "printf not found in symbol table"

echo ""
echo "Checking for instrumentation in IR:"
echo "Function entry instrumentation:"
grep -n "func_name_fmt" ./test/test_instrumented.ll || echo "No function name instrumentation found"
echo ""
echo "Pointer instrumentation:"
grep -n "ptr_fmt" ./test/test_instrumented.ll || echo "No pointer instrumentation found"

echo ""
echo "=== Test Complete ==="
