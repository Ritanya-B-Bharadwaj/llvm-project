; ./examples/test.cpp:2 -     return a + b;
  %5 = load i32, ptr %3, align 4, !dbg !20
; ./examples/test.cpp:2 -     return a + b;
  %6 = load i32, ptr %4, align 4, !dbg !21
; ./examples/test.cpp:2 -     return a + b;
  %7 = add nsw i32 %5, %6, !dbg !22
; ./examples/test.cpp:2 -     return a + b;
  ret i32 %7, !dbg !23
; ./examples/test.cpp:6 -     int c = add(3, 4);
  %3 = call noundef i32 @_Z3addii(i32 noundef 3, i32 noundef 4), !dbg !18
; ./examples/test.cpp:6 -     int c = add(3, 4);
  store i32 %3, ptr %2, align 4, !dbg !17
; ./examples/test.cpp:7 -     return 0;
  ret i32 0, !dbg !19
