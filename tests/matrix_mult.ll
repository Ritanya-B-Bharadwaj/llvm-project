; ModuleID = '../tests/matrix_mult.c'
source_filename = "../tests/matrix_mult.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nofree nosync nounwind uwtable
define dso_local void @matrix_mult(ptr nocapture noundef readonly %0, ptr nocapture noundef readonly %1, ptr nocapture noundef readonly %2, i32 noundef %3) local_unnamed_addr #0 {
  %5 = icmp sgt i32 %3, 0
  br i1 %5, label %6, label %85

6:                                                ; preds = %4
  %7 = zext i32 %3 to i64
  %8 = add nsw i64 %7, -1
  %9 = and i64 %7, 3
  %10 = icmp ult i64 %8, 3
  %11 = and i64 %7, 4294967292
  %12 = icmp eq i64 %9, 0
  br label %13

13:                                               ; preds = %6, %82
  %14 = phi i64 [ 0, %6 ], [ %83, %82 ]
  %15 = getelementptr inbounds ptr, ptr %0, i64 %14
  %16 = getelementptr inbounds ptr, ptr %2, i64 %14
  %17 = load ptr, ptr %16, align 8, !tbaa !5
  %18 = load ptr, ptr %15, align 8, !tbaa !5
  br label %19

19:                                               ; preds = %77, %13
  %20 = phi i64 [ %80, %77 ], [ 0, %13 ]
  br i1 %10, label %59, label %21

21:                                               ; preds = %19, %21
  %22 = phi i64 [ %56, %21 ], [ 0, %19 ]
  %23 = phi float [ %55, %21 ], [ 0.000000e+00, %19 ]
  %24 = phi i64 [ %57, %21 ], [ 0, %19 ]
  %25 = getelementptr inbounds float, ptr %18, i64 %22
  %26 = load float, ptr %25, align 4, !tbaa !9
  %27 = getelementptr inbounds ptr, ptr %1, i64 %22
  %28 = load ptr, ptr %27, align 8, !tbaa !5
  %29 = getelementptr inbounds float, ptr %28, i64 %20
  %30 = load float, ptr %29, align 4, !tbaa !9
  %31 = tail call float @llvm.fmuladd.f32(float %26, float %30, float %23)
  %32 = or i64 %22, 1
  %33 = getelementptr inbounds float, ptr %18, i64 %32
  %34 = load float, ptr %33, align 4, !tbaa !9
  %35 = getelementptr inbounds ptr, ptr %1, i64 %32
  %36 = load ptr, ptr %35, align 8, !tbaa !5
  %37 = getelementptr inbounds float, ptr %36, i64 %20
  %38 = load float, ptr %37, align 4, !tbaa !9
  %39 = tail call float @llvm.fmuladd.f32(float %34, float %38, float %31)
  %40 = or i64 %22, 2
  %41 = getelementptr inbounds float, ptr %18, i64 %40
  %42 = load float, ptr %41, align 4, !tbaa !9
  %43 = getelementptr inbounds ptr, ptr %1, i64 %40
  %44 = load ptr, ptr %43, align 8, !tbaa !5
  %45 = getelementptr inbounds float, ptr %44, i64 %20
  %46 = load float, ptr %45, align 4, !tbaa !9
  %47 = tail call float @llvm.fmuladd.f32(float %42, float %46, float %39)
  %48 = or i64 %22, 3
  %49 = getelementptr inbounds float, ptr %18, i64 %48
  %50 = load float, ptr %49, align 4, !tbaa !9
  %51 = getelementptr inbounds ptr, ptr %1, i64 %48
  %52 = load ptr, ptr %51, align 8, !tbaa !5
  %53 = getelementptr inbounds float, ptr %52, i64 %20
  %54 = load float, ptr %53, align 4, !tbaa !9
  %55 = tail call float @llvm.fmuladd.f32(float %50, float %54, float %47)
  %56 = add nuw nsw i64 %22, 4
  %57 = add i64 %24, 4
  %58 = icmp eq i64 %57, %11
  br i1 %58, label %59, label %21, !llvm.loop !11

59:                                               ; preds = %21, %19
  %60 = phi float [ undef, %19 ], [ %55, %21 ]
  %61 = phi i64 [ 0, %19 ], [ %56, %21 ]
  %62 = phi float [ 0.000000e+00, %19 ], [ %55, %21 ]
  br i1 %12, label %77, label %63

63:                                               ; preds = %59, %63
  %64 = phi i64 [ %74, %63 ], [ %61, %59 ]
  %65 = phi float [ %73, %63 ], [ %62, %59 ]
  %66 = phi i64 [ %75, %63 ], [ 0, %59 ]
  %67 = getelementptr inbounds float, ptr %18, i64 %64
  %68 = load float, ptr %67, align 4, !tbaa !9
  %69 = getelementptr inbounds ptr, ptr %1, i64 %64
  %70 = load ptr, ptr %69, align 8, !tbaa !5
  %71 = getelementptr inbounds float, ptr %70, i64 %20
  %72 = load float, ptr %71, align 4, !tbaa !9
  %73 = tail call float @llvm.fmuladd.f32(float %68, float %72, float %65)
  %74 = add nuw nsw i64 %64, 1
  %75 = add i64 %66, 1
  %76 = icmp eq i64 %75, %9
  br i1 %76, label %77, label %63, !llvm.loop !13

77:                                               ; preds = %63, %59
  %78 = phi float [ %60, %59 ], [ %73, %63 ]
  %79 = getelementptr inbounds float, ptr %17, i64 %20
  store float %78, ptr %79, align 4, !tbaa !9
  %80 = add nuw nsw i64 %20, 1
  %81 = icmp eq i64 %80, %7
  br i1 %81, label %82, label %19, !llvm.loop !15

82:                                               ; preds = %77
  %83 = add nuw nsw i64 %14, 1
  %84 = icmp eq i64 %83, %7
  br i1 %84, label %85, label %13, !llvm.loop !16

85:                                               ; preds = %82, %4
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare float @llvm.fmuladd.f32(float, float, float) #1

attributes #0 = { nofree nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 15.0.7"}
!5 = !{!6, !6, i64 0}
!6 = !{!"any pointer", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"float", !7, i64 0}
!11 = distinct !{!11, !12}
!12 = !{!"llvm.loop.mustprogress"}
!13 = distinct !{!13, !14}
!14 = !{!"llvm.loop.unroll.disable"}
!15 = distinct !{!15, !12}
!16 = distinct !{!16, !12}
