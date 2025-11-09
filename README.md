# Scope Analyzer Module

This module performs scope analysis on the parsed AST of a C‑style language. It validates symbol declarations, detects conflicting or duplicate definitions, resolves function prototypes vs. definitions, and verifies identifier usage across nested scopes.

## What It Does
* Builds a tree of scope frames (global + nested) while walking the AST.
* Tracks variables, functions, enums, and enum values via a `SymbolInfo` record.
* Distinguishes function prototypes (`isPrototype`) from full definitions.
* Enforces C‑style rules: matching signatures, no duplicate definitions, prototypes must not repeat.
* Reports scope errors with line/column + human readable messages.

## Core Data Structures
### SymbolInfo
Fields include: type, name, position, flags (`isFunction`, `isEnum`, `isEnumValue`, `isPrototype`), and parameter list (vector of `(TokenType, name)`). Only parameter types are used for signature comparison.

### ScopeFrame
Represents one lexical scope. Holds: symbol map, children, parent pointer, and a `level` (used for potential shadowing logic). New blocks / control structures / function bodies create child scopes.

### Lookup Strategy
Identifier resolution walks upward from the current scope to the global scope. First hit wins. This allows shadowing but currently does not emit a warning for it.

## Error Types
Defined in `compiler.h` (`ScopeErrorType`):
```cpp
// Scope Error Types and Their Purpose:
* UndeclaredVariableAccessed         // Variable used but never declared anywhere
* UndefinedFunctionCalled            // Function called but never declared/defined anywhere
* VariableRedefinition               // Variable declared multiple times with same name and type in same scope
* FunctionPrototypeRedefinition      // Function prototype (forward declaration) declared multiple times with identical signature
* ConflictingFunctionDefinition      // Function defined with different return type but same parameters
* ConflictingDeclaration             // Name used for different symbol types (e.g., variable vs function, or same function name with different return types)
* ParameterRedefinition              // Same parameter name used twice in one function's parameter list
* InvalidForwardReference            // Symbol referenced before declaration but exists later in code (currently used when symbol declared but out of scope)
* InvalidStorageClassUsage           // Declaration in wrong scope level (currently used for enums declared in non-global scope)
* EnumRedefinition                   // Enum type name declared multiple times
* EnumVariantRedefinition            // Same value name used twice within one enum definition
```

Each becomes a `ScopeError` with a formatted `message` string.

## Analysis Flow
1. Start at global scope.
2. For each top‑level AST node:
   * Variable declarations: ensure not already declared in current scope and not conflicting with function/enum names.
   * Function prototypes: add or error if a prototype with same signature exists; allow if a later definition matches.
   * Function definitions: check for existing symbol: 
      - If matching prototype in same scope: upgrade prototype to definition.
      - If definition already exists (same scope or parent): error.
      - If signature differs: error.
   * Enum declarations: ensure unique enum name; add each enum value as its own symbol (prevent conflicts).
3. Statement / block constructs (`if`, `while`, `for`, `switch`, `do‑while`, blocks) push a new scope before processing their bodies.
4. Identifier and call expressions resolve symbols and emit errors if undefined or not a function.

## Function Signature Matching
Only parameter types are compared (names ignored) for equality. Return type is currently not part of the signature comparison in this module.

## Public Entry Point
```cpp
void performScopeAnalysis(const vector<ASTPtr>& ast, const vector<Token>& tokens);
```
This builds a `ScopeAnalyzer`, runs analysis, prints all scope errors (if any), and terminates on failure (`exit(EXIT_FAILURE)`).

## Building
```bash
g++ -std=c++11 -o compiler main.cpp lexer.cpp parser.cpp scope.cpp
```
(Adjust flags and file list as needed.)

## Output Behavior
On success:
```
=== Scope Analysis Successful ===
No scope errors found.
```
On failure each error line begins with `[Scope Error]` followed by the message.

---

# Scope Analyzer - Error Detection Examples

## Overview
The Scope Analyzer performs semantic analysis to detect declaration conflicts, undefined symbols, and scope violations.

---

## Test Code
```cpp
include<main>
include"main.h"

enum Color { RED, GREEN, GREEN };
enum Color { BLUE };

int calculate(int x);
int calculate(int x);
float calculate(int x);

int calculate(int x, int x){
    processData();
    validateInput();
    return x;
}

int processData(int value){
    int result = value;
    return result + 2;
}

float processData(int value){
    int result = value;
    return result + 2;
}

main {
    enum Status {ACTIVE};

    int count = 42 + 5 / 8 * 10; 
    float pi = 3.14159;
    int total = calculate(2);

    print("Application Started");
    int status = total;
    int status = 5;
    status = undefined;
    
    float status = 3.2;
    
    return 0;
}
```

---

## Detected Errors

### 1. Enum Variant Redefinition
**Error:** `Enum variant redefinition: 'GREEN' at line: 4`
```cpp
enum Color { RED, GREEN, GREEN };  // ❌ GREEN appears twice
```

---

### 2. Enum Redefinition
**Error:** `Enum redefinition: 'Color' at line: 5`
```cpp
enum Color { RED, GREEN };
enum Color { BLUE };  // ❌ Color enum already defined
```

---

### 3. Function Prototype Redefinition
**Error:** `Function prototype redefinition: 'calculate' at line: 8`
```cpp
int calculate(int x);
int calculate(int x);  // ❌ Identical prototype declared twice
```

---

### 4. Conflicting Declaration (Prototype)
**Error:** `Conflicting declaration: 'calculate' at line: 9`
```cpp
int calculate(int x);
float calculate(int x);  // ❌ Different return type, same parameters
```

---

### 5. Conflicting Function Definition
**Error:** `Conflicting function definition: 'calculate' at line: 11`
```cpp
int calculate(int x);
int calculate(int x, int x){ ... }  // ❌ Signature mismatch with prototype
```

---

### 6. Parameter Redefinition
**Error:** `Parameter redefinition: 'x' at line: 11`
```cpp
int calculate(int x, int x){  // ❌ Parameter 'x' used twice
    return x;
}
```

---

### 7. Invalid Forward Reference
**Error:** `Invalid forward reference: 'processData' at line: 12`
```cpp
int calculate(int x, int x){
    processData();  // ❌ Called before definition
    return x;
}

int processData(int value){ ... }  // Defined later
```

---

### 8. Undefined Function Called
**Error:** `Undefined function called: 'validateInput' at line: 13`
```cpp
int calculate(int x, int x){
    validateInput();  // ❌ Never declared or defined anywhere
    return x;
}
```

---

### 9. Conflicting Function Definition (Return Type)
**Error:** `Conflicting function definition: 'processData' at line: 22`
```cpp
int processData(int value){
    return value + 2;
}

float processData(int value){  // ❌ Same parameters, different return type
    return value + 2;
}
```

---

### 10. Invalid Storage Class Usage
**Error:** `Invalid storage class usage for: 'Status' at line: 28`
```cpp
main {
    enum Status {ACTIVE};  // ❌ Enums must be declared at global scope
}
```

---

### 11. Variable Redefinition
**Error:** `Variable redefinition: 'status' at line: 36`
```cpp
int status = total;
int status = 5;  // ❌ Same name, same type in same scope
```

---

### 12. Undeclared Variable Accessed
**Error:** `Undeclared variable accessed: 'undefined' at line: 37`
```cpp
status = undefined;  // ❌ 'undefined' never declared
```

---

### 13. Conflicting Declaration (Variable)
**Error:** `Conflicting declaration: 'status' at line: 39`
```cpp
int status = 5;
float status = 3.2;  // ❌ Same name, different type in same scope
```

---

## Error Summary

| Category | Error Count |
|----------|-------------|
| Enum Errors | 2 |
| Function Errors | 6 |
| Variable Errors | 3 |
| Undefined Symbols | 2 |
| **Total Errors** | **13** |

---

## Analysis Result
```
Scope analysis failed with 13 error(s)
```

The analyzer successfully detected all semantic violations including:
- ✓ Duplicate declarations
- ✓ Type conflicts  
- ✓ Forward references
- ✓ Scope violations
- ✓ Undefined symbols