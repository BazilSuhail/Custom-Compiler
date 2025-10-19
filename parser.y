%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

void yyerror(const char *s);
int yylex();
extern void initLexer(const string& code);
extern bool includeMainFound;
%}

%union {
    char* str;
}

/* === Tokens === */
%token <str> T_IDENTIFIER T_INTLIT T_FLOATLIT T_STRINGLIT T_CHARLIT T_BOOLLIT
%token T_INT T_FLOAT T_DOUBLE T_CHAR T_BOOL T_VOID T_STRING
%token T_IF T_ELSE T_WHILE T_DO T_FOR T_SWITCH T_CASE T_DEFAULT T_BREAK T_RETURN T_PRINT T_CONST T_ENUM T_MAIN
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET
%token T_SEMICOLON T_COMMA T_DOT T_COLON
%token T_SINGLE_COMMENT T_MULTI_COMMENT
%token T_ASSIGNOP T_EQUALOP T_NE T_LT T_GT T_LE T_GE
%token T_PLUS T_MINUS T_MULTIPLY T_DIVIDE T_MODULO
%token T_INCREMENT T_DECREMENT T_AND T_OR T_NOT
%token T_INCLUDE T_ERROR T_EOF T_HASH

%start program

%%

/* === Top-Level Program === */
program
    : opt_include global_decls main_decl
    ;

opt_include
    : /* empty */
    | T_INCLUDE
    ;

/* === Global Declarations === */
global_decls
    : /* empty */
    | global_decls global_decl
    ;

global_decl
    : function_decl
    | var_decl
    | const_decl
    | enum_decl
    | comment
    ;

comment
    : T_SINGLE_COMMENT
    | T_MULTI_COMMENT
    ;

/* === Main Function === */
main_decl
    : T_MAIN block
    ;

/* === Function Declarations === */
function_decl
    : type T_IDENTIFIER T_LPAREN opt_param_list T_RPAREN block
    ;

opt_param_list
    : /* empty */
    | param_list
    ;

param_list
    : param
    | param_list T_COMMA param
    ;

param
    : type T_IDENTIFIER
    ;

/* === Type System === */
type
    : T_INT
    | T_FLOAT
    | T_DOUBLE
    | T_CHAR
    | T_BOOL
    | T_STRING
    | T_VOID
    ;

/* === Constants / Enums / Variables === */
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

/* === Statements === */
statement
    : var_decl
    | const_decl
    | enum_decl
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
    | comment
    ;

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
    : /* empty */
    | for_init
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
    : /* empty */
    | T_DEFAULT block
    ;

return_stmt
    : T_RETURN opt_expression T_SEMICOLON
    ;

opt_expression
    : /* empty */
    | expression
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
    : /* empty */
    | expression_list
    ;

expression_list
    : expression
    | expression_list T_COMMA expression
    ;

/* === Expression Grammar === */
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
    | T_INCREMENT int_identifier
    | T_DECREMENT int_identifier
    ;

postfix_expr
    : primary_expr
    | postfix_expr T_LPAREN opt_expression_list T_RPAREN
    | int_identifier T_INCREMENT
    | int_identifier T_DECREMENT
    ;

int_identifier
    : T_IDENTIFIER
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

void yyerror(const char *s) {
    cerr << "Syntax Error: " << s << endl;
}
