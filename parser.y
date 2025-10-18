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

/* === For Loop === */
for_stmt
    : T_FOR T_LPAREN opt_for_init T_SEMICOLON opt_expression T_SEMICOLON opt_expression T_RPAREN block
    ;

opt_for_init
    : for_init
    | /* empty */
    ;

for_init
    : type T_IDENTIFIER opt_init      /* allows: int i = 0 */
    | expression                      /* allows: i = 0 */
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