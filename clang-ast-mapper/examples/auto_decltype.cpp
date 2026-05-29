// Modern C++ features focusing on auto and decltype
// This is a self-contained example

// Type alias
using Integer = int;

// Function with auto return type
auto getValue() {
    return 42;
}

// Function with trailing return type
auto getDouble(int x) -> double {
    return x * 2.0;
}

// Function with decltype
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

// Class with auto member function
class Calculator {
private:
    int value;
    
public:
    Calculator(int initial) : value(initial) {}
    
    // Method with auto return type
    auto getValue() const {
        return value;
    }
    
    // Method with decltype in return type
    template <typename T>
    auto multiply(T factor) -> decltype(value * factor) {
        return value * factor;
    }
};

int main() {
    // Auto variable declarations
    auto x = getValue();
    auto y = 3.14;
    
    // decltype usage
    decltype(x) z = x + 10;
    
    // Auto with initializer lists
    auto result = add(x, y);
    
    // Creating objects with auto
    auto calc = Calculator(100);
    
    // Using auto with method returns
    auto calcResult = calc.getValue();
    
    // Using decltype with templates
    auto doubleResult = calc.multiply(2.5);
    
    return 0;
}
