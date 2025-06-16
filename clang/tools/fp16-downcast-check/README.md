# fp16-downcast-check — Clang Plugin

This plugin analyses **floating-point literals** (`float` / `double`) in C, C++ and Objective-C source files and suggests when they can be safely or near-safely represented in reduced-precision **binary16** (`__fp16`) format.

Why?  
In many performance-critical domains (ML, graphics, DSP) it is common to keep literals at unnecessarily high precision, incurring larger object-code size and potentially higher register pressure.  Converting such constants to `__fp16` (or `__bf16` in the future) can be an easy, mechanical win when the loss is provably insignificant.

---
## What the plugin does

For every `FloatingLiteral` in the translation unit it:

1. Converts the value to IEEE half-precision (`binary16`) using round-to-nearest-ties-to-even.
2. Computes the **relative error**

   \[ \text{err} = \frac{| \text{orig} - \text{downcast} |}{|\text{orig}|} \]

3. Emits diagnostics according to the result:

| case | condition                              | diagnostic |
|------|----------------------------------------|------------|
| Exact      | value converts without loss           | `warning: … can be safely downcast` |
| In-threshold | error ≤ *threshold* (default `0.001`) | `warning` + `note` with exact error |
| Exceeds    | error  > *threshold*                | `note` explaining excess |

The threshold is user-configurable via `-threshold=<float>` passed as a **plugin argument**.

---
## Building

If you're starting from a fresh tree:
```bash
mkdir -p build && cd build
cmake -G Ninja ../llvm \
  -DLLVM_ENABLE_PROJECTS=clang \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_PLUGINS=ON \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Once configured, build just the plugin:
```bash
ninja FpDowncastCheck     # or simply: ninja (to build everything)
```

The resulting shared library lives at
```
<build>/lib/FpDowncastCheck.{dylib|so|dll}
```

---
## Using the plugin

### Typical invocation (macOS example)
```bash
# In clang/tools/fp16-downcast-check directory (so relative paths match)
SDK=$(xcrun --show-sdk-path)
../../../build/bin/clang \
  -isysroot "$SDK" \
  -Xclang -load -Xclang ../../../build/lib/FpDowncastCheck.dylib \
  -Xclang -plugin -Xclang fp16-downcast-check \
  -Xclang -plugin-arg-fp16-downcast-check -Xclang -threshold=0.001 \
  -fsyntax-only  my_file_test.c
```

Key pieces:

* `-Xclang -load -Xclang <dylib>` – load the plugin.
* `-Xclang -plugin -Xclang fp16-downcast-check` – activate it.
* `-Xclang -plugin-arg-fp16-downcast-check -Xclang -threshold=<N>` – (optional) relative-error threshold.  Omit for the default `0.001`.

### Cross-platform notes
* On **Linux** you normally don't need the `-isysroot …` bit; system headers are found automatically.
* On **macOS** an un-installed development build of Clang doesn't automatically add the system SDK include path, hence the `xcrun --show-sdk-path` trick.

---
## Examples

```c
float x  = 1.5f;        // exact → warning: can be safely downcast
float pi = 3.14159f;    // within threshold → warning + note
float e  = 2.7182818f;  // exceeds threshold → note only
```

Running with the default threshold outputs something like:
```
warning: float literal '1.5' can be safely downcast to '__fp16'
warning: float literal '3.14159' can be downcast to '__fp16' within acceptable error
note: relative error is 3.07207e-04, threshold is 1e-03
note: converting to '__fp16' would introduce relative error of 0.0032, exceeding threshold 0.001
```

---
## Future work / ideas
* Support **bfloat16** (`__bf16`) in addition to `__fp16`.
* Provide automatic-fix-it hints (`-add-plugin`) to rewrite the literal.
* Extend to detect overly precise *integer* literals that fit in smaller types.

---
## License
Part of the LLVM Project, released under the Apache 2.0 License with LLVM exceptions. 