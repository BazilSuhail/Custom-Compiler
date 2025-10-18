# C-Style Language Parser

## Overview
This project is a syntax parser for a custom C-like programming language. It uses [Bison](https://www.gnu.org/software/bison/) to generate a parser from a grammar file (`parser.y`) and a custom C++ lexer (`lexer.cpp`) to tokenize input. The parser reads code from a `test.code` file, checks for syntactic correctness, and reports success or errors. It’s designed to run on Windows using the MSYS2 environment.

The language supports:
- **Data types**: `int`, `float`, `double`, `char`, `bool`, `void`
- **Control structures**: `if-else`, `while`, `do-while`, `for`, `switch-case`
- **Declarations**: variables, constants, enums
- **Functions**: including a `main` block and return statements
- **Expressions**: arithmetic (`+`, `-`, `*`, `/`, `%`), logical (`&&`, `||`, `!`), relational (`==`, `!=`, `<`, `>`, `<=`, `>=`)
- **Other**: `print` statements, `break`, single/multi-line comments

This is a syntactic parser only—it validates structure but doesn’t perform semantic checks (e.g., type correctness) or execute code.

## Installation
To build and run this project, you’ll need MSYS2, a Windows environment for compiling native software with Unix-like tools.

1. **Install MSYS2**:
   - Download the installer from [https://www.msys2.org/](https://www.msys2.org/) (e.g., `msys2-x86_64-latest.exe`).
   - Run the installer and choose a folder (e.g., `C:\msys64`) with:
     - Short, ASCII-only path
     - NTFS volume, no spaces, accents, symlinks, or network drives
   - Complete the installation and launch the MSYS2 UCRT64 terminal.

2. **Update MSYS2 and install tools**:
   In the UCRT64 terminal, run:
   ```bash
   pacman -Syu
   pacman -S make mingw-w64-ucrt-x86_64-gcc bison flex
   ```
   - `pacman -Syu`: Updates the MSYS2 system.
   - `make`: Optional for build automation.
   - `mingw-w64-ucrt-x86_64-gcc`: Provides `gcc` and `g++` for compiling.
   - `bison`: Generates the parser from `parser.y`.
   - `flex`: Included for potential lexer generation (though this project uses a custom lexer).

3. **Verify installation**:
   Check that tools are installed:
   ```bash
   gcc --version
   bison --version
   flex --version
   ```

## How to Run the Project
1. **Set up project files**:
   - Clone or download the project containing `parser.y` and `lexer.cpp`.
   - Create a `test.code` file with your code (see examples below).

2. **Navigate to project directory**:
   In the MSYS2 UCRT64 terminal:
   ```bash
   cd /path/to/your/bison_project
   ```

3. **Generate the parser**:
   Run Bison to create `parser.tab.cpp` and `parser.tab.hpp`:
   ```bash
   bison -d -o parser.tab.cpp parser.y
   ```
   - `-d`: Generates a header file (`parser.tab.hpp`).
   - `-o parser.tab.cpp`: Specifies the output file.
   - Note: You may see a warning about shift/reduce conflicts. To debug, rerun with `-Wcounterexamples`.

4. **Compile the project**:
   Compile the lexer and parser into an executable:
   ```bash
   g++ lexer.cpp parser.tab.cpp -o parser.exe
   ```

5. **Run the parser**:
   Execute the parser on `test.code`:
   ```bash
   ./parser.exe
   ```
   - If the syntax is valid, it outputs: `Parsing completed successfully.`
   - If invalid, it shows errors, e.g.:
     ```
     Starting parse...
     Syntax Error: syntax error
     Parsing failed.
     ```

## Grammar
The grammar is defined in `parser.y`, which specifies the language’s syntax rules. Below is the complete grammar for reference:

```yacc
%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

// Forward declarations
void yyerror(const char *s);
int yylex();

// Provided by lexer.cpp
extern void initLexer(const string& code);
%}

%union {
    char* str;
}

/* === Tokens === */
%token <str> T_IDENTIFIER T_INTLIT T_FLOATLIT T_STRINGLIT T_CHARLIT T_BOOLLIT
%token T_INT T_FLOAT T_DOUBLE T_CHAR T_BOOL T_VOID
%token T_IF T_ELSE T_WHILE T_DO T_FOR T_SWITCH T_CASE T_DEFAULT T_BREAK T_RETURN T_PRINT T_CONST T_ENUM T_MAIN
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET
%token T_SEMICOLON T_COMMA T_DOT T_COLON
%token T_STRING
%token T_SINGLE_COMMENT T_MULTI_COMMENT
%token T_ASSIGNOP T_EQUALOP T_NE T_LT T_GT T_LE T_GE
%token T_PLUS T_MINUS T_MULTIPLY T_DIVIDE T_MODULO
%token T_INCREMENT T_DECREMENT
%token T_AND T_OR T_NOT
%token T_ERROR T_EOF

%start program

%%

/* === Top-Level Grammar === */
program
    : include_main declaration_list
    ;

include_main
    : T_IDENTIFIER T_LT T_MAIN T_GT
    | /* empty */
    ;

declaration_list
    : declaration_list declaration
    | /* empty */
    ;

declaration
    : function_decl
    | main_decl
    | statement
    ;

/* === Function and Main === */
function_decl
    : type T_IDENTIFIER T_LPAREN opt_param_list T_RPAREN block
    ;

main_decl
    : T_MAIN block
    ;

opt_param_list
    : param_list
    | /* empty */
    ;

param_list
    : param
    | param_list T_COMMA param
    ;

param
    : type T_IDENTIFIER
    ;

type
    : T_INT
    | T_FLOAT
    | T_DOUBLE
    | T_CHAR
    | T_BOOL
    | T_VOID
    ;

/* === Statements === */
statement
    : const_decl
    | enum_decl
    | var_decl
    | print_stmt
    | if_stmt
    | while_stmt
    | do_while_stmt
    | for_stmt
    | switch_stmt
    | return_stmt
    | block
    | expr_stmt
    | T_BREAK T_SEMICOLON
    ;

/* === Const / Enum / Var Declarations === */
const_decl
    : T_CONST type T_IDENTIFIER T_ASSIGNOP expression T_SEMICOLON
    ;

enum_decl
    : T_ENUM T_IDENTIFIER T_LBRACE enum_member_list opt_comma T_RBRACE T_SEMICOLON
    ;

opt_comma
    : T_COMMA
    | /* empty */
    ;

enum_member_list
    : enum_member
    | enum_member_list T_COMMA enum_member
    ;

enum_member
    : T_IDENTIFIER
    | T_IDENTIFIER T_ASSIGNOP expression
    ;

var_decl
    : type T_IDENTIFIER opt_init T_SEMICOLON
    ;

opt_init
    : T_ASSIGNOP expression
    | /* empty */
    ;

/* === Print / Control Flow === */
print_stmt
    : T_PRINT T_LPAREN opt_expression_list T_RPAREN T_SEMICOLON
    ;

if_stmt
    : T_IF T_LPAREN expression T_RPAREN block opt_else
    ;

opt_else
    : T_ELSE block
    | /* empty */
    ;

while_stmt
    : T_WHILE T_LPAREN expression T_RPAREN block
    ;

do_while_stmt
    : T_DO block T_WHILE T_LPAREN expression T_RPAREN T_SEMICOLON
    ;

for_stmt
    : T_FOR T_LPAREN opt_for_init T_SEMICOLON opt_expression T_SEMICOLON opt_expression T_RPAREN block
    ;

opt_for_init
    : for_init
    | /* empty */
    ;

for_init
    : type T_IDENTIFIER opt_init
    | expression
    ;

switch_stmt
    : T_SWITCH T_LPAREN expression T_RPAREN T_LBRACE case_blocks opt_default_block T_RBRACE
    ;

case_blocks
    : /* empty */
    | case_blocks case_block
    ;

case_block
    : T_CASE expression block
    ;

opt_default_block
    : T_DEFAULT block
    | /* empty */
    ;

return_stmt
    : T_RETURN opt_expression T_SEMICOLON
    ;

opt_expression
    : expression
    | /* empty */
    ;

/* === Blocks and Expressions === */
block
    : T_LBRACE statement_list T_RBRACE
    ;

statement_list
    : /* empty */
    | statement_list statement
    ;

expr_stmt
    : expression T_SEMICOLON
    ;

opt_expression_list
    : expression_list
    | /* empty */
    ;

expression_list
    : expression
    | expression_list T_COMMA expression
    ;

/* === Expressions === */
expression
    : assign_expr
    ;

assign_expr
    : logical_or_expr
    | logical_or_expr T_ASSIGNOP assign_expr
    ;

logical_or_expr
    : logical_and_expr
    | logical_or_expr T_OR logical_and_expr
    ;

logical_and_expr
    : equality_expr
    | logical_and_expr T_AND equality_expr
    ;

equality_expr
    : relational_expr
    | equality_expr T_EQUALOP relational_expr
    | equality_expr T_NE relational_expr
    ;

relational_expr
    : additive_expr
    | relational_expr T_LT additive_expr
    | relational_expr T_GT additive_expr
    | relational_expr T_LE additive_expr
    | relational_expr T_GE additive_expr
    ;

additive_expr
    : multiplicative_expr
    | additive_expr T_PLUS multiplicative_expr
    | additive_expr T_MINUS multiplicative_expr
    ;

multiplicative_expr
    : unary_expr
    | multiplicative_expr T_MULTIPLY unary_expr
    | multiplicative_expr T_DIVIDE unary_expr
    | multiplicative_expr T_MODULO unary_expr
    ;

unary_expr
    : postfix_expr
    | T_MINUS unary_expr
    | T_NOT unary_expr
    | T_INCREMENT T_IDENTIFIER
    | T_DECREMENT T_IDENTIFIER
    ;

postfix_expr
    : primary_expr
    | postfix_expr T_LPAREN opt_expression_list T_RPAREN
    | T_IDENTIFIER T_INCREMENT
    | T_IDENTIFIER T_DECREMENT
    ;

primary_expr
    : T_INTLIT
    | T_FLOATLIT
    | T_STRINGLIT
    | T_CHARLIT
    | T_BOOLLIT
    | T_IDENTIFIER
    | T_LPAREN expression T_RPAREN
    ;

%%

/* === Error and Entry Point === */
void yyerror(const char *s) {
    cerr << "Syntax Error: " << s << endl;
}
```

## Examples
### Valid Code Snippets
Save these in `test.code` to test successful parsing:
1. **Variable declaration and print**:
   ```
   int main() {
       int x = 10;
       print(x);
   }
   ```

2. **If-else statement**:
   ```
   int main() {
       int x = 5;
       if (x > 0) {
           print("Positive");
       } else {
           print("Non-positive");
       }
   }
   ```

3. **While loop**:
   ```
   int main() {
       int i = 0;
       while (i < 3) {
           print(i);
           i++;
       }
   }
   ```

4. **Function definition**:
   ```
   int square(int x) {
       return x * x;
   }
   ```

### Invalid Code Snippets
These will cause parsing errors:
1. **Missing semicolon**:
   ```
   int main() {
       int x = 5
   }
   ```

2. **Unclosed brace**:
   ```
   int main() {
       if (true) {
           print("Test")
   }
   ```

3. **Invalid operator**:
   ```
   int main() {
       x = 5 &&& 3;
   }
   ```

Try creating your own `test.code` files. What patterns do valid versus invalid inputs share?

## What It Does
The parser reads code from `test.code`, tokenizes it using the custom lexer (handling identifiers, literals, operators, comments), and checks if it matches the grammar in `parser.y`. If the syntax is valid, it outputs a success message; otherwise, it reports errors like `Syntax Error: syntax error`. It’s a front-end for a compiler, focusing on lexical and syntactic analysis without executing code or checking semantics (e.g., variable types or undefined variables).

### Future Extensions
- Add semantic analysis (e.g., type checking).
- Generate intermediate code or an abstract syntax tree.
- Extend the language with arrays, structs, or classes.

What features would you prioritize to enhance this parser?