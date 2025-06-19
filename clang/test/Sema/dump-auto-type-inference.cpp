#include <vector>
#include <string>
#include <map>
#include <list>
#include <utility>


// Primitive Types and Variables
auto int_var = 42;

auto double_var = 3.14;

auto char_var = 'a';

const auto const_var = 10;

auto& ref_var = int_var;

const auto& const_ref_var = double_var;

auto complex_expr = 1 + 2.0 * 3;

// STL Containers and Iterators
auto sys_vec = std::vector<float>();

std::vector<int> vec;
auto vec_it = vec.begin();

std::string str = "hello";
auto str_it = str.begin();

std::map<int, std::string> m;
auto map_it = m.begin();

std::list<double> lst;
auto lst_it = lst.begin();

// UserDefined Templates and NTTPs
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

// Function Return Type Deduction
auto function1(int x) 
{ 
    return x * 2; 
};

auto function2(auto x) { 
    return x + int_var; 
};

decltype(auto) decltype_var = int_var;

int main() {
    auto result = template_func('a');
    function1(5);
    function2(10.0);
    return 0;
}
