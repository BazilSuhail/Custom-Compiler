# Parser Module Documentation

## Overview
The Parser is the core of the syntax analysis phase. It takes a stream of tokens from the Lexer and constructs an **Abstract Syntax Tree (AST)**. Our compiler utilizes the **Pratt Parsing** (Top-Down Operator Precedence) algorithm, which is highly efficient for handling complex expression hierarchies and operator precedence.

## Architecture

Our parser employs a **Hybrid Architectural Design**, combining the clarity of **Recursive Descent** for high-level structural constructs with the power of **Pratt Parsing** for complex expression trees.

### 1. High-Level Recursive Descent
For the macro-level structure of the language—such as program declarations, control flow statements (`if`, `while`, `for`), and function definitions—we use **Top-Down Recursive Descent**. 

- **Why?**: Recursive descent is exceptionally readable and easy to debug for "structured" syntax where the next token usually determines the entire path (e.g., seeing `if` immediately triggers `parse_if_statement`).
- **Implementation**: Methods like `parse_program()`, `parse_statement()`, and `parse_block()` navigate the token stream by matching expected keywords and recursively diving into sub-constructs.

### 2. Expression Parsing via Pratt Algorithm
For expressions—where arithmetic precedence, associativity, and complex nesting (e.g., `a + b * c == d || e`) make traditional recursive descent cumbersome—we utilize the **Pratt Parsing** algorithm (Top-Down Operator Precedence).

- **Why?**: Traditional recursive descent requires a separate function for every precedence level, leading to "infinite" call stacks and rigid code. Pratt parsing solves this by using a singular entry point and a dynamic precedence-driven loop.
- **Implementation**: The `parse_expression(precedence)` function is the engine. It uses two primary dispatch mechanisms:
    - **Nud (Null Denotation)**: Handles "leader" tokens that start an expression (e.g., Literals, `-` sign, `(` parenthesis). It doesn't care about anything to its left.
    - **Led (Left Denotation)**: Handles "infix" or "postfix" tokens that attach to an expression already parsed on the left (e.g., `+`, `*`, `(args)` for calls, `++`).
- **Binding Power**: Each operator is assigned a numerical precedence. The parser continues to consume tokens as long as the next token has a higher "binding power" than the current context.

### 3. Parsing Flow Diagram
```text
[Tokens] -> parse_program() [RD]
               |-> parse_statement() [RD]
                     |-> parse_if_statement() [RD]
                           |-> parse_expression() [Pratt]
                                 |-> parse_prefix() [Nud]
                                 |-> parse_infix()  [Led]
```

### Operator Precedence Table
The parser implements the following 12 precedence levels (from lowest to highest):

| Level | Precedence | Operators |
|-------|------------|-----------|
| 0 | Lowest | - |
| 1 | Assignment | `=` |
| 2 | Logical OR | `||` |
| 3 | Logical AND | `&&` |
| 4 | Equality | `==`, `!=` |
| 5 | Comparison | `<`, `>`, `<=`, `>=` |
| 6 | Bitwise | `&`, `|`, `^`, `<<`, `>>` |
| 7 | Term | `+`, `-` |
| 8 | Factor | `*`, `/`, `%` |
| 9 | Unary | `-`, `!`, `++`, `--` (prefix) |
| 10 | Postfix | `++`, `--` (postfix) |
| 11 | Call | `()` |

---

## Abstract Syntax Tree (AST)

The AST is the internal representation of the program structure. Every node implements `Debug` and `Clone` for easy traversal and debugging.

### Node Categories:
- **Literals**: `IntLiteral`, `FloatLiteral`, `StringLiteral`, `CharLiteral`, `BoolLiteral`.
- **Expressions**: `Identifier`, `BinaryExpr`, `UnaryExpr` (prefix/postfix), `CallExpr`.
- **Statements**: `VarDecl`, `IfStmt`, `WhileStmt`, `ForStmt`, `SwitchStmt`, `ReturnStmt`, `BreakStmt`.
- **Declarations**: `FunctionDecl`, `FunctionProto`, `MainDecl`, `EnumDecl`.

### Visualization
We provide an `ast_printer.rs` module that renders the AST in a colored, indented tree format. This is crucial for verifying that the parser correctly understood the hierarchy of nested expressions and control flow.

---

## Examples & AST Visualization

Below are examples of source code and how they are represented in the generated Abstract Syntax Tree.

### 1. Operator Precedence
The Pratt parser correctly handles complex expressions by respecting operator precedence (e.g., multiplication before addition).

**Source:**
```c
int x = 5 + 10 * 2;
```

**AST Representation:**
```text
VarDecl(int, "x")
  BinaryExpr(+)
    IntLiteral(5)
    BinaryExpr(*)
      IntLiteral(10)
      IntLiteral(2)
```

### 2. Control Flow (If-Else)
Nested structures and blocks are represented as children of the statement node.

**Source:**
```c
if (x > 0) {
    print("Positive");
} else {
    print("Non-positive");
}
```

**AST Representation:**
```text
IfStmt
  Condition:
    BinaryExpr(>)
      Identifier("x")
      IntLiteral(0)
  IfBody:
    PrintStmt
      StringLiteral("Positive")
  ElseBody:
    PrintStmt
      StringLiteral("Non-positive")
```

### 3. Function Declarations
Functions include their return type, parameter list, and a block of statements.

**Source:**
```c
int add(int a, int b) {
    return a + b;
}
```

**AST Representation:**
```text
FunctionDecl(int, "add")
  Param: int a
  Param: int b
  Body:
    ReturnStmt
      BinaryExpr(+)
        Identifier("a")
        Identifier("b")
```

---

## Language Grammar (EBNF)

### Program & Declarations
```ebnf
Program          ::= Declaration* MainDecl?

Declaration      ::= VarDecl | FunctionProto | FunctionDecl | EnumDecl

VarDecl          ::= ("const" | "global")? Type Identifier ("=" Expression)? ";"

FunctionProto    ::= Type Identifier "(" ParameterList? ")" ";"

FunctionDecl     ::= Type Identifier "(" ParameterList? ")" "{" Statement* "}"

MainDecl         ::= "main" "{" Statement* "}"

EnumDecl         ::= "enum" Identifier "{" Identifier ("," Identifier)* "}" ";"
```

### Statements & Control Flow
```ebnf
Statement        ::= IfStmt | WhileStmt | ForStmt | SwitchStmt | ReturnStmt | BreakStmt | Block | ExpressionStmt

IfStmt           ::= "if" "(" Expression ")" Block ("else" Block)?

ForStmt          ::= "for" "(" (VarDecl | ExpressionStmt | ";") (Expression)? ";" (Expression)? ")" Block

SwitchStmt       ::= "switch" "(" Expression ")" "{" CaseBlock* DefaultBlock? "}"

Block            ::= "{" Statement* "}"
```

### Expressions
```ebnf
Expression       ::= Assignment

Assignment       ::= Identifier "=" Expression | LogicalOr

LogicalOr        ::= LogicalAnd ("||" LogicalAnd)*

Term             ::= Factor (("+" | "-") Factor)*

Primary          ::= Literal | Identifier | "(" Expression ")" | Call
```

---

## Error Handling

The parser implements robust error detection and reporting. When a syntax error is encountered, the parser generates a `ParseError` struct containing:
- **Error Type**: (e.g., `UnexpectedToken`, `MissingSemicolon`, `UnclosedBlock`).
- **Token Info**: The exact token where the failure occurred.
- **Location**: Line and column number.

### Common Error Recoveries:
- **Missing Semicolon**: Detects when a statement ends abruptly and suggests a semicolon.
- **Unclosed Blocks**: Identifies missing `}` before EOF.
- **Invalid Call Targets**: Ensures only identifiers can be treated as function callees (e.g., `5()` is caught).

---

## Testing the Parser

The parser is tested using comprehensive input files like `test_input.txt`. These tests verify:
1. **Precedence Correctness**: Ensures `a + b * c` is parsed as `a + (b * c)`.
2. **Associativity**: Ensures `a = b = c` is parsed correctly for assignments.
3. **Complex Nesting**: Validates deep `if-else` and loop structures.
4. **UTF-8 Support**: Confirms that Unicode identifiers are correctly placed in the AST.

### Running a Test:
```bash
cargo run test_input.txt
```
The output will display the full AST, allowing for visual verification of the syntax tree hierarchy.