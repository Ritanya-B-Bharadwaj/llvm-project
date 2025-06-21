#include <iostream>
#include <cassert>
#include <cstring>

enum Day {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

// Tell C++ that this variable is defined externally by the compiler
extern const char *__nameof_Day[];

int main() {
    std::cout << "Symbolic map for enum Day:" << std::endl;

    // Loop through the enum and print the names from our generated array
    for (int i = Monday; i <= Sunday; ++i) {
        std::cout << "Value " << i << " -> Name: " << __nameof_Day[i] << std::endl;
    }

    // Prove it's correct with an assertion
    assert(strcmp(__nameof_Day[Friday], "Friday") == 0);

    std::cout << "\nAssertion passed." << std::endl;

    return 0;
}