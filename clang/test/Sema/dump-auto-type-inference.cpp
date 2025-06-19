#include <vector>
#include <string>
#include <map>
#include <list>
#include <utility>

auto int_var = 42;

auto double_var = 3.14;

auto char_var = 'a';

const auto const_var = 10;

auto& ref_var = int_var;

const auto& const_ref_var = double_var;


std::vector<int> vec;
auto vec_it = vec.begin();

std::string str = "hello";
auto str_it = str.begin();

std::map<int, std::string> m;
auto map_it = m.begin();

std::list<double> lst;
auto lst_it = lst.begin();

auto int_func() { return 42; }

auto string_func() { return std::string("test"); }

template<typename T>
auto template_func(T t) { return t; }

template<auto N>
struct ValueHolder {};

ValueHolder<5> int_holder;

ValueHolder<3.14> double_holder;

ValueHolder<'c'> char_holder;

template<auto* P>
struct PointerHolder {};

int int_val = 10;
PointerHolder<&int_val> ptr_holder;

auto lambda = [](int x) { return x * 2; };

auto capturing_lambda = [](auto x) { return x + int_var; };

decltype(auto) decltype_var = int_var;

auto sys_vec = std::vector<float>();

auto complex_expr = 1 + 2.0 * 3;

template<typename... Ts>
auto sum(Ts... args) { return (args + ...); }

template<typename T>
auto sfinae_func(T t) -> decltype(t.begin()) { return t.begin(); }

int main() {
    return 0;
}