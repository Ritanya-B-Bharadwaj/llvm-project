### test.cpp

#### Line 3
Source code: `    return a + b;`

Mapped IR code:
```llvm
  %5 = load i32, ptr %3, align 4, !dbg !19
  %6 = load i32, ptr %4, align 4, !dbg !20
  %7 = add nsw i32 %5, %6, !dbg !21
  ret i32 %7, !dbg !22
```

#### Line 7
Source code: `    int x = add(2, 3);`

Mapped IR code:
```llvm
  %3 = call noundef i32 @_Z3addii(i32 noundef 2, i32 noundef 3), !dbg !17
  store i32 %3, ptr %2, align 4, !dbg !16
```

#### Line 8
Source code: `    return x;`

Mapped IR code:
```llvm
  %4 = load i32, ptr %2, align 4, !dbg !18
  ret i32 %4, !dbg !19
```

