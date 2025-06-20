; ModuleID = 'test/testit.c'
source_filename = "test/testit.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.ompi_predefined_datatype_t = type opaque
%struct.ompi_predefined_communicator_t = type opaque

@ompi_mpi_int = external global %struct.ompi_predefined_datatype_t, align 1
@ompi_mpi_comm_world = external global %struct.ompi_predefined_communicator_t, align 1
@.str = private unnamed_addr constant [39 x i8] c"Process %d received reduced result %d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @manual_sum_reduction(ptr noundef %0, ptr noundef %1, i32 noundef %2, ptr noundef %3) #0 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca ptr, align 8
  %14 = alloca i64, align 8
  %15 = alloca i32, align 4
  %16 = alloca i32, align 4
  store ptr %0, ptr %5, align 8
  store ptr %1, ptr %6, align 8
  store i32 %2, ptr %7, align 4
  store ptr %3, ptr %8, align 8
  %17 = load ptr, ptr %8, align 8
  %18 = call i32 @MPI_Comm_rank(ptr noundef %17, ptr noundef %9)
  %19 = load ptr, ptr %8, align 8
  %20 = call i32 @MPI_Comm_size(ptr noundef %19, ptr noundef %10)
  store i32 0, ptr %11, align 4
  br label %21

21:                                               ; preds = %30, %4
  %22 = load i32, ptr %11, align 4
  %23 = load i32, ptr %7, align 4
  %24 = icmp slt i32 %22, %23
  br i1 %24, label %25, label %33

25:                                               ; preds = %21
  %26 = load ptr, ptr %6, align 8
  %27 = load i32, ptr %11, align 4
  %28 = sext i32 %27 to i64
  %29 = getelementptr inbounds i32, ptr %26, i64 %28
  store i32 0, ptr %29, align 4
  br label %30

30:                                               ; preds = %25
  %31 = load i32, ptr %11, align 4
  %32 = add nsw i32 %31, 1
  store i32 %32, ptr %11, align 4
  br label %21, !llvm.loop !6

33:                                               ; preds = %21
  store i32 0, ptr %12, align 4
  br label %34

34:                                               ; preds = %67, %33
  %35 = load i32, ptr %12, align 4
  %36 = load i32, ptr %10, align 4
  %37 = icmp slt i32 %35, %36
  br i1 %37, label %38, label %70

38:                                               ; preds = %34
  %39 = load i32, ptr %7, align 4
  %40 = zext i32 %39 to i64
  %41 = call ptr @llvm.stacksave()
  store ptr %41, ptr %13, align 8
  %42 = alloca i32, i64 %40, align 16
  store i64 %40, ptr %14, align 8
  %43 = load i32, ptr %7, align 4
  %44 = load i32, ptr %12, align 4
  %45 = load ptr, ptr %8, align 8
  %46 = call i32 @MPI_Recv(ptr noundef %42, i32 noundef %43, ptr noundef @ompi_mpi_int, i32 noundef %44, i32 noundef 0, ptr noundef %45, ptr noundef null)
  store i32 0, ptr %15, align 4
  br label %47

47:                                               ; preds = %62, %38
  %48 = load i32, ptr %15, align 4
  %49 = load i32, ptr %7, align 4
  %50 = icmp slt i32 %48, %49
  br i1 %50, label %51, label %65

51:                                               ; preds = %47
  %52 = load i32, ptr %15, align 4
  %53 = sext i32 %52 to i64
  %54 = getelementptr inbounds i32, ptr %42, i64 %53
  %55 = load i32, ptr %54, align 4
  %56 = load ptr, ptr %6, align 8
  %57 = load i32, ptr %15, align 4
  %58 = sext i32 %57 to i64
  %59 = getelementptr inbounds i32, ptr %56, i64 %58
  %60 = load i32, ptr %59, align 4
  %61 = add nsw i32 %60, %55
  store i32 %61, ptr %59, align 4
  br label %62

62:                                               ; preds = %51
  %63 = load i32, ptr %15, align 4
  %64 = add nsw i32 %63, 1
  store i32 %64, ptr %15, align 4
  br label %47, !llvm.loop !8

65:                                               ; preds = %47
  %66 = load ptr, ptr %13, align 8
  call void @llvm.stackrestore(ptr %66)
  br label %67

67:                                               ; preds = %65
  %68 = load i32, ptr %12, align 4
  %69 = add nsw i32 %68, 1
  store i32 %69, ptr %12, align 4
  br label %34, !llvm.loop !9

70:                                               ; preds = %34
  store i32 0, ptr %16, align 4
  br label %71

71:                                               ; preds = %81, %70
  %72 = load i32, ptr %16, align 4
  %73 = load i32, ptr %10, align 4
  %74 = icmp slt i32 %72, %73
  br i1 %74, label %75, label %84

75:                                               ; preds = %71
  %76 = load ptr, ptr %6, align 8
  %77 = load i32, ptr %7, align 4
  %78 = load i32, ptr %16, align 4
  %79 = load ptr, ptr %8, align 8
  %80 = call i32 @MPI_Send(ptr noundef %76, i32 noundef %77, ptr noundef @ompi_mpi_int, i32 noundef %78, i32 noundef 0, ptr noundef %79)
  br label %81

81:                                               ; preds = %75
  %82 = load i32, ptr %16, align 4
  %83 = add nsw i32 %82, 1
  store i32 %83, ptr %16, align 4
  br label %71, !llvm.loop !10

84:                                               ; preds = %71
  ret void
}

declare i32 @MPI_Comm_rank(ptr noundef, ptr noundef) #1

declare i32 @MPI_Comm_size(ptr noundef, ptr noundef) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave() #2

declare i32 @MPI_Recv(ptr noundef, i32 noundef, ptr noundef, i32 noundef, i32 noundef, ptr noundef, ptr noundef) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore(ptr) #2

declare i32 @MPI_Send(ptr noundef, i32 noundef, ptr noundef, i32 noundef, i32 noundef, ptr noundef) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca [1 x i32], align 4
  %8 = alloca [1 x i32], align 4
  store i32 0, ptr %3, align 4
  store i32 %0, ptr %4, align 4
  store ptr %1, ptr %5, align 8
  %9 = call i32 @MPI_Init(ptr noundef %4, ptr noundef %5)
  %10 = call i32 @MPI_Comm_rank(ptr noundef @ompi_mpi_comm_world, ptr noundef %6)
  %11 = getelementptr inbounds [1 x i32], ptr %7, i64 0, i64 0
  %12 = load i32, ptr %6, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, ptr %11, align 4
  %14 = getelementptr inbounds [1 x i32], ptr %7, i64 0, i64 0
  %15 = getelementptr inbounds [1 x i32], ptr %8, i64 0, i64 0
  call void @manual_sum_reduction(ptr noundef %14, ptr noundef %15, i32 noundef 1, ptr noundef @ompi_mpi_comm_world)
  %16 = load i32, ptr %6, align 4
  %17 = getelementptr inbounds [1 x i32], ptr %8, i64 0, i64 0
  %18 = load i32, ptr %17, align 4
  %19 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %16, i32 noundef %18)
  %20 = call i32 @MPI_Finalize()
  ret i32 0
}

declare i32 @MPI_Init(ptr noundef, ptr noundef) #1

declare i32 @printf(ptr noundef, ...) #1

declare i32 @MPI_Finalize() #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind willreturn }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.7"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
