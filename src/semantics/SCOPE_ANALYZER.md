# Scope Analyzer Documentation

## Overview
The scope analyzer is a semantic analysis module that validates symbol declarations and usage in the custom compiler. It builds a symbol table and detects 11 different types of scope-related errors.

## Architecture

### Two-Pass Analysis
1. **First Pass (Declaration Collection)**: Collects all top-level declarations for forward reference checking
2. **Second Pass (Validation)**: Performs actual scope analysis with error detection

### Key Components

#### 1. **SymbolInfo**
Stores information about each symbol:
- `symbol_type`: The type of the symbol (TypeNode)
- `name`: Symbol name
- `line`, `column`: Location in source code
- `is_function`: Whether it's a function
- `is_enum`: Whether it's an enum type
- `is_enum_value`: Whether it's an enum variant
- `is_prototype`: Whether it's a function prototype (forward declaration)
- `params`: Function parameters (if applicable)

#### 2. **ScopeFrame**
Represents a lexical scope:
- `symbols`: HashMap of symbols in this scope
- `children`: Nested child scopes
- `level`: Scope depth (0 = global)

#### 3. **ScopeAnalyzer**
Main analyzer with:
- `global_scope`: Root scope frame
- `current_scope_stack`: Navigation through scope tree
- `errors`: Collected scope errors
- `all_declared_symbols`: All declarations for forward reference checking

## Detected Errors

### 1. **UndeclaredVariableAccessed**
- **Description**: Variable used but never declared anywhere
- **Example**:
  ```c
  int x = unknownVar + 5;  // ERROR: unknownVar not declared
  ```

### 2. **UndefinedFunctionCalled**
- **Description**: Function called but never declared/defined anywhere
- **Example**:
  ```c
  int result = nonExistentFunc(10);  // ERROR
  ```

### 3. **VariableRedefinition**
- **Description**: Variable declared multiple times in same scope
- **Example**:
  ```c
  int x = 10;
  int x = 20;  // ERROR: redefinition
  ```

### 4. **FunctionPrototypeRedefinition**
- **Description**: Function prototype declared multiple times with identical signature
- **Example**:
  ```c
  void helper(int x);
  void helper(int x);  // ERROR: duplicate prototype
  ```

### 5. **ConflictingFunctionDefinition**
- **Description**: Function defined with different signature than existing declaration
- **Example**:
  ```c
  void process(int a);
  void process(float a) { }  // ERROR: different parameter type
  ```

### 6. **ConflictingDeclaration**
- **Description**: Name used for different symbol types
- **Example**:
  ```c
  enum Color { Red };
  int Color = 10;  // ERROR: Color is already an enum
  ```

### 7. **ParameterRedefinition**
- **Description**: Same parameter name used twice in function
- **Example**:
  ```c
  int func(int x, float x) { }  // ERROR: duplicate parameter
  ```

### 8. **InvalidForwardReference**
- **Description**: Symbol referenced before declaration (but exists later)
- **Example**:
  ```c
  int x = laterVar;  // ERROR: forward reference
  int laterVar = 10;
  ```

### 9. **InvalidStorageClassUsage**
- **Description**: Declaration in wrong scope level (e.g., enum in non-global scope)
- **Example**:
  ```c
  main {
      enum LocalEnum { A, B };  // ERROR: enums must be global
  }
  ```

### 10. **EnumRedefinition**
- **Description**: Enum type declared multiple times
- **Example**:
  ```c
  enum Color { Red };
  enum Color { Blue };  // ERROR: redefinition
  ```

### 11. **EnumVariantRedefinition**
- **Description**: Same value name used twice within one enum
- **Example**:
  ```c
  enum Status { Active, Inactive, Active };  // ERROR: duplicate Active
  ```

## Scope Rules

### Variable Scoping
- Variables are scoped to the block they're declared in
- Inner scopes can shadow outer scope variables (allowed)
- Variables must be declared before use (no forward references)

### Function Scoping
- Functions can have prototypes (forward declarations)
- Prototypes must match definitions exactly (C-style: only parameter types matter)
- Function definitions can replace prototypes
- Multiple definitions of same function are not allowed

### Enum Scoping
- Enums must be declared at global scope only
- Enum type names and variant names are added to symbol table
- Enum variants are treated as integer constants

### Parameter Scoping
- Parameters are scoped to the function body
- Parameter names must be unique within a function
- Parameters are added to function's local scope

## Usage

```rust
use compiler::semantics::scope::ScopeAnalyzer;

let mut scope_analyzer = ScopeAnalyzer::new();
match scope_analyzer.analyze(&ast) {
    Ok(()) => {
        println!("Scope analysis passed!");
        // Access symbol table: scope_analyzer.get_global_scope()
    }
    Err(errors) => {
        for error in errors {
            eprintln!("{}", error.message);
        }
    }
}
```

## Symbol Table Access

After successful analysis, the symbol table can be accessed:

```rust
let global_scope = scope_analyzer.get_global_scope();
// Navigate through scopes and symbols
for (name, symbol) in &global_scope.symbols {
    println!("Symbol: {} at line {}", name, symbol.line);
}
```

## Test Files

- **test_clean.txt**: Valid program with no scope errors
- **test_scope_errors.txt**: Comprehensive test of all error types
- **test_input.txt**: Sample program with one error (parameter redefinition)

## Future Enhancements

The symbol table built by this analyzer will be used by:
1. **Type Checker**: To validate type compatibility and operations
2. **Code Generator**: To generate correct variable references and function calls
3. **Optimizer**: To perform dead code elimination and other optimizations
