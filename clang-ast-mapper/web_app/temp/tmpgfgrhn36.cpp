
#include <string>

// Auto type deduction
auto getValue() {
    return 42;
}

// Function with decltype
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

int main() {
    // Auto variable declarations
    auto x = getValue();
    auto y = 3.14;
    
    // decltype usage
    decltype(x) z = x + 10;
    
    // Auto with function call
    auto result = add(x, y);
    
    return 0;
}