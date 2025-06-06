; REQUIRES: asserts
; RUN: opt < %s -aa-pipeline=basic-aa -passes=loop-vectorize,instcombine -S -debug-only=loop-vectorize -disable-output -print-after=instcombine 2>&1 | FileCheck %s
; RUN: opt < %s -passes=loop-vectorize -force-vector-width=2 -S | FileCheck %s -check-prefix=FORCE

target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; CHECK-LABEL: PR31671
;
; Check a pointer in which one of its uses is consecutive-like and another of
; its uses is non-consecutive-like. In the test case below, %tmp3 is the
; pointer operand of an interleaved load, making it consecutive-like. However,
; it is also the pointer operand of a non-interleaved store that will become a
; scatter operation. %tmp3 (and the induction variable) should not be marked
; uniform-after-vectorization.
;
; CHECK:       LV: Found uniform instruction: %tmp0 = getelementptr inbounds %data, ptr %d, i64 0, i32 3, i64 %i
; CHECK-NOT:   LV: Found uniform instruction: %tmp3 = getelementptr inbounds %data, ptr %d, i64 0, i32 0, i64 %i
; CHECK-NOT:   LV: Found uniform instruction: %i = phi i64 [ %i.next, %for.body ], [ 0, %entry ]
; CHECK-NOT:   LV: Found uniform instruction: %i.next = add nuw nsw i64 %i, 5
; CHECK:       define void @PR31671(
; CHECK:       vector.ph:
; CHECK-NEXT:    [[BROADCAST_SPLATINSERT:%.*]] = insertelement <16 x float> poison, float %x, i64 0
; CHECK-NEXT:    [[BROADCAST_SPLAT:%.*]] = shufflevector <16 x float> [[BROADCAST_SPLATINSERT]], <16 x float> poison, <16 x i32> zeroinitializer
; CHECK-NEXT:    br label %vector.body
; CHECK:       vector.body:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %vector.ph ], [ [[INDEX_NEXT:%.*]], %vector.body ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <16 x i64> [ <i64 0, i64 5, i64 10, i64 15, i64 20, i64 25, i64 30, i64 35, i64 40, i64 45, i64 50, i64 55, i64 60, i64 65, i64 70, i64 75>, %vector.ph ], [ [[VEC_IND_NEXT:%.*]], %vector.body ]
; CHECK-NEXT:    [[OFFSET_IDX:%.*]] = mul i64 [[INDEX]], 5
; CHECK-NEXT:    [[TMP0:%.*]] = getelementptr inbounds %data, ptr %d, i64 0, i32 3, i64 [[OFFSET_IDX]]
; CHECK-NEXT:    [[WIDE_VEC:%.*]] = load <80 x float>, ptr [[TMP0]], align 4
; CHECK-NEXT:    [[STRIDED_VEC:%.*]] = shufflevector <80 x float> [[WIDE_VEC]], <80 x float> poison, <16 x i32> <i32 0, i32 5, i32 10, i32 15, i32 20, i32 25, i32 30, i32 35, i32 40, i32 45, i32 50, i32 55, i32 60, i32 65, i32 70, i32 75>
; CHECK-NEXT:    [[TMP2:%.*]] = fmul <16 x float> [[BROADCAST_SPLAT]], [[STRIDED_VEC]]
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds %data, ptr %d, i64 0, i32 0, <16 x i64> [[VEC_IND]]
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <16 x ptr> [[TMP3]], i64 0
; CHECK-NEXT:    [[WIDE_VEC1:%.*]] = load <80 x float>, ptr [[TMP4]], align 4
; CHECK-NEXT:    [[STRIDED_VEC2:%.*]] = shufflevector <80 x float> [[WIDE_VEC1]], <80 x float> poison, <16 x i32> <i32 0, i32 5, i32 10, i32 15, i32 20, i32 25, i32 30, i32 35, i32 40, i32 45, i32 50, i32 55, i32 60, i32 65, i32 70, i32 75>
; CHECK-NEXT:    [[TMP5:%.*]] = fadd <16 x float> [[STRIDED_VEC2]], [[TMP2]]
; CHECK-NEXT:    call void @llvm.masked.scatter.v16f32.v16p0(<16 x float> [[TMP5]], <16 x ptr> [[TMP3]], i32 4, <16 x i1> splat (i1 true))
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i64 [[INDEX]], 16
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <16 x i64> [[VEC_IND]], splat (i64 80)
; CHECK:         br i1 {{.*}}, label %middle.block, label %vector.body

%data = type { [32000 x float], [3 x i32], [4 x i8], [32000 x float] }

define void @PR31671(float %x, ptr %d) #0 {
entry:
  br label %for.body

for.body:
  %i = phi i64 [ %i.next, %for.body ], [ 0, %entry ]
  %tmp0 = getelementptr inbounds %data, ptr %d, i64 0, i32 3, i64 %i
  %tmp1 = load float, ptr %tmp0, align 4
  %tmp2 = fmul float %x, %tmp1
  %tmp3 = getelementptr inbounds %data, ptr %d, i64 0, i32 0, i64 %i
  %tmp4 = load float, ptr %tmp3, align 4
  %tmp5 = fadd float %tmp4, %tmp2
  store float %tmp5, ptr %tmp3, align 4
  %i.next = add nuw nsw i64 %i, 5
  %cond = icmp slt i64 %i.next, 32000
  br i1 %cond, label %for.body, label %for.end

for.end:
  ret void
}

attributes #0 = { "target-cpu"="knl" }

; CHECK-LABEL: PR40816
;
; Check that scalar with predication instructions are not considered uniform
; after vectorization, because that results in replicating a region instead of
; having a single instance (out of VF). The predication stems from a tiny count
; of 3 leading to folding the tail by masking using icmp ule <i, i+1> <= <2, 2>.
;
; CHECK:     LV: Found trip count: 3
; CHECK:     LV: Found uniform instruction:   {{%.*}} = icmp eq i32 {{%.*}}, 0
; CHECK-NOT: LV: Found uniform instruction:   {{%.*}} = load i32, ptr {{%.*}}, align 1
; CHECK:     LV: Found not uniform due to requiring predication:  {{%.*}} = load i32, ptr {{%.*}}, align 1
; CHECK:     LV: Found scalar instruction:   {{%.*}} = getelementptr inbounds [3 x i32], ptr @a, i32 0, i32 {{%.*}}
;
; FORCE-LABEL: @PR40816(
; FORCE-NEXT:  entry:
; FORCE-NEXT:    br i1 false, label {{%.*}}, label [[VECTOR_PH:%.*]]
; FORCE:       vector.ph:
; FORCE-NEXT:    br label [[VECTOR_BODY:%.*]]
; FORCE:       vector.body:
; FORCE-NEXT:    [[INDEX:%.*]] = phi i32 [ 0, [[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], [[PRED_STORE_CONTINUE4:%.*]] ]
; FORCE-NEXT:    [[VEC_IND:%.*]] = phi <2 x i8> [ <i8 0, i8 1>, [[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], [[PRED_STORE_CONTINUE4]] ]
; FORCE-NEXT:    [[TMP2:%.*]] = icmp ule <2 x i8> [[VEC_IND]], splat (i8 2)
; FORCE-NEXT:    [[TMP3:%.*]] = extractelement <2 x i1> [[TMP2]], i32 0
; FORCE-NEXT:    br i1 [[TMP3]], label [[PRED_STORE_IF:%.*]], label [[PRED_STORE_CONTINUE:%.*]]
; FORCE:       pred.store.if:
; FORCE-NEXT:    [[TMP0:%.*]] = add i32 [[INDEX]], 0
; FORCE-NEXT:    store i32 [[TMP0]], ptr @b, align 1
; FORCE-NEXT:    br label [[PRED_STORE_CONTINUE]]
; FORCE:       pred.store.continue:
; FORCE-NEXT:    [[TMP10:%.*]] = extractelement <2 x i1> [[TMP2]], i32 1
; FORCE-NEXT:    br i1 [[TMP10]], label [[PRED_STORE_IF3:%.*]], label [[PRED_STORE_CONTINUE4]]
; FORCE:       pred.store.if1:
; FORCE-NEXT:    [[TMP1:%.*]] = add i32 [[INDEX]], 1
; FORCE-NEXT:    store i32 [[TMP1]], ptr @b, align 1
; FORCE-NEXT:    br label [[PRED_STORE_CONTINUE4]]
; FORCE:       pred.store.continue2:
; FORCE-NEXT:    [[INDEX_NEXT]] = add nuw i32 [[INDEX]], 2
; FORCE-NEXT:    [[VEC_IND_NEXT]] = add <2 x i8> [[VEC_IND]], splat (i8 2)
; FORCE-NEXT:    [[TMP15:%.*]] = icmp eq i32 [[INDEX_NEXT]], 4
; FORCE-NEXT:    br i1 [[TMP15]], label {{%.*}}, label [[VECTOR_BODY]]
;
@a = internal constant [3 x i32] [i32 7, i32 7, i32 0], align 1
@b = external global i32, align 1

define void @PR40816() #1 {

entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %0 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  store i32 %0, ptr @b, align 1
  %arrayidx1 = getelementptr inbounds [3 x i32], ptr @a, i32 0, i32 %0
  %1 = load i32, ptr %arrayidx1, align 1
  %cmp2 = icmp eq i32 %1, 0
  %inc = add nuw nsw i32 %0, 1
  br i1 %cmp2, label %return, label %for.body

return:                                           ; preds = %for.body
  ret void
}

attributes #1 = { "target-cpu"="core2" }
