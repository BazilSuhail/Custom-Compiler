# Type Checker Module

## Overview

The type checker validates semantic correctness of programs after parsing. It ensures variables have correct types, functions are called with matching arguments, and operations are performed on compatible types. This phase catches errors that the parser cannot detect.

## What It Does

The type checker performs three main tasks:

**Symbol Table Construction**
- Collects all variable declarations
- Records function signatures with parameter types
- Tracks enum declarations and their values
- Maintains scope hierarchy for nested blocks

**Type Validation**
- Verifies expression types match their usage
- Checks function calls have correct argument count and types
- Ensures return statements match function return types
- Validates operators are applied to compatible types

**Control Flow Validation**
- Confirms loop and conditional statements have boolean conditions
- Ensures break statements only appear inside loops or switch blocks
- Validates return statements appear in appropriate contexts

## Data Structures

**SymbolInfo**
Stores information about each declared identifier:
- Type of the symbol
- Name and source location
- Flags for functions, enums, enum values, and prototypes
- Parameter list for functions
```cpp
struct SymbolInfo {
    TokenType type;
    string name;
    int line;
    int column;
    bool isFunction;
    bool isEnum;
    bool isEnumValue;
    bool isPrototype;
    vector<pair<TokenType, string>> params;
};
```

**ScopeFrame**
Represents one scope level in the program:
- Maps symbol names to their information
- Links to parent scope for lookup
- Contains child scopes for nested blocks
- Tracks scope depth for shadowing detection
```cpp
struct ScopeFrame {
    unordered_map<string, SymbolInfo> symbols;
    vector<unique_ptr<ScopeFrame>> children;
    ScopeFrame* parent;
    int level;
};
```

**TypeCheckError**
Records each type violation found:
- Error category from enumeration
- Line and column numbers
- Optional identifier name
- Human readable error message
```cpp
struct TypeCheckError {
    TypeChkError type;
    string name;
    int line;
    int column;
    string message;
};
```

## Error Categories

```cpp
ErroneousVarDecl // Variable declared with void type or another invalid type
FnCallParamCount // Function called with the wrong number of arguments
FnCallParamType // Function called with an argument of incompatible type
ErroneousReturnType // Return statement provides a value of the wrong type or misses a required value
ExpressionTypeMismatch // Assignment or operation uses incompatible types
ExpectedBooleanExpression // Expression should evaluate to boolean but does not
ErroneousBreak // Break statement outside loop or switch
NonBooleanCondStmt // Condition in if, while, or for is not boolean
AttemptedBoolOpOnNonBools // Logical AND or OR applied to non-boolean operands
AttemptedBitOpOnNonInt // Bitwise operator applied to non-integer types
AttemptedShiftOnNonInt // Shift operator applied to non-integer types
AttemptedAddOpOnNonNumeric // Arithmetic operator applied to non-numeric types
ReturnStmtInVoid // Return statement with value in void function
NonBooleanSwitchExpression // Switch expression does not have integer or char type
InvalidCaseValueType // Case value type doesn't match switch expression type
IncrementDecrementOnNonInt // Increment or decrement operator applied to non-integer type
NotOnNonBool // Logical NOT operator applied to non-boolean type
```

## Type System Rules

**Numeric Types**
- int, float, double are numeric
- Numeric types can be mixed in arithmetic
- Result type promotes to wider type (int → float → double)

**Boolean Type**
- Only bool type is boolean
- Comparison operators return bool
- Logical operators require bool operands

**Integer Type**
- Only int is considered integer for bitwise operations
- Shift operators require integer operands on both sides
- Bitwise operators require integer operands

**Type Compatibility**
- Exact type match is always compatible
- Numeric types are compatible with each other
- No implicit conversion between bool and numeric types
- No implicit conversion between char and numeric types


## Workflow

**First Pass: Symbol Collection**
1. Traverse entire AST
2. Create scope for each block, function, loop
3. Add declarations to appropriate scope
4. Enter and exit scopes matching program structure
5. Build complete symbol table

**Second Pass: Type Checking**
1. Reset to global scope
2. Traverse AST again
3. Look up symbols in current scope chain
4. Validate each expression and statement
5. Record all errors found

**Error Reporting**
1. Collect all errors during checking
2. Print each error with location
3. Exit with failure if any errors exist
4. Print success message if no errors

## Integration

**Input**
Receives two items from previous phases:
- AST produced by parser
- Token list with location information

**Output**
Produces one of two outcomes:
- Exits program with error messages if type errors found
- Returns successfully allowing compilation to continue

**Pipeline Position**
Runs after parsing and before code generation:
```
Source Code → Lexer → Parser → Type Checker → Code Generator
```


## Usage

The type checker is invoked through the main entry point:
```cpp
void performTypeChecking(const vector<ASTPtr>& ast, const vector<Token>& tokens);
```

This function:
- Creates a TypeChecker instance
- Runs both symbol collection and type checking passes
- Prints errors if any are found
- Exits with failure on errors
- Prints success message if no errors

## Extension Points

**Adding New Types**
1. Update type checking predicates (isNumericType, etc)
2. Add compatibility rules in areTypesCompatible
3. Update getResultType for new type combinations

**Adding New Operators**
1. Add operator handling in checkBinaryExpr or checkUnaryExpr
2. Define valid operand types
3. Specify result type

**Adding New Error Categories**
1. Add entry to TypeChkError enumeration
2. Add message text in TypeCheckError constructor
3. Call addError where validation occurs

**Control Flow Analysis**
The module tracks loop and switch depth but does not perform full control flow analysis. Missing return statements are detected only at function end, not through all paths. This can be extended by analyzing all control flow paths.

## Test Cases and Examples

### Valid Code Example

This program demonstrates correct type usage:
```c
include<main>

int calculateSum(int a, int b) {
    int result = a + b;
    return result;
}

float computeAverage(int total, int count) {
    float avg = total / count;
    return avg;
}

main {
    int x = 10;
    int y = 20;
    int sum = calculateSum(x, y);
    
    float average = computeAverage(sum, 2);
    
    bool isPositive = sum > 0;
    
    if (isPositive) {
        print("Sum is positive: ", sum);
    }
    
    return 0;
}
```

Output:
```
=== Type Checking Successful ===
No type errors found.
```

### Type Mismatch Errors

**Assignment Type Mismatch**
```c
int calculateValue(int param) {
    int number = "hello";        // Error: string assigned to int
    bool flag = 42;              // Error: int assigned to bool
    return number;
}
```

Errors produced:
```
[Type Error] Expression type mismatch: 'number' (line: 2, col: 5)
[Type Error] Expression type mismatch: 'flag' (line: 3, col: 5)
```

**Return Type Mismatch**
```c
string getMessage() {
    return 100;                  // Error: int returned instead of string
}

int computeValue() {
    return "text";               // Error: string returned instead of int
}
```

Errors produced:
```
[Type Error] Function return type mismatch (line: 2, col: 5)
[Type Error] Function return type mismatch (line: 6, col: 5)
```

### Function Call Errors

**Parameter Count Mismatch**
```c
int add(int a, int b) {
    return a + b;
}

main {
    int result = add(5, 10, 15);     // Error: 3 arguments given, 2 expected
    int value = add(5);              // Error: 1 argument given, 2 expected
    return 0;
}
```

Errors produced:
```
[Type Error] Function call parameter count mismatch: 'add' (line: 6, col: 18)
[Type Error] Function call parameter count mismatch: 'add' (line: 7, col: 17)
```

**Parameter Type Mismatch**
```c
int multiply(int x, int y) {
    return x * y;
}

main {
    int result = multiply(5, true);      // Error: bool instead of int
    int value = multiply("text", 10);    // Error: string instead of int
    return 0;
}
```

Errors produced:
```
[Type Error] Function call parameter type mismatch: 'multiply' (line: 6, col: 18)
[Type Error] Function call parameter type mismatch: 'multiply' (line: 7, col: 17)
```

### Boolean Operation Errors

**Non-Boolean Logical Operations**
```c
main {
    int x = 5;
    float y = 3.14;
    bool result = x && y;            // Error: int && float
    bool value = x || true;          // Error: int || bool
    return 0;
}
```

Errors produced:
```
[Type Error] Attempted boolean operation on non-boolean types (line: 4, col: 19)
[Type Error] Expression type mismatch: 'result' (line: 4, col: 5)
[Type Error] Attempted boolean operation on non-boolean types (line: 5, col: 18)
[Type Error] Expression type mismatch: 'value' (line: 5, col: 5)
```

**Non-Boolean Conditions**
```c
main {
    int x = 10;
    
    if (x) {                         // Error: int condition instead of bool
        print("This is wrong");
    }
    
    while (x) {                      // Error: int condition instead of bool
        x = x - 1;
    }
    
    return 0;
}
```

Errors produced:
```
[Type Error] Non-boolean condition in statement (line: 4, col: 5)
[Type Error] Non-boolean condition in statement (line: 8, col: 5)
```

### Bitwise Operation Errors

**Bitwise Operations on Non-Integers**
```c
main {
    char ch = 'A';
    float num = 3.14;
    
    int result1 = ch & 5;            // Error: char & int
    int result2 = num | 10;          // Error: float | int
    int result3 = ch ^ num;          // Error: char ^ float
    
    return 0;
}
```

Errors produced:
```
[Type Error] Attempted bitwise operation on non-integer type (line: 5, col: 19)
[Type Error] Expression type mismatch: 'result1' (line: 5, col: 5)
[Type Error] Attempted bitwise operation on non-integer type (line: 6, col: 19)
[Type Error] Expression type mismatch: 'result2' (line: 6, col: 5)
[Type Error] Attempted bitwise operation on non-integer type (line: 7, col: 19)
[Type Error] Expression type mismatch: 'result3' (line: 7, col: 5)
```

**Shift Operations on Non-Integers**
```c
main {
    int x = 8;
    float y = 2.0;
    char ch = 'B';
    
    int shifted1 = x << y;           // Error: shift by float
    int shifted2 = ch >> 2;          // Error: shift char value
    
    return 0;
}
```

Errors produced:
```
[Type Error] Attempted shift operation on non-integer type (line: 6, col: 20)
[Type Error] Expression type mismatch: 'shifted1' (line: 6, col: 5)
[Type Error] Attempted shift operation on non-integer type (line: 7, col: 20)
[Type Error] Expression type mismatch: 'shifted2' (line: 7, col: 5)
```

### Arithmetic Operation Errors

**Arithmetic on Non-Numeric Types**
```c
main {
    string text = "hello";
    bool flag = true;
    
    int result1 = text + 5;          // Error: string + int
    int result2 = flag - 10;         // Error: bool - int
    int result3 = text * 2;          // Error: string * int
    
    return 0;
}
```

Errors produced:
```
[Type Error] Attempted addition/subtraction on non-numeric type (line: 5, col: 19)
[Type Error] Expression type mismatch: 'result1' (line: 5, col: 5)
[Type Error] Attempted addition/subtraction on non-numeric type (line: 6, col: 19)
[Type Error] Expression type mismatch: 'result2' (line: 6, col: 5)
[Type Error] Attempted addition/subtraction on non-numeric type (line: 7, col: 19)
[Type Error] Expression type mismatch: 'result3' (line: 7, col: 5)
```

### Control Flow Errors

**Break Outside Loop or Switch**
```c
main {
    int x = 5;
    
    if (x > 0) {
        break;                       // Error: break outside loop/switch
    }
    
    break;                           // Error: break outside loop/switch
    
    return 0;
}
```

Errors produced:
```
[Type Error] Break statement outside loop or switch (line: 5, col: 9)
[Type Error] Break statement outside loop or switch (line: 8, col: 5)
```

**Valid Break Usage**
```c
main {
    int i = 0;
    
    while (i < 10) {
        if (i == 5) {
            break;                   // Valid: inside loop
        }
        i = i + 1;
    }
    
    return 0;
}
```

Output:
```
=== Type Checking Successful ===
No type errors found.
```

### Unary Operator Errors

**Increment/Decrement on Non-Integer**
```c
main {
    float value = 3.14;
    char ch = 'A';
    bool flag = true;
    
    value++;                         // Error: ++ on float
    ch--;                            // Error: -- on char
    flag++;                          // Error: ++ on bool
    
    return 0;
}
```

Errors produced:
```
[Type Error] Increment/decrement applied to non-integer type (line: 6, col: 5)
[Type Error] Increment/decrement applied to non-integer type (line: 7, col: 5)
[Type Error] Increment/decrement applied to non-integer type (line: 8, col: 5)
```

**Logical NOT on Non-Boolean**
```c
main {
    int number = 42;
    string text = "hello";
    
    bool result1 = !number;          // Error: ! on int
    bool result2 = !text;            // Error: ! on string
    
    return 0;
}
```

Errors produced:
```
[Type Error] Logical NOT applied to non-boolean type (line: 5, col: 20)
[Type Error] Expression type mismatch: 'result1' (line: 5, col: 5)
[Type Error] Logical NOT applied to non-boolean type (line: 6, col: 20)
[Type Error] Expression type mismatch: 'result2' (line: 6, col: 5)
```

### Return Statement Errors

**Return Value in Void Function**
```c
void printMessage(string msg) {
    print(msg);
    return 100;                      // Error: returning value in void function
}

void doSomething() {
    int x = 5;
    return x;                        // Error: returning value in void function
}
```

Errors produced:
```
[Type Error] Return statement with value in void function: 'printMessage' (line: 3, col: 5)
[Type Error] Return statement with value in void function: 'doSomething' (line: 8, col: 5)
```

### Switch Statement Errors

**Non-Integer/Char Switch Expression**
```c
main {
    bool flag = true;
    float value = 3.14;
    
    switch (flag) {                  // Error: switch on bool
        case true:
            print("True");
            break;
    }
    
    switch (value) {                 // Error: switch on float
        case 3.14:
            print("Pi");
            break;
    }
    
    return 0;
}
```

Errors produced:
```
[Type Error] Switch expression must be integer or char type (line: 5, col: 5)
[Type Error] Switch expression must be integer or char type (line: 11, col: 5)
```

**Case Value Type Mismatch**
```c
main {
    int number = 5;
    char ch = 'A';
    
    switch (number) {
        case 1:
            print("One");
            break;
        case 'A':                    // Error: char in int switch
            print("Letter A");
            break;
    }
    
    switch (ch) {
        case 'A':
            print("A");
            break;
        case 100:                    // Error: int in char switch
            print("Number");
            break;
    }
    
    return 0;
}
```

Errors produced:
```
[Type Error] Case value type doesn't match switch expression (line: 9, col: 9)
[Type Error] Case value type doesn't match switch expression (line: 18, col: 9)
```

### Comprehensive Test Program

The following program combines multiple error categories:
```c
include<main>

int processData(int value, bool flag) {
    int result = value * 2;
    return result;
}

string formatMessage(string prefix) {
    string message = prefix + " complete";
    return message;
}

main {
    int number = 100;
    float decimal = 3.14;
    bool status = true;
    char letter = 'X';
    
    // Type mismatch errors
    bool incorrect = number;                    // Error: int assigned to bool
    int mixed = letter & decimal;               // Error: bitwise on non-int
    
    // Function call errors
    int output = processData(5, 10, 15);        // Error: wrong parameter count
    int value = processData(true, status);      // Error: wrong parameter type
    
    // Condition errors
    if (number) {                               // Error: int condition
        print("Invalid condition");
    }
    
    // Control flow errors
    break;                                      // Error: break outside loop
    
    // Arithmetic errors
    string text = "result";
    int calculation = text + number;            // Error: string + int
    
    // Unary operator errors
    decimal++;                                  // Error: ++ on float
    bool notInt = !number;                      // Error: ! on int
    
    return 0;
}
```

Complete error output:
```
=== Type Checking Errors ===
[Type Error] Expression type mismatch: 'incorrect' (line: 19, col: 5)
[Type Error] Attempted bitwise operation on non-integer type (line: 20, col: 17)
[Type Error] Expression type mismatch: 'mixed' (line: 20, col: 5)
[Type Error] Function call parameter count mismatch: 'processData' (line: 23, col: 18)
[Type Error] Function call parameter type mismatch: 'processData' (line: 24, col: 17)
[Type Error] Non-boolean condition in statement (line: 27, col: 5)
[Type Error] Break statement outside loop or switch (line: 32, col: 5)
[Type Error] Attempted addition/subtraction on non-numeric type (line: 36, col: 23)
[Type Error] Expression type mismatch: 'calculation' (line: 36, col: 5)
[Type Error] Increment/decrement applied to non-integer type (line: 39, col: 5)
[Type Error] Logical NOT applied to non-boolean type (line: 40, col: 19)
[Type Error] Expression type mismatch: 'notInt' (line: 40, col: 5)
Type checking failed with 12 error(s)
```