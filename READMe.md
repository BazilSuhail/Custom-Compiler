# Custom Compiler

A custom compiler written in Rust featuring a complete lexical analyzer and Pratt parser with UTF-8 support, comprehensive error handling, and Abstract Syntax Tree (AST) generation.

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

### Syntax Analysis - Parser (Implemented)
- **Pratt Parser**: Efficient operator precedence parsing using the Pratt parsing algorithm
- **AST Generation**: Constructs a complete Abstract Syntax Tree for the entire program
- **Comprehensive Language Support**:
  - Variable declarations with optional initialization
  - Function declarations and prototypes
  - Control flow statements (if/else, while, do-while, for, switch/case)
  - Expression parsing with proper precedence
  - Enum declarations
  - Include statements
  - Main function support
- **Error Recovery**: Detailed parse error messages with location information
- **AST Visualization**: Colored AST printer for debugging and visualization

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
│   ├── token.rs         # Token type definitions
│   ├── parser.rs        # Pratt parser implementation
│   ├── ast.rs           # Abstract Syntax Tree node definitions
│   └── ast_printer.rs   # AST visualization and printing
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

### Lexer Error Handling
The lexer detects and reports:
- Unterminated string/character literals
- Unterminated multi-line comments
- Invalid numeric literals (e.g., `123abc`)
- Unexpected characters

---

## 📖 Parser Architecture

### Pratt Parser Implementation
The parser uses the **Pratt parsing algorithm** (also known as "Top-Down Operator Precedence Parsing") which provides:
- **Efficient precedence handling**: Operators are parsed according to their precedence levels
- **Clean expression parsing**: Unified handling of prefix, infix, and postfix operators
- **Extensibility**: Easy to add new operators and expressions

### Operator Precedence Levels
The parser implements 12 precedence levels (from lowest to highest):

| Level | Precedence | Operators |
|-------|------------|-----------|
| 0 | Lowest | - |
| 1 | Assignment | `=` |
| 2 | Logical OR | `\|\|` |
| 3 | Logical AND | `&&` |
| 4 | Equality | `==`, `!=` |
| 5 | Comparison | `<`, `>`, `<=`, `>=` |
| 6 | Bitwise | `&`, `\|`, `^`, `<<`, `>>` |
| 7 | Term | `+`, `-` |
| 8 | Factor | `*`, `/`, `%` |
| 9 | Unary | `-`, `!`, `++`, `--` (prefix) |
| 10 | Postfix | `++`, `--` (postfix) |
| 11 | Call | `()` |

### Abstract Syntax Tree (AST) Nodes

The parser generates a comprehensive AST with the following node types:

#### Literal Nodes
- **IntLiteral**: Integer values (e.g., `42`, `-10`)
- **FloatLiteral**: Floating-point values (e.g., `3.14`, `1.5e-10`)
- **StringLiteral**: String values (e.g., `"Hello World"`)
- **CharLiteral**: Character values (e.g., `'a'`, `'\n'`)
- **BoolLiteral**: Boolean values (`true`, `false`)

#### Expression Nodes
- **Identifier**: Variable/function names
- **BinaryExpr**: Binary operations (e.g., `a + b`, `x == y`)
- **UnaryExpr**: Unary operations (prefix/postfix, e.g., `!flag`, `i++`)
- **CallExpr**: Function calls (e.g., `foo(a, b)`)

#### Declaration Nodes
- **VarDecl**: Variable declarations (e.g., `int x = 10;`)
- **FunctionProto**: Function prototypes (e.g., `int add(int a, int b);`)
- **FunctionDecl**: Function definitions with body
- **MainDecl**: Main function declaration
- **EnumDecl**: Enumeration declarations

#### Statement Nodes
- **IfStmt**: If-else statements
- **WhileStmt**: While loops
- **DoWhileStmt**: Do-while loops
- **ForStmt**: For loops
- **SwitchStmt**: Switch-case statements
- **CaseBlock**: Individual case blocks
- **ReturnStmt**: Return statements
- **BreakStmt**: Break statements
- **PrintStmt**: Print statements
- **BlockStmt**: Block statements `{ ... }`
- **ExpressionStmt**: Expression statements
- **IncludeStmt**: Include directives

### Parser Error Handling

The parser provides detailed error messages for:
- **UnexpectedEOF**: Unexpected end of file
- **FailedToFindToken**: Expected token not found
- **ExpectedTypeToken**: Type keyword expected
- **ExpectedIdentifier**: Identifier expected
- **UnexpectedToken**: Unexpected token encountered
- **ExpectedExpr**: Expression expected
- **InvalidCallTarget**: Only identifiers can be called as functions
- **MissingSemicolon**: Missing semicolon at end of statement
- **UnclosedBlock**: Unclosed block (missing `}`)

Each error includes:
- Error type and description
- Line and column number
- Token information

---

## 📝 Language Grammar (EBNF)

### Program Structure
```ebnf
Program          ::= IncludeStmt+ Declaration* MainDecl?

IncludeStmt      ::= "include" ("<" Identifier ">" | StringLiteral)

Declaration      ::= VarDecl
                   | FunctionProto
                   | FunctionDecl
                   | EnumDecl
```

### Declarations
```ebnf
VarDecl          ::= Type Identifier ("=" Expression)? ";"

FunctionProto    ::= Type Identifier "(" ParameterList? ")" ";"

FunctionDecl     ::= Type Identifier "(" ParameterList? ")" Block

MainDecl         ::= "main" Block

EnumDecl         ::= "enum" Identifier "{" EnumValueList "}" ";"

ParameterList    ::= Parameter ("," Parameter)*

Parameter        ::= Type Identifier

EnumValueList    ::= Identifier ("," Identifier)*
```

### Statements
```ebnf
Statement        ::= VarDecl
                   | FunctionDecl
                   | FunctionProto
                   | EnumDecl
                   | PrintStmt
                   | IfStmt
                   | WhileStmt
                   | DoWhileStmt
                   | ForStmt
                   | SwitchStmt
                   | ReturnStmt
                   | BreakStmt
                   | Block
                   | ExpressionStmt

PrintStmt        ::= "print" "(" ArgumentList? ")" ";"

IfStmt           ::= "if" "(" Expression ")" Block ("else" Block)?

WhileStmt        ::= "while" "(" Expression ")" Block

DoWhileStmt      ::= "do" Block "while" "(" Expression ")" ";"

ForStmt          ::= "for" "(" (VarDecl | ExpressionStmt | ";")
                                (Expression)? ";"
                                (Expression)? ")" Block

SwitchStmt       ::= "switch" "(" Expression ")" "{"
                     CaseBlock* DefaultBlock? "}"

CaseBlock        ::= "case" Expression ":" Statement*

DefaultBlock     ::= "default" ":" Statement*

ReturnStmt       ::= "return" Expression? ";"

BreakStmt        ::= "break" ";"

Block            ::= "{" Statement* "}"

ExpressionStmt   ::= Expression ";"
```

### Expressions
```ebnf
Expression       ::= Assignment

Assignment       ::= LogicalOr ("=" Assignment)?

LogicalOr        ::= LogicalAnd ("||" LogicalAnd)*

LogicalAnd       ::= Equality ("&&" Equality)*

Equality         ::= Comparison (("==" | "!=") Comparison)*

Comparison       ::= BitwiseTerm (("<" | ">" | "<=" | ">=") BitwiseTerm)*

BitwiseTerm      ::= Term (("&" | "|" | "^" | "<<" | ">>") Term)*

Term             ::= Factor (("+" | "-") Factor)*

Factor           ::= Unary (("*" | "/" | "%") Unary)*

Unary            ::= ("-" | "!" | "++" | "--") Unary
                   | Postfix

Postfix          ::= Primary ("++" | "--" | Call)*

Call             ::= Primary "(" ArgumentList? ")"

Primary          ::= IntLiteral
                   | FloatLiteral
                   | StringLiteral
                   | CharLiteral
                   | BoolLiteral
                   | Identifier
                   | "(" Expression ")"

ArgumentList     ::= Expression ("," Expression)*
```

### Types
```ebnf
Type             ::= "int" | "float" | "double" | "char" 
                   | "bool" | "void" | "string"
```

### Literals
```ebnf
IntLiteral       ::= [0-9]+

FloatLiteral     ::= [0-9]+ "." [0-9]* ([eE] [+-]? [0-9]+)?
                   | "." [0-9]+ ([eE] [+-]? [0-9]+)?

StringLiteral    ::= '"' Character* '"'

CharLiteral      ::= "'" Character "'"

BoolLiteral      ::= "true" | "false"

Identifier       ::= (Letter | "_") (Letter | Digit | "_" | UnicodeChar)*
```

---

## 🎓 What I've Built

### Lexer Achievements
1. ✅ Added `const` and `global` keyword support
2. ✅ Implemented full UTF-8/Unicode identifier support
3. ✅ Enhanced identifier matching to support emojis and special characters
4. ✅ Maintained backward compatibility with ASCII identifiers
5. ✅ Comprehensive token recognition (35+ token types)

### Parser Achievements
1. ✅ Implemented Pratt parser with 12 precedence levels
2. ✅ Complete AST generation for all language constructs
3. ✅ Support for complex expressions with proper precedence
4. ✅ Function declarations and prototypes
5. ✅ All control flow statements (if/else, loops, switch/case)
6. ✅ Enum declarations
7. ✅ Detailed error reporting with location information
8. ✅ Colored AST visualization for debugging

### Key Achievements
- **Robust Lexer**: Handles edge cases and provides detailed error messages
- **Modern Language Support**: UTF-8 identifiers enable international programming
- **Efficient Parser**: Pratt parsing algorithm for clean expression handling
- **Complete AST**: Full Abstract Syntax Tree representation of programs
- **Clean Architecture**: Modular design with separate lexer, parser, AST, and printer modules

## 🚧 Future Enhancements

- [x] ~~Lexical analysis (Lexer)~~ ✅ **Completed**
- [x] ~~Syntax analysis (Parser)~~ ✅ **Completed**
- [x] ~~Semantic analysis~~        ✅ **Completed**
- [ ] Intermediate code generation
- [ ] Code optimization
- [ ] Target code generation

## 📝 License

This project is a custom educational compiler implementation.

## 🤝 Contributing

This is a personal learning project. Feel free to fork and experiment!