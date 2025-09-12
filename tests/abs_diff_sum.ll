; ModuleID = '../tests/abs_diff_sum.c'
source_filename = "../tests/abs_diff_sum.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree nosync nounwind readonly uwtable
define dso_local float @abs_diff_sum(ptr nocapture noundef readonly %0, ptr nocapture noundef readonly %1, i32 noundef %2) local_unnamed_addr #0 {
  %4 = icmp sgt i32 %2, 0
  br i1 %4, label %5, label %24

5:                                                ; preds = %3
  %6 = zext i32 %2 to i64
  %7 = and i64 %6, 1
  %8 = icmp eq i32 %2, 1
  br i1 %8, label %11, label %9

9:                                                ; preds = %5
  %10 = and i64 %6, 4294967294
  br label %26

11:                                               ; preds = %26, %5
  %12 = phi float [ undef, %5 ], [ %44, %26 ]
  %13 = phi i64 [ 0, %5 ], [ %45, %26 ]
  %14 = phi float [ 0.000000e+00, %5 ], [ %44, %26 ]
  %15 = icmp eq i64 %7, 0
  br i1 %15, label %24, label %16

16:                                               ; preds = %11
  %17 = getelementptr inbounds float, ptr %0, i64 %13
  %18 = load float, ptr %17, align 4, !tbaa !5
  %19 = getelementptr inbounds float, ptr %1, i64 %13
  %20 = load float, ptr %19, align 4, !tbaa !5
  %21 = fsub float %18, %20
  %22 = tail call float @llvm.fabs.f32(float %21)
  %23 = fadd float %14, %22
  br label %24

24:                                               ; preds = %16, %11, %3
  %25 = phi float [ 0.000000e+00, %3 ], [ %12, %11 ], [ %23, %16 ]
  ret float %25

26:                                               ; preds = %26, %9
  %27 = phi i64 [ 0, %9 ], [ %45, %26 ]
  %28 = phi float [ 0.000000e+00, %9 ], [ %44, %26 ]
  %29 = phi i64 [ 0, %9 ], [ %46, %26 ]
  %30 = getelementptr inbounds float, ptr %0, i64 %27
  %31 = load float, ptr %30, align 4, !tbaa !5
  %32 = getelementptr inbounds float, ptr %1, i64 %27
  %33 = load float, ptr %32, align 4, !tbaa !5
  %34 = fsub float %31, %33
  %35 = tail call float @llvm.fabs.f32(float %34)
  %36 = fadd float %28, %35
  %37 = or i64 %27, 1
  %38 = getelementptr inbounds float, ptr %0, i64 %37
  %39 = load float, ptr %38, align 4, !tbaa !5
  %40 = getelementptr inbounds float, ptr %1, i64 %37
  %41 = load float, ptr %40, align 4, !tbaa !5
  %42 = fsub float %39, %41
  %43 = tail call float @llvm.fabs.f32(float %42)
  %44 = fadd float %36, %43
  %45 = add nuw nsw i64 %27, 2
  %46 = add i64 %29, 2
  %47 = icmp eq i64 %46, %10
  br i1 %47, label %11, label %26, !llvm.loop !9
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare float @llvm.fabs.f32(float) #1

attributes #0 = { argmemonly nofree nosync nounwind readonly uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }

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
