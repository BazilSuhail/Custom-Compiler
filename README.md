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
* UndeclaredVariableAccessed
* UndefinedFunctionCalled
* VariableRedefinition
* FunctionPrototypeRedefinition
* ConflictingFunctionDefinition
* ConflictingDeclaration (name used for different kinds/signatures)
* ParameterRedefinition (duplicate parameter name in same function)
* InvalidForwardReference (reserved for future use)
* InvalidStorageClassUsage (reserved for future use)
* EnumRedefinition
* EnumVariantRedefinition

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

## Usage Example
```cpp
// After lexing and parsing:
auto tokens = lexAndDumpToFile("tester/sample.txt", "tester/tokens.txt");
auto ast = parseFromFile(tokens);
performScopeAnalysis(ast, tokens); // exits on error, prints success message otherwise
```

### Sample Source Illustrating Errors
```c
int foo(int x);        // prototype
int foo(int y) {       // OK: matches prototype
    return y;          
}
int foo(int z) {       // Error: conflicting function definition (duplicate definition)
    return z;          
}

int foo(float z);      // Error: conflicting declaration (different signature)

int a = 1;
int a = 2;             // Error: variable redefinition

enum Color { RED, GREEN, RED }; // Error: enum variant redefinition (RED twice)
```

### Example Identifier Resolution
```c
int a = 10;
void f() {
    int a = 3;   // shadows outer a (allowed – no warning yet)
    int b = a;   // resolves to inner a
}
int b = a;       // resolves to global a
```

## Integration Order
1. Lexing (`lexer.cpp`)
2. Parsing (`parser.cpp` → AST)
3. Scope analysis (`scope.cpp`)
4. (Future) Type checking / semantic passes

Run sequence in `main.cpp` already follows this order.

## Performance Notes
Complexity is roughly O(N * D) where N = number of AST nodes and D = maximum scope depth; typical code keeps D small. Symbol lookups are O(1) per scope via `unordered_map`.

## Limitations / Future Work
* No warnings for shadowing yet (level field could enable this).
* `InvalidForwardReference` and `InvalidStorageClassUsage` are placeholders.
* Does not verify return type consistency with call sites.
* No type compatibility checking for assignments or expressions.
* Function overloading by parameter type beyond exact match isn't supported (first conflicting signature wins an error).

## Extending The Analyzer
Add new checks by:
1. Introducing a new `ScopeErrorType` value.
2. Adding handling logic inside `ScopeAnalyzer::analyzeNode` or specific helpers.
3. Emitting errors through `addError(...)`.

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
