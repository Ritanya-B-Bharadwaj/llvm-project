// Template examples
template<typename T>
class Container {
private:
    T data;
    
public:
    Container(const T& value) : data(value) {}
    
    T getValue() const {
        return data;
    }
    
    void setValue(const T& value) {
        data = value;
    }
};

template<typename T>
T max(const T& a, const T& b) {
    return (a > b) ? a : b;
}

int main() {
    Container<int> intContainer(42);
    Container<double> doubleContainer(3.14);
    
    int maxInt = max(10, 20);
    double maxDouble = max(1.5, 2.5);
    
    intContainer.setValue(maxInt);
    doubleContainer.setValue(maxDouble);
    
    return intContainer.getValue();
}
