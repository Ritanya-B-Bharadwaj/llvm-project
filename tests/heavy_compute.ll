; ModuleID = '../tests/heavy_compute.c'
source_filename = "../tests/heavy_compute.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: argmemonly nofree nosync nounwind writeonly uwtable
define dso_local void @heavy_compute(ptr nocapture noundef writeonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %4, label %23

4:                                                ; preds = %2
  %5 = add nsw i32 %1, -1
  %6 = sitofp i32 %5 to float
  %7 = fpext float %6 to double
  %8 = tail call fast double @llvm.sin.f64(double %7)
  %9 = tail call fast double @llvm.cos.f64(double %7)
  %10 = fadd fast double %8, %9
  %11 = fptrunc double %10 to float
  %12 = fmul fast float %11, %6
  %13 = fadd fast float %6, 2.000000e+00
  %14 = tail call fast float @llvm.sqrt.f32(float %13)
  %15 = fsub fast float %12, %14
  %16 = fadd fast float %11, 1.000000e+00
  %17 = fdiv fast float %6, %16
  %18 = fadd fast float %15, %17
  %19 = fpext float %18 to double
  %20 = tail call fast double @llvm.exp.f64(double %19)
  %21 = fmul fast double %20, %19
  %22 = fptrunc double %21 to float
  store float %22, ptr %0, align 4, !tbaa !5
  br label %23

23:                                               ; preds = %4, %2
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.sin.f64(double) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.cos.f64(double) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare float @llvm.sqrt.f32(float) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.exp.f64(double) #1

attributes #0 = { argmemonly nofree nosync nounwind writeonly uwtable "approx-func-fp-math"="true" "denormal-fp-math"="preserve-sign,preserve-sign" "frame-pointer"="none" "min-legal-vector-width"="0" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="true" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn }

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
