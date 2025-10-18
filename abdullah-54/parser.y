/* parser.y
   Bison grammar for the assignment language.

   Group Members:
   23-XXXX Abdullah
   23-YYYY Another Member
*/

%{
#include <cstdio>
#include <cstdlib>
#include <iostream>

int yylex(void *yylval); // temporary declaration
void yyerror(const char *s);
%}



%token INCLUDE MAIN CONST ENUM IF ELSE WHILE DO FOR SWITCH CASE DEFAULT BREAK RETURN PRINT
%token INT_TYPE FLOAT_TYPE DOUBLE_TYPE CHAR_TYPE BOOL_TYPE VOID_TYPE STRING_TYPE
%token IDENTIFIER INTLIT FLOATLIT STRINGLIT CHARLIT TRUE_KW FALSE_KW
%token INC DEC AND_OP OR_OP EQ NE LE GE ASSIGN

/* You can keep single characters as literal tokens: ; , { } ( ) < > : + - * / % ! < > etc. */

/* Precedence and associativity (from lowest to highest) */
%right ASSIGN
%left OR_OP
%left AND_OP
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right '!' /* unary not (we will treat unary precedence higher) */
%right INC DEC /* prefix inc/dec (we'll handle them in grammar) */

/* Start symbol */
%start program

%code requires {
  #include <string>
}

%code {
  #include "lexer.hpp"   // or declare the final lexer signature
  int yylex(yy::parser::semantic_type *yylval);
  // Define the error function for the parser class
  namespace yy {
    void parser::error(const std::string& msg) {
      std::cerr << "Error: " << msg << std::endl;
    }
  }
}


%%

/* Grammar rules follow, matching the grammar you supplied */

/* program ::= include_main declaration* */
program
    : include_main declaration_list_opt
      {
        std::cout << "Parsing completed successfully.\n";
      }
    ;

/* include_main ::= "include" "<" "main" ">" */
include_main
    : INCLUDE '<' MAIN '>'
    ;

/* zero or more declarations */
declaration_list_opt
    : /* empty */
    | declaration_list_opt declaration
    ;

/* declaration ::= function_decl | main_decl | statement */
declaration
    : function_decl
    | main_decl
    | statement
    ;

/* function_decl ::= type identifier "(" [ param_list ] ")" block */
function_decl
    : type IDENTIFIER '(' param_list_opt ')' block
    ;

/* main_decl ::= "main" block */
main_decl
    : MAIN block
    ;

/* param_list ::= param { "," param }* */
param_list_opt
    : /* empty */
    | param_list
    ;

param_list
    : param
    | param_list ',' param
    ;

param
    : type IDENTIFIER
    ;

/* type ::= "int" | "float" | "double" | "char" | "bool" | "void" */
type
    : INT_TYPE
    | FLOAT_TYPE
    | DOUBLE_TYPE
    | CHAR_TYPE
    | BOOL_TYPE
    | VOID_TYPE
    | STRING_TYPE
    ;

/* statement ::= many alternatives */
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
    | BREAK ';'
    ;

/* const_decl ::= "const" type identifier "=" expression ";" */
const_decl
    : CONST type IDENTIFIER ASSIGN expression ';'
    ;

/* enum_decl ::= "enum" identifier "{" enum_member_list [ "," ] "}" ";" */
enum_decl
    : ENUM IDENTIFIER '{' enum_member_list_opt maybe_trailing_comma '}' ';'
    ;

enum_member_list_opt
    : /* empty */
    | enum_member_list
    ;

enum_member_list
    : enum_member
    | enum_member_list ',' enum_member
    ;

enum_member
    : IDENTIFIER
    | IDENTIFIER ASSIGN expression
    ;

/* optional trailing comma */
maybe_trailing_comma
    : /* empty */
    | ','
    ;

/* var_decl ::= type identifier [ "=" expression ] ";" */
var_decl
    : type IDENTIFIER maybe_initializer ';'
    ;

maybe_initializer
    : /* empty */
    | ASSIGN expression
    ;

/* print_stmt ::= "print" "(" [ expression_list ] ")" ";" */
print_stmt
    : PRINT '(' expression_list_opt ')' ';'
    ;

expression_list_opt
    : /* empty */
    | expression_list
    ;

expression_list
    : expression
    | expression_list ',' expression
    ;

/* if_stmt ::= "if" "(" expression ")" block [ "else" block ] */
if_stmt
    : IF '(' expression ')' block else_opt
    ;

else_opt
    : /* empty */
    | ELSE block
    ;

/* while_stmt ::= "while" "(" expression ")" block */
while_stmt
    : WHILE '(' expression ')' block
    ;

/* do_while_stmt ::= "do" block "while" "(" expression ")" ";" */
do_while_stmt
    : DO block WHILE '(' expression ')' ';'
    ;

/* for_stmt ::= "for" "(" [ for_init ] ";" [ expression ] ";" [ expression ] ")" block */
for_stmt
    : FOR '(' for_init_opt ';' expression_opt ';' expression_opt ')' block
    ;

for_init_opt
    : /* empty */
    | for_init
    ;

for_init
    : var_decl_no_semi
    | expression
    ;

/* var_decl without trailing semicolon (used in for-init) */
var_decl_no_semi
    : type IDENTIFIER maybe_initializer
    ;

/* optional expression */
expression_opt
    : /* empty */
    | expression
    ;

/* switch_stmt ::= "switch" "(" expression ")" "{" { case_block } [ default_block ] "}" */
switch_stmt
    : SWITCH '(' expression ')' '{' case_block_list default_block_opt '}'
    ;

case_block_list
    : /* empty */
    | case_block_list case_block
    ;

case_block
    : CASE expression block
    ;

/* default_block ::= "default" block */
default_block_opt
    : /* empty */
    | DEFAULT block
    ;

/* return_stmt ::= "return" [ expression ] ";" */
return_stmt
    : RETURN expression_opt ';'
    ;

/* block ::= "{" statement* "}" */
block
    : '{' block_statements_opt '}'
    ;

block_statements_opt
    : /* empty */
    | block_statements_opt statement
    ;

/* expr_stmt ::= expression ";" */
expr_stmt
    : expression ';'
    ;

/* expression ::= assign_expr */
expression
    : assign_expr
    ;

/* assign_expr ::= logical_or_expr [ "=" assign_expr ] */
assign_expr
    : logical_or_expr
    | logical_or_expr ASSIGN assign_expr
    ;

/* logical_or_expr ::= logical_and_expr { "||" logical_and_expr }* */
logical_or_expr
    : logical_and_expr
    | logical_or_expr OR_OP logical_and_expr
    ;

/* logical_and_expr ::= equality_expr { "&&" equality_expr }* */
logical_and_expr
    : equality_expr
    | logical_and_expr AND_OP equality_expr
    ;

/* equality_expr ::= relational_expr { ( "==" | "!=" ) relational_expr }* */
equality_expr
    : relational_expr
    | equality_expr EQ relational_expr
    | equality_expr NE relational_expr
    ;

/* relational_expr ::= additive_expr { ( "<" | ">" | "<=" | ">=" ) additive_expr }* */
relational_expr
    : additive_expr
    | relational_expr '<' additive_expr
    | relational_expr '>' additive_expr
    | relational_expr LE additive_expr
    | relational_expr GE additive_expr
    ;

/* additive_expr ::= multiplicative_expr { ( "+" | "-" ) multiplicative_expr }* */
additive_expr
    : multiplicative_expr
    | additive_expr '+' multiplicative_expr
    | additive_expr '-' multiplicative_expr
    ;

/* multiplicative_expr ::= unary_expr { ( "*" | "/" | "%" ) unary_expr }* */
multiplicative_expr
    : unary_expr
    | multiplicative_expr '*' unary_expr
    | multiplicative_expr '/' unary_expr
    | multiplicative_expr '%' unary_expr
    ;

/* unary_expr ::= postfix_expr | ( "-" | "!" ) unary_expr | ( "++" | "--" ) identifier */
unary_expr
    : postfix_expr
    | '-' unary_expr
    | '!' unary_expr
    | INC IDENTIFIER
    | DEC IDENTIFIER
    ;

/* postfix_expr ::= primary_expr | postfix_expr "(" [ expression_list ] ")" | identifier ( "++" | "--" ) */
postfix_expr
    : primary_expr
    | postfix_expr '(' expression_list_opt ')'
    | IDENTIFIER INC
    | IDENTIFIER DEC
    ;

/* primary_expr ::= integer_literal | float_literal | string_literal | char_literal | bool_literal | identifier | "(" expression ")" */
primary_expr
    : INTLIT
    | FLOATLIT
    | STRINGLIT
    | CHARLIT
    | TRUE_KW
    | FALSE_KW
    | IDENTIFIER
    | '(' expression ')'
    ;

%%

/* Additional C / C++ code */
void yyerror(const char *s) {
    std::cerr << "Error: " << s << std::endl;
}
int main(int argc, char **argv) {
    yy::parser parser;
    int res = parser.parse();
    return (res == 0) ? 0 : 1;
}
