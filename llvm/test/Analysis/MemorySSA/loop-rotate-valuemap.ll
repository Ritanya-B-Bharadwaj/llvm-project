; RUN: opt -passes=loop-rotate %s -S | FileCheck %s
; REQUIRES: asserts

; Check that loop rotate keeps proper mapping between cloned instructions,
; otherwise, MemorySSA will assert.

; CHECK-LABEL: @f
define void @f(i1 %arg) {
entry:
  br label %for.body16

for.cond.cleanup15:                               ; preds = %for.body16
  ret void

for.body16:                                       ; preds = %for.body16.for.body16_crit_edge, %entry
  %call.i = tail call float @expf(float 0.000000e+00) #1
  %0 = load ptr, ptr undef, align 8
  br i1 %arg, label %for.cond.cleanup15, label %for.body16.for.body16_crit_edge

for.body16.for.body16_crit_edge:                  ; preds = %for.body16
  %.pre = load float, ptr undef, align 8
  br label %for.body16
}

declare float @expf(float)

