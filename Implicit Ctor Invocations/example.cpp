// example.cpp

class MyClass {
public:
    MyClass() {}                     // Default constructor
    MyClass(int x) {}                // Parameterized constructor
    MyClass(const MyClass& other) {} // Copy constructor
};

int main() {
    MyClass obj1;           // Implicit default constructor
    MyClass obj2(42);       // Implicit parameterized constructor
    MyClass obj3 = obj1;    // Implicit copy constructor
    return 0;
}

