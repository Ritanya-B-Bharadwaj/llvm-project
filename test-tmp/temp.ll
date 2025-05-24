; ModuleID = 'test.cpp'
source_filename = "test.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef i32 @_Z3addii(i32 noundef %0, i32 noundef %1) #0 !dbg !10 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
    #dbg_declare(ptr %3, !15, !DIExpression(), !16)
  store i32 %1, ptr %4, align 4
    #dbg_declare(ptr %4, !17, !DIExpression(), !18)
  %5 = load i32, ptr %3, align 4, !dbg !19
  %6 = load i32, ptr %4, align 4, !dbg !20
  %7 = add nsw i32 %5, %6, !dbg !21
  ret i32 %7, !dbg !22
}

; Function Attrs: mustprogress noinline norecurse nounwind optnone sspstrong uwtable
define dso_local noundef i32 @main() #1 !dbg !23 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
    #dbg_declare(ptr %2, !26, !DIExpression(), !27)
  %3 = call noundef i32 @_Z3addii(i32 noundef 2, i32 noundef 3), !dbg !28
  store i32 %3, ptr %2, align 4, !dbg !27
  %4 = load i32, ptr %2, align 4, !dbg !29
  ret i32 %4, !dbg !30
}

attributes #0 = { mustprogress noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress noinline norecurse nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "clang version 19.1.7", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "test.cpp", directory: "/run/media/echo/hdd/RVCE/CD-Lab/Part-B/llvm-project/test-tmp", checksumkind: CSK_MD5, checksum: "be9828c5229f43a07ce59cd15f870413")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"clang version 19.1.7"}
!10 = distinct !DISubprogram(name: "add", linkageName: "_Z3addii", scope: !1, file: !1, line: 2, type: !11, scopeLine: 2, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !13, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", arg: 1, scope: !10, file: !1, line: 2, type: !13)
!16 = !DILocation(line: 2, column: 13, scope: !10)
!17 = !DILocalVariable(name: "b", arg: 2, scope: !10, file: !1, line: 2, type: !13)
!18 = !DILocation(line: 2, column: 20, scope: !10)
!19 = !DILocation(line: 3, column: 12, scope: !10)
!20 = !DILocation(line: 3, column: 16, scope: !10)
!21 = !DILocation(line: 3, column: 14, scope: !10)
!22 = !DILocation(line: 3, column: 5, scope: !10)
!23 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 6, type: !24, scopeLine: 6, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!24 = !DISubroutineType(types: !25)
!25 = !{!13}
!26 = !DILocalVariable(name: "x", scope: !23, file: !1, line: 7, type: !13)
!27 = !DILocation(line: 7, column: 9, scope: !23)
!28 = !DILocation(line: 7, column: 13, scope: !23)
!29 = !DILocation(line: 8, column: 12, scope: !23)
!30 = !DILocation(line: 8, column: 5, scope: !23)
