# C-Style Language Parser

A comprehensive recursive descent parser for a C-style programming language, built in C++. This parser works in conjunction with a custom lexical analyzer to parse source code and generate an Abstract Syntax Tree (AST).

## Features

### Language Support
- **Data Types**: `int`, `float`, `double`, `char`, `bool`, `void`
- **Type Qualifiers**: `const`
- **Control Flow**: `if/else`, `while`, `do-while`, `for`, `switch/case/default`
- **Flow Control**: `break`, `continue`, `return`
- **Functions**: Function declarations with parameters and return types
- **Variables**: Variable declarations with optional initialization
- **Preprocessor**: `#include`, `#define` (with macro support)
- **Enumerations**: `enum` declarations

### Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%` (including modulo)
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical**: `&&`, `||`, `!`
- **Bitwise**: `&`, `|`, `^`, `~`, `<<`, `>>`
- **Assignment**: `=`
- **Increment/Decrement**: `++`, `--` (both prefix and postfix)
- **Unary**: `+`, `-`, `!`, `~`

### Advanced Features
- **Operator Precedence**: Proper handling of operator precedence and associativity
- **Complex Expressions**: Support for nested and complex expressions
- **Function Calls**: Function invocation with argument lists
- **Nested Control Flow**: Support for deeply nested control structures
- **Type Combinations**: Simple type specifiers like `const int`
- **Macro Definitions**: Both object-like and function-like macros

## Architecture

### Parser Design
The parser uses a **recursive descent** parsing strategy with the following key components:

- **LexerState Integration**: Works directly with the lexer's token stream
- **AST Generation**: Builds a comprehensive Abstract Syntax Tree
- **Error Handling**: Robust error reporting with line and column information
- **Memory Management**: Uses smart pointers for automatic memory management

### AST Node Types
```cpp
// Declarations
- FnDecl (Function Declaration)
- VarDecl (Variable Declaration)
- EnumDecl (Enumeration Declaration)

// Statements
- IfStmt, WhileStmt, DoWhileStmt, ForStmt, SwitchStmt
- ReturnStmt, BreakStmt, ContinueStmt
- AssignStmt, ExprStmt, CaseStmt

// Expressions
- BinaryExpr, UnaryExpr, CallExpr
- LiteralExpr, IdentifierExpr

// Preprocessor
- IncludeDirective, DefineDirective
```

### Parsing Methods
The parser follows a clear hierarchy of parsing methods:

```
declaration() → statement() → expression()
    ↓              ↓             ↓
fnDecl()      ifStmt()     assignment()
varDecl()     whileStmt()  logicalOr()
enumDecl()    forStmt()    logicalAnd()
              ...          bitwiseOr()
                          ...
                          primary()
```

## Usage

### Compilation
```bash
g++ -o parser parser.cpp
```

### Running Tests
The parser includes comprehensive tests covering all language features:
```bash
./parser
```

### Using in Your Project
```cpp
#include "parser.cpp"  // Includes lexer.cpp automatically

// Create lexer state
LexerState state = createLexerState(sourceCode);

// Create parser and parse
Parser parser(state);
vector<unique_ptr<AstNode>> ast = parser.parse();

// Process AST nodes
for (const auto& node : ast) {
    cout << node->toString() << endl;
}
```

## Code Examples

### Variable Declarations
```c
// Basic types
int x = 10;
float pi = 3.14;
bool flag = true;

// With const qualifier
const int constantVar = 42;
const double value = 3.14159;
```

### Function Declarations
```c
// Simple function
int add(int a, int b) {
    return a + b;
}

// Function with const parameters
int compute(const int a, int b, const double c) {
    return a + b + (int)c;
}
```

### Control Flow
```c
// Conditional
if (x > 0) {
    processPositive(x);
} else {
    processNegative(x);
}

// Loops
while (running) {
    update();
}

for (int i = 0; i < count; i++) {
    if (i % 2 == 0) continue;
    processOdd(i);
}

// Switch statement
switch (option) {
    case 1:
        handleOption1();
        break;
    case 2:
        handleOption2();
        break;
    default:
        handleDefault();
        break;
}
```

### Expressions
```c
// Arithmetic with proper precedence
int result = (a + b) * c - d / 2 % 3;

// Logical operations
bool condition = (x > 5) && (y < 10) || !flag;

// Bitwise operations
int bits = a & b | c ^ d;
int shifted = value << 2 >> 1;
int negated = ~mask;

// Complex expressions
bool complex = ((a & 2) << 8) | (b >> 4) && !(c % 2) || (d >= e);
```

### Preprocessor Directives
```c
// Include directives
#include <stdio.h>
#include "myheader.h"

// Macro definitions
#define PI 3.14159
#define MAX_SIZE 100
#define SQUARE(x) ((x) * (x))
#define MIN(a, b) ((a) < (b))
```

### Enumerations
```c
enum Color {
    RED,
    GREEN,
    BLUE
};

enum Status {
    ACTIVE,
    INACTIVE,
    PENDING
};
```

## AST Output Format

The parser generates human-readable AST representations. For example:

**Source Code:**
```c
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

**AST Output:**
```
Fn(FnDecl { 
    returnType: "int", 
    name: "factorial", 
    params: [("int", "n")], 
    block: [
        If(IfStmt { 
            cond: ::Comp(LessEqual)(<=) "n" 1, 
            if_block: [Ret(ExprStmt { expr: 1 })], 
            else_block: [] 
        }),
        Ret(ExprStmt { 
            expr: ::Mul(*) "n" Call(FnCall { 
                ident: "factorial", 
                args: [::Sub(-) "n" 1] 
            })
        })
    ]
})
```

## Error Handling

The parser provides detailed error messages with location information:
```
Parse Error: Expected ';' after variable declaration at line 5, column 12
Parse Error: Expected ')' after condition at line 8, column 15
Parse Error: Unexpected token at line 10, column 3
```

## Testing

The parser includes comprehensive test suites covering:

1. Arithmetic operators with modulo
2. Logical operators  
3. Bitwise operators
4. While loops
5. Do-while loops
6. Switch-case statements
7. Continue statements
8. Type qualifiers (const)
9. Simple type combinations
10. Functions with simple const types
11. Nested control flow
12. Complex expressions
13. All unary operators
14. Include directives
15. Define directives
16. Mixed preprocessor and code
17. Enum declarations
18. Error handling and edge cases

All tests demonstrate successful parsing and correct AST generation.

## Dependencies

- **Lexer**: Requires `lexer.cpp` for tokenization
- **C++ Standard**: C++11 or later (uses smart pointers, auto, etc.)
- **Compiler**: GCC, Clang, or any C++11 compatible compiler

## Architecture Benefits

1. **Memory Efficient**: Streams tokens one at a time instead of loading all into memory
2. **Modular Design**: Clear separation between lexing and parsing phases  
3. **Extensible**: Easy to add new language constructs
4. **Robust**: Comprehensive error handling and recovery
5. **Fast**: Efficient recursive descent parsing
6. **Standard Compliant**: Follows established C language grammar patterns

## Future Enhancements

Potential areas for expansion:
- Pointer and array declarations
- Struct and union support  
- Typedef declarations
- More complex expressions (ternary operator, etc.)
- Improved error recovery
- Symbol table integration
- Semantic analysis

---

*This parser is part of a custom compiler project and serves as the syntax analysis phase of the compilation pipeline.*