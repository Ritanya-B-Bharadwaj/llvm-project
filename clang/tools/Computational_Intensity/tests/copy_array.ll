; ModuleID = '../tests/copy_array.c'
source_filename = "../tests/copy_array.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree norecurse nosync nounwind uwtable
define dso_local void @copy_array(ptr nocapture noundef readonly %0, ptr nocapture noundef writeonly %1, i32 noundef %2) local_unnamed_addr #0 {
  %4 = icmp sgt i32 %2, 0
  br i1 %4, label %5, label %90

5:                                                ; preds = %3
  %6 = ptrtoint ptr %1 to i64
  %7 = ptrtoint ptr %0 to i64
  %8 = zext i32 %2 to i64
  %9 = icmp ult i32 %2, 8
  %10 = sub i64 %6, %7
  %11 = icmp ult i64 %10, 32
  %12 = or i1 %9, %11
  br i1 %12, label %72, label %13

13:                                               ; preds = %5
  %14 = and i64 %8, 4294967288
  %15 = add nsw i64 %14, -8
  %16 = lshr exact i64 %15, 3
  %17 = add nuw nsw i64 %16, 1
  %18 = and i64 %17, 3
  %19 = icmp ult i64 %15, 24
  br i1 %19, label %55, label %20

20:                                               ; preds = %13
  %21 = and i64 %17, 4611686018427387900
  br label %22

22:                                               ; preds = %22, %20
  %23 = phi i64 [ 0, %20 ], [ %52, %22 ]
  %24 = phi i64 [ 0, %20 ], [ %53, %22 ]
  %25 = getelementptr inbounds float, ptr %0, i64 %23
  %26 = load <4 x float>, ptr %25, align 4, !tbaa !5
  %27 = getelementptr inbounds float, ptr %25, i64 4
  %28 = load <4 x float>, ptr %27, align 4, !tbaa !5
  %29 = getelementptr inbounds float, ptr %1, i64 %23
  store <4 x float> %26, ptr %29, align 4, !tbaa !5
  %30 = getelementptr inbounds float, ptr %29, i64 4
  store <4 x float> %28, ptr %30, align 4, !tbaa !5
  %31 = or i64 %23, 8
  %32 = getelementptr inbounds float, ptr %0, i64 %31
  %33 = load <4 x float>, ptr %32, align 4, !tbaa !5
  %34 = getelementptr inbounds float, ptr %32, i64 4
  %35 = load <4 x float>, ptr %34, align 4, !tbaa !5
  %36 = getelementptr inbounds float, ptr %1, i64 %31
  store <4 x float> %33, ptr %36, align 4, !tbaa !5
  %37 = getelementptr inbounds float, ptr %36, i64 4
  store <4 x float> %35, ptr %37, align 4, !tbaa !5
  %38 = or i64 %23, 16
  %39 = getelementptr inbounds float, ptr %0, i64 %38
  %40 = load <4 x float>, ptr %39, align 4, !tbaa !5
  %41 = getelementptr inbounds float, ptr %39, i64 4
  %42 = load <4 x float>, ptr %41, align 4, !tbaa !5
  %43 = getelementptr inbounds float, ptr %1, i64 %38
  store <4 x float> %40, ptr %43, align 4, !tbaa !5
  %44 = getelementptr inbounds float, ptr %43, i64 4
  store <4 x float> %42, ptr %44, align 4, !tbaa !5
  %45 = or i64 %23, 24
  %46 = getelementptr inbounds float, ptr %0, i64 %45
  %47 = load <4 x float>, ptr %46, align 4, !tbaa !5
  %48 = getelementptr inbounds float, ptr %46, i64 4
  %49 = load <4 x float>, ptr %48, align 4, !tbaa !5
  %50 = getelementptr inbounds float, ptr %1, i64 %45
  store <4 x float> %47, ptr %50, align 4, !tbaa !5
  %51 = getelementptr inbounds float, ptr %50, i64 4
  store <4 x float> %49, ptr %51, align 4, !tbaa !5
  %52 = add nuw i64 %23, 32
  %53 = add i64 %24, 4
  %54 = icmp eq i64 %53, %21
  br i1 %54, label %55, label %22, !llvm.loop !9

55:                                               ; preds = %22, %13
  %56 = phi i64 [ 0, %13 ], [ %52, %22 ]
  %57 = icmp eq i64 %18, 0
  br i1 %57, label %70, label %58

58:                                               ; preds = %55, %58
  %59 = phi i64 [ %67, %58 ], [ %56, %55 ]
  %60 = phi i64 [ %68, %58 ], [ 0, %55 ]
  %61 = getelementptr inbounds float, ptr %0, i64 %59
  %62 = load <4 x float>, ptr %61, align 4, !tbaa !5
  %63 = getelementptr inbounds float, ptr %61, i64 4
  %64 = load <4 x float>, ptr %63, align 4, !tbaa !5
  %65 = getelementptr inbounds float, ptr %1, i64 %59
  store <4 x float> %62, ptr %65, align 4, !tbaa !5
  %66 = getelementptr inbounds float, ptr %65, i64 4
  store <4 x float> %64, ptr %66, align 4, !tbaa !5
  %67 = add nuw i64 %59, 8
  %68 = add i64 %60, 1
  %69 = icmp eq i64 %68, %18
  br i1 %69, label %70, label %58, !llvm.loop !12

70:                                               ; preds = %58, %55
  %71 = icmp eq i64 %14, %8
  br i1 %71, label %90, label %72

72:                                               ; preds = %5, %70
  %73 = phi i64 [ 0, %5 ], [ %14, %70 ]
  %74 = xor i64 %73, -1
  %75 = add nsw i64 %74, %8
  %76 = and i64 %8, 3
  %77 = icmp eq i64 %76, 0
  br i1 %77, label %87, label %78

78:                                               ; preds = %72, %78
  %79 = phi i64 [ %84, %78 ], [ %73, %72 ]
  %80 = phi i64 [ %85, %78 ], [ 0, %72 ]
  %81 = getelementptr inbounds float, ptr %0, i64 %79
  %82 = load float, ptr %81, align 4, !tbaa !5
  %83 = getelementptr inbounds float, ptr %1, i64 %79
  store float %82, ptr %83, align 4, !tbaa !5
  %84 = add nuw nsw i64 %79, 1
  %85 = add i64 %80, 1
  %86 = icmp eq i64 %85, %76
  br i1 %86, label %87, label %78, !llvm.loop !14

87:                                               ; preds = %78, %72
  %88 = phi i64 [ %73, %72 ], [ %84, %78 ]
  %89 = icmp ult i64 %75, 3
  br i1 %89, label %90, label %91

90:                                               ; preds = %87, %91, %70, %3
  ret void

91:                                               ; preds = %87, %91
  %92 = phi i64 [ %108, %91 ], [ %88, %87 ]
  %93 = getelementptr inbounds float, ptr %0, i64 %92
  %94 = load float, ptr %93, align 4, !tbaa !5
  %95 = getelementptr inbounds float, ptr %1, i64 %92
  store float %94, ptr %95, align 4, !tbaa !5
  %96 = add nuw nsw i64 %92, 1
  %97 = getelementptr inbounds float, ptr %0, i64 %96
  %98 = load float, ptr %97, align 4, !tbaa !5
  %99 = getelementptr inbounds float, ptr %1, i64 %96
  store float %98, ptr %99, align 4, !tbaa !5
  %100 = add nuw nsw i64 %92, 2
  %101 = getelementptr inbounds float, ptr %0, i64 %100
  %102 = load float, ptr %101, align 4, !tbaa !5
  %103 = getelementptr inbounds float, ptr %1, i64 %100
  store float %102, ptr %103, align 4, !tbaa !5
  %104 = add nuw nsw i64 %92, 3
  %105 = getelementptr inbounds float, ptr %0, i64 %104
  %106 = load float, ptr %105, align 4, !tbaa !5
  %107 = getelementptr inbounds float, ptr %1, i64 %104
  store float %106, ptr %107, align 4, !tbaa !5
  %108 = add nuw nsw i64 %92, 4
  %109 = icmp eq i64 %108, %8
  br i1 %109, label %90, label %91, !llvm.loop !15
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
!14 = distinct !{!14, !13}
!15 = distinct !{!15, !10, !11}
