# Custom Compiler 🚀

A professional, industrial-grade multi-pass compiler written in Rust. This project transforms a high-level C-like language into an optimized **Three-Address Code (TAC)** intermediate representation. It features a robust Pratt parser, a context-sensitive semantic analyzer, and a sophisticated 5-pass IR optimization pipeline.

---

## �️ Compiler Architecture

The compiler is organized into a modular pipeline, ensuring high maintainability and clear separation of concerns:

1.  **Lexical Analysis (Lexer)**: Scans source code into a stream of typed tokens with full UTF-8/Unicode support.
2.  **Syntax Analysis (Parser)**: Implements a **Pratt Parser** to construct a high-fidelity Abstract Syntax Tree (AST).
3.  **Semantic Analysis**:
    *   **Scope Analyzer**: Manages symbol tables, handle shadowing, and ensures declaration-before-use.
    *   **Type Checker**: Enforces strict typing rules and validates operation compatibility.
4.  **IR Generation**: Translates the validated AST into flat, machine-agnostic **Three-Address Code (TAC)**.
5.  **IR Optimization**: A persistent optimizer that runs 5 distinct passes (Constant Folding, Propagation, DCE, etc.) until code reachability and efficiency are maximized.

---

## ✨ Language Features & Specifications

### 🔬 Advanced Type System
*   **Strongly Typed**: No implicit type coercion (e.g., `int` + `float` is an error).
*   **Native Types**: `int`, `float`, `double`, `char`, `bool`, `string`, `void`.
*   **Enumerations**: 
    *   Declared globally with `enum Name { ... }`.
    *   Automatically assigned sequential integer values.
    *   Compatible with `int` for logic and switch statements.

### 🔐 Variable Modifiers
*   **`const`**: Marks variables as immutable post-initialization, enabling aggressive compile-time optimizations.
*   **`global`**: Declares variables in the global segment, surviving local scope pruning.

### 🌐 Modern UTF-8 Support
Identifiers can include any Unicode character:
*   Standard: `int count = 0;`
*   International: `int 变量 = 10;`, `int перем = 5;`
*   Creative: `float x😒😒 = 3.14;`

---

## 📝 Language Grammar (EBNF)

The full formal grammar used by the parser:

### Program Structure
```ebnf
Program          ::= Includes* Declaration* MainDecl?
Includes         ::= "include" ("<" Identifier ">" | StringLiteral)
Declaration      ::= VarDecl | FunctionProto | FunctionDecl | EnumDecl
```

### Declarations & Types
```ebnf
VarDecl          ::= ("const" | "global")? Type Identifier ("=" Expression)? ";"
Type             ::= "int" | "float" | "double" | "char" | "bool" | "string" | "void" | Identifier
FunctionProto    ::= Type Identifier "(" ParamList? ")" ";"
FunctionDecl     ::= Type Identifier "(" ParamList? ")" Block
MainDecl         ::= "main" Block
EnumDecl         ::= "enum" Identifier "{" Identifier ("," Identifier)* "}" ";"
ParamList        ::= (Type Identifier) ("," Type Identifier)*
```

### Statements & Control Flow
```ebnf
Statement        ::= VarDecl | Block | IfStmt | WhileStmt | DoWhileStmt | ForStmt 
                   | SwitchStmt | ReturnStmt | BreakStmt | PrintStmt | ExpressionStmt
Block            ::= "{" Statement* "}"
IfStmt           ::= "if" "(" Expression ")" Block ("else" Block)?
WhileStmt        ::= "while" "(" Expression ")" Block
DoWhileStmt      ::= "do" Block "while" "(" Expression ")" ";"
ForStmt          ::= "for" "(" (VarDecl | ExpressionStmt | ";") Expression? ";" Expression? ")" Block
SwitchStmt       ::= "switch" "(" Expression ")" "{" CaseBlock* DefaultBlock? "}"
CaseBlock        ::= "case" Expression ":" Statement*
ReturnStmt       ::= "return" Expression? ";"
PrintStmt        ::= "print" "(" ExpressionList? ")" ";"
```

### Expressions (Precedence-based)
```ebnf
Expression       ::= Assignment
Assignment       ::= LogicalOr ( "=" Assignment )?
LogicalOr        ::= LogicalAnd ( "||" LogicalAnd )*
Equality         ::= Comparison ( ("==" | "!=") Comparison )*
Comparison       ::= Term ( ("<" | ">" | "<=" | ">=") Term )*
Term             ::= Factor ( ("+" | "-") Factor )*
Factor           ::= Unary ( ("*" | "/" | "%") Unary )*
Unary            ::= ("-" | "!" | "++" | "--") Unary | Postfix
Postfix          ::= Primary ("++" | "--" | Call)*
Primary          ::= Literal | Identifier | "(" Expression ")"
```

---

## 📖 Syntax Analysis: Pratt Parsing

The parser uses the **Top-Down Operator Precedence (Pratt)** algorithm. This allows the compiler to handle 12 levels of precedence with clean, non-recursive calls for infix operations.

| Precedence | Operators | Association |
| :--- | :--- | :--- |
| **Call** | `()` | Left |
| **Postfix** | `++`, `--` | Left |
| **Unary** | `-`, `!`, `++`, `--` | Right |
| **Factor** | `*`, `/`, `%` | Left |
| **Term** | `+`, `-` | Left |
| **Bitwise** | `&`, `|`, `^`, `<<`, `>>` | Left |
| **Comparison** | `<`, `>`, `<=`, `>=` | Left |
| **Equality** | `==`, `!=` | Left |
| **Logical AND** | `&&` | Left |
| **Logical OR** | `||` | Left |
| **Assignment** | `=` | Right |

---

## 🧠 Semantic Intelligence

### Scope Analyzer (`ScopeAnalyzer`)
Implements a 2-pass approach:
1.  **Declaration Collection**: Harvests top-level symbols for forward-reference safety.
2.  **Lexical Validation**: Traverses the AST, maintaining a tree of `ScopeFrame` nodes. It handles **Shadowing** (inner blocks overriden outer ones) and prevents duplicate declarations.

### Type Checker (`TypeChecker`)
Enforces the safety of the program:
*   **Parameter Checking**: Verifies function call argument types and counts.
*   **Condition Validation**: `if`/`while` conditions *must* be `bool`.
*   **Switch Safety**: Only `int`, `char`, or `enum` are allowed for switch expressions.
*   **Context Awareness**: Prevents `break` statements outside of loops and `return` in `void` functions.

---

## 🚀 Intermediate Representation (TAC)

The compiler translates the high-level AST into optimized **Three-Address Code**.

### TAC Instruction Set
*   **Register-like**: Uses virtual temporaries `t0`, `t1`, `t2`...
*   **Control Flow**: Uses `Label:`, `Goto Label`, and `ifFalse/ifTrue`.
*   **Metadata**: Preserves `const` and `global` qualifiers for downstream backends.

**Example Transformation:**
```text
// Source: x = 10 + 5 * 2;
t0 = 5 Multiply 2
t1 = 10 Plus t0
int x = t1
```

---

## ✨ IR Optimization passes

The compiler features a persistent optimization engine that reaches a "Fixed Point" (no further changes possible).

1.  **Constant Folding**: Solves `5 + 10` → `15` at compile time.
2.  **Constant Propagation**: Replaces variable uses with known constants (e.g., `const int a = 10; b = a + 5` → `b = 10 + 5`).
3.  **Copy Propagation**: Eliminates chains like `t1 = x; t2 = t1` → `t2 = x`.
4.  **Dead Code Elimination (DCE)**: Multi-step algorithm that prunes assignments to unused variables and removes unreachable labels.
5.  **Peephole Optimization**:
    *   **Algebraic Simplification**: `x + 0` → `x`, `x * 1` → `x`, `x * 0` → `0`.
    *   **Redundant Jumps**: Deletes `goto L1` if `L1` is the next instruction.

---

## 🏗️ Project Structure

```text
Custom-Compiler/
├── src/
│   ├── core/           # Token definitions and AST Node models
│   ├── lexer/          # State-machine based Lexical Analyzer
│   ├── parser/         # Pratt Parser & AST Visualization tools
│   ├── semantics/      # Scope Analyzer and Type Checker
│   ├── ir_pipeline/    # TAC Generator and 5-pass Optimizer
│   └── main.rs         # CLI Entry Point
├── docs/               # Detailed technical documentation for each module
├── test_input.txt      # Main integration test file
└── Cargo.toml          # Rust dependencies
```

## 📦 Installation & Developer Guide

### Prerequisites
*   [Rust & Cargo](https://rustup.rs/) (Stable)

### Building the Project
```bash
cargo build --release
```

### Running Tests
The compiler takes a source file as an argument and outputs the AST, Raw TAC, and Optimized TAC.
```bash
cargo run -- test_input.txt
```

---

## 🎓 Project Achievements
*   ✅ **Robust Lexer**: 35+ token types with full Unicode/Emoji identification.
*   ✅ **Pratt Parser**: Efficient 12-level precedence handling.
*   ✅ **Semantic Suite**: Detection of 15+ scope errors and 17+ type errors.
*   ✅ **IR Pipeline**: Full TAC generation for all control flow constructs.
*   ✅ **Fixed-Point Optimizer**: Significant reduction in code density and complexity through 5-pass analysis.

## 🚧 Road Map
- [x] Full IR Optimization Suite
- [ ] RISC-V Code Generation Backend
- [ ] Register Allocation (Graph Coloring)
- [ ] Structs and Pointers support
- [ ] Standard Library (I/O, String manipulation)