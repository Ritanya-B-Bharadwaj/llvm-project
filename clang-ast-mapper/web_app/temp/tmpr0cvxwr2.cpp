// Complex C++ example with classes and methods
class Calculator {
private:
    int value;
    
public:
    Calculator(int initial) : value(initial) {}
    
    int add(int a, int b) {
        return a + b;
    }
    
    int multiply(int a, int b) {
        return a * b;
    }
    
    void setValue(int newValue) {
        value = newValue;
    }
    
    int getValue() const {
        return value;
    }
};

int main() {
    Calculator calc(10);
    
    int x = 5;
    int y = 3;
    
    if (x > y) {
        int result = calc.add(x, y);
        calc.setValue(result);
    } else {
        int result = calc.multiply(x, y);
        calc.setValue(result);
    }
    
    for (int i = 0; i < 3; i++) {
        int current = calc.getValue();
        calc.setValue(current + 1);
    }
    
    return calc.getValue();
}
