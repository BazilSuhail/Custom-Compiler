
```mathematica
program         ::= declaration* EOF ;

declaration     ::= includeDirective
                  | defineDirective
                  | enumDecl
                  | varDecl
                  | fnDecl
                  | statement ;

varDecl         ::= typeSpec IDENTIFIER ( "=" expression )? ";" ;

fnDecl          ::= typeSpec IDENTIFIER "(" parameters? ")" block ;

parameters      ::= parameter ( "," parameter )* ;

parameter       ::= typeSpec IDENTIFIER ;

typeSpec        ::= typeQualifier* typeModifier* baseType ;

typeQualifier   ::= "const" | "static" ;

typeModifier    ::= "signed" | "unsigned" | "short" | "long" ;

baseType        ::= "int" | "float" | "double" | "char" | "bool" | "void" ;

includeDirective ::= "#include" ( "<" IDENTIFIER ( "." IDENTIFIER )? ">" 
                                | STRINGLIT ) ;

defineDirective ::= "#define" IDENTIFIER ( "(" parameterList? ")" )? macroBody? ;

parameterList   ::= IDENTIFIER ( "," IDENTIFIER )* ;

macroBody       ::= ( IDENTIFIER | literal | operator )* ;

enumDecl        ::= "enum" IDENTIFIER "{" enumValueList? "}" ";" ;

enumValueList   ::= IDENTIFIER ( "," IDENTIFIER )* ;

statement       ::= returnStmt
                  | ifStmt
                  | whileStmt
                  | doWhileStmt
                  | forStmt
                  | switchStmt
                  | breakStmt
                  | continueStmt
                  | assignStmt
                  | exprStmt ;

assignStmt      ::= IDENTIFIER "=" expression ";" ;

returnStmt      ::= "return" expression? ";" ;

breakStmt       ::= "break" ";" ;

continueStmt    ::= "continue" ";" ;

ifStmt          ::= "if" "(" expression ")" block ("else" block)? ;

whileStmt       ::= "while" "(" expression ")" block ;

doWhileStmt     ::= "do" block "while" "(" expression ")" ";" ;

forStmt         ::= "for" "(" ( varDecl | exprStmt | ";" ) 
                        expression? ";" 
                        expression? ")" block ;

switchStmt      ::= "switch" "(" expression ")" "{" caseStmt* "}" ;

caseStmt        ::= ( "case" expression ":" | "default" ":" ) declaration* ;

block           ::= "{" declaration* "}" ;

exprStmt        ::= expression ";" ;

expression      ::= assignment ;

assignment      ::= logicalOr ( "=" assignment )? ;

logicalOr       ::= logicalAnd ( "||" logicalAnd )* ;

logicalAnd      ::= bitwiseOr ( "&&" bitwiseOr )* ;

bitwiseOr       ::= bitwiseXor ( "|" bitwiseXor )* ;

bitwiseXor      ::= bitwiseAnd ( "^" bitwiseAnd )* ;

bitwiseAnd      ::= equality ( "&" equality )* ;

equality        ::= comparison ( ( "==" | "!=" ) comparison )* ;

comparison      ::= shift ( ( "<" | ">" | "<=" | ">=" ) shift )* ;

shift           ::= term ( ( "<<" | ">>" ) term )* ;

term            ::= factor ( ( "+" | "-" ) factor )* ;

factor          ::= unary ( ( "*" | "/" | "%" ) unary )* ;

unary           ::= ( "+" | "-" | "!" | "~" | "++" | "--" ) unary
                  | postfix ;

postfix         ::= call ( "++" | "--" )? ;

call            ::= primary ( "(" arguments? ")" )? ;

arguments       ::= expression ( "," expression )* ;

primary         ::= literal
                  | IDENTIFIER  
                  | "(" expression ")"
                  | ( "++" | "--" ) unary ;

literal         ::= INTLIT | FLOATLIT | BOOLLIT | STRINGLIT | CHARLIT ;

operator        ::= "+" | "-" | "*" | "/" | "%" | "=" | "==" | "!=" | "<" | ">" 
                  | "<=" | ">=" | "&&" | "||" | "!" | "&" | "|" | "^" | "~" 
                  | "<<" | ">>" | "++" | "--" | "(" | ")" | "{" | "}" | ";" 
                  | "," | "." | "[" | "]" ;
```

## Grammar Features

### ✨ New Features Added:

#### **1. Enhanced Type System**
- **Type Qualifiers**: `const`, `static`
- **Type Modifiers**: `signed`, `unsigned`, `short`, `long`
- **Complex Type Combinations**: Support for combinations like `static const unsigned long int`

#### **2. Complete Operator Support**
- **Arithmetic**: `+`, `-`, `*`, `/`, `%` (including modulo)
- **Logical**: `&&`, `||`, `!` (with proper precedence)
- **Bitwise**: `&`, `|`, `^`, `~`, `<<`, `>>` (all bitwise operations)
- **Relational**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Assignment**: `=`
- **Increment/Decrement**: `++`, `--` (both prefix and postfix)

#### **3. Extended Control Flow**
- **Loops**: `while`, `do-while`, `for` loops
- **Conditionals**: `if-else`, `switch-case-default`
- **Jump Statements**: `return`, `break`, `continue`

#### **4. Preprocessor Directives**
- **Include Directives**: `#include <header.h>`, `#include "header.h"`
- **Define Macros**: 
  - **Object-like**: `#define PI 3.14159`
  - **Function-like**: `#define MAX(a,b) ((a)>(b)?(a):(b))`

#### **5. Enum Declarations**
- **Enum Support**: `enum Color { RED, GREEN, BLUE };`
- **Named Enumerations**: Support for enum types with named values

#### **6. Operator Precedence (Highest to Lowest)**
1. **Postfix**: `()`, `++`, `--`
2. **Unary**: `+`, `-`, `!`, `~`, `++`, `--` (prefix)
3. **Multiplicative**: `*`, `/`, `%`
4. **Additive**: `+`, `-`
5. **Shift**: `<<`, `>>`
6. **Relational**: `<`, `>`, `<=`, `>=`
7. **Equality**: `==`, `!=`
8. **Bitwise AND**: `&`
9. **Bitwise XOR**: `^`
10. **Bitwise OR**: `|`
11. **Logical AND**: `&&`
12. **Logical OR**: `||`
13. **Assignment**: `=`

### 📝 Grammar Notes:
- All statements require blocks (`{}`) for consistency
- Switch cases support fall-through behavior
- Type specifications are flexible and composable
- Expression precedence follows C language standards
- Preprocessor directives are parsed at the declaration level
- Include directives support both system (`<>`) and user (`""`) includes
- Define macros support both object-like and function-like definitions
- Enum declarations require semicolon termination
- Enum values are comma-separated identifiers 
