# Custom Compiler

A custom compiler written in Rust with a full-featured lexical analyzer supporting UTF-8 identifiers and comprehensive token recognition.

## 🚀 Features

### Lexical Analysis (Implemented)
- **UTF-8 Support**: Full Unicode support for identifiers, including emojis and special characters (e.g., `x😒😒`, `变量`, `переменная`)
- **Comment Handling**: Single-line (`//`) and multi-line (`/* */`) comments
- **String & Character Literals**: Proper handling of escape sequences
- **Numeric Literals**: 
  - Integer literals
  - Floating-point literals (including scientific notation)
  - Decimal-only literals (e.g., `.22`)
- **Comprehensive Operator Support**: Arithmetic, logical, bitwise, comparison, and assignment operators
- **Error Detection**: Detailed error reporting with line and column numbers

### Supported Token Types

#### Data Types
- `int`, `float`, `double`, `char`, `void`, `bool`, `string`, `enum`

#### Keywords
- **Control Flow**: `if`, `else`, `while`, `for`, `do`, `switch`, `case`, `default`, `break`
- **Functions**: `return`, `main`, `print`
- **Modifiers**: `const`, `global`, `include`

#### Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`, `++`, `--`
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical**: `&&`, `||`, `!`
- **Bitwise**: `&`, `|`, `^`, `<<`, `>>`
- **Assignment**: `=`

#### Literals
- Integer: `42`, `0`, `-10`
- Float: `3.14`, `.22`, `1.5e-10`
- String: `"Hello World!"`
- Character: `'a'`, `'\n'`
- Boolean: `true`, `false`

#### Delimiters & Punctuation
- Parentheses: `(`, `)`
- Braces: `{`, `}`
- Brackets: `[`, `]`
- Others: `;`, `,`, `.`, `:`

## 📦 Installation

### Prerequisites
- Rust (latest stable version)
- Cargo

### Build
```bash
# Clone the repository
git clone <repository-url>
cd Custom-Compiler

# Build the project
cargo build --release
```

## 🎯 Usage

### Basic Usage
```bash
cargo run <input_file>
```

### Example
```bash
cargo run test_input.txt
```

### Input File Example
```c
// Sample program
const int MAX = 100;
global float pi = 3.14;

main() {
    int x😒😒 = 42;  // UTF-8 identifiers supported!
    float y = 3.14;
    
    if (x > 10 && y <= 5.0) {
        print("Hello World!");
        x++;
    }
    
    return 0;
}
```

### Output Format
Each token is printed in the format:
```
TOKEN_TYPE(value), line: X, col: Y
```

Example output:
```
T_CONST(const), line: 2, col: 1
T_INT(int), line: 2, col: 7
T_IDENTIFIER(MAX), line: 2, col: 11
T_ASSIGNOP(=), line: 2, col: 15
T_INTLIT(100), line: 2, col: 17
T_SEMICOLON(;), line: 2, col: 20
```

## 🏗️ Project Structure

```
Custom-Compiler/
├── src/
│   ├── main.rs          # Entry point and CLI handling
│   ├── lexer.rs         # Lexical analyzer implementation
│   └── token.rs         # Token type definitions
├── test_input.txt       # Sample test file
├── Cargo.toml          # Project dependencies
└── README.md           # This file
```

## 🔧 Implementation Details

### Lexer Architecture
The lexer is implemented as a state machine that:
1. **Scans** the input character by character
2. **Matches** patterns for different token types
3. **Tracks** line and column numbers for error reporting
4. **Handles** edge cases like unterminated strings and invalid numeric literals

### UTF-8 Identifier Support
Identifiers can:
- Start with: Any Unicode alphabetic character or underscore (`_`)
- Continue with: Alphanumeric characters, underscores, or any Unicode character except whitespace, operators, and delimiters

### Error Handling
The lexer detects and reports:
- Unterminated string/character literals
- Unterminated multi-line comments
- Invalid numeric literals (e.g., `123abc`)
- Unexpected characters

## 🎓 What We've Built

### Session Summary
1. ✅ Added `const` and `global` keyword support
2. ✅ Implemented full UTF-8/Unicode identifier support
3. ✅ Enhanced identifier matching to support emojis and special characters
4. ✅ Maintained backward compatibility with ASCII identifiers
5. ✅ Simplified main.rs for clean token output

### Key Achievements
- **Robust Lexer**: Handles edge cases and provides detailed error messages
- **Modern Language Support**: UTF-8 identifiers enable international programming
- **Comprehensive Token Set**: 35+ token types covering all common programming constructs
- **Clean Architecture**: Modular design with separate token and lexer modules

## 🚧 Future Enhancements

- [ ] Syntax analysis (Parser)
- [ ] Semantic analysis
- [ ] Intermediate code generation
- [ ] Code optimization
- [ ] Target code generation

## 📝 License

This project is a custom educational compiler implementation.

## 🤝 Contributing

This is a personal learning project. Feel free to fork and experiment!