; ModuleID = '../tests/fused_op.c'
source_filename = "../tests/fused_op.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree nosync nounwind uwtable
define dso_local void @fused_ops(ptr nocapture noundef readonly %0, ptr nocapture noundef readonly %1, ptr nocapture noundef writeonly %2, i32 noundef %3) local_unnamed_addr #0 {
  %5 = ptrtoint ptr %1 to i64
  %6 = ptrtoint ptr %0 to i64
  %7 = ptrtoint ptr %2 to i64
  %8 = icmp sgt i32 %3, 0
  br i1 %8, label %9, label %93

9:                                                ; preds = %4
  %10 = zext i32 %3 to i64
  %11 = icmp ult i32 %3, 8
  br i1 %11, label %76, label %12

12:                                               ; preds = %9
  %13 = sub i64 %7, %6
  %14 = icmp ult i64 %13, 32
  %15 = sub i64 %7, %5
  %16 = icmp ult i64 %15, 32
  %17 = or i1 %14, %16
  br i1 %17, label %76, label %18

18:                                               ; preds = %12
  %19 = and i64 %10, 4294967288
  %20 = add nsw i64 %19, -8
  %21 = lshr exact i64 %20, 3
  %22 = add nuw nsw i64 %21, 1
  %23 = and i64 %22, 1
  %24 = icmp eq i64 %20, 0
  br i1 %24, label %58, label %25

25:                                               ; preds = %18
  %26 = and i64 %22, 4611686018427387902
  br label %27

27:                                               ; preds = %27, %25
  %28 = phi i64 [ 0, %25 ], [ %55, %27 ]
  %29 = phi i64 [ 0, %25 ], [ %56, %27 ]
  %30 = getelementptr inbounds float, ptr %0, i64 %28
  %31 = load <4 x float>, ptr %30, align 4, !tbaa !5
  %32 = getelementptr inbounds float, ptr %30, i64 4
  %33 = load <4 x float>, ptr %32, align 4, !tbaa !5
  %34 = getelementptr inbounds float, ptr %1, i64 %28
  %35 = load <4 x float>, ptr %34, align 4, !tbaa !5
  %36 = getelementptr inbounds float, ptr %34, i64 4
  %37 = load <4 x float>, ptr %36, align 4, !tbaa !5
  %38 = tail call <4 x float> @llvm.fmuladd.v4f32(<4 x float> %31, <4 x float> %35, <4 x float> %31)
  %39 = tail call <4 x float> @llvm.fmuladd.v4f32(<4 x float> %33, <4 x float> %37, <4 x float> %33)
  %40 = getelementptr inbounds float, ptr %2, i64 %28
  store <4 x float> %38, ptr %40, align 4, !tbaa !5
  %41 = getelementptr inbounds float, ptr %40, i64 4
  store <4 x float> %39, ptr %41, align 4, !tbaa !5
  %42 = or i64 %28, 8
  %43 = getelementptr inbounds float, ptr %0, i64 %42
  %44 = load <4 x float>, ptr %43, align 4, !tbaa !5
  %45 = getelementptr inbounds float, ptr %43, i64 4
  %46 = load <4 x float>, ptr %45, align 4, !tbaa !5
  %47 = getelementptr inbounds float, ptr %1, i64 %42
  %48 = load <4 x float>, ptr %47, align 4, !tbaa !5
  %49 = getelementptr inbounds float, ptr %47, i64 4
  %50 = load <4 x float>, ptr %49, align 4, !tbaa !5
  %51 = tail call <4 x float> @llvm.fmuladd.v4f32(<4 x float> %44, <4 x float> %48, <4 x float> %44)
  %52 = tail call <4 x float> @llvm.fmuladd.v4f32(<4 x float> %46, <4 x float> %50, <4 x float> %46)
  %53 = getelementptr inbounds float, ptr %2, i64 %42
  store <4 x float> %51, ptr %53, align 4, !tbaa !5
  %54 = getelementptr inbounds float, ptr %53, i64 4
  store <4 x float> %52, ptr %54, align 4, !tbaa !5
  %55 = add nuw i64 %28, 16
  %56 = add i64 %29, 2
  %57 = icmp eq i64 %56, %26
  br i1 %57, label %58, label %27, !llvm.loop !9

58:                                               ; preds = %27, %18
  %59 = phi i64 [ 0, %18 ], [ %55, %27 ]
  %60 = icmp eq i64 %23, 0
  br i1 %60, label %74, label %61

61:                                               ; preds = %58
  %62 = getelementptr inbounds float, ptr %0, i64 %59
  %63 = load <4 x float>, ptr %62, align 4, !tbaa !5
  %64 = getelementptr inbounds float, ptr %62, i64 4
  %65 = load <4 x float>, ptr %64, align 4, !tbaa !5
  %66 = getelementptr inbounds float, ptr %1, i64 %59
  %67 = load <4 x float>, ptr %66, align 4, !tbaa !5
  %68 = getelementptr inbounds float, ptr %66, i64 4
  %69 = load <4 x float>, ptr %68, align 4, !tbaa !5
  %70 = tail call <4 x float> @llvm.fmuladd.v4f32(<4 x float> %63, <4 x float> %67, <4 x float> %63)
  %71 = tail call <4 x float> @llvm.fmuladd.v4f32(<4 x float> %65, <4 x float> %69, <4 x float> %65)
  %72 = getelementptr inbounds float, ptr %2, i64 %59
  store <4 x float> %70, ptr %72, align 4, !tbaa !5
  %73 = getelementptr inbounds float, ptr %72, i64 4
  store <4 x float> %71, ptr %73, align 4, !tbaa !5
  br label %74

74:                                               ; preds = %58, %61
  %75 = icmp eq i64 %19, %10
  br i1 %75, label %93, label %76

76:                                               ; preds = %12, %9, %74
  %77 = phi i64 [ 0, %12 ], [ 0, %9 ], [ %19, %74 ]
  %78 = xor i64 %77, -1
  %79 = and i64 %10, 1
  %80 = icmp eq i64 %79, 0
  br i1 %80, label %89, label %81

81:                                               ; preds = %76
  %82 = getelementptr inbounds float, ptr %0, i64 %77
  %83 = load float, ptr %82, align 4, !tbaa !5
  %84 = getelementptr inbounds float, ptr %1, i64 %77
  %85 = load float, ptr %84, align 4, !tbaa !5
  %86 = tail call float @llvm.fmuladd.f32(float %83, float %85, float %83)
  %87 = getelementptr inbounds float, ptr %2, i64 %77
  store float %86, ptr %87, align 4, !tbaa !5
  %88 = or i64 %77, 1
  br label %89

89:                                               ; preds = %81, %76
  %90 = phi i64 [ %77, %76 ], [ %88, %81 ]
  %91 = sub nsw i64 0, %10
  %92 = icmp eq i64 %78, %91
  br i1 %92, label %93, label %94

93:                                               ; preds = %89, %94, %74, %4
  ret void

94:                                               ; preds = %89, %94
  %95 = phi i64 [ %109, %94 ], [ %90, %89 ]
  %96 = getelementptr inbounds float, ptr %0, i64 %95
  %97 = load float, ptr %96, align 4, !tbaa !5
  %98 = getelementptr inbounds float, ptr %1, i64 %95
  %99 = load float, ptr %98, align 4, !tbaa !5
  %100 = tail call float @llvm.fmuladd.f32(float %97, float %99, float %97)
  %101 = getelementptr inbounds float, ptr %2, i64 %95
  store float %100, ptr %101, align 4, !tbaa !5
  %102 = add nuw nsw i64 %95, 1
  %103 = getelementptr inbounds float, ptr %0, i64 %102
  %104 = load float, ptr %103, align 4, !tbaa !5
  %105 = getelementptr inbounds float, ptr %1, i64 %102
  %106 = load float, ptr %105, align 4, !tbaa !5
  %107 = tail call float @llvm.fmuladd.f32(float %104, float %106, float %104)
  %108 = getelementptr inbounds float, ptr %2, i64 %102
  store float %107, ptr %108, align 4, !tbaa !5
  %109 = add nuw nsw i64 %95, 2
  %110 = icmp eq i64 %109, %10
  br i1 %110, label %93, label %94, !llvm.loop !12
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare float @llvm.fmuladd.f32(float, float, float) #1

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare <4 x float> @llvm.fmuladd.v4f32(<4 x float>, <4 x float>, <4 x float>) #2

attributes #0 = { argmemonly nofree nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { nocallback nofree nosync nounwind readnone speculatable willreturn }

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
!11 = !{!"llvm.loop.isvectorized", i32 1}
!12 = distinct !{!12, !10, !11}
