# The LLVM Compiler Infrastructure

[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/llvm/llvm-project/badge)](https://securityscorecards.dev/viewer/?uri=github.com/llvm/llvm-project)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/8273/badge)](https://www.bestpractices.dev/projects/8273)
[![libc++](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml/badge.svg?branch=main&event=schedule)](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml?query=event%3Aschedule)

Welcome to the LLVM project!

This repository contains the source code for LLVM, a toolkit for the
construction of highly optimized compilers, optimizers, and run-time
environments.

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and convert them into
object files. Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.

C-like languages use the [Clang](https://clang.llvm.org/) frontend. This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

## Getting the Source Code and Building LLVM

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
page for information on building and running LLVM.

For information on how to contribute to the LLVM project, please take a look at
the [Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting in touch

Join the [LLVM Discourse forums](https://discourse.llvm.org/), [Discord
chat](https://discord.gg/xS7Z362),
[LLVM Office Hours](https://llvm.org/docs/GettingInvolved.html#office-hours) or
[Regular sync-ups](https://llvm.org/docs/GettingInvolved.html#online-sync-ups).

The LLVM project has adopted a [code of conduct](https://llvm.org/docs/CodeOfConduct.html) for
participants to all modes of communication within the project.
---
# Modifications done in this Repo
# Clang Source Extent and Structural Analysis Plugins

## Overview

This project enhances the Clang compiler infrastructure by introducing two analytical plugins designed for precise static source code examination:

### Function Extent and Call Graph Plugin

- **Plugin Identifier**: `dump-function-extents`
- **Compiler Invocation Flag**: `-fdump-function-extents`
- **Core Capabilities**:
  - Emits precise lexical extents (line number ranges) for all function definitions.
  - Constructs a non-redundant, canonical call graph delineating interprocedural invocation relationships.

### Class Extent and Inheritance Analysis Plugin

- **Plugin Identifier**: `dump-class-extents`
- **Compiler Invocation Flag**: `-fdump-class-extents`
- **Core Capabilities**:
  - Reports lexical source extents for all class and struct definitions.
  - Discovers and enumerates direct inheritance relationships, capturing derived class hierarchies.

## Technical Implementation

### Function Extent Extraction and Call Graph Construction

- Utilizes Clang's `RecursiveASTVisitor` to traverse the Abstract Syntax Tree (AST).
- For each `FunctionDecl` encountered:
  - Extracts fully qualified function signatures.
  - Resolves precise source locations and corresponding line spans.
- Constructs a deterministic call graph with the format:
  ```
  <Caller> -> <Callee>
  ```
- Eliminates redundant edges to preserve clarity.

### Class Extent Reporting and Inheritance Mapping

- Employs `RecursiveASTVisitor` to locate all class and struct declarations within the translation unit.
- For each class declaration:
  - Computes source extent boundaries (file name and line range).
  - Detects direct inheritance relationships to assemble a derived class map.

### Compiler Integration Details

- Custom flags specified within `Options.td`:

  ```td
  def fdump_function_extents : Flag<["-"], "fdump-function-extents">,
    HelpText<"Dump function extents and call graph">,
    Visibility<[CC1Option]>;

  def fdump_class_extents : Flag<["-"], "fdump-class-extents">,
    HelpText<"Dump class extents and derived class relationships">,
    Visibility<[CC1Option]>;
  ```

- Plugin activation logic integrated in `CompilerInvocation.cpp`:

  ```cpp
  if (Args.hasArg(OPT_fdump_function_extents))
    Opts.AddPluginActions.push_back("dump-function-extents");

  if (Args.hasArg(OPT_fdump_class_extents))
    Opts.AddPluginActions.push_back("dump-class-extents");
  ```

## Modified Source Files

- `clang/lib/Frontend/FunctionExtentConsumer.cpp`
- `clang/include/clang/Frontend/FunctionExtentConsumer.h`
- `clang/lib/Frontend/ClassExtentConsumer.cpp`
- `clang/include/clang/Frontend/ClassExtentConsumer.h`
- `clang/lib/Frontend/CompilerInvocation.cpp`
- `clang/include/clang/Frontend/FrontendOptions.h`
- `clang/include/clang/Driver/Options.td`

## Representative Example

### Source File: `test.cpp`

```cpp
class Base {
public:
    void baseMethod() {}
};

class Derived : public Base {
public:
    void derivedMethod() {
        helper();
    }

    void helper() {}
};

void globalFunction() {
    Base b;
    Derived d;
    b.baseMethod();
    d.derivedMethod();
}

int main() {
    globalFunction();
}
```

### Function Extents and Call Graph Extraction

**Command**:

```bash
clang -cc1 -fdump-function-extents test.cpp
```

**Output**:

```
Function Extents:
Base::baseMethod:test.cpp:5-7
Derived::derivedMethod:test.cpp:12-14
Derived::helper:test.cpp:17-19
globalFunction:test.cpp:22-26
main:test.cpp:28-31

Call Graphs:
Derived::derivedMethod -> Derived::helper
globalFunction -> Base::baseMethod
globalFunction -> Derived::derivedMethod
main -> globalFunction
```

### Class Extents and Inheritance Relationships

**Command**:

```bash
clang -cc1 -fdump-class-extents test.cpp
```

**Output**:

```
Class Extents:
Base:test.cpp:3-8
Derived:test.cpp:10-20

Inheritance Tree:
Base <- Derived
```

## Validation and Testing Protocols

- Empirically verified using:
  - `clang -cc1 -fdump-function-extents input.cpp`
  - `clang -cc1 -fdump-class-extents input.cpp`
- Confirmed:
  - Accurate function extent computation and call graph correctness.
  - Faithful representation of class extents and derived class relationships.


