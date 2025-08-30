#include <iostream>
#define MAYBE_INLINE inline

namespace NS {
class Sample {
public:
  void regularMethod(int x) {
    if (x > 0) {
      std::cout << "Positive\n";
    } else {
      std::cout << "Non-positive\n";
    }
  }

  static int staticMethod() {
    return 42;
  }

  template <typename T>
  T templatedMethod(T val) {
    return val * 2;
  }

  void methodWithLambda() {
    auto lambda = [](int a) {
      return a + 1;
    };
    lambda(5);
  }

  void methodWithNestedFunction() {
    struct Local {
      void operator()() {
        std::cout << "Local struct with operator()\n";
      }
    };
    Local local;
    local();
  }
};
} // namespace NS

int globalFunction() {
  return NS::Sample::staticMethod();
}

MAYBE_INLINE void inlineFunction() {
  std::cout << "I'm inline!\n";
}

static void staticFileScopeFunction() {
  std::cout << "File scope\n";
}

auto trailingReturnTypeFunc(int x) -> int {
  return x * x;
}

void overloaded();
void overloaded(int x) {
  std::cout << "Overloaded: " << x << "\n";
}

#if 1
void conditionalFunction() {
  std::cout << "This function is conditionally included.\n";
}
#endif

void emptyFunction() {}

int main() {
  NS::Sample s;
  s.regularMethod(10);
  s.templatedMethod<int>(3);
  s.methodWithLambda();
  s.methodWithNestedFunction();

  globalFunction();
  inlineFunction();
  staticFileScopeFunction();
  trailingReturnTypeFunc(5);
  overloaded(5);
  conditionalFunction();
  emptyFunction();
  return 0;
}
