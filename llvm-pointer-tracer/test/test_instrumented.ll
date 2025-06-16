; ModuleID = './test/test.ll'
source_filename = "./test/test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@__const.bar.arr = private unnamed_addr constant [5 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5], align 16
@func_name_fmt = private constant [7 x i8] c"foo(),\00"
@ptr_fmt = private constant [8 x i8] c" 0x%lx,\00"
@newline_fmt = private constant [2 x i8] c"\0A\00"
@func_name_fmt.1 = private constant [7 x i8] c"bar(),\00"
@ptr_fmt.2 = private constant [8 x i8] c" 0x%lx,\00"
@newline_fmt.3 = private constant [2 x i8] c"\0A\00"
@func_name_fmt.4 = private constant [8 x i8] c"main(),\00"
@ptr_fmt.5 = private constant [8 x i8] c" 0x%lx,\00"
@newline_fmt.6 = private constant [2 x i8] c"\0A\00"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @foo() #0 {
  %1 = call i32 (ptr, ...) @printf(ptr @func_name_fmt)
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 42, ptr %2, align 4
  %8 = ptrtoint ptr %2 to i64
  %9 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %8)
  store i32 24, ptr %3, align 4
  %10 = ptrtoint ptr %3 to i64
  %11 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %10)
  store ptr %2, ptr %4, align 8
  %12 = ptrtoint ptr %4 to i64
  %13 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %12)
  store ptr %3, ptr %5, align 8
  %14 = ptrtoint ptr %5 to i64
  %15 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %14)
  %16 = load ptr, ptr %4, align 8
  %17 = ptrtoint ptr %4 to i64
  %18 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %17)
  %19 = load i32, ptr %16, align 4
  %20 = ptrtoint ptr %16 to i64
  %21 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %20)
  store i32 %19, ptr %6, align 4
  %22 = ptrtoint ptr %6 to i64
  %23 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %22)
  %24 = load ptr, ptr %5, align 8
  %25 = ptrtoint ptr %5 to i64
  %26 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %25)
  %27 = load i32, ptr %24, align 4
  %28 = ptrtoint ptr %24 to i64
  %29 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %28)
  store i32 %27, ptr %7, align 4
  %30 = ptrtoint ptr %7 to i64
  %31 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %30)
  %32 = load i32, ptr %7, align 4
  %33 = ptrtoint ptr %7 to i64
  %34 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %33)
  %35 = load ptr, ptr %4, align 8
  %36 = ptrtoint ptr %4 to i64
  %37 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %36)
  store i32 %32, ptr %35, align 4
  %38 = ptrtoint ptr %35 to i64
  %39 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %38)
  %40 = load i32, ptr %6, align 4
  %41 = ptrtoint ptr %6 to i64
  %42 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %41)
  %43 = load ptr, ptr %5, align 8
  %44 = ptrtoint ptr %5 to i64
  %45 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %44)
  store i32 %40, ptr %43, align 4
  %46 = ptrtoint ptr %43 to i64
  %47 = call i32 (ptr, ...) @printf(ptr @ptr_fmt, i64 %46)
  %48 = call i32 (ptr, ...) @printf(ptr @newline_fmt)
  %49 = call i32 @fflush(ptr null)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @bar() #0 {
  %1 = call i32 (ptr, ...) @printf(ptr @func_name_fmt.1)
  %2 = alloca [5 x i32], align 16
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 16 %2, ptr align 16 @__const.bar.arr, i64 20, i1 false)
  %6 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 0
  %7 = ptrtoint ptr %6 to i64
  %8 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %7)
  store ptr %6, ptr %3, align 8
  %9 = ptrtoint ptr %3 to i64
  %10 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %9)
  store i32 0, ptr %4, align 4
  %11 = ptrtoint ptr %4 to i64
  %12 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %11)
  br label %13

13:                                               ; preds = %50, %0
  %14 = load i32, ptr %4, align 4
  %15 = ptrtoint ptr %4 to i64
  %16 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %15)
  %17 = icmp slt i32 %14, 3
  br i1 %17, label %18, label %57

18:                                               ; preds = %13
  %19 = load ptr, ptr %3, align 8
  %20 = ptrtoint ptr %3 to i64
  %21 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %20)
  %22 = load i32, ptr %4, align 4
  %23 = ptrtoint ptr %4 to i64
  %24 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %23)
  %25 = sext i32 %22 to i64
  %26 = getelementptr inbounds i32, ptr %19, i64 %25
  %27 = ptrtoint ptr %26 to i64
  %28 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %27)
  %29 = load i32, ptr %26, align 4
  %30 = ptrtoint ptr %26 to i64
  %31 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %30)
  store i32 %29, ptr %5, align 4
  %32 = ptrtoint ptr %5 to i64
  %33 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %32)
  %34 = load i32, ptr %5, align 4
  %35 = ptrtoint ptr %5 to i64
  %36 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %35)
  %37 = mul nsw i32 %34, 2
  %38 = load ptr, ptr %3, align 8
  %39 = ptrtoint ptr %3 to i64
  %40 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %39)
  %41 = load i32, ptr %4, align 4
  %42 = ptrtoint ptr %4 to i64
  %43 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %42)
  %44 = sext i32 %41 to i64
  %45 = getelementptr inbounds i32, ptr %38, i64 %44
  %46 = ptrtoint ptr %45 to i64
  %47 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %46)
  store i32 %37, ptr %45, align 4
  %48 = ptrtoint ptr %45 to i64
  %49 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %48)
  br label %50

50:                                               ; preds = %18
  %51 = load i32, ptr %4, align 4
  %52 = ptrtoint ptr %4 to i64
  %53 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %52)
  %54 = add nsw i32 %51, 1
  store i32 %54, ptr %4, align 4
  %55 = ptrtoint ptr %4 to i64
  %56 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.2, i64 %55)
  br label %13, !llvm.loop !6

57:                                               ; preds = %13
  %58 = call i32 (ptr, ...) @printf(ptr @newline_fmt.3)
  %59 = call i32 @fflush(ptr null)
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (ptr, ...) @printf(ptr @func_name_fmt.4)
  %2 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  %3 = ptrtoint ptr %2 to i64
  %4 = call i32 (ptr, ...) @printf(ptr @ptr_fmt.5, i64 %3)
  call void @foo()
  call void @bar()
  %5 = call i32 (ptr, ...) @printf(ptr @newline_fmt.6)
  %6 = call i32 @fflush(ptr null)
  ret i32 0
}

declare i32 @printf(ptr, ...)

declare i32 @fflush(ptr)

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 21.0.0git (https://github.com/PranavDarshan/llvm-project e29eb6637d6b8ee54f746a9c914304f83309c4ee)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
