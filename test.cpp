// example.cpp

class MyClass {
public:
    MyClass() {}                     // Default constructor
    MyClass(int x) {}                // Parameterized constructor
    MyClass(const MyClass& other) {} // Copy constructor
};

/**
 * @brief Demonstrates the usage of different constructors for the MyClass class.
 *
 * This main function creates three objects of MyClass:
 * - obj1: Created using the implicit default constructor.
 * - obj2: Created using the implicit parameterized constructor with an integer argument.
 * - obj3: Created using the implicit copy constructor, copying obj1.
 *
 * @return int Returns 0 upon successful execution.
 */
int main() {
    MyClass obj1;           // Implicit default constructor
    MyClass obj2(42);       // Implicit parameterized constructor
    MyClass obj3 = obj1;    // Implicit copy constructor
    return 0;
}