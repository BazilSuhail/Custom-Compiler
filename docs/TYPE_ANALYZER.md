# Type Analyzer Documentation

## Overview
The Type Analyzer is the final semantic analysis stage of the compiler. It ensures that all operations, assignments, and function calls are performed with compatible types. It leverages the symbol table built by the `ScopeAnalyzer` to perform its checks without redundant work.

## Architecture

### Symbol Table Reuse
Unlike traditional compilers that might rebuild the symbol table, this analyzer takes a reference to the pre-built `ScopeFrame` tree. It maintains a `scope_child_indices` stack to navigate the tree in perfect sync with the AST traversal, ensuring it always looks up symbols in the correct lexical scope.

### Key Components

#### 1. **TypeErrorType**
A categorization of all 17 semantic type errors detected by the module.

#### 2. **TypeChecker**
The main execution engine:
- `global_scope`: Reference to the shared symbol table.
- `current_scope_path`: Maintains the position within the scope tree.
- `context`: Tracks `current_fn_return_type`, `loop_depth`, and `switch_depth` for context-sensitive checks (like `break` or `return`).

## Detected Errors

### 1. **ErroneousVarDecl**
- **Description**: Occurs when a variable is declared with an invalid type, specifically `void`.
- **Example**: `void x; // ERROR: Variable cannot be void`

### 2. **FnCallParamCount**
- **Description**: Number of arguments in a function call does not match the declaration.
- **Example**: 
  ```c
  void func(int a, int b);
  func(10); // ERROR: Arg count mismatch
  ```

### 3. **FnCallParamType**
- **Description**: The type of an argument passed to a function does not match the parameter type.
- **Example**: `func(10, 3.14); // ERROR: Arg 2 type mismatch (Expected int, got float)`

### 4. **ErroneousReturnType**
- **Description**: Function returns a value that doesn't match its declared return type, or a non-void function is missing a return statement.
- **Example**: 
  ```c
  int test() { return 3.14; } // ERROR: Incorrect return type
  ```

### 5. **ExpressionTypeMismatch**
- **Description**: General type mismatch in assignments or arithmetic between incompatible types.
- **Example**: `int x = 3.14; // ERROR: Assignment type mismatch`

### 6. **ExpectedBooleanExpression**
- **Description**: A boolean value was expected but another type was found (e.g., in logical operations).

### 7. **ErroneousBreak**
- **Description**: `break` statement used outside of a `while`, `for`, or `switch` block.
- **Example**: `main { break; } // ERROR`

### 8. **NonBooleanCondStmt**
- **Description**: The condition in an `if`, `while`, or `for` statement is not a boolean.
- **Example**: `if (10) { ... } // ERROR: Condition must be boolean`

### 9. **AttemptedBoolOpOnNonBools**
- **Description**: Logical operators (`&&`, `||`) used on non-boolean types.
- **Example**: `int x = 10 && 20; // ERROR`

### 10. **AttemptedBitOpOnNonInt**
- **Description**: Bitwise operators (`&`, `|`, `^`) used on non-integer types (e.g., floats).

### 11. **AttemptedShiftOnNonInt**
- **Description**: Bitwise shift operators (`<<`, `>>`) used on non-integer types.

### 12. **AttemptedAddOpOnNonNumeric**
- **Description**: Arithmetic operators used on non-numeric types like `bool` or `void`.

### 13. **ReturnStmtInVoid**
- **Description**: A `return` statement with a value found inside a function declared as `void`.

### 14. **NonBooleanSwitchExpression**
- **Description**: Switch expression must be an `int` or `char`.

### 15. **InvalidCaseValueType**
- **Description**: The type of a `case` constant does not match the type of the `switch` expression.

### 16. **IncrementDecrementOnNonInt**
- **Description**: `++` or `--` applied to a non-integer variable (e.g., a float or bool).

### 17. **NotOnNonBool**
- **Description**: Unary NOT (`!`) applied to a non-boolean expression.

## Type System Rules

### Strict Typing
The compiler enforces strict type checking. There are **no implicit conversions** (coercions) between types (e.g., `int` to `float` must be explicit if the language supported casting).

### Numeric Types
- Arithmetic operations (`+`, `-`, `*`, `/`, `%`) are only allowed between two `int`s or two `float`s. 
- Mixed arithmetic (e.g., `int + float`) is treated as a `ExpressionTypeMismatch`.

### Boolean Logic
- Logical operators only accept `bool`.
- Comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=`) require both operands to be of the same type and return `bool`.

## Usage

```rust
use compiler::semantics::typeChecker::TypeChecker;

// scope_analyzer.get_global_scope() provides the symbol table
let mut type_checker = TypeChecker::new(scope_analyzer.get_global_scope());

match type_checker.check(&ast) {
    Ok(()) => println!("Type checking passed!"),
    Err(errors) => {
        for err in errors {
            eprintln!("[Type Error] {}", err.message);
        }
    }
}
```

## Implementation Details

### Type Inference
The `infer()` method recursively determines the resulting type of any expression tree. It handles:
- Literals (Int, Float, String, Char, Bool)
- Identifiers (via symbol table lookup)
- Binary/Unary operations
- Function calls (via return type lookup)

### Context Tracking
- `loop_depth`: Incremented when entering `while`/`for`, used to validate `break`.
- `switch_depth`: Incremented when entering `switch`, used to validate `break` and `case`.
- `current_fn_return_type`: Stores the expected return type of the function currently being analyzed.
- `found_return_stmt`: A flag used to ensure non-void functions actually return a value before they exit.