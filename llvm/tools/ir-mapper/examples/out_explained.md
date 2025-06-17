### Line 1: `return a + b;`

**LLVM IR Block:**
```llvm
### ./examples/test.cpp


  %5 = load i32, ptr %3, align 4, !dbg !20
  %6 = load i32, ptr %4, align 4, !dbg !21
  %7 = add nsw i32 %5, %6, !dbg !22
  ret i32 %7, !dbg !23

```

**Explanation:**

Here's what this block of LLVM IR does:

This block takes two numbers (`a` and `b`) from memory, adds them up, and then returns the result.

Here's how it relates to the source line: 

- The `load` instructions fetch the values of `a` and `b` from memory.
- The `add` instruction performs the actual addition of `a` and `b`.
- The `ret` instruction returns the result of the addition, which is the sum of `a` and `b`.

---

### Line 2: `int c = add(3, 4);`

**LLVM IR Block:**
```llvm

  %3 = call noundef i32 @_Z3addii(i32 noundef 3, i32 noundef 4), !dbg !18
  store i32 %3, ptr %2, align 4, !dbg !17

```

**Explanation:**

Here's what this LLVM IR block does:

This block of code is equivalent to the C++ source line `int c = add(3, 4);`. 

Here's what happens step by step:

1. The `add` function is called with arguments 3 and 4. The result of this function call is stored in a temporary variable (let's call it `%3`).

2. The value stored in `%3` (which is the result of `add(3, 4)`) is then stored in a memory location (let's call it `%2`). This memory location is where the variable `c` from the C++ source line is stored. 

In other words, this IR block is doing exactly what the C++ source line: calling the `add` function with 3 and 4, and then storing the return value of that function in a variable `c`.

---

### Line 3: `return 0;`

**LLVM IR Block:**
```llvm

  ret i32 0, !dbg !19

```

**Explanation:**

This LLVM IR block returns an integer value of 0 to the caller. 

In other words, when the program reaches this point, it will stop executing and return 0 as the result. This IR block directly corresponds to the C++ source line "return 0;" which also returns 0 to the caller.

---

