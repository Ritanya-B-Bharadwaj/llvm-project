; ModuleID = '../tests/normalize.c'
source_filename = "../tests/normalize.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree norecurse nosync nounwind uwtable
define dso_local void @normalize(ptr nocapture noundef %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %2
  %5 = zext i32 %1 to i64
  br label %11

6:                                                ; preds = %11, %2
  %7 = phi float [ 0.000000e+00, %2 ], [ %16, %11 ]
  %8 = icmp sgt i32 %1, 0
  br i1 %8, label %9, label %19

9:                                                ; preds = %6
  %10 = zext i32 %1 to i64
  br label %20

11:                                               ; preds = %4, %11
  %12 = phi i64 [ 0, %4 ], [ %17, %11 ]
  %13 = phi float [ 0.000000e+00, %4 ], [ %16, %11 ]
  %14 = getelementptr inbounds float, ptr %0, i64 %12
  %15 = load float, ptr %14, align 4, !tbaa !5
  %16 = fadd float %13, %15
  %17 = add nuw nsw i64 %12, 1
  %18 = icmp eq i64 %17, %5
  br i1 %18, label %6, label %11, !llvm.loop !9

19:                                               ; preds = %20, %6
  ret void

20:                                               ; preds = %9, %20
  %21 = phi i64 [ 0, %9 ], [ %25, %20 ]
  %22 = getelementptr inbounds float, ptr %0, i64 %21
  %23 = load float, ptr %22, align 4, !tbaa !5
  %24 = fdiv float %23, %7
  store float %24, ptr %22, align 4, !tbaa !5
  %25 = add nuw nsw i64 %21, 1
  %26 = icmp eq i64 %25, %10
  br i1 %26, label %19, label %20, !llvm.loop !12
}

attributes #0 = { argmemonly nofree norecurse nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 15.0.7"}
!5 = !{!6, !6, i64 0}
!6 = !{!"float", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
!12 = distinct !{!12, !10, !11}
