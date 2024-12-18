; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 5
; RUN: opt -p loop-vectorize -S %s | FileCheck %s

target datalayout = "e-m:e-p:64:64-i64:64-i128:128-n32:64-S128"
target triple = "riscv64-unknown-linux-gnu"

; Test for https://github.com/llvm/llvm-project/issues/114860.
define void @test_invariant_cond_for_select(ptr %dst, i8 %x) #0 {
; CHECK-LABEL: define void @test_invariant_cond_for_select(
; CHECK-SAME: ptr [[DST:%.*]], i8 [[X:%.*]]) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ 0, %[[ENTRY]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[C_1:%.*]] = icmp eq i8 [[X]], 0
; CHECK-NEXT:    [[C_2:%.*]] = icmp sgt i64 [[IV]], 0
; CHECK-NEXT:    [[C_2_EXT:%.*]] = zext i1 [[C_2]] to i64
; CHECK-NEXT:    [[SEL:%.*]] = select i1 [[C_1]], i64 [[C_2_EXT]], i64 0
; CHECK-NEXT:    [[SEL_TRUNC:%.*]] = trunc i64 [[SEL]] to i8
; CHECK-NEXT:    [[GEP:%.*]] = getelementptr inbounds i8, ptr [[DST]], i64 [[IV]]
; CHECK-NEXT:    store i8 [[SEL_TRUNC]], ptr [[GEP]], align 1
; CHECK-NEXT:    [[IV_NEXT]] = add i64 [[IV]], 4
; CHECK-NEXT:    [[EC:%.*]] = icmp ult i64 [[IV]], 14
; CHECK-NEXT:    br i1 [[EC]], label %[[LOOP]], label %[[EXIT:.*]]
; CHECK:       [[EXIT]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %c.1 = icmp eq i8 %x, 0
  %c.2 = icmp sgt i64 %iv, 0
  %c.2.ext = zext i1 %c.2 to i64
  %sel = select i1 %c.1, i64 %c.2.ext, i64 0
  %sel.trunc = trunc i64 %sel to i8
  %gep = getelementptr inbounds i8, ptr %dst, i64 %iv
  store i8 %sel.trunc, ptr %gep, align 1
  %iv.next = add i64 %iv, 4
  %ec = icmp ult i64 %iv, 14
  br i1 %ec, label %loop, label %exit

exit:
  ret void
}

attributes #0 = { "target-features"="+64bit,+v" }

