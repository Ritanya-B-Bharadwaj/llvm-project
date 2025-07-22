; ModuleID = '../tests/test.c'
source_filename = "../tests/test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree nounwind willreturn uwtable
define dso_local void @compute(ptr nocapture noundef %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = load float, ptr %0, align 4, !tbaa !5
  %4 = fpext float %3 to double
  %5 = tail call double @sin(double noundef %4) #2
  %6 = load float, ptr %0, align 4, !tbaa !5
  %7 = fpext float %6 to double
  %8 = tail call double @cos(double noundef %7) #2
  %9 = fadd double %5, %8
  %10 = fptrunc double %9 to float
  store float %10, ptr %0, align 4, !tbaa !5
  %11 = getelementptr inbounds float, ptr %0, i64 1
  %12 = load float, ptr %11, align 4, !tbaa !5
  %13 = fpext float %12 to double
  %14 = tail call double @sin(double noundef %13) #2
  %15 = load float, ptr %11, align 4, !tbaa !5
  %16 = fpext float %15 to double
  %17 = tail call double @cos(double noundef %16) #2
  %18 = fadd double %14, %17
  %19 = fptrunc double %18 to float
  store float %19, ptr %11, align 4, !tbaa !5
  %20 = getelementptr inbounds float, ptr %0, i64 2
  %21 = load float, ptr %20, align 4, !tbaa !5
  %22 = fpext float %21 to double
  %23 = tail call double @sin(double noundef %22) #2
  %24 = load float, ptr %20, align 4, !tbaa !5
  %25 = fpext float %24 to double
  %26 = tail call double @cos(double noundef %25) #2
  %27 = fadd double %23, %26
  %28 = fptrunc double %27 to float
  store float %28, ptr %20, align 4, !tbaa !5
  %29 = getelementptr inbounds float, ptr %0, i64 3
  %30 = load float, ptr %29, align 4, !tbaa !5
  %31 = fpext float %30 to double
  %32 = tail call double @sin(double noundef %31) #2
  %33 = load float, ptr %29, align 4, !tbaa !5
  %34 = fpext float %33 to double
  %35 = tail call double @cos(double noundef %34) #2
  %36 = fadd double %32, %35
  %37 = fptrunc double %36 to float
  store float %37, ptr %29, align 4, !tbaa !5
  %38 = getelementptr inbounds float, ptr %0, i64 4
  %39 = load float, ptr %38, align 4, !tbaa !5
  %40 = fpext float %39 to double
  %41 = tail call double @sin(double noundef %40) #2
  %42 = load float, ptr %38, align 4, !tbaa !5
  %43 = fpext float %42 to double
  %44 = tail call double @cos(double noundef %43) #2
  %45 = fadd double %41, %44
  %46 = fptrunc double %45 to float
  store float %46, ptr %38, align 4, !tbaa !5
  %47 = getelementptr inbounds float, ptr %0, i64 5
  %48 = load float, ptr %47, align 4, !tbaa !5
  %49 = fpext float %48 to double
  %50 = tail call double @sin(double noundef %49) #2
  %51 = load float, ptr %47, align 4, !tbaa !5
  %52 = fpext float %51 to double
  %53 = tail call double @cos(double noundef %52) #2
  %54 = fadd double %50, %53
  %55 = fptrunc double %54 to float
  store float %55, ptr %47, align 4, !tbaa !5
  %56 = getelementptr inbounds float, ptr %0, i64 6
  %57 = load float, ptr %56, align 4, !tbaa !5
  %58 = fpext float %57 to double
  %59 = tail call double @sin(double noundef %58) #2
  %60 = load float, ptr %56, align 4, !tbaa !5
  %61 = fpext float %60 to double
  %62 = tail call double @cos(double noundef %61) #2
  %63 = fadd double %59, %62
  %64 = fptrunc double %63 to float
  store float %64, ptr %56, align 4, !tbaa !5
  %65 = getelementptr inbounds float, ptr %0, i64 7
  %66 = load float, ptr %65, align 4, !tbaa !5
  %67 = fpext float %66 to double
  %68 = tail call double @sin(double noundef %67) #2
  %69 = load float, ptr %65, align 4, !tbaa !5
  %70 = fpext float %69 to double
  %71 = tail call double @cos(double noundef %70) #2
  %72 = fadd double %68, %71
  %73 = fptrunc double %72 to float
  store float %73, ptr %65, align 4, !tbaa !5
  %74 = getelementptr inbounds float, ptr %0, i64 8
  %75 = load float, ptr %74, align 4, !tbaa !5
  %76 = fpext float %75 to double
  %77 = tail call double @sin(double noundef %76) #2
  %78 = load float, ptr %74, align 4, !tbaa !5
  %79 = fpext float %78 to double
  %80 = tail call double @cos(double noundef %79) #2
  %81 = fadd double %77, %80
  %82 = fptrunc double %81 to float
  store float %82, ptr %74, align 4, !tbaa !5
  %83 = getelementptr inbounds float, ptr %0, i64 9
  %84 = load float, ptr %83, align 4, !tbaa !5
  %85 = fpext float %84 to double
  %86 = tail call double @sin(double noundef %85) #2
  %87 = load float, ptr %83, align 4, !tbaa !5
  %88 = fpext float %87 to double
  %89 = tail call double @cos(double noundef %88) #2
  %90 = fadd double %86, %89
  %91 = fptrunc double %90 to float
  store float %91, ptr %83, align 4, !tbaa !5
  ret void
}

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @sin(double noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nofree nounwind willreturn writeonly
declare double @cos(double noundef) local_unnamed_addr #1

attributes #0 = { mustprogress nofree nounwind willreturn uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
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
