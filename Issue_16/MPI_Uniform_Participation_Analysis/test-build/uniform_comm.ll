; ModuleID = '../tests/uniform_comm.c'
source_filename = "../tests/uniform_comm.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.44.35209"

$sprintf = comdat any

$vsprintf = comdat any

$_snprintf = comdat any

$_vsnprintf = comdat any

$fprintf = comdat any

$printf = comdat any

$_vsprintf_l = comdat any

$_vsnprintf_l = comdat any

$__local_stdio_printf_options = comdat any

$_vfprintf_l = comdat any

$"??_C@_0DE@JICLHAEB@World?5size?5must?5be?5greater?5than?5@" = comdat any

$"??_C@_0CF@CBCGDACK@Process?50?5sent?5data?5?$CFd?5to?5proces@" = comdat any

$"??_C@_0BN@NPOPGPPD@Process?51?5received?5data?3?5?$CFd?6?$AA@" = comdat any

@"??_C@_0DE@JICLHAEB@World?5size?5must?5be?5greater?5than?5@" = linkonce_odr dso_local unnamed_addr constant [52 x i8] c"World size must be greater than 1 for this example\0A\00", comdat, align 1, !dbg !0
@"??_C@_0CF@CBCGDACK@Process?50?5sent?5data?5?$CFd?5to?5proces@" = linkonce_odr dso_local unnamed_addr constant [37 x i8] c"Process 0 sent data %d to process 1\0A\00", comdat, align 1, !dbg !7
@"??_C@_0BN@NPOPGPPD@Process?51?5received?5data?3?5?$CFd?6?$AA@" = linkonce_odr dso_local unnamed_addr constant [29 x i8] c"Process 1 received data: %d\0A\00", comdat, align 1, !dbg !12
@__local_stdio_printf_options._OptionsStorage = internal global i64 0, align 8, !dbg !17

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @sprintf(ptr noundef %0, ptr noundef %1, ...) #0 comdat !dbg !58 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca ptr, align 8
  store ptr %1, ptr %3, align 8
    #dbg_declare(ptr %3, !68, !DIExpression(), !69)
  store ptr %0, ptr %4, align 8
    #dbg_declare(ptr %4, !70, !DIExpression(), !71)
    #dbg_declare(ptr %5, !72, !DIExpression(), !73)
    #dbg_declare(ptr %6, !74, !DIExpression(), !77)
  call void @llvm.va_start.p0(ptr %6), !dbg !78
  %7 = load ptr, ptr %6, align 8, !dbg !79
  %8 = load ptr, ptr %3, align 8, !dbg !79
  %9 = load ptr, ptr %4, align 8, !dbg !79
  %10 = call i32 @_vsprintf_l(ptr noundef %9, ptr noundef %8, ptr noundef null, ptr noundef %7), !dbg !79
  store i32 %10, ptr %5, align 4, !dbg !79
  call void @llvm.va_end.p0(ptr %6), !dbg !80
  %11 = load i32, ptr %5, align 4, !dbg !81
  ret i32 %11, !dbg !81
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @vsprintf(ptr noundef %0, ptr noundef %1, ptr noundef %2) #0 comdat !dbg !82 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
    #dbg_declare(ptr %4, !85, !DIExpression(), !86)
  store ptr %1, ptr %5, align 8
    #dbg_declare(ptr %5, !87, !DIExpression(), !88)
  store ptr %0, ptr %6, align 8
    #dbg_declare(ptr %6, !89, !DIExpression(), !90)
  %7 = load ptr, ptr %4, align 8, !dbg !91
  %8 = load ptr, ptr %5, align 8, !dbg !91
  %9 = load ptr, ptr %6, align 8, !dbg !91
  %10 = call i32 @_vsnprintf_l(ptr noundef %9, i64 noundef -1, ptr noundef %8, ptr noundef null, ptr noundef %7), !dbg !91
  ret i32 %10, !dbg !91
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_snprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ...) #0 comdat !dbg !92 {
  %4 = alloca ptr, align 8
  %5 = alloca i64, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
    #dbg_declare(ptr %4, !96, !DIExpression(), !97)
  store i64 %1, ptr %5, align 8
    #dbg_declare(ptr %5, !98, !DIExpression(), !99)
  store ptr %0, ptr %6, align 8
    #dbg_declare(ptr %6, !100, !DIExpression(), !101)
    #dbg_declare(ptr %7, !102, !DIExpression(), !103)
    #dbg_declare(ptr %8, !104, !DIExpression(), !105)
  call void @llvm.va_start.p0(ptr %8), !dbg !106
  %9 = load ptr, ptr %8, align 8, !dbg !107
  %10 = load ptr, ptr %4, align 8, !dbg !107
  %11 = load i64, ptr %5, align 8, !dbg !107
  %12 = load ptr, ptr %6, align 8, !dbg !107
  %13 = call i32 @_vsnprintf(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef %9), !dbg !107
  store i32 %13, ptr %7, align 4, !dbg !107
  call void @llvm.va_end.p0(ptr %8), !dbg !108
  %14 = load i32, ptr %7, align 4, !dbg !109
  ret i32 %14, !dbg !109
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !110 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i64, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
    #dbg_declare(ptr %5, !113, !DIExpression(), !114)
  store ptr %2, ptr %6, align 8
    #dbg_declare(ptr %6, !115, !DIExpression(), !116)
  store i64 %1, ptr %7, align 8
    #dbg_declare(ptr %7, !117, !DIExpression(), !118)
  store ptr %0, ptr %8, align 8
    #dbg_declare(ptr %8, !119, !DIExpression(), !120)
  %9 = load ptr, ptr %5, align 8, !dbg !121
  %10 = load ptr, ptr %6, align 8, !dbg !121
  %11 = load i64, ptr %7, align 8, !dbg !121
  %12 = load ptr, ptr %8, align 8, !dbg !121
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef null, ptr noundef %9), !dbg !121
  ret i32 %13, !dbg !121
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32 noundef %0, ptr noundef %1) #0 !dbg !122 {
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  store ptr %1, ptr %4, align 8
    #dbg_declare(ptr %4, !126, !DIExpression(), !127)
  store i32 %0, ptr %5, align 4
    #dbg_declare(ptr %5, !128, !DIExpression(), !127)
  %10 = call i32 @MPI_Init(ptr noundef null, ptr noundef null), !dbg !129
    #dbg_declare(ptr %6, !130, !DIExpression(), !131)
  %11 = call i32 @MPI_Comm_rank(i32 noundef 1140850688, ptr noundef %6), !dbg !132
    #dbg_declare(ptr %7, !133, !DIExpression(), !134)
  %12 = call i32 @MPI_Comm_size(i32 noundef 1140850688, ptr noundef %7), !dbg !135
  %13 = load i32, ptr %7, align 4, !dbg !136
  %14 = icmp slt i32 %13, 2, !dbg !136
  br i1 %14, label %15, label %19, !dbg !136

15:                                               ; preds = %2
  %16 = call ptr @__acrt_iob_func(i32 noundef 2), !dbg !137
  %17 = call i32 (ptr, ptr, ...) @fprintf(ptr noundef %16, ptr noundef @"??_C@_0DE@JICLHAEB@World?5size?5must?5be?5greater?5than?5@") #3, !dbg !137
  %18 = call i32 @MPI_Abort(i32 noundef 1140850688, i32 noundef 1), !dbg !140
  br label %19, !dbg !141

19:                                               ; preds = %15, %2
  %20 = load i32, ptr %6, align 4, !dbg !142
  %21 = icmp eq i32 %20, 0, !dbg !142
  br i1 %21, label %22, label %26, !dbg !142

22:                                               ; preds = %19
    #dbg_declare(ptr %8, !143, !DIExpression(), !146)
  store i32 12345, ptr %8, align 4, !dbg !146
  %23 = call i32 @MPI_Send(ptr noundef %8, i32 noundef 1, i32 noundef 1275069445, i32 noundef 1, i32 noundef 0, i32 noundef 1140850688), !dbg !147
  %24 = load i32, ptr %8, align 4, !dbg !148
  %25 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0CF@CBCGDACK@Process?50?5sent?5data?5?$CFd?5to?5proces@", i32 noundef %24), !dbg !148
  br label %34, !dbg !149

26:                                               ; preds = %19
  %27 = load i32, ptr %6, align 4, !dbg !150
  %28 = icmp eq i32 %27, 1, !dbg !150
  br i1 %28, label %29, label %33, !dbg !150

29:                                               ; preds = %26
    #dbg_declare(ptr %9, !151, !DIExpression(), !154)
  %30 = call i32 @MPI_Recv(ptr noundef %9, i32 noundef 1, i32 noundef 1275069445, i32 noundef 0, i32 noundef 0, i32 noundef 1140850688, ptr noundef inttoptr (i64 1 to ptr)), !dbg !155
  %31 = load i32, ptr %9, align 4, !dbg !156
  %32 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BN@NPOPGPPD@Process?51?5received?5data?3?5?$CFd?6?$AA@", i32 noundef %31), !dbg !156
  br label %33, !dbg !157

33:                                               ; preds = %29, %26
  br label %34, !dbg !158

34:                                               ; preds = %33, %22
  %35 = call i32 @MPI_Finalize(), !dbg !159
  ret i32 0, !dbg !160
}

declare dso_local i32 @MPI_Init(ptr noundef, ptr noundef) #1

declare dso_local i32 @MPI_Comm_rank(i32 noundef, ptr noundef) #1

declare dso_local i32 @MPI_Comm_size(i32 noundef, ptr noundef) #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @fprintf(ptr noundef %0, ptr noundef %1, ...) #0 comdat !dbg !161 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca ptr, align 8
  store ptr %1, ptr %3, align 8
    #dbg_declare(ptr %3, !172, !DIExpression(), !173)
  store ptr %0, ptr %4, align 8
    #dbg_declare(ptr %4, !174, !DIExpression(), !175)
    #dbg_declare(ptr %5, !176, !DIExpression(), !177)
    #dbg_declare(ptr %6, !178, !DIExpression(), !179)
  call void @llvm.va_start.p0(ptr %6), !dbg !180
  %7 = load ptr, ptr %6, align 8, !dbg !181
  %8 = load ptr, ptr %3, align 8, !dbg !181
  %9 = load ptr, ptr %4, align 8, !dbg !181
  %10 = call i32 @_vfprintf_l(ptr noundef %9, ptr noundef %8, ptr noundef null, ptr noundef %7), !dbg !181
  store i32 %10, ptr %5, align 4, !dbg !181
  call void @llvm.va_end.p0(ptr %6), !dbg !182
  %11 = load i32, ptr %5, align 4, !dbg !183
  ret i32 %11, !dbg !183
}

declare dso_local ptr @__acrt_iob_func(i32 noundef) #1

declare dso_local i32 @MPI_Abort(i32 noundef, i32 noundef) #1

declare dso_local i32 @MPI_Send(ptr noundef, i32 noundef, i32 noundef, i32 noundef, i32 noundef, i32 noundef) #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @printf(ptr noundef %0, ...) #0 comdat !dbg !184 {
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
    #dbg_declare(ptr %2, !187, !DIExpression(), !188)
    #dbg_declare(ptr %3, !189, !DIExpression(), !190)
    #dbg_declare(ptr %4, !191, !DIExpression(), !192)
  call void @llvm.va_start.p0(ptr %4), !dbg !193
  %5 = load ptr, ptr %4, align 8, !dbg !194
  %6 = load ptr, ptr %2, align 8, !dbg !194
  %7 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !194
  %8 = call i32 @_vfprintf_l(ptr noundef %7, ptr noundef %6, ptr noundef null, ptr noundef %5), !dbg !194
  store i32 %8, ptr %3, align 4, !dbg !194
  call void @llvm.va_end.p0(ptr %4), !dbg !195
  %9 = load i32, ptr %3, align 4, !dbg !196
  ret i32 %9, !dbg !196
}

declare dso_local i32 @MPI_Recv(ptr noundef, i32 noundef, i32 noundef, i32 noundef, i32 noundef, i32 noundef, ptr noundef) #1

declare dso_local i32 @MPI_Finalize() #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start.p0(ptr) #2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !197 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
    #dbg_declare(ptr %5, !213, !DIExpression(), !214)
  store ptr %2, ptr %6, align 8
    #dbg_declare(ptr %6, !215, !DIExpression(), !216)
  store ptr %1, ptr %7, align 8
    #dbg_declare(ptr %7, !217, !DIExpression(), !218)
  store ptr %0, ptr %8, align 8
    #dbg_declare(ptr %8, !219, !DIExpression(), !220)
  %9 = load ptr, ptr %5, align 8, !dbg !221
  %10 = load ptr, ptr %6, align 8, !dbg !221
  %11 = load ptr, ptr %7, align 8, !dbg !221
  %12 = load ptr, ptr %8, align 8, !dbg !221
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef -1, ptr noundef %11, ptr noundef %10, ptr noundef %9), !dbg !221
  ret i32 %13, !dbg !221
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end.p0(ptr) #2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf_l(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) #0 comdat !dbg !222 {
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  %9 = alloca i64, align 8
  %10 = alloca ptr, align 8
  %11 = alloca i32, align 4
  store ptr %4, ptr %6, align 8
    #dbg_declare(ptr %6, !225, !DIExpression(), !226)
  store ptr %3, ptr %7, align 8
    #dbg_declare(ptr %7, !227, !DIExpression(), !228)
  store ptr %2, ptr %8, align 8
    #dbg_declare(ptr %8, !229, !DIExpression(), !230)
  store i64 %1, ptr %9, align 8
    #dbg_declare(ptr %9, !231, !DIExpression(), !232)
  store ptr %0, ptr %10, align 8
    #dbg_declare(ptr %10, !233, !DIExpression(), !234)
    #dbg_declare(ptr %11, !235, !DIExpression(), !237)
  %12 = load ptr, ptr %6, align 8, !dbg !237
  %13 = load ptr, ptr %7, align 8, !dbg !237
  %14 = load ptr, ptr %8, align 8, !dbg !237
  %15 = load i64, ptr %9, align 8, !dbg !237
  %16 = load ptr, ptr %10, align 8, !dbg !237
  %17 = call ptr @__local_stdio_printf_options(), !dbg !237
  %18 = load i64, ptr %17, align 8, !dbg !237
  %19 = or i64 %18, 1, !dbg !237
  %20 = call i32 @__stdio_common_vsprintf(i64 noundef %19, ptr noundef %16, i64 noundef %15, ptr noundef %14, ptr noundef %13, ptr noundef %12), !dbg !237
  store i32 %20, ptr %11, align 4, !dbg !237
  %21 = load i32, ptr %11, align 4, !dbg !238
  %22 = icmp slt i32 %21, 0, !dbg !238
  br i1 %22, label %23, label %24, !dbg !238

23:                                               ; preds = %5
  br label %26, !dbg !238

24:                                               ; preds = %5
  %25 = load i32, ptr %11, align 4, !dbg !238
  br label %26, !dbg !238

26:                                               ; preds = %24, %23
  %27 = phi i32 [ -1, %23 ], [ %25, %24 ], !dbg !238
  ret i32 %27, !dbg !238
}

declare dso_local i32 @__stdio_common_vsprintf(i64 noundef, ptr noundef, i64 noundef, ptr noundef, ptr noundef, ptr noundef) #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local ptr @__local_stdio_printf_options() #0 comdat !dbg !19 {
  ret ptr @__local_stdio_printf_options._OptionsStorage, !dbg !239
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vfprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !240 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
    #dbg_declare(ptr %5, !243, !DIExpression(), !244)
  store ptr %2, ptr %6, align 8
    #dbg_declare(ptr %6, !245, !DIExpression(), !246)
  store ptr %1, ptr %7, align 8
    #dbg_declare(ptr %7, !247, !DIExpression(), !248)
  store ptr %0, ptr %8, align 8
    #dbg_declare(ptr %8, !249, !DIExpression(), !250)
  %9 = load ptr, ptr %5, align 8, !dbg !251
  %10 = load ptr, ptr %6, align 8, !dbg !251
  %11 = load ptr, ptr %7, align 8, !dbg !251
  %12 = load ptr, ptr %8, align 8, !dbg !251
  %13 = call ptr @__local_stdio_printf_options(), !dbg !251
  %14 = load i64, ptr %13, align 8, !dbg !251
  %15 = call i32 @__stdio_common_vfprintf(i64 noundef %14, ptr noundef %12, ptr noundef %11, ptr noundef %10, ptr noundef %9), !dbg !251
  ret i32 %15, !dbg !251
}

declare dso_local i32 @__stdio_common_vfprintf(i64 noundef, ptr noundef, ptr noundef, ptr noundef, ptr noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind willreturn }
attributes #3 = { nounwind }

!llvm.dbg.cu = !{!25}
!llvm.module.flags = !{!51, !52, !53, !54, !55, !56}
!llvm.ident = !{!57}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 21, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "../tests/uniform_comm.c", directory: "D:\\MPI-polished-tool\\build", checksumkind: CSK_MD5, checksum: "5ffae89bbd8d3350e8fd3680e9e4fa75")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 416, elements: !5)
!4 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!5 = !{!6}
!6 = !DISubrange(count: 52)
!7 = !DIGlobalVariableExpression(var: !8, expr: !DIExpression())
!8 = distinct !DIGlobalVariable(scope: null, file: !2, line: 32, type: !9, isLocal: true, isDefinition: true)
!9 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 296, elements: !10)
!10 = !{!11}
!11 = !DISubrange(count: 37)
!12 = !DIGlobalVariableExpression(var: !13, expr: !DIExpression())
!13 = distinct !DIGlobalVariable(scope: null, file: !2, line: 41, type: !14, isLocal: true, isDefinition: true)
!14 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 232, elements: !15)
!15 = !{!16}
!16 = !DISubrange(count: 29)
!17 = !DIGlobalVariableExpression(var: !18, expr: !DIExpression())
!18 = distinct !DIGlobalVariable(name: "_OptionsStorage", scope: !19, file: !20, line: 91, type: !24, isLocal: true, isDefinition: true)
!19 = distinct !DISubprogram(name: "__local_stdio_printf_options", scope: !20, file: !20, line: 89, type: !21, scopeLine: 90, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25)
!20 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.26100.0\\ucrt\\corecrt_stdio_config.h", directory: "", checksumkind: CSK_MD5, checksum: "dacf907bda504afb0b64f53a242bdae6")
!21 = !DISubroutineType(types: !22)
!22 = !{!23}
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!25 = distinct !DICompileUnit(language: DW_LANG_C11, file: !26, producer: "clang version 20.1.6", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !27, globals: !50, splitDebugInlining: false, nameTableKind: None)
!26 = !DIFile(filename: "../tests\\uniform_comm.c", directory: "D:\\MPI-polished-tool\\build", checksumkind: CSK_MD5, checksum: "5ffae89bbd8d3350e8fd3680e9e4fa75")
!27 = !{!28, !31, !32, !43, !48}
!28 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Comm", file: !29, line: 375, baseType: !30)
!29 = !DIFile(filename: "D:/CompilerDesign-MPI-Analysis/MPI/Include\\mpi.h", directory: "", checksumkind: CSK_MD5, checksum: "34d90c21db29f99babcc2cdcf0c58ae3")
!30 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!31 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Datatype", file: !29, line: 281, baseType: !30)
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !33, size: 64)
!33 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Status", file: !29, line: 486, baseType: !34)
!34 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "MPI_Status", file: !29, line: 478, size: 160, elements: !35)
!35 = !{!36, !40, !41, !42}
!36 = !DIDerivedType(tag: DW_TAG_member, name: "internal", scope: !34, file: !29, line: 480, baseType: !37, size: 64)
!37 = !DICompositeType(tag: DW_TAG_array_type, baseType: !30, size: 64, elements: !38)
!38 = !{!39}
!39 = !DISubrange(count: 2)
!40 = !DIDerivedType(tag: DW_TAG_member, name: "MPI_SOURCE", scope: !34, file: !29, line: 482, baseType: !30, size: 32, offset: 64)
!41 = !DIDerivedType(tag: DW_TAG_member, name: "MPI_TAG", scope: !34, file: !29, line: 483, baseType: !30, size: 32, offset: 96)
!42 = !DIDerivedType(tag: DW_TAG_member, name: "MPI_ERROR", scope: !34, file: !29, line: 484, baseType: !30, size: 32, offset: 128)
!43 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Aint", file: !29, line: 259, baseType: !44)
!44 = !DIDerivedType(tag: DW_TAG_typedef, name: "_MSMPI_int64_t", file: !29, line: 61, baseType: !45)
!45 = !DIDerivedType(tag: DW_TAG_typedef, name: "int64_t", file: !46, line: 21, baseType: !47)
!46 = !DIFile(filename: "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Tools\\MSVC\\14.44.35207\\include\\stdint.h", directory: "", checksumkind: CSK_MD5, checksum: "56e2956fe219a08d408dc2fb7a857cfc")
!47 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!48 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !49, line: 188, baseType: !24)
!49 = !DIFile(filename: "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Tools\\MSVC\\14.44.35207\\include\\vcruntime.h", directory: "", checksumkind: CSK_MD5, checksum: "52b0f67d23fb299eb670469dd77ef832")
!50 = !{!0, !7, !12, !17}
!51 = !{i32 2, !"CodeView", i32 1}
!52 = !{i32 2, !"Debug Info Version", i32 3}
!53 = !{i32 1, !"wchar_size", i32 2}
!54 = !{i32 8, !"PIC Level", i32 2}
!55 = !{i32 7, !"uwtable", i32 2}
!56 = !{i32 1, !"MaxTLSAlign", i32 65536}
!57 = !{!"clang version 20.1.6"}
!58 = distinct !DISubprogram(name: "sprintf", scope: !59, file: !59, line: 1764, type: !60, scopeLine: 1771, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!59 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.26100.0\\ucrt\\stdio.h", directory: "", checksumkind: CSK_MD5, checksum: "c1a1fbc43e7d45f0ea4ae539ddcffb19")
!60 = !DISubroutineType(types: !61)
!61 = !{!30, !62, !64, null}
!62 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !63)
!63 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!64 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !65)
!65 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !66, size: 64)
!66 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !4)
!67 = !{}
!68 = !DILocalVariable(name: "_Format", arg: 2, scope: !58, file: !59, line: 1766, type: !64)
!69 = !DILocation(line: 1766, scope: !58)
!70 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !58, file: !59, line: 1765, type: !62)
!71 = !DILocation(line: 1765, scope: !58)
!72 = !DILocalVariable(name: "_Result", scope: !58, file: !59, line: 1772, type: !30)
!73 = !DILocation(line: 1772, scope: !58)
!74 = !DILocalVariable(name: "_ArgList", scope: !58, file: !59, line: 1773, type: !75)
!75 = !DIDerivedType(tag: DW_TAG_typedef, name: "va_list", file: !76, line: 72, baseType: !63)
!76 = !DIFile(filename: "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Tools\\MSVC\\14.44.35207\\include\\vadefs.h", directory: "", checksumkind: CSK_MD5, checksum: "a4b8f96637d0704c82f39ecb6bde2ab4")
!77 = !DILocation(line: 1773, scope: !58)
!78 = !DILocation(line: 1774, scope: !58)
!79 = !DILocation(line: 1776, scope: !58)
!80 = !DILocation(line: 1778, scope: !58)
!81 = !DILocation(line: 1779, scope: !58)
!82 = distinct !DISubprogram(name: "vsprintf", scope: !59, file: !59, line: 1465, type: !83, scopeLine: 1473, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!83 = !DISubroutineType(types: !84)
!84 = !{!30, !62, !64, !75}
!85 = !DILocalVariable(name: "_ArgList", arg: 3, scope: !82, file: !59, line: 1468, type: !75)
!86 = !DILocation(line: 1468, scope: !82)
!87 = !DILocalVariable(name: "_Format", arg: 2, scope: !82, file: !59, line: 1467, type: !64)
!88 = !DILocation(line: 1467, scope: !82)
!89 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !82, file: !59, line: 1466, type: !62)
!90 = !DILocation(line: 1466, scope: !82)
!91 = !DILocation(line: 1474, scope: !82)
!92 = distinct !DISubprogram(name: "_snprintf", scope: !59, file: !59, line: 1939, type: !93, scopeLine: 1947, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!93 = !DISubroutineType(types: !94)
!94 = !{!30, !62, !95, !64, null}
!95 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !48)
!96 = !DILocalVariable(name: "_Format", arg: 3, scope: !92, file: !59, line: 1942, type: !64)
!97 = !DILocation(line: 1942, scope: !92)
!98 = !DILocalVariable(name: "_BufferCount", arg: 2, scope: !92, file: !59, line: 1941, type: !95)
!99 = !DILocation(line: 1941, scope: !92)
!100 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !92, file: !59, line: 1940, type: !62)
!101 = !DILocation(line: 1940, scope: !92)
!102 = !DILocalVariable(name: "_Result", scope: !92, file: !59, line: 1948, type: !30)
!103 = !DILocation(line: 1948, scope: !92)
!104 = !DILocalVariable(name: "_ArgList", scope: !92, file: !59, line: 1949, type: !75)
!105 = !DILocation(line: 1949, scope: !92)
!106 = !DILocation(line: 1950, scope: !92)
!107 = !DILocation(line: 1951, scope: !92)
!108 = !DILocation(line: 1952, scope: !92)
!109 = !DILocation(line: 1953, scope: !92)
!110 = distinct !DISubprogram(name: "_vsnprintf", scope: !59, file: !59, line: 1402, type: !111, scopeLine: 1411, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!111 = !DISubroutineType(types: !112)
!112 = !{!30, !62, !95, !64, !75}
!113 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !110, file: !59, line: 1406, type: !75)
!114 = !DILocation(line: 1406, scope: !110)
!115 = !DILocalVariable(name: "_Format", arg: 3, scope: !110, file: !59, line: 1405, type: !64)
!116 = !DILocation(line: 1405, scope: !110)
!117 = !DILocalVariable(name: "_BufferCount", arg: 2, scope: !110, file: !59, line: 1404, type: !95)
!118 = !DILocation(line: 1404, scope: !110)
!119 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !110, file: !59, line: 1403, type: !62)
!120 = !DILocation(line: 1403, scope: !110)
!121 = !DILocation(line: 1412, scope: !110)
!122 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 9, type: !123, scopeLine: 9, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!123 = !DISubroutineType(types: !124)
!124 = !{!30, !30, !125}
!125 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !63, size: 64)
!126 = !DILocalVariable(name: "argv", arg: 2, scope: !122, file: !2, line: 9, type: !125)
!127 = !DILocation(line: 9, scope: !122)
!128 = !DILocalVariable(name: "argc", arg: 1, scope: !122, file: !2, line: 9, type: !30)
!129 = !DILocation(line: 11, scope: !122)
!130 = !DILocalVariable(name: "world_rank", scope: !122, file: !2, line: 13, type: !30)
!131 = !DILocation(line: 13, scope: !122)
!132 = !DILocation(line: 14, scope: !122)
!133 = !DILocalVariable(name: "world_size", scope: !122, file: !2, line: 16, type: !30)
!134 = !DILocation(line: 16, scope: !122)
!135 = !DILocation(line: 17, scope: !122)
!136 = !DILocation(line: 20, scope: !122)
!137 = !DILocation(line: 21, scope: !138)
!138 = distinct !DILexicalBlock(scope: !139, file: !2, line: 20)
!139 = distinct !DILexicalBlock(scope: !122, file: !2, line: 20)
!140 = !DILocation(line: 22, scope: !138)
!141 = !DILocation(line: 23, scope: !138)
!142 = !DILocation(line: 25, scope: !122)
!143 = !DILocalVariable(name: "data_to_send", scope: !144, file: !2, line: 27, type: !30)
!144 = distinct !DILexicalBlock(scope: !145, file: !2, line: 25)
!145 = distinct !DILexicalBlock(scope: !122, file: !2, line: 25)
!146 = !DILocation(line: 27, scope: !144)
!147 = !DILocation(line: 30, scope: !144)
!148 = !DILocation(line: 32, scope: !144)
!149 = !DILocation(line: 34, scope: !144)
!150 = !DILocation(line: 34, scope: !145)
!151 = !DILocalVariable(name: "received_data", scope: !152, file: !2, line: 36, type: !30)
!152 = distinct !DILexicalBlock(scope: !153, file: !2, line: 34)
!153 = distinct !DILexicalBlock(scope: !145, file: !2, line: 34)
!154 = !DILocation(line: 36, scope: !152)
!155 = !DILocation(line: 39, scope: !152)
!156 = !DILocation(line: 41, scope: !152)
!157 = !DILocation(line: 42, scope: !152)
!158 = !DILocation(line: 34, scope: !153)
!159 = !DILocation(line: 45, scope: !122)
!160 = !DILocation(line: 46, scope: !122)
!161 = distinct !DISubprogram(name: "fprintf", scope: !59, file: !59, line: 830, type: !162, scopeLine: 837, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!162 = !DISubroutineType(types: !163)
!163 = !{!30, !164, !64, null}
!164 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !165)
!165 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !166, size: 64)
!166 = !DIDerivedType(tag: DW_TAG_typedef, name: "FILE", file: !167, line: 31, baseType: !168)
!167 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.26100.0\\ucrt\\corecrt_wstdio.h", directory: "", checksumkind: CSK_MD5, checksum: "bf50373b435d0afd0235dd3e05c4a277")
!168 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_iobuf", file: !167, line: 28, size: 64, align: 64, elements: !169)
!169 = !{!170}
!170 = !DIDerivedType(tag: DW_TAG_member, name: "_Placeholder", scope: !168, file: !167, line: 30, baseType: !171, size: 64)
!171 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!172 = !DILocalVariable(name: "_Format", arg: 2, scope: !161, file: !59, line: 832, type: !64)
!173 = !DILocation(line: 832, scope: !161)
!174 = !DILocalVariable(name: "_Stream", arg: 1, scope: !161, file: !59, line: 831, type: !164)
!175 = !DILocation(line: 831, scope: !161)
!176 = !DILocalVariable(name: "_Result", scope: !161, file: !59, line: 838, type: !30)
!177 = !DILocation(line: 838, scope: !161)
!178 = !DILocalVariable(name: "_ArgList", scope: !161, file: !59, line: 839, type: !75)
!179 = !DILocation(line: 839, scope: !161)
!180 = !DILocation(line: 840, scope: !161)
!181 = !DILocation(line: 841, scope: !161)
!182 = !DILocation(line: 842, scope: !161)
!183 = !DILocation(line: 843, scope: !161)
!184 = distinct !DISubprogram(name: "printf", scope: !59, file: !59, line: 950, type: !185, scopeLine: 956, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!185 = !DISubroutineType(types: !186)
!186 = !{!30, !64, null}
!187 = !DILocalVariable(name: "_Format", arg: 1, scope: !184, file: !59, line: 951, type: !64)
!188 = !DILocation(line: 951, scope: !184)
!189 = !DILocalVariable(name: "_Result", scope: !184, file: !59, line: 957, type: !30)
!190 = !DILocation(line: 957, scope: !184)
!191 = !DILocalVariable(name: "_ArgList", scope: !184, file: !59, line: 958, type: !75)
!192 = !DILocation(line: 958, scope: !184)
!193 = !DILocation(line: 959, scope: !184)
!194 = !DILocation(line: 960, scope: !184)
!195 = !DILocation(line: 961, scope: !184)
!196 = !DILocation(line: 962, scope: !184)
!197 = distinct !DISubprogram(name: "_vsprintf_l", scope: !59, file: !59, line: 1449, type: !198, scopeLine: 1458, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!198 = !DISubroutineType(types: !199)
!199 = !{!30, !62, !64, !200, !75}
!200 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !201)
!201 = !DIDerivedType(tag: DW_TAG_typedef, name: "_locale_t", file: !202, line: 623, baseType: !203)
!202 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.26100.0\\ucrt\\corecrt.h", directory: "", checksumkind: CSK_MD5, checksum: "4ce81db8e96f94c79f8dce86dd46b97f")
!203 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !204, size: 64)
!204 = !DIDerivedType(tag: DW_TAG_typedef, name: "__crt_locale_pointers", file: !202, line: 621, baseType: !205)
!205 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_locale_pointers", file: !202, line: 617, size: 128, align: 64, elements: !206)
!206 = !{!207, !210}
!207 = !DIDerivedType(tag: DW_TAG_member, name: "locinfo", scope: !205, file: !202, line: 619, baseType: !208, size: 64)
!208 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !209, size: 64)
!209 = !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_locale_data", file: !202, line: 619, flags: DIFlagFwdDecl)
!210 = !DIDerivedType(tag: DW_TAG_member, name: "mbcinfo", scope: !205, file: !202, line: 620, baseType: !211, size: 64, offset: 64)
!211 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !212, size: 64)
!212 = !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_multibyte_data", file: !202, line: 620, flags: DIFlagFwdDecl)
!213 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !197, file: !59, line: 1453, type: !75)
!214 = !DILocation(line: 1453, scope: !197)
!215 = !DILocalVariable(name: "_Locale", arg: 3, scope: !197, file: !59, line: 1452, type: !200)
!216 = !DILocation(line: 1452, scope: !197)
!217 = !DILocalVariable(name: "_Format", arg: 2, scope: !197, file: !59, line: 1451, type: !64)
!218 = !DILocation(line: 1451, scope: !197)
!219 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !197, file: !59, line: 1450, type: !62)
!220 = !DILocation(line: 1450, scope: !197)
!221 = !DILocation(line: 1459, scope: !197)
!222 = distinct !DISubprogram(name: "_vsnprintf_l", scope: !59, file: !59, line: 1381, type: !223, scopeLine: 1391, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!223 = !DISubroutineType(types: !224)
!224 = !{!30, !62, !95, !64, !200, !75}
!225 = !DILocalVariable(name: "_ArgList", arg: 5, scope: !222, file: !59, line: 1386, type: !75)
!226 = !DILocation(line: 1386, scope: !222)
!227 = !DILocalVariable(name: "_Locale", arg: 4, scope: !222, file: !59, line: 1385, type: !200)
!228 = !DILocation(line: 1385, scope: !222)
!229 = !DILocalVariable(name: "_Format", arg: 3, scope: !222, file: !59, line: 1384, type: !64)
!230 = !DILocation(line: 1384, scope: !222)
!231 = !DILocalVariable(name: "_BufferCount", arg: 2, scope: !222, file: !59, line: 1383, type: !95)
!232 = !DILocation(line: 1383, scope: !222)
!233 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !222, file: !59, line: 1382, type: !62)
!234 = !DILocation(line: 1382, scope: !222)
!235 = !DILocalVariable(name: "_Result", scope: !222, file: !59, line: 1392, type: !236)
!236 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !30)
!237 = !DILocation(line: 1392, scope: !222)
!238 = !DILocation(line: 1396, scope: !222)
!239 = !DILocation(line: 92, scope: !19)
!240 = distinct !DISubprogram(name: "_vfprintf_l", scope: !59, file: !59, line: 635, type: !241, scopeLine: 644, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !25, retainedNodes: !67)
!241 = !DISubroutineType(types: !242)
!242 = !{!30, !164, !64, !200, !75}
!243 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !240, file: !59, line: 639, type: !75)
!244 = !DILocation(line: 639, scope: !240)
!245 = !DILocalVariable(name: "_Locale", arg: 3, scope: !240, file: !59, line: 638, type: !200)
!246 = !DILocation(line: 638, scope: !240)
!247 = !DILocalVariable(name: "_Format", arg: 2, scope: !240, file: !59, line: 637, type: !64)
!248 = !DILocation(line: 637, scope: !240)
!249 = !DILocalVariable(name: "_Stream", arg: 1, scope: !240, file: !59, line: 636, type: !164)
!250 = !DILocation(line: 636, scope: !240)
!251 = !DILocation(line: 645, scope: !240)
