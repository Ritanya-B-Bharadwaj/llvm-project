
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

echo "=== Generating LLVM IR ==="

# Generate LLVM IR with opaque pointers (default in LLVM 21)
$CLANG -S -emit-llvm -O0 ./test/test.c -o ./test/test.ll

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate LLVM IR"
    exit 1
fi

echo "LLVM IR generated: ./test/test.ll"

echo ""
echo "=== Applying the Pass ==="

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
    echo "   nm -D ./pass/PointerTracerPass.so | grep llvm"
    exit 1
fi

echo "Pass applied successfully: ./test/test_instrumented.ll"


echo ""
echo "=== Compiling Instrumented Code ==="

# Compile the instrumented IR
$CLANG ./test/test_instrumented.ll -o ./test/test_instrumented

if [ $? -ne 0 ]; then
    echo "Error: Failed to compile instrumented code"
    echo "Try linking with libc explicitly:"
    echo "   $CLANG -lc ./test/test_instrumented.ll -o ./test/test_instrumented"
    exit 1
fi

echo "Instrumented program compiled: ./test/test_instrumented"

echo ""
echo "=== Running the Instrumented Program ==="
echo "Expected output format: function_name(), 0xaddress1, 0xaddress2, ..."
echo ""

# Run the instrumented program
./test/test_instrumented
