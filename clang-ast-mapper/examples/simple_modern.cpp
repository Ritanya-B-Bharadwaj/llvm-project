// Simple modern C++ features example without external headers
// This is a self-contained example

// Using aliases for types
using Size = unsigned int;

// Class template
template <typename T>
class Container {
private:
    T* data;
    Size capacity;
    Size count;
    
public:
    // Constructor
    Container(Size size) : capacity(size), count(0) {
        data = new T[size];
    }
    
    // Destructor
    ~Container() {
        delete[] data;
    }
    
    // Copy constructor (Rule of Three)
    Container(const Container& other) : capacity(other.capacity), count(other.count) {
        data = new T[capacity];
        for (Size i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
    }
    
    // Method with auto return type
    auto size() const { 
        return count; 
    }
    
    // Method to add items
    void add(const T& item) {
        if (count < capacity) {
            data[count++] = item;
        }
    }
    
    // Method to get item
    T get(Size index) const {
        if (index < count) {
            return data[index];
        }
        return T{};
    }
};

// Function template
template <typename T>
T add(T a, T b) {
    return a + b;
}

// Function with decltype
template <typename T, typename U>
auto addMixed(T a, U b) -> decltype(a + b) {
    return a + b;
}

int main() {
    // Using auto
    auto result = add(5, 10);
    
    // Class template instantiation
    Container<int> intContainer(10);
    intContainer.add(42);
    intContainer.add(123);
    
    // Auto and decltype usage
    auto value = intContainer.get(0);
    auto sum = add(value, 100);
    
    // Using decltype
    decltype(sum) anotherValue = intContainer.get(1);
    
    return 0;
}
