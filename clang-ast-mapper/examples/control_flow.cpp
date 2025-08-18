// Control flow examples
int main() {
    int x = 10;
    int y = 20;
    int result = 0;
    
    // If statement
    if (x < y) {
        result = x + y;
    } else if (x > y) {
        result = x - y;
    } else {
        result = x * y;
    }
    
    // Switch statement
    switch (result) {
        case 10:
            result += 5;
            break;
        case 20:
            result += 10;
            break;
        default:
            result += 1;
            break;
    }
    
    // While loop
    int counter = 0;
    while (counter < 5) {
        result += counter;
        counter++;
    }
    
    // For loop
    for (int i = 0; i < 3; i++) {
        result *= 2;
    }
    
    // Do-while loop
    int j = 0;
    do {
        result += j;
        j++;
    } while (j < 2);
    
    return result;
}
