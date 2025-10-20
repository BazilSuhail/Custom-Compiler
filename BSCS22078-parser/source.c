// Test file for C-like language parser

/* Multi-line comment */

// Preprocessor directives
#include <stdio.h>
#include "myheader.h"

// Enum declaration
enum Color {
    RED,
    GREEN,
    BLUE
};

// Global variables
int count = 0;
float pi = 3.14;
const int SIZE = 50;
bool flag = true;

// Function declarations
int add(int a, int b) {
    return a + b;
}

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// Main function
int main() {
    int x = 10;
    int y = 20;
    int z;
    
    // Arithmetic operations
    z = x + y * 2;
    x = (x + y) / 2;
    
    // Bitwise operations
    int bits = x & y;
    int shift = x << 2;
    
    // Logical operations
    bool result = (x > 5) && (y < 100);
    
    // If-else statement
    if (x > y) {
        z = x;
    } else {
        z = y;
    }
    
    // While loop
    while (x < 50) {
        x = x + 1;
    }
    
    // Do-while loop
    do {
        y = y - 1;
    } while (y > 0);
    
    // For loop
    for (int i = 0; i < 10; i++) {
        z = z + i;
        if (z > 100) {
            break;
        }
    }
    
    // Switch statement
    switch (x) {
        case 1:
            y = 10;
            break;
        case 2:
            y = 20;
            break;
        default:
            y = 0;
    }
    
    // Function calls
    int sum = add(x, y);
    int fact = factorial(5);
    
    // Increment operators
    x++;
    ++y;
    
    return 0;
}
