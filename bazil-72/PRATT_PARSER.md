# Custom Compiler – Pratt Parser

This README is of **pratt-parser**'s implementation describing' the features, grammar, and operator precedence
of the toy language implemented in your compiler front-end.

---

## Features Implemented

-   **Include directive**
    -   Supports `include<main>` at the top of the file.
-   **Main entry point**
    -   Declared with `main { ... }`.
-   **Functions**
    -   Declaration syntax: `<type> name(type param, ...) { ... }`
    -   Return statements with optional expression.
-   **Statements**
    -   Variable declarations with semicolons.
    -   Assignments with semicolons.
    -   Print statements with multiple arguments.
    -   If/Else statements (brace-delimited blocks only).
    -   While loops (brace-delimited blocks only).
    -   Do-While loops (brace-delimited blocks only).
    -   For loops (brace-delimited blocks only).
    -   Switch statements with case blocks and optional default.
    -   Break statements (simple placeholder).
    -   Blocks `{ ... }` containing multiple statements.
-   **Expressions**
    -   Literals: int, float, char, string, bool.
    -   Identifiers.
    -   Binary operations (+, -, \*, /, %, relational, equality,
        logical).
    -   Unary operations (-, !).
    -   Function calls (`f(expr, ...)`) with identifier callees only.
-   **Semantics / Constraints**
    -   Each statement must end with `;` unless it is a block.
    -   Blocks must close with `}`. If EOF occurs first, error is
        reported as *Unclosed block*.
    -   Only identifiers can be used as function call targets.
        Expressions like `(x+1)(2)` are rejected.
    -   Assignment requires an identifier on the left-hand side. Chained
        assignment is not supported.
    -   `print` is a reserved statement, not a general function.
    -   Do-While loops execute body at least once, condition checked after execution.
    -   For loops support initialization, condition, and update expressions.
    -   Switch statements evaluate expression once and match against case values.
    -   Case statements execute block directly after value without colon separator.
    -   Default block executes if no case matches (optional).
    -   Break statements provide early exit from loops/switches.

------------------------------------------------------------------------

## Grammar (BNF)

```

// Program Structure
program ::= include_stmt* (function_decl | main_decl)*

// Include Statements
include_stmt ::= "include" ("<main>" | string_literal | identifier) ";"

// Function Declarations
function_decl ::= type_specifier identifier "(" parameter_list? ")" block_stmt
parameter_list ::= (type_specifier identifier) ("," type_specifier identifier)*

// Main Declaration
main_decl ::= "main" block_stmt

// Type Specifiers
type_specifier ::= "int" | "float" | "double" | "char" | "bool" | "void"

// Statements
statement ::= 
    | var_decl
    | expr_stmt
    | block_stmt
    | if_stmt
    | while_stmt
    | do_while_stmt
    | for_stmt
    | switch_stmt
    | return_stmt
    | print_stmt
    | break_stmt

// Variable Declaration
var_decl ::= type_specifier identifier ("=" expression)? ";"

// Expression Statement
expr_stmt ::= expression ";"

// Block Statement
block_stmt ::= "{" statement* "}"

// If Statement
if_stmt ::= "if" "(" expression ")" block_stmt ("else" block_stmt)?

// While Loop
while_stmt ::= "while" "(" expression ")" block_stmt

// Do-While Loop
do_while_stmt ::= "do" block_stmt "while" "(" expression ")" ";"

// For Loop
for_stmt ::= "for" "(" (var_decl | expression? ";") expression? ";" expression? ")" block_stmt

// Switch Statement
switch_stmt ::= "switch" "(" expression ")" "{" case_block* default_block? "}"
case_block ::= "case" expression block_stmt
default_block ::= "default" block_stmt

// Return Statement
return_stmt ::= "return" expression? ";"

// Print Statement
print_stmt ::= "print" "(" expression_list? ")" ";"

// Break Statement
break_stmt ::= "break" ";"

// Expressions
expression ::= assignment_expr

assignment_expr ::= logical_or_expr ("=" assignment_expr)?

logical_or_expr ::= logical_and_expr ("||" logical_and_expr)*

logical_and_expr ::= equality_expr ("&&" equality_expr)*

equality_expr ::= comparison_expr (("==" | "!=") comparison_expr)*

comparison_expr ::= term_expr (("<" | ">" | "<=" | ">=") term_expr)*

term_expr ::= factor_expr (("+" | "-") factor_expr)*

factor_expr ::= unary_expr (("*" | "/" | "%") unary_expr)*

unary_expr ::= ("-" | "!") unary_expr | call_expr

call_expr ::= primary_expr ("(" expression_list? ")")?

primary_expr ::= 
    | literal
    | identifier
    | "(" expression ")"
    | identifier "[" expression "]"  // Array access (if supported)

expression_list ::= expression ("," expression)*

// Literals
literal ::= 
    | integer_literal
    | float_literal
    | string_literal
    | char_literal
    | bool_literal

integer_literal ::= [0-9]+
float_literal ::= [0-9]+\.[0-9]+
string_literal ::= "\"" .*? "\""
char_literal ::= "'" . "'"
bool_literal ::= "true" | "false"

// Identifier
identifier ::= [a-zA-Z_][a-zA-Z0-9_]*

```

---

## Operator Precedence and Associativity

| Level | Operators | Associativity | Notes |
|-------|-----------|---------------|-------|
| 9 | function call `()` | left (postfix) | Callee must be IDENTIFIER only |
| 8 | unary `-`, `!` | right | Prefix operators |
| 7 | `*`, `/`, `%` | left | Multiplicative |
| 6 | `+`, `-` | left | Additive |
| 5 | `<`, `>`, `<=`, `>=` | left | Relational |
| 4 | `==`, `!=` | left | Equality |
| 3 | `&&` | left | Logical AND |
| 2 | `||` | left | Logical OR |
| 1 | `=` | right | Assignment, left side must be IDENTIFIER |

## Error Handling

-   **MissingSemicolon**: Thrown if a statement misses its semicolon.
-   **UnclosedBlock**: Thrown if EOF occurs before a `}`.
-   **InvalidCallTarget**: Thrown if a function call target is not an
    identifier.
-   **UnexpectedToken**: Thrown when an unexpected token appears.

------------------------------------------------------------------------

## Examples

**Valid program:**

    include<main>

    int add(int a, int b) {
        return a + b;
    }

    main {
        int x = 10;
        print("Hello World!\n");

        if (x > 5) {
            x = add(x, 2);
        }

        while (x > 0) {
            print("x = ", x, "\n");
            x = x - 1;
        }

        { int z = 5; }
        return 0;
    }

**Invalid program (missing `;`):**

    int x = 10
    print(x);

Error: `Missing semicolon at end of statement`.

**Invalid call target:**

    (x+1)(2);

Error: `Invalid function call target: only identifiers can be called`.
