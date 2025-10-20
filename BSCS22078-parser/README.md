# Bison Parser for C-like Language

This directory contains a complete Flex/Bison parser implementation for a C-like language based on the provided grammar specification. The parser builds an **Abstract Syntax Tree (AST)** during parsing and outputs a visual representation of the tree structure.

## Usage

To parse your own C-like source files:

```bash
./parser your_file.c
```

## More Details

### Data Types
- Basic types: int, float, double, char, bool, void
- Type qualifiers: const
- Enumerations

### Preprocessor Directives
- `#include <header.h>`
- `#include "header.h"`

### Declarations
- Variable declarations with optional initialization
- Function declarations with parameters
- Enum declarations

### Statements
- Assignment statements
- Control flow: if, if-else, while, do-while, for, switch-case
- Jump statements: return, break, continue
- Expression statements

### Expressions
- Arithmetic operators: +, -, *, /, %
- Comparison operators: ==, !=, <, >, <=, >=
- Logical operators: &&, ||, !
- Bitwise operators: &, |, ^, ~, <<, >>
- Increment/decrement: ++, --
- Function calls
- Proper operator precedence and associativity

### Literals
- Integer literals: 123
- Float literals: 3.14, 1.5e-10
- Boolean literals: true, false
- Character literals: 'a', '\n'
- String literals: "hello world"

### Comments
- Single-line comments: // comment
- Multi-line comments: /* comment */

## Building the Parser

### Prerequisites
- Flex (Fast Lexical Analyzer)
- Bison (GNU Parser Generator)
- GCC (GNU Compiler Collection)

### Build Commands

#### (Windows PowerShell)

```powershell
# Generate parser
bison -d parser.y

# Generate lexer
flex lexer.l

# Compile
gcc -o parser parser.tab.c lex.yy.c

# Run with your file
./parser your_file.c
```

#### Manual Build (Linux/Unix)

```bash
# Generate parser
bison -d parser.y

# Generate lexer
flex lexer.l

# Compile
gcc -o parser parser.tab.c lex.yy.c

# Run with your file
./parser your_file.c
```

The parser will:
- Report "Parse successful!" if the file is syntactically correct
- Display the complete Abstract Syntax Tree with hierarchical structure
- Report parse errors with line and column numbers if there are syntax errors

## AST Structure

The AST represents the parsed code as a tree with the following node types:

### Declaration Nodes
- **NODE_PROGRAM** - Root node containing all declarations
- **NODE_VAR_DECL** - Variable declarations with type and optional initializer
- **NODE_FUNC_DECL** - Function declarations with return type, parameters, and body
- **NODE_ENUM_DECL** - Enumeration declarations
- **NODE_INCLUDE_DIRECTIVE** - Include directives (#include)

### Statement Nodes
- **NODE_IF_STMT** - If/if-else statements
- **NODE_WHILE_STMT** - While loops
- **NODE_DO_WHILE_STMT** - Do-while loops
- **NODE_FOR_STMT** - For loops with init, condition, increment
- **NODE_SWITCH_STMT** - Switch statements
- **NODE_CASE_STMT** - Case/default labels
- **NODE_RETURN_STMT** - Return statements
- **NODE_BREAK_STMT** - Break statements
- **NODE_CONTINUE_STMT** - Continue statements
- **NODE_BLOCK** - Code blocks {...}

### Expression Nodes
- **NODE_BINARY_OP** - Binary operations (+, -, *, /, ==, &&, etc.)
- **NODE_UNARY_OP** - Unary operations (!, ~, ++, --, unary +/-)
- **NODE_FUNC_CALL** - Function calls with arguments
- **NODE_LITERAL** - Constant values (int, float, char, string, bool)
- **NODE_IDENTIFIER** - Variable/function names

### Type Nodes
- **NODE_TYPE_SPEC** - Type specifications (const int, float, etc.)
- **NODE_PARAM** - Function parameters

### List Nodes
- **NODE_DECLARATION_LIST** - List of declarations
- **NODE_PARAM_LIST** - Parameter lists
- **NODE_ARG_LIST** - Argument lists
- **NODE_ENUM_VALUE_LIST** - Enum value lists

