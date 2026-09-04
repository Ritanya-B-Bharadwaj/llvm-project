#!/bin/bash
set -e

# Always run from this script's directory
cd "$(dirname "$0")"

../../../build/bin/clang \
  -Xclang -load -Xclang ../../../build/lib/FpDowncastCheck.so \
  -Xclang -plugin -Xclang fp16-downcast-check \
  -Xclang -plugin-arg-fp16-downcast-check -Xclang -threshold=0.001 \
  -fsyntax-only my_file_test.c
