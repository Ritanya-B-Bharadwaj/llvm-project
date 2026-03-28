```
; RUN: %clang_cc1 -fopenmp -emit-llvm %s -o - | FileCheck %s

target datalayout = "e-m:o-p:32:32-f64:32:64-f80:128:128"
target triple = "i686--linux-gnu"

define i32 @test(i32 %x, i32 %y) {
  %res = sub nsw i64 %x, %y
  call i32 (i32, i32, ...) *@__kmpc_for_static_init_4(
    i32 4, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1)
  ret i32 %res
}

; CHECK: define i32 @test
; CHECK: %res = sub nsw i64 %x, %y
; CHECK: call i32 (i32, i32, ...) *@__kmpc_for_static_init_4
```