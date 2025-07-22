; ModuleID = '../test_debug.ll'
source_filename = "../test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [9 x i8] c"Sum: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [13 x i8] c"Product: %d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 !dbg !10 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !15, metadata !DIExpression()), !dbg !16
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !17, metadata !DIExpression()), !dbg !18
  call void @llvm.dbg.declare(metadata i32* %5, metadata !19, metadata !DIExpression()), !dbg !20
  %6 = load i32, i32* %3, align 4, !dbg !21
  %7 = load i32, i32* %4, align 4, !dbg !22
  %8 = add nsw i32 %6, %7, !dbg !23
  store i32 %8, i32* %5, align 4, !dbg !20
  %9 = load i32, i32* %5, align 4, !dbg !24
  ret i32 %9, !dbg !25
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @multiply(i32 noundef %0, i32 noundef %1) #2 !dbg !26 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !27, metadata !DIExpression()), !dbg !28
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !29, metadata !DIExpression()), !dbg !30
  call void @llvm.dbg.declare(metadata i32* %5, metadata !31, metadata !DIExpression()), !dbg !32
  store i32 1, i32* %5, align 4, !dbg !32
  call void @llvm.dbg.declare(metadata i32* %6, metadata !33, metadata !DIExpression()), !dbg !35
  store i32 0, i32* %6, align 4, !dbg !35
  br label %7, !dbg !36

7:                                                ; preds = %15, %2
  %8 = load i32, i32* %6, align 4, !dbg !37
  %9 = load i32, i32* %4, align 4, !dbg !39
  %10 = icmp slt i32 %8, %9, !dbg !40
  br i1 %10, label %11, label %18, !dbg !41

11:                                               ; preds = %7
  %12 = load i32, i32* %3, align 4, !dbg !42
  %13 = load i32, i32* %5, align 4, !dbg !44
  %14 = add nsw i32 %13, %12, !dbg !44
  store i32 %14, i32* %5, align 4, !dbg !44
  br label %15, !dbg !45

15:                                               ; preds = %11
  %16 = load i32, i32* %6, align 4, !dbg !46
  %17 = add nsw i32 %16, 1, !dbg !46
  store i32 %17, i32* %6, align 4, !dbg !46
  br label %7, !dbg !47, !llvm.loop !48

18:                                               ; preds = %7
  %19 = load i32, i32* %5, align 4, !dbg !51
  ret i32 %19, !dbg !52
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !53 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !56, metadata !DIExpression()), !dbg !57
  store i32 5, i32* %2, align 4, !dbg !57
  call void @llvm.dbg.declare(metadata i32* %3, metadata !58, metadata !DIExpression()), !dbg !59
  store i32 3, i32* %3, align 4, !dbg !59
  call void @llvm.dbg.declare(metadata i32* %4, metadata !60, metadata !DIExpression()), !dbg !61
  %6 = load i32, i32* %2, align 4, !dbg !62
  %7 = load i32, i32* %3, align 4, !dbg !63
  %8 = call i32 @add(i32 noundef %6, i32 noundef %7), !dbg !64
  store i32 %8, i32* %4, align 4, !dbg !61
  call void @llvm.dbg.declare(metadata i32* %5, metadata !65, metadata !DIExpression()), !dbg !66
  %9 = load i32, i32* %2, align 4, !dbg !67
  %10 = load i32, i32* %3, align 4, !dbg !68
  %11 = call i32 @multiply(i32 noundef %9, i32 noundef %10), !dbg !69
  store i32 %11, i32* %5, align 4, !dbg !66
  %12 = load i32, i32* %4, align 4, !dbg !70
  %13 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([9 x i8], [9 x i8]* @.str, i64 0, i64 0), i32 noundef %12), !dbg !71
  %14 = load i32, i32* %5, align 4, !dbg !72
  %15 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([13 x i8], [13 x i8]* @.str.1, i64 0, i64 0), i32 noundef %14), !dbg !73
  ret i32 0, !dbg !74
}

declare i32 @printf(i8* noundef, ...) #3

attributes #0 = { noinline nounwind optnone uwtable "IsHighlighted" "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Ubuntu clang version 14.0.0-1ubuntu1.1", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "../test.c", directory: "/root/llvm-my-pass/build", checksumkind: CSK_MD5, checksum: "65cea469df44ecebaea83fbfe02e1496")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 1}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!10 = distinct !DISubprogram(name: "add", scope: !1, file: !1, line: 3, type: !11, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !13, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", arg: 1, scope: !10, file: !1, line: 3, type: !13)
!16 = !DILocation(line: 3, column: 13, scope: !10)
!17 = !DILocalVariable(name: "b", arg: 2, scope: !10, file: !1, line: 3, type: !13)
!18 = !DILocation(line: 3, column: 20, scope: !10)
!19 = !DILocalVariable(name: "result", scope: !10, file: !1, line: 4, type: !13)
!20 = !DILocation(line: 4, column: 9, scope: !10)
!21 = !DILocation(line: 4, column: 18, scope: !10)
!22 = !DILocation(line: 4, column: 22, scope: !10)
!23 = !DILocation(line: 4, column: 20, scope: !10)
!24 = !DILocation(line: 5, column: 12, scope: !10)
!25 = !DILocation(line: 5, column: 5, scope: !10)
!26 = distinct !DISubprogram(name: "multiply", scope: !1, file: !1, line: 8, type: !11, scopeLine: 8, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!27 = !DILocalVariable(name: "x", arg: 1, scope: !26, file: !1, line: 8, type: !13)
!28 = !DILocation(line: 8, column: 18, scope: !26)
!29 = !DILocalVariable(name: "y", arg: 2, scope: !26, file: !1, line: 8, type: !13)
!30 = !DILocation(line: 8, column: 25, scope: !26)
!31 = !DILocalVariable(name: "product", scope: !26, file: !1, line: 9, type: !13)
!32 = !DILocation(line: 9, column: 9, scope: !26)
!33 = !DILocalVariable(name: "i", scope: !34, file: !1, line: 10, type: !13)
!34 = distinct !DILexicalBlock(scope: !26, file: !1, line: 10, column: 5)
!35 = !DILocation(line: 10, column: 14, scope: !34)
!36 = !DILocation(line: 10, column: 10, scope: !34)
!37 = !DILocation(line: 10, column: 21, scope: !38)
!38 = distinct !DILexicalBlock(scope: !34, file: !1, line: 10, column: 5)
!39 = !DILocation(line: 10, column: 25, scope: !38)
!40 = !DILocation(line: 10, column: 23, scope: !38)
!41 = !DILocation(line: 10, column: 5, scope: !34)
!42 = !DILocation(line: 11, column: 20, scope: !43)
!43 = distinct !DILexicalBlock(scope: !38, file: !1, line: 10, column: 33)
!44 = !DILocation(line: 11, column: 17, scope: !43)
!45 = !DILocation(line: 12, column: 5, scope: !43)
!46 = !DILocation(line: 10, column: 29, scope: !38)
!47 = !DILocation(line: 10, column: 5, scope: !38)
!48 = distinct !{!48, !41, !49, !50}
!49 = !DILocation(line: 12, column: 5, scope: !34)
!50 = !{!"llvm.loop.mustprogress"}
!51 = !DILocation(line: 13, column: 12, scope: !26)
!52 = !DILocation(line: 13, column: 5, scope: !26)
!53 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 16, type: !54, scopeLine: 16, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!54 = !DISubroutineType(types: !55)
!55 = !{!13}
!56 = !DILocalVariable(name: "num1", scope: !53, file: !1, line: 17, type: !13)
!57 = !DILocation(line: 17, column: 9, scope: !53)
!58 = !DILocalVariable(name: "num2", scope: !53, file: !1, line: 18, type: !13)
!59 = !DILocation(line: 18, column: 9, scope: !53)
!60 = !DILocalVariable(name: "sum", scope: !53, file: !1, line: 20, type: !13)
!61 = !DILocation(line: 20, column: 9, scope: !53)
!62 = !DILocation(line: 20, column: 19, scope: !53)
!63 = !DILocation(line: 20, column: 25, scope: !53)
!64 = !DILocation(line: 20, column: 15, scope: !53)
!65 = !DILocalVariable(name: "product", scope: !53, file: !1, line: 21, type: !13)
!66 = !DILocation(line: 21, column: 9, scope: !53)
!67 = !DILocation(line: 21, column: 28, scope: !53)
!68 = !DILocation(line: 21, column: 34, scope: !53)
!69 = !DILocation(line: 21, column: 19, scope: !53)
!70 = !DILocation(line: 23, column: 25, scope: !53)
!71 = !DILocation(line: 23, column: 5, scope: !53)
!72 = !DILocation(line: 24, column: 29, scope: !53)
!73 = !DILocation(line: 24, column: 5, scope: !53)
!74 = !DILocation(line: 26, column: 5, scope: !53)
