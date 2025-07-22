; ModuleID = '../tests/running_average.c'
source_filename = "../tests/running_average.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree norecurse nosync nounwind uwtable
define dso_local void @running_average(ptr nocapture noundef readonly %0, ptr nocapture noundef writeonly %1, i32 noundef %2) local_unnamed_addr #0 {
  %4 = icmp sgt i32 %2, 0
  br i1 %4, label %5, label %24

5:                                                ; preds = %3
  %6 = zext i32 %2 to i64
  %7 = and i64 %6, 1
  %8 = icmp eq i32 %2, 1
  br i1 %8, label %11, label %9

9:                                                ; preds = %5
  %10 = and i64 %6, 4294967294
  br label %25

11:                                               ; preds = %25, %5
  %12 = phi i64 [ 0, %5 ], [ %40, %25 ]
  %13 = phi float [ 0.000000e+00, %5 ], [ %39, %25 ]
  %14 = icmp eq i64 %7, 0
  br i1 %14, label %24, label %15

15:                                               ; preds = %11
  %16 = getelementptr inbounds float, ptr %0, i64 %12
  %17 = load float, ptr %16, align 4, !tbaa !5
  %18 = fadd float %13, %17
  %19 = trunc i64 %12 to i32
  %20 = add i32 %19, 1
  %21 = sitofp i32 %20 to float
  %22 = fdiv float %18, %21
  %23 = getelementptr inbounds float, ptr %1, i64 %12
  store float %22, ptr %23, align 4, !tbaa !5
  br label %24

24:                                               ; preds = %15, %11, %3
  ret void

25:                                               ; preds = %25, %9
  %26 = phi i64 [ 0, %9 ], [ %40, %25 ]
  %27 = phi float [ 0.000000e+00, %9 ], [ %39, %25 ]
  %28 = phi i64 [ 0, %9 ], [ %45, %25 ]
  %29 = getelementptr inbounds float, ptr %0, i64 %26
  %30 = load float, ptr %29, align 4, !tbaa !5
  %31 = fadd float %27, %30
  %32 = or i64 %26, 1
  %33 = trunc i64 %32 to i32
  %34 = sitofp i32 %33 to float
  %35 = fdiv float %31, %34
  %36 = getelementptr inbounds float, ptr %1, i64 %26
  store float %35, ptr %36, align 4, !tbaa !5
  %37 = getelementptr inbounds float, ptr %0, i64 %32
  %38 = load float, ptr %37, align 4, !tbaa !5
  %39 = fadd float %31, %38
  %40 = add nuw nsw i64 %26, 2
  %41 = trunc i64 %40 to i32
  %42 = sitofp i32 %41 to float
  %43 = fdiv float %39, %42
  %44 = getelementptr inbounds float, ptr %1, i64 %32
  store float %43, ptr %44, align 4, !tbaa !5
  %45 = add i64 %28, 2
  %46 = icmp eq i64 %45, %10
  br i1 %46, label %11, label %25, !llvm.loop !9
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
!9 = distinct !{!9, !10}
!10 = !{!"llvm.loop.mustprogress"}
