  %this.addr.i45 = alloca ptr, align 8
  %this.addr.i43 = alloca ptr, align 8
  %this.addr.i41 = alloca ptr, align 8
  %this.addr.i = alloca ptr, align 8
  %retval = alloca i32, align 4
  %N = alloca i32, align 4
  %data = alloca %"class.std::vector", align 8
  %ref.tmp = alloca i32, align 4
  %ref.tmp1 = alloca %"class.std::allocator", align 1
  %exn.slot = alloca ptr, align 8
  %ehselector.slot = alloca i32, align 4
  %sum = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %counter = alloca i32, align 4
  %atomic_counter = alloca i32, align 4
  %tmp = alloca i32, align 4
  %.omp.iv = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  store i32 1000, ptr %N, align 4, !dbg !1352
  store i32 1, ptr %ref.tmp, align 4, !dbg !1355
  store ptr %ref.tmp1, ptr %this.addr.i, align 8
  %this1.i = load ptr, ptr %this.addr.i, align 8
  store ptr %this1.i, ptr %this.addr.i45, align 8
  %this1.i46 = load ptr, ptr %this.addr.i45, align 8
  invoke void @_ZNSt6vectorIiSaIiEEC2EmRKiRKS0_(ptr noundef nonnull align 8 dereferenceable(24) %data, i64 noundef 1000, ptr noundef nonnull align 4 dereferenceable(4) %ref.tmp, ptr noundef nonnull align 1 dereferenceable(1) %ref.tmp1)
          to label %invoke.cont unwind label %lpad, !dbg !1354
  store ptr %ref.tmp1, ptr %this.addr.i41, align 8
  %this1.i42 = load ptr, ptr %this.addr.i41, align 8
  call void @_ZNSt15__new_allocatorIiED2Ev(ptr noundef nonnull align 1 dereferenceable(1) %this1.i42) #3, !dbg !1370
  store i32 0, ptr %sum, align 4, !dbg !1373
; From: ../test.cpp:12  →  #pragma omp parallel for reduction(+:sum)
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @5, i32 3, ptr @main.omp_outlined, ptr %sum, ptr %data, ptr %N), !dbg !1374
  %call = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str)
          to label %invoke.cont3 unwind label %lpad2, !dbg !1375
  %0 = load i32, ptr %sum, align 4, !dbg !1376
  %call5 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %call, i32 noundef %0)
          to label %invoke.cont4 unwind label %lpad2, !dbg !1377
  %call7 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8) %call5, ptr noundef @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
          to label %invoke.cont6 unwind label %lpad2, !dbg !1378
  store i32 0, ptr %x, align 4, !dbg !1380
  store i32 0, ptr %y, align 4, !dbg !1382
; From: ../test.cpp:21  →  #pragma omp parallel sections
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @10, i32 2, ptr @main.omp_outlined.1, ptr %x, ptr %y), !dbg !1383
  %call9 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.3)
          to label %invoke.cont8 unwind label %lpad2, !dbg !1384
  %1 = load i32, ptr %x, align 4, !dbg !1385
  %call11 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %call9, i32 noundef %1)
          to label %invoke.cont10 unwind label %lpad2, !dbg !1386
  %call13 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) %call11, ptr noundef @.str.4)
          to label %invoke.cont12 unwind label %lpad2, !dbg !1387
  %2 = load i32, ptr %y, align 4, !dbg !1388
  %call15 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %call13, i32 noundef %2)
          to label %invoke.cont14 unwind label %lpad2, !dbg !1389
  %call17 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8) %call15, ptr noundef @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
          to label %invoke.cont16 unwind label %lpad2, !dbg !1390
  store i32 0, ptr %counter, align 4, !dbg !1392
; From: ../test.cpp:33  →  #pragma omp parallel for
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @17, i32 1, ptr @main.omp_outlined.5, ptr %counter), !dbg !1393
  %call19 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.7)
          to label %invoke.cont18 unwind label %lpad2, !dbg !1394
  %3 = load i32, ptr %counter, align 4, !dbg !1395
  %call21 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %call19, i32 noundef %3)
          to label %invoke.cont20 unwind label %lpad2, !dbg !1396
  %call23 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8) %call21, ptr noundef @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
          to label %invoke.cont22 unwind label %lpad2, !dbg !1397
  store i32 0, ptr %atomic_counter, align 4, !dbg !1399
; From: ../test.cpp:42  →  #pragma omp parallel for
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr @22, i32 1, ptr @main.omp_outlined.8, ptr %atomic_counter), !dbg !1400
  %call25 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.10)
          to label %invoke.cont24 unwind label %lpad2, !dbg !1401
  %4 = load i32, ptr %atomic_counter, align 4, !dbg !1402
  %call27 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %call25, i32 noundef %4)
          to label %invoke.cont26 unwind label %lpad2, !dbg !1403
  %call29 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8) %call27, ptr noundef @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
          to label %invoke.cont28 unwind label %lpad2, !dbg !1404
  store i32 0, ptr %.omp.iv, align 4, !dbg !1408
  br label %omp.inner.for.cond, !dbg !1410
  %5 = load i32, ptr %.omp.iv, align 4, !dbg !1408, !llvm.access.group !1411
  %cmp = icmp slt i32 %5, 1000, !dbg !1412
  br i1 %cmp, label %omp.inner.for.body, label %omp.inner.for.end, !dbg !1410
  %6 = load i32, ptr %.omp.iv, align 4, !dbg !1408, !llvm.access.group !1411
  %mul = mul nsw i32 %6, 1, !dbg !1413
  %add = add nsw i32 0, %mul, !dbg !1413
  store i32 %add, ptr %i, align 4, !dbg !1413, !llvm.access.group !1411
  %7 = load i32, ptr %i, align 4, !dbg !1414, !llvm.access.group !1411
  %conv = sext i32 %7 to i64, !dbg !1414
  %call30 = call noundef nonnull align 4 dereferenceable(4) ptr @_ZNSt6vectorIiSaIiEEixEm(ptr noundef nonnull align 8 dereferenceable(24) %data, i64 noundef %conv) #3, !dbg !1416, !llvm.access.group !1411
  %8 = load i32, ptr %call30, align 4, !dbg !1417, !llvm.access.group !1411
  %mul31 = mul nsw i32 %8, 2, !dbg !1417
  store i32 %mul31, ptr %call30, align 4, !dbg !1417, !llvm.access.group !1411
  br label %omp.body.continue, !dbg !1418
  br label %omp.inner.for.inc, !dbg !1419
  %9 = load i32, ptr %.omp.iv, align 4, !dbg !1408, !llvm.access.group !1411
  %add32 = add nsw i32 %9, 1, !dbg !1412
  store i32 %add32, ptr %.omp.iv, align 4, !dbg !1412, !llvm.access.group !1411
  br label %omp.inner.for.cond, !dbg !1419, !llvm.loop !1420
  %10 = landingpad { ptr, i32 }
          cleanup, !dbg !1424
  %11 = extractvalue { ptr, i32 } %10, 0, !dbg !1424
  store ptr %11, ptr %exn.slot, align 8, !dbg !1424
  %12 = extractvalue { ptr, i32 } %10, 1, !dbg !1424
  store i32 %12, ptr %ehselector.slot, align 4, !dbg !1424
  store ptr %ref.tmp1, ptr %this.addr.i43, align 8
  %this1.i44 = load ptr, ptr %this.addr.i43, align 8
  call void @_ZNSt15__new_allocatorIiED2Ev(ptr noundef nonnull align 1 dereferenceable(1) %this1.i44) #3, !dbg !1427
  br label %eh.resume, !dbg !1354
  %13 = landingpad { ptr, i32 }
          cleanup, !dbg !1424
  %14 = extractvalue { ptr, i32 } %13, 0, !dbg !1424
  store ptr %14, ptr %exn.slot, align 8, !dbg !1424
  %15 = extractvalue { ptr, i32 } %13, 1, !dbg !1424
  store i32 %15, ptr %ehselector.slot, align 4, !dbg !1424
  call void @_ZNSt6vectorIiSaIiEED2Ev(ptr noundef nonnull align 8 dereferenceable(24) %data) #3, !dbg !1424
  br label %eh.resume, !dbg !1424
  store i32 1000, ptr %i, align 4, !dbg !1413
  %call34 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.11)
          to label %invoke.cont33 unwind label %lpad2, !dbg !1428
  %call35 = call noundef nonnull align 4 dereferenceable(4) ptr @_ZNSt6vectorIiSaIiEEixEm(ptr noundef nonnull align 8 dereferenceable(24) %data, i64 noundef 0) #3, !dbg !1429
  %16 = load i32, ptr %call35, align 4, !dbg !1429
  %call37 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %call34, i32 noundef %16)
          to label %invoke.cont36 unwind label %lpad2, !dbg !1430
  %call39 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8) %call37, ptr noundef @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
          to label %invoke.cont38 unwind label %lpad2, !dbg !1431
  store i32 0, ptr %retval, align 4, !dbg !1432
  call void @_ZNSt6vectorIiSaIiEED2Ev(ptr noundef nonnull align 8 dereferenceable(24) %data) #3, !dbg !1424
  %17 = load i32, ptr %retval, align 4, !dbg !1424
  ret i32 %17, !dbg !1424
  %exn = load ptr, ptr %exn.slot, align 8, !dbg !1354
  %sel = load i32, ptr %ehselector.slot, align 4, !dbg !1354
  %lpad.val = insertvalue { ptr, i32 } poison, ptr %exn, 0, !dbg !1354
  %lpad.val40 = insertvalue { ptr, i32 } %lpad.val, i32 %sel, 1, !dbg !1354
  resume { ptr, i32 } %lpad.val40, !dbg !1354
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__value.addr = alloca ptr, align 8
  %__a.addr = alloca ptr, align 8
  %exn.slot = alloca ptr, align 8
  %ehselector.slot = alloca i32, align 4
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__value, ptr %__value.addr, align 8
  store ptr %__a, ptr %__a.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load i64, ptr %__n.addr, align 8, !dbg !1360
  %1 = load ptr, ptr %__a.addr, align 8, !dbg !1361, !nonnull !176
  %call = call noundef i64 @_ZNSt6vectorIiSaIiEE17_S_check_init_lenEmRKS0_(i64 noundef %0, ptr noundef nonnull align 1 dereferenceable(1) %1), !dbg !1362
  %2 = load ptr, ptr %__a.addr, align 8, !dbg !1363, !nonnull !176
  call void @_ZNSt12_Vector_baseIiSaIiEEC2EmRKS0_(ptr noundef nonnull align 8 dereferenceable(24) %this1, i64 noundef %call, ptr noundef nonnull align 1 dereferenceable(1) %2), !dbg !1364
  %3 = load i64, ptr %__n.addr, align 8, !dbg !1365
  %4 = load ptr, ptr %__value.addr, align 8, !dbg !1367, !nonnull !176, !align !1368
  invoke void @_ZNSt6vectorIiSaIiEE18_M_fill_initializeEmRKi(ptr noundef nonnull align 8 dereferenceable(24) %this1, i64 noundef %3, ptr noundef nonnull align 4 dereferenceable(4) %4)
          to label %invoke.cont unwind label %lpad, !dbg !1369
  ret void, !dbg !1370
  %5 = landingpad { ptr, i32 }
          cleanup, !dbg !1371
  %6 = extractvalue { ptr, i32 } %5, 0, !dbg !1371
  store ptr %6, ptr %exn.slot, align 8, !dbg !1371
  %7 = extractvalue { ptr, i32 } %5, 1, !dbg !1371
  store i32 %7, ptr %ehselector.slot, align 4, !dbg !1371
  call void @_ZNSt12_Vector_baseIiSaIiEED2Ev(ptr noundef nonnull align 8 dereferenceable(24) %this1) #3, !dbg !1371
  br label %eh.resume, !dbg !1371
  %exn = load ptr, ptr %exn.slot, align 8, !dbg !1371
  %sel = load i32, ptr %ehselector.slot, align 4, !dbg !1371
  %lpad.val = insertvalue { ptr, i32 } poison, ptr %exn, 0, !dbg !1371
  %lpad.val2 = insertvalue { ptr, i32 } %lpad.val, i32 %sel, 1, !dbg !1371
  resume { ptr, i32 } %lpad.val2, !dbg !1371
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %sum.addr = alloca ptr, align 8
  %data.addr = alloca ptr, align 8
  %N.addr = alloca ptr, align 8
  %.omp.iv = alloca i32, align 4
  %tmp = alloca i32, align 4
  %.omp.lb = alloca i32, align 4
  %.omp.ub = alloca i32, align 4
  %.omp.stride = alloca i32, align 4
  %.omp.is_last = alloca i32, align 4
  %sum1 = alloca i32, align 4
  %i = alloca i32, align 4
  %.omp.reduction.red_list = alloca [1 x ptr], align 8
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %sum, ptr %sum.addr, align 8
  store ptr %data, ptr %data.addr, align 8
  store ptr %N, ptr %N.addr, align 8
  %0 = load ptr, ptr %sum.addr, align 8, !dbg !1364, !nonnull !176, !align !1365
  %1 = load ptr, ptr %data.addr, align 8, !dbg !1364, !nonnull !176, !align !1366
  %2 = load ptr, ptr %N.addr, align 8, !dbg !1364, !nonnull !176, !align !1365
  store i32 0, ptr %.omp.lb, align 4, !dbg !1369
  store i32 999, ptr %.omp.ub, align 4, !dbg !1369
  store i32 1, ptr %.omp.stride, align 4, !dbg !1369
  store i32 0, ptr %.omp.is_last, align 4, !dbg !1369
  store i32 0, ptr %sum1, align 4, !dbg !1374
  %3 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1364
  %4 = load i32, ptr %3, align 4, !dbg !1364
; From: ../test.cpp:12  →  #pragma omp parallel for reduction(+:sum)
  call void @__kmpc_for_static_init_4(ptr @1, i32 %4, i32 34, ptr %.omp.is_last, ptr %.omp.lb, ptr %.omp.ub, ptr %.omp.stride, i32 1, i32 1), !dbg !1376
  %5 = load i32, ptr %.omp.ub, align 4, !dbg !1369
  %cmp = icmp sgt i32 %5, 999, !dbg !1369
  br i1 %cmp, label %cond.true, label %cond.false, !dbg !1369
  br label %cond.end, !dbg !1369
  %6 = load i32, ptr %.omp.ub, align 4, !dbg !1369
  br label %cond.end, !dbg !1369
  %cond = phi i32 [ 999, %cond.true ], [ %6, %cond.false ], !dbg !1369
  store i32 %cond, ptr %.omp.ub, align 4, !dbg !1369
  %7 = load i32, ptr %.omp.lb, align 4, !dbg !1369
  store i32 %7, ptr %.omp.iv, align 4, !dbg !1369
  br label %omp.inner.for.cond, !dbg !1364
  %8 = load i32, ptr %.omp.iv, align 4, !dbg !1369
  %9 = load i32, ptr %.omp.ub, align 4, !dbg !1369
  %cmp2 = icmp sle i32 %8, %9, !dbg !1364
  br i1 %cmp2, label %omp.inner.for.body, label %omp.inner.for.end, !dbg !1364
  %10 = load i32, ptr %.omp.iv, align 4, !dbg !1369
  %mul = mul nsw i32 %10, 1, !dbg !1377
  %add = add nsw i32 0, %mul, !dbg !1377
  store i32 %add, ptr %i, align 4, !dbg !1377
  %11 = load i32, ptr %i, align 4, !dbg !1378
  %conv = sext i32 %11 to i64, !dbg !1378
  %call = call noundef nonnull align 4 dereferenceable(4) ptr @_ZNSt6vectorIiSaIiEEixEm(ptr noundef nonnull align 8 dereferenceable(24) %1, i64 noundef %conv) #3, !dbg !1380
  %12 = load i32, ptr %call, align 4, !dbg !1380
  %13 = load i32, ptr %sum1, align 4, !dbg !1381
  %add3 = add nsw i32 %13, %12, !dbg !1381
  store i32 %add3, ptr %sum1, align 4, !dbg !1381
  br label %omp.body.continue, !dbg !1382
  br label %omp.inner.for.inc, !dbg !1376
  %14 = load i32, ptr %.omp.iv, align 4, !dbg !1369
  %add4 = add nsw i32 %14, 1, !dbg !1364
  store i32 %add4, ptr %.omp.iv, align 4, !dbg !1364
  br label %omp.inner.for.cond, !dbg !1376, !llvm.loop !1383
  br label %omp.loop.exit, !dbg !1376
; From: ../test.cpp:12  →  #pragma omp parallel for reduction(+:sum)
  call void @__kmpc_for_static_fini(ptr @3, i32 %4), !dbg !1384
  %15 = getelementptr inbounds [1 x ptr], ptr %.omp.reduction.red_list, i64 0, i64 0, !dbg !1376
  store ptr %sum1, ptr %15, align 8, !dbg !1376
; From: ../test.cpp:12  →  #pragma omp parallel for reduction(+:sum)
  %16 = call i32 @__kmpc_reduce_nowait(ptr @4, i32 %4, i32 1, i64 8, ptr %.omp.reduction.red_list, ptr @main.omp_outlined_debug__.omp.reduction.reduction_func, ptr @.gomp_critical_user_.reduction.var), !dbg !1376
  switch i32 %16, label %.omp.reduction.default [
    i32 1, label %.omp.reduction.case1
    i32 2, label %.omp.reduction.case2
  ], !dbg !1376
  %17 = load i32, ptr %0, align 4, !dbg !1374
  %18 = load i32, ptr %sum1, align 4, !dbg !1374
  %add5 = add nsw i32 %17, %18, !dbg !1385
  store i32 %add5, ptr %0, align 4, !dbg !1385
; From: ../test.cpp:12  →  #pragma omp parallel for reduction(+:sum)
  call void @__kmpc_end_reduce_nowait(ptr @4, i32 %4, ptr @.gomp_critical_user_.reduction.var), !dbg !1376
  br label %.omp.reduction.default, !dbg !1376
  %19 = load i32, ptr %sum1, align 4, !dbg !1374
  %20 = atomicrmw add ptr %0, i32 %19 monotonic, align 4, !dbg !1376
  br label %.omp.reduction.default, !dbg !1376
  ret void, !dbg !1386
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %sum.addr = alloca ptr, align 8
  %data.addr = alloca ptr, align 8
  %N.addr = alloca ptr, align 8
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %sum, ptr %sum.addr, align 8
  store ptr %data, ptr %data.addr, align 8
  store ptr %N, ptr %N.addr, align 8
  %0 = load ptr, ptr %sum.addr, align 8, !dbg !1361, !nonnull !176, !align !1362
  %1 = load ptr, ptr %data.addr, align 8, !dbg !1361, !nonnull !176, !align !1363
  %2 = load ptr, ptr %N.addr, align 8, !dbg !1361, !nonnull !176, !align !1362
  %3 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1361
  %4 = load ptr, ptr %.bound_tid..addr, align 8, !dbg !1361
  %5 = load ptr, ptr %sum.addr, align 8, !dbg !1361
  %6 = load ptr, ptr %data.addr, align 8, !dbg !1361
  %7 = load ptr, ptr %N.addr, align 8, !dbg !1361
; From: ../test.cpp:12  →  #pragma omp parallel for reduction(+:sum)
  call void @main.omp_outlined_debug__(ptr %3, ptr %4, ptr %5, ptr %6, ptr %7) #3, !dbg !1361
  ret void, !dbg !1361
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1356
  %_M_start = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl, i32 0, i32 0, !dbg !1357
  %0 = load ptr, ptr %_M_start, align 8, !dbg !1357
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1358
  %add.ptr = getelementptr inbounds nuw i32, ptr %0, i64 %1, !dbg !1359
  ret ptr %add.ptr, !dbg !1360
  %.addr = alloca ptr, align 8
  %.addr1 = alloca ptr, align 8
  store ptr %0, ptr %.addr, align 8
  store ptr %1, ptr %.addr1, align 8
  %2 = load ptr, ptr %.addr, align 8, !dbg !1355
  %3 = load ptr, ptr %.addr1, align 8, !dbg !1355
  %4 = getelementptr inbounds [1 x ptr], ptr %3, i64 0, i64 0, !dbg !1355
  %5 = load ptr, ptr %4, align 8, !dbg !1355
  %6 = getelementptr inbounds [1 x ptr], ptr %2, i64 0, i64 0, !dbg !1355
  %7 = load ptr, ptr %6, align 8, !dbg !1355
  %8 = load i32, ptr %7, align 4, !dbg !1356
  %9 = load i32, ptr %5, align 4, !dbg !1356
  %add = add nsw i32 %8, %9, !dbg !1357
  store i32 %add, ptr %7, align 4, !dbg !1357
  ret void, !dbg !1356
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %x.addr = alloca ptr, align 8
  %y.addr = alloca ptr, align 8
  %.omp.sections.lb. = alloca i32, align 4
  %.omp.sections.ub. = alloca i32, align 4
  %.omp.sections.st. = alloca i32, align 4
  %.omp.sections.il. = alloca i32, align 4
  %.omp.sections.iv. = alloca i32, align 4
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %x, ptr %x.addr, align 8
  store ptr %y, ptr %y.addr, align 8
  %0 = load ptr, ptr %x.addr, align 8, !dbg !1362, !nonnull !176, !align !1363
  %1 = load ptr, ptr %y.addr, align 8, !dbg !1362, !nonnull !176, !align !1363
  store i32 0, ptr %.omp.sections.lb., align 4, !dbg !1362
  store i32 1, ptr %.omp.sections.ub., align 4, !dbg !1362
  store i32 1, ptr %.omp.sections.st., align 4, !dbg !1362
  store i32 0, ptr %.omp.sections.il., align 4, !dbg !1362
  %2 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1362
  %3 = load i32, ptr %2, align 4, !dbg !1362
; From: ../test.cpp:21  →  #pragma omp parallel sections
  call void @__kmpc_for_static_init_4(ptr @7, i32 %3, i32 34, ptr %.omp.sections.il., ptr %.omp.sections.lb., ptr %.omp.sections.ub., ptr %.omp.sections.st., i32 1, i32 1), !dbg !1364
  %4 = load i32, ptr %.omp.sections.ub., align 4, !dbg !1362
  %5 = icmp slt i32 %4, 1, !dbg !1362
  %6 = select i1 %5, i32 %4, i32 1, !dbg !1362
  store i32 %6, ptr %.omp.sections.ub., align 4, !dbg !1362
  %7 = load i32, ptr %.omp.sections.lb., align 4, !dbg !1362
  store i32 %7, ptr %.omp.sections.iv., align 4, !dbg !1362
  br label %omp.inner.for.cond, !dbg !1362
  %8 = load i32, ptr %.omp.sections.iv., align 4, !dbg !1364
  %9 = load i32, ptr %.omp.sections.ub., align 4, !dbg !1364
  %cmp = icmp sle i32 %8, %9, !dbg !1364
  br i1 %cmp, label %omp.inner.for.body, label %omp.inner.for.end, !dbg !1362
  %10 = load i32, ptr %.omp.sections.iv., align 4, !dbg !1362
  switch i32 %10, label %.omp.sections.exit [
    i32 0, label %.omp.sections.case
    i32 1, label %.omp.sections.case1
  ], !dbg !1362
  store i32 42, ptr %0, align 4, !dbg !1365
  br label %.omp.sections.exit, !dbg !1367
  store i32 24, ptr %1, align 4, !dbg !1368
  br label %.omp.sections.exit, !dbg !1370
  br label %omp.inner.for.inc, !dbg !1370
  %11 = load i32, ptr %.omp.sections.iv., align 4, !dbg !1364
  %inc = add nsw i32 %11, 1, !dbg !1364
  store i32 %inc, ptr %.omp.sections.iv., align 4, !dbg !1364
  br label %omp.inner.for.cond, !dbg !1370, !llvm.loop !1371
; From: ../test.cpp:21  →  #pragma omp parallel sections
  call void @__kmpc_for_static_fini(ptr @9, i32 %3), !dbg !1372
  ret void, !dbg !1373
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %x.addr = alloca ptr, align 8
  %y.addr = alloca ptr, align 8
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %x, ptr %x.addr, align 8
  store ptr %y, ptr %y.addr, align 8
  %0 = load ptr, ptr %x.addr, align 8, !dbg !1360, !nonnull !176, !align !1361
  %1 = load ptr, ptr %y.addr, align 8, !dbg !1360, !nonnull !176, !align !1361
  %2 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1360
  %3 = load ptr, ptr %.bound_tid..addr, align 8, !dbg !1360
  %4 = load ptr, ptr %x.addr, align 8, !dbg !1360
  %5 = load ptr, ptr %y.addr, align 8, !dbg !1360
; From: ../test.cpp:21  →  #pragma omp parallel sections
  call void @main.omp_outlined_debug__.2(ptr %2, ptr %3, ptr %4, ptr %5), !dbg !1360
  ret void, !dbg !1360
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %counter.addr = alloca ptr, align 8
  %.omp.iv = alloca i32, align 4
  %tmp = alloca i32, align 4
  %.omp.lb = alloca i32, align 4
  %.omp.ub = alloca i32, align 4
  %.omp.stride = alloca i32, align 4
  %.omp.is_last = alloca i32, align 4
  %i = alloca i32, align 4
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %counter, ptr %counter.addr, align 8
  %0 = load ptr, ptr %counter.addr, align 8, !dbg !1360, !nonnull !176, !align !1361
  store i32 0, ptr %.omp.lb, align 4, !dbg !1364
  store i32 999, ptr %.omp.ub, align 4, !dbg !1364
  store i32 1, ptr %.omp.stride, align 4, !dbg !1364
  store i32 0, ptr %.omp.is_last, align 4, !dbg !1364
  %1 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1360
  %2 = load i32, ptr %1, align 4, !dbg !1360
; From: ../test.cpp:33  →  #pragma omp parallel for
  call void @__kmpc_for_static_init_4(ptr @12, i32 %2, i32 34, ptr %.omp.is_last, ptr %.omp.lb, ptr %.omp.ub, ptr %.omp.stride, i32 1, i32 1), !dbg !1369
  %3 = load i32, ptr %.omp.ub, align 4, !dbg !1364
  %cmp = icmp sgt i32 %3, 999, !dbg !1364
  br i1 %cmp, label %cond.true, label %cond.false, !dbg !1364
  br label %cond.end, !dbg !1364
  %4 = load i32, ptr %.omp.ub, align 4, !dbg !1364
  br label %cond.end, !dbg !1364
  %cond = phi i32 [ 999, %cond.true ], [ %4, %cond.false ], !dbg !1364
  store i32 %cond, ptr %.omp.ub, align 4, !dbg !1364
  %5 = load i32, ptr %.omp.lb, align 4, !dbg !1364
  store i32 %5, ptr %.omp.iv, align 4, !dbg !1364
  br label %omp.inner.for.cond, !dbg !1360
  %6 = load i32, ptr %.omp.iv, align 4, !dbg !1364
  %7 = load i32, ptr %.omp.ub, align 4, !dbg !1364
  %cmp1 = icmp sle i32 %6, %7, !dbg !1360
  br i1 %cmp1, label %omp.inner.for.body, label %omp.inner.for.end, !dbg !1360
  %8 = load i32, ptr %.omp.iv, align 4, !dbg !1364
  %mul = mul nsw i32 %8, 1, !dbg !1370
  %add = add nsw i32 0, %mul, !dbg !1370
  store i32 %add, ptr %i, align 4, !dbg !1370
; From: ../test.cpp:35  →  #pragma omp critical
  call void @__kmpc_critical(ptr @14, i32 %2, ptr @.gomp_critical_user_.var), !dbg !1371
  %9 = load i32, ptr %0, align 4, !dbg !1374
  %add2 = add nsw i32 %9, 1, !dbg !1374
  store i32 %add2, ptr %0, align 4, !dbg !1374
; From: ../test.cpp:36
  call void @__kmpc_end_critical(ptr @14, i32 %2, ptr @.gomp_critical_user_.var), !dbg !1375
  br label %omp.body.continue, !dbg !1376
  br label %omp.inner.for.inc, !dbg !1369
  %10 = load i32, ptr %.omp.iv, align 4, !dbg !1364
  %add3 = add nsw i32 %10, 1, !dbg !1360
  store i32 %add3, ptr %.omp.iv, align 4, !dbg !1360
  br label %omp.inner.for.cond, !dbg !1369, !llvm.loop !1377
  br label %omp.loop.exit, !dbg !1369
; From: ../test.cpp:33  →  #pragma omp parallel for
  call void @__kmpc_for_static_fini(ptr @16, i32 %2), !dbg !1378
  ret void, !dbg !1379
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %counter.addr = alloca ptr, align 8
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %counter, ptr %counter.addr, align 8
  %0 = load ptr, ptr %counter.addr, align 8, !dbg !1359, !nonnull !176, !align !1360
  %1 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1359
  %2 = load ptr, ptr %.bound_tid..addr, align 8, !dbg !1359
  %3 = load ptr, ptr %counter.addr, align 8, !dbg !1359
; From: ../test.cpp:33  →  #pragma omp parallel for
  call void @main.omp_outlined_debug__.6(ptr %1, ptr %2, ptr %3) #3, !dbg !1359
  ret void, !dbg !1359
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %atomic_counter.addr = alloca ptr, align 8
  %.omp.iv = alloca i32, align 4
  %tmp = alloca i32, align 4
  %.omp.lb = alloca i32, align 4
  %.omp.ub = alloca i32, align 4
  %.omp.stride = alloca i32, align 4
  %.omp.is_last = alloca i32, align 4
  %i = alloca i32, align 4
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %atomic_counter, ptr %atomic_counter.addr, align 8
  %0 = load ptr, ptr %atomic_counter.addr, align 8, !dbg !1360, !nonnull !176, !align !1361
  store i32 0, ptr %.omp.lb, align 4, !dbg !1364
  store i32 999, ptr %.omp.ub, align 4, !dbg !1364
  store i32 1, ptr %.omp.stride, align 4, !dbg !1364
  store i32 0, ptr %.omp.is_last, align 4, !dbg !1364
  %1 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1360
  %2 = load i32, ptr %1, align 4, !dbg !1360
; From: ../test.cpp:42  →  #pragma omp parallel for
  call void @__kmpc_for_static_init_4(ptr @19, i32 %2, i32 34, ptr %.omp.is_last, ptr %.omp.lb, ptr %.omp.ub, ptr %.omp.stride, i32 1, i32 1), !dbg !1369
  %3 = load i32, ptr %.omp.ub, align 4, !dbg !1364
  %cmp = icmp sgt i32 %3, 999, !dbg !1364
  br i1 %cmp, label %cond.true, label %cond.false, !dbg !1364
  br label %cond.end, !dbg !1364
  %4 = load i32, ptr %.omp.ub, align 4, !dbg !1364
  br label %cond.end, !dbg !1364
  %cond = phi i32 [ 999, %cond.true ], [ %4, %cond.false ], !dbg !1364
  store i32 %cond, ptr %.omp.ub, align 4, !dbg !1364
  %5 = load i32, ptr %.omp.lb, align 4, !dbg !1364
  store i32 %5, ptr %.omp.iv, align 4, !dbg !1364
  br label %omp.inner.for.cond, !dbg !1360
  %6 = load i32, ptr %.omp.iv, align 4, !dbg !1364
  %7 = load i32, ptr %.omp.ub, align 4, !dbg !1364
  %cmp1 = icmp sle i32 %6, %7, !dbg !1360
  br i1 %cmp1, label %omp.inner.for.body, label %omp.inner.for.end, !dbg !1360
  %8 = load i32, ptr %.omp.iv, align 4, !dbg !1364
  %mul = mul nsw i32 %8, 1, !dbg !1370
  %add = add nsw i32 0, %mul, !dbg !1370
  store i32 %add, ptr %i, align 4, !dbg !1370
  %9 = atomicrmw add ptr %0, i32 1 monotonic, align 4, !dbg !1371
  br label %omp.body.continue, !dbg !1374
  br label %omp.inner.for.inc, !dbg !1369
  %10 = load i32, ptr %.omp.iv, align 4, !dbg !1364
  %add2 = add nsw i32 %10, 1, !dbg !1360
  store i32 %add2, ptr %.omp.iv, align 4, !dbg !1360
  br label %omp.inner.for.cond, !dbg !1369, !llvm.loop !1375
  br label %omp.loop.exit, !dbg !1369
; From: ../test.cpp:42  →  #pragma omp parallel for
  call void @__kmpc_for_static_fini(ptr @21, i32 %2), !dbg !1376
  ret void, !dbg !1377
  %.global_tid..addr = alloca ptr, align 8
  %.bound_tid..addr = alloca ptr, align 8
  %atomic_counter.addr = alloca ptr, align 8
  store ptr %.global_tid., ptr %.global_tid..addr, align 8
  store ptr %.bound_tid., ptr %.bound_tid..addr, align 8
  store ptr %atomic_counter, ptr %atomic_counter.addr, align 8
  %0 = load ptr, ptr %atomic_counter.addr, align 8, !dbg !1359, !nonnull !176, !align !1360
  %1 = load ptr, ptr %.global_tid..addr, align 8, !dbg !1359
  %2 = load ptr, ptr %.bound_tid..addr, align 8, !dbg !1359
  %3 = load ptr, ptr %atomic_counter.addr, align 8, !dbg !1359
; From: ../test.cpp:42  →  #pragma omp parallel for
  call void @main.omp_outlined_debug__.9(ptr %1, ptr %2, ptr %3) #3, !dbg !1359
  ret void, !dbg !1359
  %__first.addr.i = alloca ptr, align 8
  %__last.addr.i = alloca ptr, align 8
  %.addr.i = alloca ptr, align 8
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1354
  %_M_start = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl, i32 0, i32 0, !dbg !1356
  %0 = load ptr, ptr %_M_start, align 8, !dbg !1356
  %_M_impl2 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1357
  %_M_finish = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl2, i32 0, i32 1, !dbg !1358
  %1 = load ptr, ptr %_M_finish, align 8, !dbg !1358
  %call = call noundef nonnull align 1 dereferenceable(1) ptr @_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv(ptr noundef nonnull align 8 dereferenceable(24) %this1) #3, !dbg !1359
  store ptr %0, ptr %__first.addr.i, align 8
  store ptr %1, ptr %__last.addr.i, align 8
  store ptr %call, ptr %.addr.i, align 8
  %2 = load ptr, ptr %__first.addr.i, align 8, !dbg !1372
  %3 = load ptr, ptr %__last.addr.i, align 8, !dbg !1373
  invoke void @_ZSt8_DestroyIPiEvT_S1_(ptr noundef %2, ptr noundef %3)
          to label %_ZSt8_DestroyIPiiEvT_S1_RSaIT0_E.exit unwind label %terminate.lpad, !dbg !1374
  br label %invoke.cont, !dbg !1375
  call void @_ZNSt12_Vector_baseIiSaIiEED2Ev(ptr noundef nonnull align 8 dereferenceable(24) %this1) #3, !dbg !1376
  ret void, !dbg !1377
  %4 = landingpad { ptr, i32 }
          catch ptr null, !dbg !1378
  %5 = extractvalue { ptr, i32 } %4, 0, !dbg !1378
  call void @__clang_call_terminate(ptr %5) #13, !dbg !1378
  unreachable, !dbg !1378
  %this.addr.i3 = alloca ptr, align 8
  %.addr.i = alloca ptr, align 8
  %this.addr.i1 = alloca ptr, align 8
  %__a.addr.i = alloca ptr, align 8
  %this.addr.i = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__a.addr = alloca ptr, align 8
  %ref.tmp = alloca %"class.std::allocator", align 1
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__a, ptr %__a.addr, align 8
  %0 = load i64, ptr %__n.addr, align 8, !dbg !1355
  %1 = load ptr, ptr %__a.addr, align 8, !dbg !1357, !nonnull !176
  store ptr %ref.tmp, ptr %this.addr.i1, align 8
  store ptr %1, ptr %__a.addr.i, align 8
  %this1.i2 = load ptr, ptr %this.addr.i1, align 8
  %2 = load ptr, ptr %__a.addr.i, align 8, !dbg !1365, !nonnull !176
  store ptr %this1.i2, ptr %this.addr.i3, align 8
  store ptr %2, ptr %.addr.i, align 8
  %this1.i4 = load ptr, ptr %this.addr.i3, align 8
  %call = call noundef i64 @_ZNSt6vectorIiSaIiEE11_S_max_sizeERKS0_(ptr noundef nonnull align 1 dereferenceable(1) %ref.tmp) #3, !dbg !1373
  %cmp = icmp ugt i64 %0, %call, !dbg !1374
  store ptr %ref.tmp, ptr %this.addr.i, align 8
  %this1.i = load ptr, ptr %this.addr.i, align 8
  call void @_ZNSt15__new_allocatorIiED2Ev(ptr noundef nonnull align 1 dereferenceable(1) %this1.i) #3, !dbg !1379
  br i1 %cmp, label %if.then, label %if.end, !dbg !1355
  call void @_ZSt20__throw_length_errorPKc(ptr noundef @.str.12) #13, !dbg !1381
  unreachable, !dbg !1381
  %3 = load i64, ptr %__n.addr, align 8, !dbg !1382
  ret i64 %3, !dbg !1383
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__a.addr = alloca ptr, align 8
  %exn.slot = alloca ptr, align 8
  %ehselector.slot = alloca i32, align 4
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__a, ptr %__a.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1358
  %0 = load ptr, ptr %__a.addr, align 8, !dbg !1359, !nonnull !176
  call void @_ZNSt12_Vector_baseIiSaIiEE12_Vector_implC2ERKS0_(ptr noundef nonnull align 8 dereferenceable(24) %_M_impl, ptr noundef nonnull align 1 dereferenceable(1) %0) #3, !dbg !1358
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1360
  invoke void @_ZNSt12_Vector_baseIiSaIiEE17_M_create_storageEm(ptr noundef nonnull align 8 dereferenceable(24) %this1, i64 noundef %1)
          to label %invoke.cont unwind label %lpad, !dbg !1362
  ret void, !dbg !1363
  %2 = landingpad { ptr, i32 }
          cleanup, !dbg !1364
  %3 = extractvalue { ptr, i32 } %2, 0, !dbg !1364
  store ptr %3, ptr %exn.slot, align 8, !dbg !1364
  %4 = extractvalue { ptr, i32 } %2, 1, !dbg !1364
  store i32 %4, ptr %ehselector.slot, align 4, !dbg !1364
  call void @_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev(ptr noundef nonnull align 8 dereferenceable(24) %_M_impl) #3, !dbg !1364
  br label %eh.resume, !dbg !1364
  %exn = load ptr, ptr %exn.slot, align 8, !dbg !1364
  %sel = load i32, ptr %ehselector.slot, align 4, !dbg !1364
  %lpad.val = insertvalue { ptr, i32 } poison, ptr %exn, 0, !dbg !1364
  %lpad.val2 = insertvalue { ptr, i32 } %lpad.val, i32 %sel, 1, !dbg !1364
  resume { ptr, i32 } %lpad.val2, !dbg !1364
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__value.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__value, ptr %__value.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1358
  %_M_start = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl, i32 0, i32 0, !dbg !1359
  %0 = load ptr, ptr %_M_start, align 8, !dbg !1359
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1360
  %2 = load ptr, ptr %__value.addr, align 8, !dbg !1361, !nonnull !176, !align !1362
  %call = call noundef nonnull align 1 dereferenceable(1) ptr @_ZNSt12_Vector_baseIiSaIiEE19_M_get_Tp_allocatorEv(ptr noundef nonnull align 8 dereferenceable(24) %this1) #3, !dbg !1363
  %call2 = call noundef ptr @_ZSt24__uninitialized_fill_n_aIPimiiET_S1_T0_RKT1_RSaIT2_E(ptr noundef %0, i64 noundef %1, ptr noundef nonnull align 4 dereferenceable(4) %2, ptr noundef nonnull align 1 dereferenceable(1) %call), !dbg !1364
  %_M_impl3 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1365
  %_M_finish = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl3, i32 0, i32 1, !dbg !1366
  store ptr %call2, ptr %_M_finish, align 8, !dbg !1367
  ret void, !dbg !1368
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1354
  %_M_start = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl, i32 0, i32 0, !dbg !1356
  %0 = load ptr, ptr %_M_start, align 8, !dbg !1356
  %_M_impl2 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1357
  %_M_end_of_storage = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl2, i32 0, i32 2, !dbg !1358
  %1 = load ptr, ptr %_M_end_of_storage, align 8, !dbg !1358
  %_M_impl3 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1359
  %_M_start4 = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl3, i32 0, i32 0, !dbg !1360
  %2 = load ptr, ptr %_M_start4, align 8, !dbg !1360
  %sub.ptr.lhs.cast = ptrtoint ptr %1 to i64, !dbg !1361
  %sub.ptr.rhs.cast = ptrtoint ptr %2 to i64, !dbg !1361
  %sub.ptr.sub = sub i64 %sub.ptr.lhs.cast, %sub.ptr.rhs.cast, !dbg !1361
  %sub.ptr.div = sdiv exact i64 %sub.ptr.sub, 4, !dbg !1361
  invoke void @_ZNSt12_Vector_baseIiSaIiEE13_M_deallocateEPim(ptr noundef nonnull align 8 dereferenceable(24) %this1, ptr noundef %0, i64 noundef %sub.ptr.div)
          to label %invoke.cont unwind label %terminate.lpad, !dbg !1362
  %_M_impl5 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1363
  call void @_ZNSt12_Vector_baseIiSaIiEE12_Vector_implD2Ev(ptr noundef nonnull align 8 dereferenceable(24) %_M_impl5) #3, !dbg !1363
  ret void, !dbg !1364
  %3 = landingpad { ptr, i32 }
          catch ptr null, !dbg !1362
  %4 = extractvalue { ptr, i32 } %3, 0, !dbg !1362
  call void @__clang_call_terminate(ptr %4) #13, !dbg !1362
  unreachable, !dbg !1362
  %this.addr.i3 = alloca ptr, align 8
  %this.addr.i = alloca ptr, align 8
  %__a.addr.i = alloca ptr, align 8
  %__a.addr = alloca ptr, align 8
  %__diffmax = alloca i64, align 8
  %__allocmax = alloca i64, align 8
  store ptr %__a, ptr %__a.addr, align 8
  store i64 2305843009213693951, ptr %__diffmax, align 8, !dbg !1355
  %0 = load ptr, ptr %__a.addr, align 8, !dbg !1358, !nonnull !176
  store ptr %0, ptr %__a.addr.i, align 8
  %1 = load ptr, ptr %__a.addr.i, align 8, !dbg !1363, !nonnull !176
  store ptr %1, ptr %this.addr.i, align 8
  %this1.i = load ptr, ptr %this.addr.i, align 8
  store ptr %this1.i, ptr %this.addr.i3, align 8
  %this1.i4 = load ptr, ptr %this.addr.i3, align 8
  store i64 2305843009213693951, ptr %__allocmax, align 8, !dbg !1357
  %call1 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt3minImERKT_S2_S2_(ptr noundef nonnull align 8 dereferenceable(8) %__diffmax, ptr noundef nonnull align 8 dereferenceable(8) %__allocmax)
          to label %invoke.cont unwind label %terminate.lpad, !dbg !1373
  %2 = load i64, ptr %call1, align 8, !dbg !1373
  ret i64 %2, !dbg !1374
  %3 = landingpad { ptr, i32 }
          catch ptr null, !dbg !1373
  %4 = extractvalue { ptr, i32 } %3, 0, !dbg !1373
  call void @__clang_call_terminate(ptr %4) #13, !dbg !1373
  unreachable, !dbg !1373
  %retval = alloca ptr, align 8
  %__a.addr = alloca ptr, align 8
  %__b.addr = alloca ptr, align 8
  store ptr %__a, ptr %__a.addr, align 8
  store ptr %__b, ptr %__b.addr, align 8
  %0 = load ptr, ptr %__b.addr, align 8, !dbg !1362, !nonnull !176, !align !1364
  %1 = load i64, ptr %0, align 8, !dbg !1362
  %2 = load ptr, ptr %__a.addr, align 8, !dbg !1365, !nonnull !176, !align !1364
  %3 = load i64, ptr %2, align 8, !dbg !1365
  %cmp = icmp ult i64 %1, %3, !dbg !1366
  br i1 %cmp, label %if.then, label %if.end, !dbg !1366
  %4 = load ptr, ptr %__b.addr, align 8, !dbg !1367, !nonnull !176, !align !1364
  store ptr %4, ptr %retval, align 8, !dbg !1368
  br label %return, !dbg !1368
  %5 = load ptr, ptr %__a.addr, align 8, !dbg !1369, !nonnull !176, !align !1364
  store ptr %5, ptr %retval, align 8, !dbg !1370
  br label %return, !dbg !1370
  %6 = load ptr, ptr %retval, align 8, !dbg !1371
  ret ptr %6, !dbg !1371
  %2 = call ptr @__cxa_begin_catch(ptr %0) #3
  call void @_ZSt9terminatev() #13
  unreachable
  %this.addr.i2 = alloca ptr, align 8
  %.addr.i = alloca ptr, align 8
  %this.addr.i = alloca ptr, align 8
  %__a.addr.i = alloca ptr, align 8
  %this.addr = alloca ptr, align 8
  %__a.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  store ptr %__a, ptr %__a.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load ptr, ptr %__a.addr, align 8, !dbg !1356, !nonnull !176
  store ptr %this1, ptr %this.addr.i, align 8
  store ptr %0, ptr %__a.addr.i, align 8
  %this1.i = load ptr, ptr %this.addr.i, align 8
  %1 = load ptr, ptr %__a.addr.i, align 8, !dbg !1364, !nonnull !176
  store ptr %this1.i, ptr %this.addr.i2, align 8
  store ptr %1, ptr %.addr.i, align 8
  %this1.i3 = load ptr, ptr %this.addr.i2, align 8
  call void @_ZNSt12_Vector_baseIiSaIiEE17_Vector_impl_dataC2Ev(ptr noundef nonnull align 8 dereferenceable(24) %this1) #3, !dbg !1372
  ret void, !dbg !1373
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load i64, ptr %__n.addr, align 8, !dbg !1356
  %call = call noundef ptr @_ZNSt12_Vector_baseIiSaIiEE11_M_allocateEm(ptr noundef nonnull align 8 dereferenceable(24) %this1, i64 noundef %0), !dbg !1357
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1358
  %_M_start = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl, i32 0, i32 0, !dbg !1359
  store ptr %call, ptr %_M_start, align 8, !dbg !1360
  %_M_impl2 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1361
  %_M_start3 = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl2, i32 0, i32 0, !dbg !1362
  %1 = load ptr, ptr %_M_start3, align 8, !dbg !1362
  %_M_impl4 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1363
  %_M_finish = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl4, i32 0, i32 1, !dbg !1364
  store ptr %1, ptr %_M_finish, align 8, !dbg !1365
  %_M_impl5 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1366
  %_M_start6 = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl5, i32 0, i32 0, !dbg !1367
  %2 = load ptr, ptr %_M_start6, align 8, !dbg !1367
  %3 = load i64, ptr %__n.addr, align 8, !dbg !1368
  %add.ptr = getelementptr inbounds nuw i32, ptr %2, i64 %3, !dbg !1369
  %_M_impl7 = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1370
  %_M_end_of_storage = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %_M_impl7, i32 0, i32 2, !dbg !1371
  store ptr %add.ptr, ptr %_M_end_of_storage, align 8, !dbg !1372
  ret void, !dbg !1373
  %this.addr.i = alloca ptr, align 8
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  store ptr %this1, ptr %this.addr.i, align 8
  %this1.i = load ptr, ptr %this.addr.i, align 8
  call void @_ZNSt15__new_allocatorIiED2Ev(ptr noundef nonnull align 1 dereferenceable(1) %this1.i) #3, !dbg !1361
  ret void, !dbg !1363
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_start = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %this1, i32 0, i32 0, !dbg !1354
  store ptr null, ptr %_M_start, align 8, !dbg !1354
  %_M_finish = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %this1, i32 0, i32 1, !dbg !1355
  store ptr null, ptr %_M_finish, align 8, !dbg !1355
  %_M_end_of_storage = getelementptr inbounds nuw %"struct.std::_Vector_base<int, std::allocator<int>>::_Vector_impl_data", ptr %this1, i32 0, i32 2, !dbg !1356
  store ptr null, ptr %_M_end_of_storage, align 8, !dbg !1356
  ret void, !dbg !1357
  %__a.addr.i = alloca ptr, align 8
  %__n.addr.i = alloca i64, align 8
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load i64, ptr %__n.addr, align 8, !dbg !1356
  %cmp = icmp ne i64 %0, 0, !dbg !1357
  br i1 %cmp, label %cond.true, label %cond.false, !dbg !1356
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1358
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1359
  store ptr %_M_impl, ptr %__a.addr.i, align 8
  store i64 %1, ptr %__n.addr.i, align 8
  %2 = load ptr, ptr %__a.addr.i, align 8, !dbg !1366, !nonnull !176
  %3 = load i64, ptr %__n.addr.i, align 8, !dbg !1367
  %call.i = call noundef ptr @_ZNSt15__new_allocatorIiE8allocateEmPKv(ptr noundef nonnull align 1 dereferenceable(1) %2, i64 noundef %3, ptr noundef null), !dbg !1368
  br label %cond.end, !dbg !1356
  br label %cond.end, !dbg !1356
  %cond = phi ptr [ %call.i, %cond.true ], [ null, %cond.false ], !dbg !1356
  ret ptr %cond, !dbg !1369
  %this.addr.i = alloca ptr, align 8
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %0, ptr %.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1358
  store ptr %this1, ptr %this.addr.i, align 8
  %this1.i = load ptr, ptr %this.addr.i, align 8
  %cmp = icmp ugt i64 %1, 2305843009213693951, !dbg !1365
  br i1 %cmp, label %if.then, label %if.end4, !dbg !1366
  %2 = load i64, ptr %__n.addr, align 8, !dbg !1367
  %cmp2 = icmp ugt i64 %2, 4611686018427387903, !dbg !1370
  br i1 %cmp2, label %if.then3, label %if.end, !dbg !1370
  call void @_ZSt28__throw_bad_array_new_lengthv() #13, !dbg !1371
  unreachable, !dbg !1371
  call void @_ZSt17__throw_bad_allocv() #13, !dbg !1372
  unreachable, !dbg !1372
  %3 = load i64, ptr %__n.addr, align 8, !dbg !1373
  %mul = mul i64 %3, 4, !dbg !1374
  %call5 = call noalias noundef nonnull ptr @_Znwm(i64 noundef %mul) #14, !dbg !1375
  ret ptr %call5, !dbg !1376
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  ret void, !dbg !1354
  %__first.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__x.addr = alloca ptr, align 8
  %.addr = alloca ptr, align 8
  store ptr %__first, ptr %__first.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__x, ptr %__x.addr, align 8
  store ptr %0, ptr %.addr, align 8
  %1 = load ptr, ptr %__first.addr, align 8, !dbg !1366
  %2 = load i64, ptr %__n.addr, align 8, !dbg !1367
  %3 = load ptr, ptr %__x.addr, align 8, !dbg !1368, !nonnull !176, !align !1369
  %call = call noundef ptr @_ZSt20uninitialized_fill_nIPimiET_S1_T0_RKT1_(ptr noundef %1, i64 noundef %2, ptr noundef nonnull align 4 dereferenceable(4) %3), !dbg !1370
  ret ptr %call, !dbg !1371
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1354
  ret ptr %_M_impl, !dbg !1355
  %__first.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__x.addr = alloca ptr, align 8
  %__can_fill = alloca i8, align 1
  store ptr %__first, ptr %__first.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__x, ptr %__x.addr, align 8
  store i8 1, ptr %__can_fill, align 1, !dbg !1364
  %0 = load ptr, ptr %__first.addr, align 8, !dbg !1365
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1366
  %2 = load ptr, ptr %__x.addr, align 8, !dbg !1367, !nonnull !176, !align !1368
  %call = call noundef ptr @_ZNSt22__uninitialized_fill_nILb1EE15__uninit_fill_nIPimiEET_S3_T0_RKT1_(ptr noundef %0, i64 noundef %1, ptr noundef nonnull align 4 dereferenceable(4) %2), !dbg !1369
  ret ptr %call, !dbg !1370
  %__first.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__x.addr = alloca ptr, align 8
  store ptr %__first, ptr %__first.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__x, ptr %__x.addr, align 8
  %0 = load ptr, ptr %__first.addr, align 8, !dbg !1367
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1368
  %2 = load ptr, ptr %__x.addr, align 8, !dbg !1369, !nonnull !176, !align !1370
  %call = call noundef ptr @_ZSt6fill_nIPimiET_S1_T0_RKT1_(ptr noundef %0, i64 noundef %1, ptr noundef nonnull align 4 dereferenceable(4) %2), !dbg !1371
  ret ptr %call, !dbg !1372
  %.addr.i = alloca ptr, align 8
  %__first.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__value.addr = alloca ptr, align 8
  %agg.tmp = alloca %"struct.std::random_access_iterator_tag", align 1
  %undef.agg.tmp = alloca %"struct.std::random_access_iterator_tag", align 1
  store ptr %__first, ptr %__first.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__value, ptr %__value.addr, align 8
  %0 = load ptr, ptr %__first.addr, align 8, !dbg !1363
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1364
  %call = call noundef i64 @_ZSt17__size_to_integerm(i64 noundef %1), !dbg !1365
  %2 = load ptr, ptr %__value.addr, align 8, !dbg !1366, !nonnull !176, !align !1367
  store ptr %__first.addr, ptr %.addr.i, align 8
  %call1 = call noundef ptr @_ZSt10__fill_n_aIPimiET_S1_T0_RKT1_St26random_access_iterator_tag(ptr noundef %0, i64 noundef %call, ptr noundef nonnull align 4 dereferenceable(4) %2), !dbg !1393
  ret ptr %call1, !dbg !1394
  %retval = alloca ptr, align 8
  %0 = alloca %"struct.std::random_access_iterator_tag", align 1
  %__first.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %__value.addr = alloca ptr, align 8
  store ptr %__first, ptr %__first.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %__value, ptr %__value.addr, align 8
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1376
  %cmp = icmp ule i64 %1, 0, !dbg !1378
  br i1 %cmp, label %if.then, label %if.end, !dbg !1378
  %2 = load ptr, ptr %__first.addr, align 8, !dbg !1379
  store ptr %2, ptr %retval, align 8, !dbg !1380
  br label %return, !dbg !1380
  %3 = load ptr, ptr %__first.addr, align 8, !dbg !1381
  %4 = load ptr, ptr %__first.addr, align 8, !dbg !1382
  %5 = load i64, ptr %__n.addr, align 8, !dbg !1383
  %add.ptr = getelementptr inbounds nuw i32, ptr %4, i64 %5, !dbg !1384
  %6 = load ptr, ptr %__value.addr, align 8, !dbg !1385, !nonnull !176, !align !1386
  call void @_ZSt8__fill_aIPiiEvT_S1_RKT0_(ptr noundef %3, ptr noundef %add.ptr, ptr noundef nonnull align 4 dereferenceable(4) %6), !dbg !1387
  %7 = load ptr, ptr %__first.addr, align 8, !dbg !1388
  %8 = load i64, ptr %__n.addr, align 8, !dbg !1389
  %add.ptr1 = getelementptr inbounds nuw i32, ptr %7, i64 %8, !dbg !1390
  store ptr %add.ptr1, ptr %retval, align 8, !dbg !1391
  br label %return, !dbg !1391
  %9 = load ptr, ptr %retval, align 8, !dbg !1392
  ret ptr %9, !dbg !1392
  %__n.addr = alloca i64, align 8
  store i64 %__n, ptr %__n.addr, align 8
  %0 = load i64, ptr %__n.addr, align 8, !dbg !1356
  ret i64 %0, !dbg !1357
  %__first.addr = alloca ptr, align 8
  %__last.addr = alloca ptr, align 8
  %__value.addr = alloca ptr, align 8
  store ptr %__first, ptr %__first.addr, align 8
  store ptr %__last, ptr %__last.addr, align 8
  store ptr %__value, ptr %__value.addr, align 8
  %0 = load ptr, ptr %__first.addr, align 8, !dbg !1362
  %1 = load ptr, ptr %__last.addr, align 8, !dbg !1363
  %2 = load ptr, ptr %__value.addr, align 8, !dbg !1364, !nonnull !176, !align !1365
  call void @_ZSt9__fill_a1IPiiEN9__gnu_cxx11__enable_ifIXsr11__is_scalarIT0_EE7__valueEvE6__typeET_S6_RKS3_(ptr noundef %0, ptr noundef %1, ptr noundef nonnull align 4 dereferenceable(4) %2), !dbg !1366
  ret void, !dbg !1367
  %__first.addr = alloca ptr, align 8
  %__last.addr = alloca ptr, align 8
  %__value.addr = alloca ptr, align 8
  %__tmp = alloca i32, align 4
  store ptr %__first, ptr %__first.addr, align 8
  store ptr %__last, ptr %__last.addr, align 8
  store ptr %__value, ptr %__value.addr, align 8
  %0 = load ptr, ptr %__value.addr, align 8, !dbg !1370, !nonnull !176, !align !1371
  %1 = load i32, ptr %0, align 4, !dbg !1370
  store i32 %1, ptr %__tmp, align 4, !dbg !1369
  br label %for.cond, !dbg !1372
  %2 = load ptr, ptr %__first.addr, align 8, !dbg !1373
  %3 = load ptr, ptr %__last.addr, align 8, !dbg !1376
  %cmp = icmp ne ptr %2, %3, !dbg !1377
  br i1 %cmp, label %for.body, label %for.end, !dbg !1378
  %4 = load i32, ptr %__tmp, align 4, !dbg !1379
  %5 = load ptr, ptr %__first.addr, align 8, !dbg !1380
  store i32 %4, ptr %5, align 4, !dbg !1381
  br label %for.inc, !dbg !1382
  %6 = load ptr, ptr %__first.addr, align 8, !dbg !1383
  %incdec.ptr = getelementptr inbounds nuw i32, ptr %6, i32 1, !dbg !1383
  store ptr %incdec.ptr, ptr %__first.addr, align 8, !dbg !1383
  br label %for.cond, !dbg !1384, !llvm.loop !1385
  ret void, !dbg !1388
  %__a.addr.i = alloca ptr, align 8
  %__p.addr.i = alloca ptr, align 8
  %__n.addr.i = alloca i64, align 8
  %this.addr = alloca ptr, align 8
  %__p.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  store ptr %__p, ptr %__p.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load ptr, ptr %__p.addr, align 8, !dbg !1358
  %tobool = icmp ne ptr %0, null, !dbg !1358
  br i1 %tobool, label %if.then, label %if.end, !dbg !1358
  %_M_impl = getelementptr inbounds nuw %"struct.std::_Vector_base", ptr %this1, i32 0, i32 0, !dbg !1360
  %1 = load ptr, ptr %__p.addr, align 8, !dbg !1361
  %2 = load i64, ptr %__n.addr, align 8, !dbg !1362
  store ptr %_M_impl, ptr %__a.addr.i, align 8
  store ptr %1, ptr %__p.addr.i, align 8
  store i64 %2, ptr %__n.addr.i, align 8
  %3 = load ptr, ptr %__a.addr.i, align 8, !dbg !1371, !nonnull !176
  %4 = load ptr, ptr %__p.addr.i, align 8, !dbg !1372
  %5 = load i64, ptr %__n.addr.i, align 8, !dbg !1373
  call void @_ZNSt15__new_allocatorIiE10deallocateEPim(ptr noundef nonnull align 1 dereferenceable(1) %3, ptr noundef %4, i64 noundef %5), !dbg !1374
  br label %if.end, !dbg !1375
  ret void, !dbg !1376
  %this.addr = alloca ptr, align 8
  %__p.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  store ptr %__p, ptr %__p.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load ptr, ptr %__p.addr, align 8, !dbg !1358
  %1 = load i64, ptr %__n.addr, align 8, !dbg !1358
  %mul = mul i64 %1, 4, !dbg !1358
  call void @_ZdlPvm(ptr noundef %0, i64 noundef %mul) #13, !dbg !1359
  ret void, !dbg !1360
  %__first.addr = alloca ptr, align 8
  %__last.addr = alloca ptr, align 8
  store ptr %__first, ptr %__first.addr, align 8
  store ptr %__last, ptr %__last.addr, align 8
  %0 = load ptr, ptr %__first.addr, align 8, !dbg !1360
  %1 = load ptr, ptr %__last.addr, align 8, !dbg !1361
  call void @_ZNSt12_Destroy_auxILb1EE9__destroyIPiEEvT_S3_(ptr noundef %0, ptr noundef %1), !dbg !1362
  ret void, !dbg !1363
  %.addr = alloca ptr, align 8
  %.addr1 = alloca ptr, align 8
  store ptr %0, ptr %.addr, align 8
  store ptr %1, ptr %.addr1, align 8
  ret void, !dbg !1364
