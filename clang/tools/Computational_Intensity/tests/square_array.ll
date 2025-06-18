; ModuleID = '../tests/square_array.c'
source_filename = "../tests/square_array.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree norecurse nosync nounwind uwtable
define dso_local void @square_array(ptr nocapture noundef readonly %0, ptr nocapture noundef writeonly %1, i32 noundef %2) local_unnamed_addr #0 {
  %4 = icmp sgt i32 %2, 0
  br i1 %4, label %5, label %78

5:                                                ; preds = %3
  %6 = ptrtoint ptr %1 to i64
  %7 = ptrtoint ptr %0 to i64
  %8 = zext i32 %2 to i64
  %9 = icmp ult i32 %2, 8
  %10 = sub i64 %6, %7
  %11 = icmp ult i64 %10, 32
  %12 = or i1 %9, %11
  br i1 %12, label %59, label %13

13:                                               ; preds = %5
  %14 = and i64 %8, 4294967288
  %15 = add nsw i64 %14, -8
  %16 = lshr exact i64 %15, 3
  %17 = add nuw nsw i64 %16, 1
  %18 = and i64 %17, 1
  %19 = icmp eq i64 %15, 0
  br i1 %19, label %45, label %20

20:                                               ; preds = %13
  %21 = and i64 %17, 4611686018427387902
  br label %22

22:                                               ; preds = %22, %20
  %23 = phi i64 [ 0, %20 ], [ %42, %22 ]
  %24 = phi i64 [ 0, %20 ], [ %43, %22 ]
  %25 = getelementptr inbounds float, ptr %0, i64 %23
  %26 = load <4 x float>, ptr %25, align 4, !tbaa !5
  %27 = getelementptr inbounds float, ptr %25, i64 4
  %28 = load <4 x float>, ptr %27, align 4, !tbaa !5
  %29 = fmul <4 x float> %26, %26
  %30 = fmul <4 x float> %28, %28
  %31 = getelementptr inbounds float, ptr %1, i64 %23
  store <4 x float> %29, ptr %31, align 4, !tbaa !5
  %32 = getelementptr inbounds float, ptr %31, i64 4
  store <4 x float> %30, ptr %32, align 4, !tbaa !5
  %33 = or i64 %23, 8
  %34 = getelementptr inbounds float, ptr %0, i64 %33
  %35 = load <4 x float>, ptr %34, align 4, !tbaa !5
  %36 = getelementptr inbounds float, ptr %34, i64 4
  %37 = load <4 x float>, ptr %36, align 4, !tbaa !5
  %38 = fmul <4 x float> %35, %35
  %39 = fmul <4 x float> %37, %37
  %40 = getelementptr inbounds float, ptr %1, i64 %33
  store <4 x float> %38, ptr %40, align 4, !tbaa !5
  %41 = getelementptr inbounds float, ptr %40, i64 4
  store <4 x float> %39, ptr %41, align 4, !tbaa !5
  %42 = add nuw i64 %23, 16
  %43 = add i64 %24, 2
  %44 = icmp eq i64 %43, %21
  br i1 %44, label %45, label %22, !llvm.loop !9

45:                                               ; preds = %22, %13
  %46 = phi i64 [ 0, %13 ], [ %42, %22 ]
  %47 = icmp eq i64 %18, 0
  br i1 %47, label %57, label %48

48:                                               ; preds = %45
  %49 = getelementptr inbounds float, ptr %0, i64 %46
  %50 = load <4 x float>, ptr %49, align 4, !tbaa !5
  %51 = getelementptr inbounds float, ptr %49, i64 4
  %52 = load <4 x float>, ptr %51, align 4, !tbaa !5
  %53 = fmul <4 x float> %50, %50
  %54 = fmul <4 x float> %52, %52
  %55 = getelementptr inbounds float, ptr %1, i64 %46
  store <4 x float> %53, ptr %55, align 4, !tbaa !5
  %56 = getelementptr inbounds float, ptr %55, i64 4
  store <4 x float> %54, ptr %56, align 4, !tbaa !5
  br label %57

57:                                               ; preds = %45, %48
  %58 = icmp eq i64 %14, %8
  br i1 %58, label %78, label %59

59:                                               ; preds = %5, %57
  %60 = phi i64 [ 0, %5 ], [ %14, %57 ]
  %61 = xor i64 %60, -1
  %62 = add nsw i64 %61, %8
  %63 = and i64 %8, 3
  %64 = icmp eq i64 %63, 0
  br i1 %64, label %75, label %65

65:                                               ; preds = %59, %65
  %66 = phi i64 [ %72, %65 ], [ %60, %59 ]
  %67 = phi i64 [ %73, %65 ], [ 0, %59 ]
  %68 = getelementptr inbounds float, ptr %0, i64 %66
  %69 = load float, ptr %68, align 4, !tbaa !5
  %70 = fmul float %69, %69
  %71 = getelementptr inbounds float, ptr %1, i64 %66
  store float %70, ptr %71, align 4, !tbaa !5
  %72 = add nuw nsw i64 %66, 1
  %73 = add i64 %67, 1
  %74 = icmp eq i64 %73, %63
  br i1 %74, label %75, label %65, !llvm.loop !12

75:                                               ; preds = %65, %59
  %76 = phi i64 [ %60, %59 ], [ %72, %65 ]
  %77 = icmp ult i64 %62, 3
  br i1 %77, label %78, label %79

78:                                               ; preds = %75, %79, %57, %3
  ret void

79:                                               ; preds = %75, %79
  %80 = phi i64 [ %100, %79 ], [ %76, %75 ]
  %81 = getelementptr inbounds float, ptr %0, i64 %80
  %82 = load float, ptr %81, align 4, !tbaa !5
  %83 = fmul float %82, %82
  %84 = getelementptr inbounds float, ptr %1, i64 %80
  store float %83, ptr %84, align 4, !tbaa !5
  %85 = add nuw nsw i64 %80, 1
  %86 = getelementptr inbounds float, ptr %0, i64 %85
  %87 = load float, ptr %86, align 4, !tbaa !5
  %88 = fmul float %87, %87
  %89 = getelementptr inbounds float, ptr %1, i64 %85
  store float %88, ptr %89, align 4, !tbaa !5
  %90 = add nuw nsw i64 %80, 2
  %91 = getelementptr inbounds float, ptr %0, i64 %90
  %92 = load float, ptr %91, align 4, !tbaa !5
  %93 = fmul float %92, %92
  %94 = getelementptr inbounds float, ptr %1, i64 %90
  store float %93, ptr %94, align 4, !tbaa !5
  %95 = add nuw nsw i64 %80, 3
  %96 = getelementptr inbounds float, ptr %0, i64 %95
  %97 = load float, ptr %96, align 4, !tbaa !5
  %98 = fmul float %97, %97
  %99 = getelementptr inbounds float, ptr %1, i64 %95
  store float %98, ptr %99, align 4, !tbaa !5
  %100 = add nuw nsw i64 %80, 4
  %101 = icmp eq i64 %100, %8
  br i1 %101, label %78, label %79, !llvm.loop !14
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
!11 = !{!"llvm.loop.isvectorized", i32 1}
!12 = distinct !{!12, !13}
!13 = !{!"llvm.loop.unroll.disable"}
!14 = distinct !{!14, !10, !11}
