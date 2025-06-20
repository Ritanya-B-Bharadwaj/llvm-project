#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <vector>

auto foo = 13;                 // simple int
auto bar = 3.14;               // double
auto str = "hello";            // const char *
auto s = std::string("world"); // std::string

int main() {
  auto x = "sjkfnd"; // const char *
  auto y = foo + 1;  // int
  auto z = bar * 2;  // double

  std::vector<int> v = {1, 2, 3};
  auto it = v.begin(); // std::vector<int>::iterator

  const auto cit = v.cbegin(); // std::vector<int>::const_iterator

  std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
  for (auto &p : m) {    // std::pair<const std::string, int>&
    auto key = p.first;  // const std::string
    auto val = p.second; // int
  }

  auto lambda = [](int a, int b) { return a + b; }; // lambda type

  auto ptr = std::make_unique<int>(42); // std::unique_ptr<int>
  auto sp =
      std::make_shared<std::string>("shared"); // std::shared_ptr<std::string>

  auto il = {1, 2, 3}; // std::initializer_list<int>

  // auto with function return type
  auto ret = lambda(1, 2); // int

  // auto in structured bindings (C++17)
#if __cplusplus >= 201703L
  std::pair<int, double> pr = {1, 2.5};
  auto [a, b] = pr; // int a, double b
#endif

  // auto with const/volatile
  const auto cval = 123;
  volatile auto vval = 456;

  // auto with reference
  int q = 10;
  auto &ref = q;
  const auto &cref = q;

  // auto with pointer
  auto *pval = &q;

  // auto in range-based for with const ref
  for (const auto &elem : v) {
    // elem is const int&
  }

  return 0;
}

// auto as trailing return type
auto add(int a, int b) -> int { return a + b; }

// auto in template
template <typename T> auto templated(T t) -> T { return t; }