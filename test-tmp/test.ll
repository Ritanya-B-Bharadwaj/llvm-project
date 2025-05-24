; test.cpp:5 -     if (n <= 1)
  %4 = load i32, ptr %3, align 4, !dbg !808
; test.cpp:5 -     if (n <= 1)
  %5 = icmp sle i32 %4, 1, !dbg !810
; test.cpp:5 -     if (n <= 1)
  br i1 %5, label %6, label %8, !dbg !811
; test.cpp:6 -         return n;
  %7 = load i32, ptr %3, align 4, !dbg !812
; test.cpp:6 -         return n;
  store i32 %7, ptr %2, align 4, !dbg !813
; test.cpp:6 -         return n;
  br label %16, !dbg !813
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  %9 = load i32, ptr %3, align 4, !dbg !814
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  %10 = sub nsw i32 %9, 1, !dbg !815
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  %11 = call noundef i32 @_Z9fibonaccii(i32 noundef %10), !dbg !816
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  %12 = load i32, ptr %3, align 4, !dbg !817
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  %13 = sub nsw i32 %12, 2, !dbg !818
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  %14 = call noundef i32 @_Z9fibonaccii(i32 noundef %13), !dbg !819
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  %15 = add nsw i32 %11, %14, !dbg !820
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  store i32 %15, ptr %2, align 4, !dbg !821
; test.cpp:7 -     return fibonacci(n - 1) + fibonacci(n - 2);
  br label %16, !dbg !821
; test.cpp:8 - }
  %17 = load i32, ptr %2, align 4, !dbg !822
; test.cpp:8 - }
  ret i32 %17, !dbg !822
; test.cpp:12 -     std::cout << "Enter a number: ";
  %4 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str), !dbg !808
; test.cpp:13 -     std::cin >> n;
  %5 = call noundef nonnull align 8 dereferenceable(16) ptr @_ZNSirsERi(ptr noundef nonnull align 8 dereferenceable(16) @_ZSt3cin, ptr noundef nonnull align 4 dereferenceable(4) %2), !dbg !809
; test.cpp:14 -     int result = fibonacci(n);
  %6 = load i32, ptr %2, align 4, !dbg !812
; test.cpp:14 -     int result = fibonacci(n);
  %7 = call noundef i32 @_Z9fibonaccii(i32 noundef %6), !dbg !813
; test.cpp:14 -     int result = fibonacci(n);
  store i32 %7, ptr %3, align 4, !dbg !811
; test.cpp:15 -     std::cout << "Fibonacci(" << n << ") = " << result << "\n";
  %8 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.1), !dbg !814
; test.cpp:15 -     std::cout << "Fibonacci(" << n << ") = " << result << "\n";
  %9 = load i32, ptr %2, align 4, !dbg !815
; test.cpp:15 -     std::cout << "Fibonacci(" << n << ") = " << result << "\n";
  %10 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %8, i32 noundef %9), !dbg !816
; test.cpp:15 -     std::cout << "Fibonacci(" << n << ") = " << result << "\n";
  %11 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) %10, ptr noundef @.str.2), !dbg !817
; test.cpp:15 -     std::cout << "Fibonacci(" << n << ") = " << result << "\n";
  %12 = load i32, ptr %3, align 4, !dbg !818
; test.cpp:15 -     std::cout << "Fibonacci(" << n << ") = " << result << "\n";
  %13 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %11, i32 noundef %12), !dbg !819
; test.cpp:15 -     std::cout << "Fibonacci(" << n << ") = " << result << "\n";
  %14 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) %13, ptr noundef @.str.3), !dbg !820
; test.cpp:16 -     return 0;
  ret i32 0, !dbg !821
