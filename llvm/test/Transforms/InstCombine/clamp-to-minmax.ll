; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -passes=instcombine -S | FileCheck %s
; RUN: opt < %s -passes=instcombine -use-constant-fp-for-fixed-length-splat -use-constant-int-for-fixed-length-splat -S | FileCheck %s

; (X < C1) ? C1 : MIN(X, C2)
define float @clamp_float_fast_ordered_strict_maxmin(float %x) {
; CHECK-LABEL: @clamp_float_fast_ordered_strict_maxmin(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp fast olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast oge float [[MIN]], 1.000000e+00
; CHECK-NEXT:    [[R1:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MIN]], float 1.000000e+00
; CHECK-NEXT:    ret float [[R1]]
;
  %cmp2 = fcmp fast olt float %x, 255.0
  %min = select i1 %cmp2, float %x, float 255.0
  %cmp1 = fcmp fast olt float %x, 1.0
  %r = select i1 %cmp1, float 1.0, float %min
  ret float %r
}

; (X <= C1) ? C1 : MIN(X, C2)
define float @clamp_float_fast_ordered_nonstrict_maxmin(float %x) {
; CHECK-LABEL: @clamp_float_fast_ordered_nonstrict_maxmin(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp fast olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast oge float [[MIN]], 1.000000e+00
; CHECK-NEXT:    [[R1:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MIN]], float 1.000000e+00
; CHECK-NEXT:    ret float [[R1]]
;
  %cmp2 = fcmp fast olt float %x, 255.0
  %min = select i1 %cmp2, float %x, float 255.0
  %cmp1 = fcmp fast ole float %x, 1.0
  %r = select i1 %cmp1, float 1.0, float %min
  ret float %r
}

; (X > C1) ? C1 : MAX(X, C2)
define float @clamp_float_fast_ordered_strict_minmax(float %x) {
; CHECK-LABEL: @clamp_float_fast_ordered_strict_minmax(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp fast ogt float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], float [[X]], float 1.000000e+00
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast ole float [[MAX]], 2.550000e+02
; CHECK-NEXT:    [[R1:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MAX]], float 2.550000e+02
; CHECK-NEXT:    ret float [[R1]]
;
  %cmp2 = fcmp fast ogt float %x, 1.0
  %max = select i1 %cmp2, float %x, float 1.0
  %cmp1 = fcmp fast ogt float %x, 255.0
  %r = select i1 %cmp1, float 255.0, float %max
  ret float %r
}

; (X >= C1) ? C1 : MAX(X, C2)
define float @clamp_float_fast_ordered_nonstrict_minmax(float %x) {
; CHECK-LABEL: @clamp_float_fast_ordered_nonstrict_minmax(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp fast ogt float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], float [[X]], float 1.000000e+00
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast ole float [[MAX]], 2.550000e+02
; CHECK-NEXT:    [[R1:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MAX]], float 2.550000e+02
; CHECK-NEXT:    ret float [[R1]]
;
  %cmp2 = fcmp fast ogt float %x, 1.0
  %max = select i1 %cmp2, float %x, float 1.0
  %cmp1 = fcmp fast oge float %x, 255.0
  %r = select i1 %cmp1, float 255.0, float %max
  ret float %r
}


; The same for unordered

; (X < C1) ? C1 : MIN(X, C2)
define float @clamp_float_fast_unordered_strict_maxmin(float %x) {
; CHECK-LABEL: @clamp_float_fast_unordered_strict_maxmin(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp fast oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select nnan ninf i1 [[CMP2_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast oge float [[MIN]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MIN]], float 1.000000e+00
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp fast ult float %x, 255.0
  %min = select i1 %cmp2, float %x, float 255.0
  %cmp1 = fcmp fast ult float %x, 1.0
  %r = select i1 %cmp1, float 1.0, float %min
  ret float %r
}

; (X <= C1) ? C1 : MIN(X, C2)
define float @clamp_float_fast_unordered_nonstrict_maxmin(float %x) {
; CHECK-LABEL: @clamp_float_fast_unordered_nonstrict_maxmin(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp fast oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select nnan ninf i1 [[CMP2_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast oge float [[MIN]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MIN]], float 1.000000e+00
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp fast ult float %x, 255.0
  %min = select i1 %cmp2, float %x, float 255.0
  %cmp1 = fcmp fast ule float %x, 1.0
  %r = select i1 %cmp1, float 1.0, float %min
  ret float %r
}

; (X > C1) ? C1 : MAX(X, C2)
define float @clamp_float_fast_unordered_strict_minmax(float %x) {
; CHECK-LABEL: @clamp_float_fast_unordered_strict_minmax(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp fast ole float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select nnan ninf i1 [[CMP2_INV]], float 1.000000e+00, float [[X]]
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast ole float [[MAX]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MAX]], float 2.550000e+02
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp fast ugt float %x, 1.0
  %max = select i1 %cmp2, float %x, float 1.0
  %cmp1 = fcmp fast ugt float %x, 255.0
  %r = select i1 %cmp1, float 255.0, float %max
  ret float %r
}

; (X >= C1) ? C1 : MAX(X, C2)
define float @clamp_float_fast_unordered_nonstrict_minmax(float %x) {
; CHECK-LABEL: @clamp_float_fast_unordered_nonstrict_minmax(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp fast ole float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select nnan ninf i1 [[CMP2_INV]], float 1.000000e+00, float [[X]]
; CHECK-NEXT:    [[DOTINV:%.*]] = fcmp fast ole float [[MAX]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select nnan ninf i1 [[DOTINV]], float [[MAX]], float 2.550000e+02
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp fast ugt float %x, 1.0
  %max = select i1 %cmp2, float %x, float 1.0
  %cmp1 = fcmp fast uge float %x, 255.0
  %r = select i1 %cmp1, float 255.0, float %max
  ret float %r
}

; Some more checks with fast

; (X > 1.0) ? min(x, 255.0) : 1.0
define float @clamp_test_1(float %x) {
; CHECK-LABEL: @clamp_test_1(
; CHECK-NEXT:    [[INNER_CMP_INV:%.*]] = fcmp fast oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[INNER_SEL:%.*]] = select nnan ninf i1 [[INNER_CMP_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[OUTER_CMP:%.*]] = fcmp fast oge float [[INNER_SEL]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select nnan ninf i1 [[OUTER_CMP]], float [[INNER_SEL]], float 1.000000e+00
; CHECK-NEXT:    ret float [[R]]
;
  %inner_cmp = fcmp fast ult float %x, 255.0
  %inner_sel = select i1 %inner_cmp, float %x, float 255.0
  %outer_cmp = fcmp fast ugt float %x, 1.0
  %r = select i1 %outer_cmp, float %inner_sel, float 1.0
  ret float %r
}

; And something negative

; Like @clamp_test_1 but HighConst < LowConst
define float @clamp_negative_wrong_const(float %x) {
; CHECK-LABEL: @clamp_negative_wrong_const(
; CHECK-NEXT:    [[INNER_CMP_INV:%.*]] = fcmp fast oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[INNER_SEL:%.*]] = select nnan ninf i1 [[INNER_CMP_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[OUTER_CMP:%.*]] = fcmp fast ugt float [[X]], 5.120000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[OUTER_CMP]], float [[INNER_SEL]], float 5.120000e+02
; CHECK-NEXT:    ret float [[R]]
;
  %inner_cmp = fcmp fast ult float %x, 255.0
  %inner_sel = select i1 %inner_cmp, float %x, float 255.0
  %outer_cmp = fcmp fast ugt float %x, 512.0
  %r = select i1 %outer_cmp, float %inner_sel, float 512.0
  ret float %r
}

; Like @clamp_test_1 but both are min
define float @clamp_negative_same_op(float %x) {
; CHECK-LABEL: @clamp_negative_same_op(
; CHECK-NEXT:    [[INNER_CMP_INV:%.*]] = fcmp fast oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[INNER_SEL:%.*]] = select nnan ninf i1 [[INNER_CMP_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[OUTER_CMP:%.*]] = fcmp fast ult float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[OUTER_CMP]], float [[INNER_SEL]], float 1.000000e+00
; CHECK-NEXT:    ret float [[R]]
;
  %inner_cmp = fcmp fast ult float %x, 255.0
  %inner_sel = select i1 %inner_cmp, float %x, float 255.0
  %outer_cmp = fcmp fast ult float %x, 1.0
  %r = select i1 %outer_cmp, float %inner_sel, float 1.0
  ret float %r
}


; And now without fast.

; First, check that we don't do bad things in the presence of signed zeros
define float @clamp_float_with_zero1(float %x) {
; CHECK-LABEL: @clamp_float_with_zero1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp fast olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ole float [[X]], 0.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 0.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp fast olt float %x, 255.0
  %min = select i1 %cmp2, float %x, float 255.0
  %cmp1 = fcmp ole float %x, 0.0
  %r = select i1 %cmp1, float 0.0, float %min
  ret float %r
}

define float @clamp_float_with_zero2(float %x) {
; CHECK-LABEL: @clamp_float_with_zero2(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp fast olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp olt float [[X]], 0.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 0.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp fast olt float %x, 255.0
  %min = select i1 %cmp2, float %x, float 255.0
  %cmp1 = fcmp olt float %x, 0.0
  %r = select i1 %cmp1, float 0.0, float %min
  ret float %r
}

; Also, here we care more about the ordering of the inner min/max, so
; two times more cases.
; TODO: that is not implemented yet, so these checks are for the
;       future. This means that checks below can just check that
;       "fcmp.*%x" happens twice for each label.

; (X < C1) ? C1 : MIN(X, C2)
define float @clamp_float_ordered_strict_maxmin1(float %x) {
; CHECK-LABEL: @clamp_float_ordered_strict_maxmin1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp olt float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp olt float %x, 255.0                   ; X is NaN => false
  %min = select i1 %cmp2, float %x, float 255.0      ;             255.0
  %cmp1 = fcmp olt float %x, 1.0                     ;             false
  %r = select i1 %cmp1, float 1.0, float %min        ;             min (255.0)
  ret float %r
}

define float @clamp_float_ordered_strict_maxmin2(float %x) {
; CHECK-LABEL: @clamp_float_ordered_strict_maxmin2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp olt float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ult float %x, 255.0                  ; X is NaN => true
  %min = select i1 %cmp2, float %x, float 255.0     ;             NaN
  %cmp1 = fcmp olt float %x, 1.0                    ;             false
  %r = select i1 %cmp1, float 1.0, float %min       ;             min (NaN)
  ret float %r
}

; (X <= C1) ? C1 : MIN(X, C2)
define float @clamp_float_ordered_nonstrict_maxmin1(float %x) {
; CHECK-LABEL: @clamp_float_ordered_nonstrict_maxmin1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ole float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp olt float %x, 255.0                  ; X is NaN => false
  %min = select i1 %cmp2, float %x, float 255.0     ;             255.0
  %cmp1 = fcmp ole float %x, 1.0                    ;             false
  %r = select i1 %cmp1, float 1.0, float %min       ;             min (255.0)
  ret float %r
}

define float @clamp_float_ordered_nonstrict_maxmin2(float %x) {
; CHECK-LABEL: @clamp_float_ordered_nonstrict_maxmin2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ole float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ult float %x, 255.0                  ; x is NaN => true
  %min = select i1 %cmp2, float %x, float 255.0     ;             NaN
  %cmp1 = fcmp ole float %x, 1.0                    ;             false
  %r = select i1 %cmp1, float 1.0, float %min       ;             min (NaN)
  ret float %r
}

; (X > C1) ? C1 : MAX(X, C2)
define float @clamp_float_ordered_strict_minmax1(float %x) {
; CHECK-LABEL: @clamp_float_ordered_strict_minmax1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp ogt float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], float [[X]], float 1.000000e+00
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ogt float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ogt float %x, 1.0                    ; x is NaN => false
  %max = select i1 %cmp2, float %x, float 1.0       ;             1.0
  %cmp1 = fcmp ogt float %x, 255.0                  ;             false
  %r = select i1 %cmp1, float 255.0, float %max     ;             max (1.0)
  ret float %r
}

define float @clamp_float_ordered_strict_minmax2(float %x) {
; CHECK-LABEL: @clamp_float_ordered_strict_minmax2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp ole float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2_INV]], float 1.000000e+00, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ogt float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ugt float %x, 1.0                    ; x is NaN => true
  %max = select i1 %cmp2, float %x, float 1.0       ;             NaN
  %cmp1 = fcmp ogt float %x, 255.0                  ;             false
  %r = select i1 %cmp1, float 255.0, float %max     ;             max (NaN)
  ret float %r
}

; (X >= C1) ? C1 : MAX(X, C2)
define float @clamp_float_ordered_nonstrict_minmax1(float %x) {
; CHECK-LABEL: @clamp_float_ordered_nonstrict_minmax1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp ogt float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], float [[X]], float 1.000000e+00
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp oge float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ogt float %x, 1.0                    ; x is NaN => false
  %max = select i1 %cmp2, float %x, float 1.0       ;             1.0
  %cmp1 = fcmp oge float %x, 255.0                  ;             false
  %r = select i1 %cmp1, float 255.0, float %max     ;             max (1.0)
  ret float %r
}

define float @clamp_float_ordered_nonstrict_minmax2(float %x) {
; CHECK-LABEL: @clamp_float_ordered_nonstrict_minmax2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp ole float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2_INV]], float 1.000000e+00, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp oge float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ugt float %x, 1.0                    ; x is NaN => true
  %max = select i1 %cmp2, float %x, float 1.0       ;             NaN
  %cmp1 = fcmp oge float %x, 255.0                  ;             false
  %r = select i1 %cmp1, float 255.0, float %max     ;             max (NaN)
  ret float %r
}


; The same for unordered

; (X < C1) ? C1 : MIN(X, C2)
define float @clamp_float_unordered_strict_maxmin1(float %x) {
; CHECK-LABEL: @clamp_float_unordered_strict_maxmin1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ult float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp olt float %x, 255.0                  ; x is NaN => false
  %min = select i1 %cmp2, float %x, float 255.0     ;             255.0
  %cmp1 = fcmp ult float %x, 1.0                    ;             true
  %r = select i1 %cmp1, float 1.0, float %min       ;             1.0
  ret float %r
}

define float @clamp_float_unordered_strict_maxmin2(float %x) {
; CHECK-LABEL: @clamp_float_unordered_strict_maxmin2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ult float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ult float %x, 255.0                  ; x is NaN => true
  %min = select i1 %cmp2, float %x, float 255.0     ;             NaN
  %cmp1 = fcmp ult float %x, 1.0                    ;             true
  %r = select i1 %cmp1, float 1.0, float %min       ;             1.0
  ret float %r
}

; (X <= C1) ? C1 : MIN(X, C2)
define float @clamp_float_unordered_nonstrict_maxmin1(float %x) {
; CHECK-LABEL: @clamp_float_unordered_nonstrict_maxmin1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp olt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2]], float [[X]], float 2.550000e+02
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ule float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp olt float %x, 255.0                  ; x is NaN => false
  %min = select i1 %cmp2, float %x, float 255.0     ;             255.0
  %cmp1 = fcmp ule float %x, 1.0                    ;             true
  %r = select i1 %cmp1, float 1.0, float %min       ;             1.0
  ret float %r
}

define float @clamp_float_unordered_nonstrict_maxmin2(float %x) {
; CHECK-LABEL: @clamp_float_unordered_nonstrict_maxmin2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp oge float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[MIN:%.*]] = select i1 [[CMP2_INV]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ule float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ult float %x, 255.0                  ; x is NaN => true
  %min = select i1 %cmp2, float %x, float 255.0     ;             NaN
  %cmp1 = fcmp ule float %x, 1.0                    ;             true
  %r = select i1 %cmp1, float 1.0, float %min       ;             1.0
  ret float %r
}

; (X > C1) ? C1 : MAX(X, C2)
define float @clamp_float_unordered_strict_minmax1(float %x) {
; CHECK-LABEL: @clamp_float_unordered_strict_minmax1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp ogt float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], float [[X]], float 1.000000e+00
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ugt float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ogt float %x, 1.0                    ; x is NaN => false
  %max = select i1 %cmp2, float %x, float 1.0       ;             1.0
  %cmp1 = fcmp ugt float %x, 255.0                  ;             true
  %r = select i1 %cmp1, float 255.0, float %max     ;             255.0
  ret float %r
}

define float @clamp_float_unordered_strict_minmax2(float %x) {
; CHECK-LABEL: @clamp_float_unordered_strict_minmax2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp ole float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2_INV]], float 1.000000e+00, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp ugt float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ugt float %x, 1.0                    ; x is NaN => true
  %max = select i1 %cmp2, float %x, float 1.0       ;             NaN
  %cmp1 = fcmp ugt float %x, 255.0                  ;             true
  %r = select i1 %cmp1, float 255.0, float %max     ;             255.0
  ret float %r
}

; (X >= C1) ? C1 : MAX(X, C2)
define float @clamp_float_unordered_nonstrict_minmax1(float %x) {
; CHECK-LABEL: @clamp_float_unordered_nonstrict_minmax1(
; CHECK-NEXT:    [[CMP2:%.*]] = fcmp ogt float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2]], float [[X]], float 1.000000e+00
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp uge float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ogt float %x, 1.0                    ; x is NaN => false
  %max = select i1 %cmp2, float %x, float 1.0       ;             1.0
  %cmp1 = fcmp uge float %x, 255.0                  ;             true
  %r = select i1 %cmp1, float 255.0, float %max     ;             255.0
  ret float %r
}

define float @clamp_float_unordered_nonstrict_minmax2(float %x) {
; CHECK-LABEL: @clamp_float_unordered_nonstrict_minmax2(
; CHECK-NEXT:    [[CMP2_INV:%.*]] = fcmp ole float [[X:%.*]], 1.000000e+00
; CHECK-NEXT:    [[MAX:%.*]] = select i1 [[CMP2_INV]], float 1.000000e+00, float [[X]]
; CHECK-NEXT:    [[CMP1:%.*]] = fcmp uge float [[X]], 2.550000e+02
; CHECK-NEXT:    [[R:%.*]] = select i1 [[CMP1]], float 2.550000e+02, float [[MAX]]
; CHECK-NEXT:    ret float [[R]]
;
  %cmp2 = fcmp ugt float %x, 1.0                    ; x is NaN => true
  %max = select i1 %cmp2, float %x, float 1.0       ;             NaN
  %cmp1 = fcmp uge float %x, 255.0                  ;             true
  %r = select i1 %cmp1, float 255.0, float %max     ;             255.0
  ret float %r
}

;; Check casts behavior
define float @ui32_clamp_and_cast_to_float(i32 %x) {
; CHECK-LABEL: @ui32_clamp_and_cast_to_float(
; CHECK-NEXT:    [[LO_CMP:%.*]] = icmp eq i32 [[X:%.*]], 0
; CHECK-NEXT:    [[MIN1:%.*]] = call i32 @llvm.umin.i32(i32 [[X]], i32 255)
; CHECK-NEXT:    [[MIN:%.*]] = uitofp nneg i32 [[MIN1]] to float
; CHECK-NEXT:    [[R:%.*]] = select i1 [[LO_CMP]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %f_x = uitofp i32 %x to float
  %up_cmp = icmp ugt i32 %x, 255
  %lo_cmp = icmp ult i32 %x, 1
  %min = select i1 %up_cmp, float 255.0, float %f_x
  %r = select i1 %lo_cmp, float 1.0, float %min
  ret float %r
}

define float @ui64_clamp_and_cast_to_float(i64 %x) {
; CHECK-LABEL: @ui64_clamp_and_cast_to_float(
; CHECK-NEXT:    [[LO_CMP:%.*]] = icmp eq i64 [[X:%.*]], 0
; CHECK-NEXT:    [[MIN1:%.*]] = call i64 @llvm.umin.i64(i64 [[X]], i64 255)
; CHECK-NEXT:    [[MIN:%.*]] = uitofp nneg i64 [[MIN1]] to float
; CHECK-NEXT:    [[R:%.*]] = select i1 [[LO_CMP]], float 1.000000e+00, float [[MIN]]
; CHECK-NEXT:    ret float [[R]]
;
  %f_x = uitofp i64 %x to float
  %up_cmp = icmp ugt i64 %x, 255
  %lo_cmp = icmp ult i64 %x, 1
  %min = select i1 %up_cmp, float 255.0, float %f_x
  %r = select i1 %lo_cmp, float 1.0, float %min
  ret float %r
}

define float @mixed_clamp_to_float_1(i32 %x) {
; CHECK-LABEL: @mixed_clamp_to_float_1(
; CHECK-NEXT:    [[R1:%.*]] = call i32 @llvm.smax.i32(i32 [[SI_MIN:%.*]], i32 1)
; CHECK-NEXT:    [[R2:%.*]] = call i32 @llvm.smin.i32(i32 [[R1]], i32 255)
; CHECK-NEXT:    [[R:%.*]] = uitofp nneg i32 [[R2]] to float
; CHECK-NEXT:    ret float [[R]]
;
  %si_min_cmp = icmp sgt i32 %x, 255
  %si_min = select i1 %si_min_cmp, i32 255, i32 %x
  %f_min = sitofp i32 %si_min to float
  %f_x = sitofp i32 %x to float
  %lo_cmp = fcmp ult float %f_x, 1.0
  %r = select i1 %lo_cmp, float 1.0, float %f_min
  ret float %r
}

define i32 @mixed_clamp_to_i32_1(float %x) {
; CHECK-LABEL: @mixed_clamp_to_i32_1(
; CHECK-NEXT:    [[FLOAT_MIN_CMP:%.*]] = fcmp ogt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[FLOAT_MIN:%.*]] = select i1 [[FLOAT_MIN_CMP]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[I32_MIN:%.*]] = fptosi float [[FLOAT_MIN]] to i32
; CHECK-NEXT:    [[I32_X:%.*]] = fptosi float [[X]] to i32
; CHECK-NEXT:    [[LO_CMP:%.*]] = icmp eq i32 [[I32_X]], 0
; CHECK-NEXT:    [[R:%.*]] = select i1 [[LO_CMP]], i32 1, i32 [[I32_MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %float_min_cmp = fcmp ogt float %x, 255.0
  %float_min = select i1 %float_min_cmp, float 255.0, float %x
  %i32_min = fptosi float %float_min to i32
  %i32_x = fptosi float %x to i32
  %lo_cmp = icmp ult i32 %i32_x, 1
  %r = select i1 %lo_cmp, i32 1, i32 %i32_min
  ret i32 %r
}

define float @mixed_clamp_to_float_2(i32 %x) {
; CHECK-LABEL: @mixed_clamp_to_float_2(
; CHECK-NEXT:    [[R1:%.*]] = call i32 @llvm.smax.i32(i32 [[SI_MIN:%.*]], i32 1)
; CHECK-NEXT:    [[R2:%.*]] = call i32 @llvm.smin.i32(i32 [[R1]], i32 255)
; CHECK-NEXT:    [[R:%.*]] = uitofp nneg i32 [[R2]] to float
; CHECK-NEXT:    ret float [[R]]
;
  %si_min_cmp = icmp sgt i32 %x, 255
  %si_min = select i1 %si_min_cmp, i32 255, i32 %x
  %f_min = sitofp i32 %si_min to float
  %lo_cmp = icmp slt i32 %x, 1
  %r = select i1 %lo_cmp, float 1.0, float %f_min
  ret float %r
}

define i32 @mixed_clamp_to_i32_2(float %x) {
; CHECK-LABEL: @mixed_clamp_to_i32_2(
; CHECK-NEXT:    [[FLOAT_MIN_CMP:%.*]] = fcmp ogt float [[X:%.*]], 2.550000e+02
; CHECK-NEXT:    [[FLOAT_MIN:%.*]] = select i1 [[FLOAT_MIN_CMP]], float 2.550000e+02, float [[X]]
; CHECK-NEXT:    [[I32_MIN:%.*]] = fptosi float [[FLOAT_MIN]] to i32
; CHECK-NEXT:    [[LO_CMP:%.*]] = fcmp olt float [[X]], 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = select i1 [[LO_CMP]], i32 1, i32 [[I32_MIN]]
; CHECK-NEXT:    ret i32 [[R]]
;
  %float_min_cmp = fcmp ogt float %x, 255.0
  %float_min = select i1 %float_min_cmp, float 255.0, float %x
  %i32_min = fptosi float %float_min to i32
  %lo_cmp = fcmp olt float %x, 1.0
  %r = select i1 %lo_cmp, i32 1, i32 %i32_min
  ret i32 %r
}


define <2 x float> @mixed_clamp_to_float_vec(<2 x i32> %x) {
; CHECK-LABEL: @mixed_clamp_to_float_vec(
; CHECK-NEXT:    [[R1:%.*]] = call <2 x i32> @llvm.smax.v2i32(<2 x i32> [[SI_MIN:%.*]], <2 x i32> splat (i32 1))
; CHECK-NEXT:    [[R2:%.*]] = call <2 x i32> @llvm.smin.v2i32(<2 x i32> [[R1]], <2 x i32> splat (i32 255))
; CHECK-NEXT:    [[R:%.*]] = uitofp nneg <2 x i32> [[R2]] to <2 x float>
; CHECK-NEXT:    ret <2 x float> [[R]]
;
  %si_min_cmp = icmp sgt <2 x i32> %x, <i32 255, i32 255>
  %si_min = select <2 x i1> %si_min_cmp, <2 x i32> <i32 255, i32 255>, <2 x i32> %x
  %f_min = sitofp <2 x i32> %si_min to <2 x float>
  %f_x = sitofp <2 x i32> %x to <2 x float>
  %lo_cmp = fcmp ult <2 x float> %f_x, <float 1.0, float 1.0>
  %r = select <2 x i1> %lo_cmp, <2 x float> <float 1.0, float 1.0>, <2 x float> %f_min
  ret <2 x float> %r
}
