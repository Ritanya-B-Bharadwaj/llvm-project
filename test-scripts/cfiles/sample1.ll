; ModuleID = '..\test-scripts\sample1.c'
source_filename = "..\\test-scripts\\sample1.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%struct.ident_t = type { i32, i32, i32, i32, ptr }

@.str = private unnamed_addr constant [22 x i8] c"Hello from thread %d\0A\00", align 1
@0 = private unnamed_addr constant [23 x i8] c";unknown;unknown;0;0;;\00", align 1
@1 = private unnamed_addr constant %struct.ident_t { i32 0, i32 34, i32 0, i32 22, ptr @0 }, align 8
@2 = private unnamed_addr constant %struct.ident_t { i32 0, i32 514, i32 0, i32 22, ptr @0 }, align 8
@3 = private unnamed_addr constant %struct.ident_t { i32 0, i32 2, i32 0, i32 22, ptr @0 }, align 8
@.gomp_critical_user_.var = common global [8 x i32] zeroinitializer, align 8
@4 = private unnamed_addr constant %struct.ident_t { i32 0, i32 66, i32 0, i32 22, ptr @0 }, align 8
@.str.1 = private unnamed_addr constant [11 x i8] c"Sum is %d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %2, align 4
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @3, i32 1, ptr @main.omp_outlined, ptr %2)
  %3 = load i32, ptr %2, align 4
  %4 = call i32 (ptr, ...) @__mingw_printf(ptr noundef @.str.1, i32 noundef %3)
  ret i32 0
}

; Function Attrs: noinline norecurse nounwind optnone uwtable
define internal void @main.omp_outlined(ptr noalias noundef %0, ptr noalias noundef %1, ptr noundef nonnull align 4 dereferenceable(4) %2) #1 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  store ptr %2, ptr %6, align 8
  %15 = load ptr, ptr %6, align 8, !nonnull !6, !align !7
  %16 = call i32 @omp_get_thread_num() #5
  store i32 %16, ptr %7, align 4
  %17 = load i32, ptr %7, align 4
  %18 = call i32 (ptr, ...) @__mingw_printf(ptr noundef @.str, i32 noundef %17)
  %19 = load ptr, ptr %4, align 8
  %20 = load i32, ptr %19, align 4
  call void @__kmpc_barrier(ptr @1, i32 %20)
  store i32 0, ptr %10, align 4
  store i32 9, ptr %11, align 4
  store i32 1, ptr %12, align 4
  store i32 0, ptr %13, align 4
  call void @__kmpc_for_static_init_4(ptr @2, i32 %20, i32 34, ptr %13, ptr %10, ptr %11, ptr %12, i32 1, i32 1)
  %21 = load i32, ptr %11, align 4
  %22 = icmp sgt i32 %21, 9
  br i1 %22, label %23, label %24

23:                                               ; preds = %3
  br label %26

24:                                               ; preds = %3
  %25 = load i32, ptr %11, align 4
  br label %26

26:                                               ; preds = %24, %23
  %27 = phi i32 [ 9, %23 ], [ %25, %24 ]
  store i32 %27, ptr %11, align 4
  %28 = load i32, ptr %10, align 4
  store i32 %28, ptr %8, align 4
  br label %29

29:                                               ; preds = %41, %26
  %30 = load i32, ptr %8, align 4
  %31 = load i32, ptr %11, align 4
  %32 = icmp sle i32 %30, %31
  br i1 %32, label %33, label %44

33:                                               ; preds = %29
  %34 = load i32, ptr %8, align 4
  %35 = mul nsw i32 %34, 1
  %36 = add nsw i32 0, %35
  store i32 %36, ptr %14, align 4
  call void @__kmpc_critical(ptr @3, i32 %20, ptr @.gomp_critical_user_.var)
  %37 = load i32, ptr %14, align 4
  %38 = load i32, ptr %15, align 4
  %39 = add nsw i32 %38, %37
  store i32 %39, ptr %15, align 4
  call void @__kmpc_end_critical(ptr @3, i32 %20, ptr @.gomp_critical_user_.var)
  br label %40

40:                                               ; preds = %33
  br label %41

41:                                               ; preds = %40
  %42 = load i32, ptr %8, align 4
  %43 = add nsw i32 %42, 1
  store i32 %43, ptr %8, align 4
  br label %29

44:                                               ; preds = %29
  br label %45

45:                                               ; preds = %44
  call void @__kmpc_for_static_fini(ptr @2, i32 %20)
  call void @__kmpc_barrier(ptr @4, i32 %20)
  ret void
}

; Function Attrs: nounwind
declare dso_local i32 @omp_get_thread_num() #2

declare dso_local i32 @__mingw_printf(ptr noundef, ...) #3

; Function Attrs: convergent nounwind
declare void @__kmpc_barrier(ptr, i32) #4

; Function Attrs: nounwind
declare void @__kmpc_for_static_init_4(ptr, i32, i32, ptr, ptr, ptr, ptr, i32, i32) #5

; Function Attrs: convergent nounwind
declare void @__kmpc_end_critical(ptr, i32, ptr) #4

; Function Attrs: convergent nounwind
declare void @__kmpc_critical(ptr, i32, ptr) #4

; Function Attrs: nounwind
declare void @__kmpc_for_static_fini(ptr, i32) #5

; Function Attrs: nounwind
declare !callback !8 void @__kmpc_fork_call(ptr, i32, ptr, ...) #5

attributes #0 = { noinline nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { noinline norecurse nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { convergent nounwind }
attributes #5 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 7, !"openmp", i32 51}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 1, !"MaxTLSAlign", i32 65536}
!5 = !{!"clang version 21.0.0git (https://github.com/Ritanya-B-Bharadwaj/llvm-project.git e29eb6637d6b8ee54f746a9c914304f83309c4ee)"}
!6 = !{}
!7 = !{i64 4}
!8 = !{!9}
!9 = !{i64 2, i64 -1, i64 -1, i1 true}
