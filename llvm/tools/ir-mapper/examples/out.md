### ./examples/test.cpp

#### Line 2
Source code: `    return a + b;`

Mapped IR code:
```llvm
  %5 = load i32, ptr %3, align 4, !dbg !20
  %6 = load i32, ptr %4, align 4, !dbg !21
  %7 = add nsw i32 %5, %6, !dbg !22
  ret i32 %7, !dbg !23
```

#### Line 6
Source code: `    int c = add(3, 4);`

Mapped IR code:
```llvm
  %3 = call noundef i32 @_Z3addii(i32 noundef 3, i32 noundef 4), !dbg !18
  store i32 %3, ptr %2, align 4, !dbg !17
```

#### Line 7
Source code: `    return 0;`

Mapped IR code:
```llvm
  ret i32 0, !dbg !19
```

