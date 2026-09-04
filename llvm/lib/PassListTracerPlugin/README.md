# PassListTracer

**PassListTracer** is an LLVM pass plugin that logs selected optimization passes when they are about to be executed. This is useful for debugging and understanding which optimization passes are being run during a compilation pipeline.

## Features

- Dynamically registers via the LLVM pass plugin interface.
- Logs optimization passes containing keywords such as:
  - `Opt`, `Combine`, `DCE`, `Simplify`, `Vector`, `Unroll`, `Inlining`, `Promote`, `GVN`, etc.
- Uses LLVM's `PassInstrumentationCallbacks` to hook into the pass pipeline.
- Outputs to `stdout` using `llvm::outs()`.

## Example Output

When compiling with optimizations, this plugin may print:

## Build Instructions

Make sure you have a recent LLVM build with plugin support. Build as usual and run -

`clang -O2 -fpass-plugin=./PassListTracer.so test.c -c`
