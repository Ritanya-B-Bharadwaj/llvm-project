; ModuleID = '../tests/trig_compute.c'
source_filename = "../tests/trig_compute.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nofree nounwind uwtable
define dso_local void @trig_compute(ptr nocapture noundef %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %4, label %23

4:                                                ; preds = %2
  %5 = zext i32 %1 to i64
  %6 = and i64 %5, 1
  %7 = icmp eq i32 %1, 1
  br i1 %7, label %10, label %8

8:                                                ; preds = %4
  %9 = and i64 %5, 4294967294
  br label %24

10:                                               ; preds = %24, %4
  %11 = phi i64 [ 0, %4 ], [ %46, %24 ]
  %12 = icmp eq i64 %6, 0
  br i1 %12, label %23, label %13

13:                                               ; preds = %10
  %14 = getelementptr inbounds float, ptr %0, i64 %11
  %15 = load float, ptr %14, align 4, !tbaa !5
  %16 = fpext float %15 to double
  %17 = tail call double @sin(double noundef %16) #2
  %18 = load float, ptr %14, align 4, !tbaa !5
  %19 = fpext float %18 to double
  %20 = tail call double @cos(double noundef %19) #2
  %21 = fadd double %17, %20
  %22 = fptrunc double %21 to float
  store float %22, ptr %14, align 4, !tbaa !5
  br label %23

23:                                               ; preds = %13, %10, %2
  ret void

24:                                               ; preds = %24, %8
  %25 = phi i64 [ 0, %8 ], [ %46, %24 ]
  %26 = phi i64 [ 0, %8 ], [ %47, %24 ]
  %27 = getelementptr inbounds float, ptr %0, i64 %25
  %28 = load float, ptr %27, align 4, !tbaa !5
  %29 = fpext float %28 to double
  %30 = tail call double @sin(double noundef %29) #2
  %31 = load float, ptr %27, align 4, !tbaa !5
  %32 = fpext float %31 to double
  %33 = tail call double @cos(double noundef %32) #2
  %34 = fadd double %30, %33
  %35 = fptrunc double %34 to float
  store float %35, ptr %27, align 4, !tbaa !5
  %36 = or i64 %25, 1
  %37 = getelementptr inbounds float, ptr %0, i64 %36
  %38 = load float, ptr %37, align 4, !tbaa !5
  %39 = fpext float %38 to double
  %40 = tail call double @sin(double noundef %39) #2
  %41 = load float, ptr %37, align 4, !tbaa !5
  %42 = fpext float %41 to double
  %43 = tail call double @cos(double noundef %42) #2
  %44 = fadd double %40, %43
  %45 = fptrunc double %44 to float
  store float %45, ptr %37, align 4, !tbaa !5
  %46 = add nuw nsw i64 %25, 2
  %47 = add i64 %26, 2
  %48 = icmp eq i64 %47, %9
  br i1 %48, label %10, label %24, !llvm.loop !9
}

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @sin(double noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @cos(double noundef) local_unnamed_addr #1

attributes #0 = { nofree nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree nounwind willreturn writeonly "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

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
