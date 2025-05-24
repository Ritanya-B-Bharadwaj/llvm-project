; test.cpp:3 -     return a + b;
  %5 = load i32, ptr %3, align 4, !dbg !19
; test.cpp:3 -     return a + b;
  %6 = load i32, ptr %4, align 4, !dbg !20
; test.cpp:3 -     return a + b;
  %7 = add nsw i32 %5, %6, !dbg !21
; test.cpp:3 -     return a + b;
  ret i32 %7, !dbg !22
; test.cpp:7 -     int x = add(2, 3);
  %3 = call noundef i32 @_Z3addii(i32 noundef 2, i32 noundef 3), !dbg !17
; test.cpp:7 -     int x = add(2, 3);
  store i32 %3, ptr %2, align 4, !dbg !16
; test.cpp:8 -     return x;
  %4 = load i32, ptr %2, align 4, !dbg !18
; test.cpp:8 -     return x;
  ret i32 %4, !dbg !19
