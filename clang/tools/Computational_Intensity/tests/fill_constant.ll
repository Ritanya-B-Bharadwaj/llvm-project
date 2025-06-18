; ModuleID = '../tests/fill_constant.c'
source_filename = "../tests/fill_constant.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree norecurse nosync nounwind writeonly uwtable
define dso_local void @fill_constant(ptr nocapture noundef writeonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %4, label %48

4:                                                ; preds = %2
  %5 = zext i32 %1 to i64
  %6 = icmp ult i32 %1, 8
  br i1 %6, label %46, label %7

7:                                                ; preds = %4
  %8 = and i64 %5, 4294967288
  %9 = add nsw i64 %8, -8
  %10 = lshr exact i64 %9, 3
  %11 = add nuw nsw i64 %10, 1
  %12 = and i64 %11, 3
  %13 = icmp ult i64 %9, 24
  br i1 %13, label %33, label %14

14:                                               ; preds = %7
  %15 = and i64 %11, 4611686018427387900
  br label %16

16:                                               ; preds = %16, %14
  %17 = phi i64 [ 0, %14 ], [ %30, %16 ]
  %18 = phi i64 [ 0, %14 ], [ %31, %16 ]
  %19 = getelementptr inbounds float, ptr %0, i64 %17
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %19, align 4, !tbaa !5
  %20 = getelementptr inbounds float, ptr %19, i64 4
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %20, align 4, !tbaa !5
  %21 = or i64 %17, 8
  %22 = getelementptr inbounds float, ptr %0, i64 %21
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %22, align 4, !tbaa !5
  %23 = getelementptr inbounds float, ptr %22, i64 4
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %23, align 4, !tbaa !5
  %24 = or i64 %17, 16
  %25 = getelementptr inbounds float, ptr %0, i64 %24
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %25, align 4, !tbaa !5
  %26 = getelementptr inbounds float, ptr %25, i64 4
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %26, align 4, !tbaa !5
  %27 = or i64 %17, 24
  %28 = getelementptr inbounds float, ptr %0, i64 %27
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %28, align 4, !tbaa !5
  %29 = getelementptr inbounds float, ptr %28, i64 4
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %29, align 4, !tbaa !5
  %30 = add nuw i64 %17, 32
  %31 = add i64 %18, 4
  %32 = icmp eq i64 %31, %15
  br i1 %32, label %33, label %16, !llvm.loop !9

33:                                               ; preds = %16, %7
  %34 = phi i64 [ 0, %7 ], [ %30, %16 ]
  %35 = icmp eq i64 %12, 0
  br i1 %35, label %44, label %36

36:                                               ; preds = %33, %36
  %37 = phi i64 [ %41, %36 ], [ %34, %33 ]
  %38 = phi i64 [ %42, %36 ], [ 0, %33 ]
  %39 = getelementptr inbounds float, ptr %0, i64 %37
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %39, align 4, !tbaa !5
  %40 = getelementptr inbounds float, ptr %39, i64 4
  store <4 x float> <float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000, float 0x40091EB860000000>, ptr %40, align 4, !tbaa !5
  %41 = add nuw i64 %37, 8
  %42 = add i64 %38, 1
  %43 = icmp eq i64 %42, %12
  br i1 %43, label %44, label %36, !llvm.loop !12

44:                                               ; preds = %36, %33
  %45 = icmp eq i64 %8, %5
  br i1 %45, label %48, label %46

46:                                               ; preds = %4, %44
  %47 = phi i64 [ 0, %4 ], [ %8, %44 ]
  br label %49

48:                                               ; preds = %49, %44, %2
  ret void

49:                                               ; preds = %46, %49
  %50 = phi i64 [ %52, %49 ], [ %47, %46 ]
  %51 = getelementptr inbounds float, ptr %0, i64 %50
  store float 0x40091EB860000000, ptr %51, align 4, !tbaa !5
  %52 = add nuw nsw i64 %50, 1
  %53 = icmp eq i64 %52, %5
  br i1 %53, label %48, label %49, !llvm.loop !14
}

attributes #0 = { argmemonly nofree norecurse nosync nounwind writeonly uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

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
!14 = distinct !{!14, !10, !15, !11}
!15 = !{!"llvm.loop.unroll.runtime.disable"}
