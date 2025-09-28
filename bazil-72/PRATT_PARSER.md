# Custom Compiler – Pratt Parser

This README is of **pratt-parser**'s implementation describing' the features, grammar, and operator precedence
of the toy language implemented in your compiler front-end.

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

------------------------------------------------------------------------

## Grammar (BNF)

    <program> ::= <top-level>* EOF

    <top-level> ::= <include-stmt>
                  | <function-decl>
                  | <main-decl>
                  | <statement>

    <include-stmt> ::= "include" "<" "main" ">"

    <function-decl> ::= <type> IDENTIFIER "(" <param-list>? ")" <block>

    <param-list> ::= <param> ("," <param>)*
    <param> ::= <type> IDENTIFIER

    <main-decl> ::= "main" <block>

    <statement> ::= <var-decl>
                  | <print-stmt>
                  | <if-stmt>
                  | <while-stmt>
                  | <return-stmt>
                  | <block-stmt>
                  | <expression-stmt>

    <var-decl> ::= <type> IDENTIFIER ("=" <expression>)? ";" 

    <print-stmt> ::= "print" "(" <arg-list>? ")" ";"

    <if-stmt> ::= "if" "(" <expression> ")" <block-stmt> ("else" <block-stmt>)?

    <while-stmt> ::= "while" "(" <expression> ")" <block-stmt>

    <return-stmt> ::= "return" <expression>? ";" 

    <block-stmt> ::= <block>
    <block> ::= "{" <statement>* "}" 

    <expression-stmt> ::= <expression> ";" 

    <arg-list> ::= <expression> ("," <expression>)*

    <expression> ::= <assignment>

    <assignment> ::= IDENTIFIER "=" <expression>
                   | <logical-or>

    <logical-or> ::= <logical-and> ( "||" <logical-and> )*

    <logical-and> ::= <equality> ( "&&" <equality> )*

    <equality> ::= <comparison> ( ("==" | "!=") <comparison> )*

    <comparison> ::= <term> ( ("<" | ">" | "<=" | ">=") <term> )*

    <term> ::= <factor> ( ("+" | "-") <factor> )*

    <factor> ::= <unary> ( ("*" | "/" | "%") <unary> )*

    <unary> ::= ("-" | "!") <unary>
              | <postfix>

    <postfix> ::= IDENTIFIER "(" <arg-list>? ")"
                | <primary>

    <primary> ::= INTLIT | FLOATLIT | STRINGLIT | CHARLIT | BOOLLIT | IDENTIFIER | "(" <expression> ")"

    <type> ::= "int" | "float" | "double" | "char" | "bool" | "void"

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
