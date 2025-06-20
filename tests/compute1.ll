; ModuleID = 'compute1.c'
source_filename = "compute1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nofree nounwind writeonly uwtable
define dso_local void @heavy_compute(ptr nocapture noundef writeonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %5, label %4

4:                                                ; preds = %5, %2
  ret void

5:                                                ; preds = %2, %5
  %6 = phi i32 [ %23, %5 ], [ 0, %2 ]
  %7 = sitofp i32 %6 to float
  %8 = fpext float %7 to double
  %9 = tail call double @sin(double noundef %8) #3
  %10 = tail call double @cos(double noundef %8) #3
  %11 = fadd double %9, %10
  %12 = fptrunc double %11 to float
  %13 = fadd float %12, 1.000000e+00
  %14 = fdiv float %7, %13
  %15 = tail call float @llvm.fmuladd.f32(float %7, float %12, float %14)
  %16 = fadd float %7, 2.000000e+00
  %17 = tail call float @sqrtf(float noundef %16) #3
  %18 = fsub float %15, %17
  %19 = fpext float %18 to double
  %20 = tail call double @exp(double noundef %19) #3
  %21 = fmul double %20, %19
  %22 = fptrunc double %21 to float
  store float %22, ptr %0, align 4, !tbaa !5
  %23 = add nuw nsw i32 %6, 1
  %24 = icmp eq i32 %23, %1
  br i1 %24, label %4, label %5, !llvm.loop !9
}

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @sin(double noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @cos(double noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare float @llvm.fmuladd.f32(float, float, float) #2

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare float @sqrtf(float noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @exp(double noundef) local_unnamed_addr #1

attributes #0 = { nofree nounwind writeonly uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree nounwind willreturn writeonly "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #3 = { nounwind }

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
