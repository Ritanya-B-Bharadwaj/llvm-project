; ModuleID = '../test-scripts/test.ll'
source_filename = "C:\\Manasvini\\WebDev\\llvm-project\\test-scripts\\test.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%struct.ident_t = type { i32, i32, i32, i32, ptr }

@0 = private unnamed_addr constant [23 x i8] c";unknown;unknown;0;0;;\00", align 1
@1 = private unnamed_addr constant %struct.ident_t { i32 0, i32 2, i32 0, i32 22, ptr @0 }, align 8
@.str = private unnamed_addr constant [22 x i8] c"Hello from thread %d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = call i32 @__kmpc_global_thread_num(ptr @1)
  br label %3

3:                                                ; preds = %0
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @1, i32 0, ptr @main..omp_par)
  br label %4

4:                                                ; preds = %3
  ret i32 0
}

; Function Attrs: nounwind
define internal void @main..omp_par(ptr noalias %0, ptr noalias %1) #1 {
  %3 = alloca i32, align 4
  %4 = load i32, ptr %0, align 4
  store i32 %4, ptr %3, align 4
  %5 = load i32, ptr %3, align 4
  br label %6

6:                                                ; preds = %2
  %7 = call i32 @omp_get_thread_num() #2, !omp.annotation !6
  %8 = call i32 (ptr, ...) @__mingw_printf(ptr noundef @.str, i32 noundef %7)
  br label %9

9:                                                ; preds = %6
  br label %10

10:                                               ; preds = %9
  br label %11

11:                                               ; preds = %10
  ret void
}

; Function Attrs: nounwind
declare i32 @__kmpc_global_thread_num(ptr) #2

declare dso_local i32 @__mingw_printf(ptr noundef, ...) #3

; Function Attrs: nounwind
declare dso_local i32 @omp_get_thread_num() #4

; Function Attrs: nounwind
declare !callback !7 void @__kmpc_fork_call(ptr, i32, ptr, ...) #2

attributes #0 = { noinline nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 7, !"openmp", i32 51}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 1, !"MaxTLSAlign", i32 65536}
!5 = !{!"clang version 21.0.0git (https://github.com/Ritanya-B-Bharadwaj/llvm-project.git e29eb6637d6b8ee54f746a9c914304f83309c4ee)"}
!6 = !{!"omp.annotation"}
!7 = !{!8}
!8 = !{i64 2, i64 -1, i64 -1, i1 true}
