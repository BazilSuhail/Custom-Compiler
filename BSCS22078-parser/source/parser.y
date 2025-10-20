%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int line_number;
extern int column_number;

void yyerror(const char *s);

ASTNode *root = NULL;  // Global AST root

%}

%locations

%union {
    int intval;
    float floatval;
    char charval;
    int boolval;
    char *strval;
    ASTNode *node;
    DataType datatype;
    OperatorType optype;
}

/* Token declarations with types */
%token T_INT T_FLOAT T_DOUBLE T_CHAR T_VOID T_BOOL
%token <strval> T_IDENTIFIER T_STRINGLIT
%token <intval> T_INTLIT
%token <floatval> T_FLOATLIT
%token <charval> T_CHARLIT
%token <boolval> T_BOOLLIT
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET
%token T_SEMICOLON T_COMMA T_DOT T_COLON
%token T_ASSIGNOP T_EQUALOP T_NE T_LT T_GT T_LE T_GE
%token T_PLUS T_MINUS T_MULTIPLY T_DIVIDE T_MODULO
%token T_INCREMENT T_DECREMENT
%token T_AND T_OR T_NOT
%token T_BITWISE_AND T_BITWISE_OR T_BITWISE_XOR T_BITWISE_NOT
%token T_LEFT_SHIFT T_RIGHT_SHIFT
%token T_INCLUDE T_DEFINE
%token T_IF T_ELSE T_WHILE T_FOR T_RETURN T_BREAK T_CONTINUE
%token T_SWITCH T_CASE T_DEFAULT T_DO
%token T_CONST T_ENUM
%token T_SINGLE_COMMENT T_MULTI_COMMENT
%token T_ERROR

/* Non-terminal types */
%type <node> program declaration_list declaration varDecl fnDecl
%type <node> parameters parameter typeSpec block statement
%type <node> returnStmt breakStmt continueStmt ifStmt whileStmt
%type <node> doWhileStmt forStmt switchStmt caseStmt caseStmt_list
%type <node> assignStmt exprStmt expression assignment logicalOr
%type <node> logicalAnd bitwiseOr bitwiseXor bitwiseAnd equality
%type <node> comparison shift term factor unary postfix call
%type <node> arguments primary literal
%type <node> includeDirective defineDirective enumDecl
%type <node> parameterList macroBody macroBodyElement
%type <node> enumValueList operator
%type <datatype> baseType
%type <intval> typeQualifier

/* Operator precedence and associativity (lowest to highest) */
%right T_ASSIGNOP
%left T_OR
%left T_AND
%left T_BITWISE_OR
%left T_BITWISE_XOR
%left T_BITWISE_AND
%left T_EQUALOP T_NE
%left T_LT T_GT T_LE T_GE
%left T_LEFT_SHIFT T_RIGHT_SHIFT
%left T_PLUS T_MINUS
%left T_MULTIPLY T_DIVIDE T_MODULO
%right T_NOT T_BITWISE_NOT UNARY_PREC
%left T_INCREMENT T_DECREMENT

%start program

%%

/* Program */
program
    : declaration_list { $$ = create_list(NODE_PROGRAM); 
                         for (int i = 0; i < $1->data.list.count; i++) 
                             list_append($$, $1->data.list.items[i]);
                         root = $$; }
    | /* empty */ { $$ = create_list(NODE_PROGRAM); root = $$; }
    ;

declaration_list
    : declaration { $$ = create_list(NODE_DECLARATION_LIST); list_append($$, $1); }
    | declaration_list declaration { $$ = $1; list_append($$, $2); }
    ;

/* Declarations */
declaration
    : includeDirective { $$ = $1; }
    | defineDirective { $$ = $1; }
    | enumDecl { $$ = $1; }
    | varDecl { $$ = $1; }
    | fnDecl { $$ = $1; }
    | statement { $$ = $1; }
    ;

/* Variable Declaration */
varDecl
    : typeSpec T_IDENTIFIER T_SEMICOLON 
        { $$ = create_var_decl($1, $2, NULL); free($2); }
    | typeSpec T_IDENTIFIER T_ASSIGNOP expression T_SEMICOLON
        { $$ = create_var_decl($1, $2, $4); free($2); }
    ;

/* Function Declaration */
fnDecl
    : typeSpec T_IDENTIFIER T_LPAREN parameters T_RPAREN block
        { $$ = create_func_decl($1, $2, $4, $6); free($2); }
    | typeSpec T_IDENTIFIER T_LPAREN T_RPAREN block
        { $$ = create_func_decl($1, $2, NULL, $5); free($2); }
    ;

parameters
    : parameter { $$ = create_list(NODE_PARAM_LIST); list_append($$, $1); }
    | parameters T_COMMA parameter { $$ = $1; list_append($$, $3); }
    ;

parameter
    : typeSpec T_IDENTIFIER { $$ = create_param($1, $2); free($2); }
    ;

/* Type Specification */
typeSpec
    : typeQualifier baseType { $$ = create_type_spec($1, $2); }
    | baseType { $$ = create_type_spec(0, $1); }
    ;

typeQualifier
    : T_CONST { $$ = 1; }
    ;

baseType
    : T_INT { $$ = TYPE_INT; }
    | T_FLOAT { $$ = TYPE_FLOAT; }
    | T_DOUBLE { $$ = TYPE_DOUBLE; }
    | T_CHAR { $$ = TYPE_CHAR; }
    | T_BOOL { $$ = TYPE_BOOL; }
    | T_VOID { $$ = TYPE_VOID; }
    ;

/* Include Directive */
includeDirective
    : T_INCLUDE T_LT T_IDENTIFIER T_GT
        { $$ = create_include_directive($3, 1); free($3); }
    | T_INCLUDE T_LT T_IDENTIFIER T_DOT T_IDENTIFIER T_GT
        { char *file = malloc(strlen($3) + strlen($5) + 2);
          sprintf(file, "%s.%s", $3, $5);
          $$ = create_include_directive(file, 1);
          free($3); free($5); free(file); }
    | T_INCLUDE T_STRINGLIT
        { $$ = create_include_directive($2, 0); free($2); }
    ;

/* Define Directive */
defineDirective
    : T_DEFINE T_IDENTIFIER
        { $$ = create_define_directive($2, NULL, NULL); free($2); }
    | T_DEFINE T_IDENTIFIER macroBody
        { $$ = create_define_directive($2, NULL, "macro_body"); free($2); }
    | T_DEFINE T_IDENTIFIER T_LPAREN parameterList T_RPAREN
        { $$ = create_define_directive($2, $4, NULL); free($2); }
    | T_DEFINE T_IDENTIFIER T_LPAREN parameterList T_RPAREN macroBody
        { $$ = create_define_directive($2, $4, "macro_body"); free($2); }
    | T_DEFINE T_IDENTIFIER T_LPAREN T_RPAREN
        { $$ = create_define_directive($2, NULL, NULL); free($2); }
    | T_DEFINE T_IDENTIFIER T_LPAREN T_RPAREN macroBody
        { $$ = create_define_directive($2, NULL, "macro_body"); free($2); }
    ;

parameterList
    : T_IDENTIFIER { $$ = create_list(NODE_PARAM_LIST); 
                     list_append($$, create_identifier($1)); free($1); }
    | parameterList T_COMMA T_IDENTIFIER 
        { $$ = $1; list_append($$, create_identifier($3)); free($3); }
    ;

macroBody
    : macroBodyElement { $$ = $1; }
    | macroBody macroBodyElement { $$ = $1; }
    ;

macroBodyElement
    : T_IDENTIFIER { $$ = create_identifier($1); free($1); }
    | literal { $$ = $1; }
    | operator { $$ = $1; }
    ;

/* Enum Declaration */
enumDecl
    : T_ENUM T_IDENTIFIER T_LBRACE enumValueList T_RBRACE T_SEMICOLON
        { $$ = create_enum_decl($2, $4); free($2); }
    | T_ENUM T_IDENTIFIER T_LBRACE T_RBRACE T_SEMICOLON
        { $$ = create_enum_decl($2, NULL); free($2); }
    ;

enumValueList
    : T_IDENTIFIER { $$ = create_list(NODE_ENUM_VALUE_LIST); 
                     list_append($$, create_identifier($1)); free($1); }
    | enumValueList T_COMMA T_IDENTIFIER 
        { $$ = $1; list_append($$, create_identifier($3)); free($3); }
    ;

/* Statements */
statement
    : returnStmt { $$ = $1; }
    | ifStmt { $$ = $1; }
    | whileStmt { $$ = $1; }
    | doWhileStmt { $$ = $1; }
    | forStmt { $$ = $1; }
    | switchStmt { $$ = $1; }
    | breakStmt { $$ = $1; }
    | continueStmt { $$ = $1; }
    | assignStmt { $$ = $1; }
    | exprStmt { $$ = $1; }
    ;

assignStmt
    : T_IDENTIFIER T_ASSIGNOP expression T_SEMICOLON
        { $$ = create_binary_op(OP_ASSIGN, create_identifier($1), $3); free($1); }
    ;

returnStmt
    : T_RETURN expression T_SEMICOLON { $$ = create_return_stmt($2); }
    | T_RETURN T_SEMICOLON { $$ = create_return_stmt(NULL); }
    ;

breakStmt
    : T_BREAK T_SEMICOLON { $$ = create_break_stmt(); }
    ;

continueStmt
    : T_CONTINUE T_SEMICOLON { $$ = create_continue_stmt(); }
    ;

ifStmt
    : T_IF T_LPAREN expression T_RPAREN block
        { $$ = create_if_stmt($3, $5, NULL); }
    | T_IF T_LPAREN expression T_RPAREN block T_ELSE block
        { $$ = create_if_stmt($3, $5, $7); }
    ;

whileStmt
    : T_WHILE T_LPAREN expression T_RPAREN block
        { $$ = create_while_stmt($3, $5); }
    ;

doWhileStmt
    : T_DO block T_WHILE T_LPAREN expression T_RPAREN T_SEMICOLON
        { $$ = create_do_while_stmt($2, $5); }
    ;

forStmt
    : T_FOR T_LPAREN varDecl expression T_SEMICOLON expression T_RPAREN block
        { $$ = create_for_stmt($3, $4, $6, $8); }
    | T_FOR T_LPAREN varDecl expression T_SEMICOLON T_RPAREN block
        { $$ = create_for_stmt($3, $4, NULL, $7); }
    | T_FOR T_LPAREN varDecl T_SEMICOLON expression T_RPAREN block
        { $$ = create_for_stmt($3, NULL, $5, $7); }
    | T_FOR T_LPAREN varDecl T_SEMICOLON T_RPAREN block
        { $$ = create_for_stmt($3, NULL, NULL, $6); }
    | T_FOR T_LPAREN exprStmt expression T_SEMICOLON expression T_RPAREN block
        { $$ = create_for_stmt($3, $4, $6, $8); }
    | T_FOR T_LPAREN exprStmt expression T_SEMICOLON T_RPAREN block
        { $$ = create_for_stmt($3, $4, NULL, $7); }
    | T_FOR T_LPAREN exprStmt T_SEMICOLON expression T_RPAREN block
        { $$ = create_for_stmt($3, NULL, $5, $7); }
    | T_FOR T_LPAREN exprStmt T_SEMICOLON T_RPAREN block
        { $$ = create_for_stmt($3, NULL, NULL, $6); }
    | T_FOR T_LPAREN T_SEMICOLON expression T_SEMICOLON expression T_RPAREN block
        { $$ = create_for_stmt(NULL, $4, $6, $8); }
    | T_FOR T_LPAREN T_SEMICOLON expression T_SEMICOLON T_RPAREN block
        { $$ = create_for_stmt(NULL, $4, NULL, $7); }
    | T_FOR T_LPAREN T_SEMICOLON T_SEMICOLON expression T_RPAREN block
        { $$ = create_for_stmt(NULL, NULL, $5, $7); }
    | T_FOR T_LPAREN T_SEMICOLON T_SEMICOLON T_RPAREN block
        { $$ = create_for_stmt(NULL, NULL, NULL, $6); }
    ;

switchStmt
    : T_SWITCH T_LPAREN expression T_RPAREN T_LBRACE caseStmt_list T_RBRACE
        { $$ = create_switch_stmt($3, $6); }
    | T_SWITCH T_LPAREN expression T_RPAREN T_LBRACE T_RBRACE
        { $$ = create_switch_stmt($3, NULL); }
    ;

caseStmt_list
    : caseStmt { $$ = create_list(NODE_STATEMENT_LIST); list_append($$, $1); }
    | caseStmt_list caseStmt { $$ = $1; list_append($$, $2); }
    ;

caseStmt
    : T_CASE expression T_COLON declaration_list
        { $$ = create_case_stmt($2, $4); }
    | T_CASE expression T_COLON
        { $$ = create_case_stmt($2, NULL); }
    | T_DEFAULT T_COLON declaration_list
        { $$ = create_case_stmt(NULL, $3); }
    | T_DEFAULT T_COLON
        { $$ = create_case_stmt(NULL, NULL); }
    ;

block
    : T_LBRACE declaration_list T_RBRACE { $$ = create_block($2); }
    | T_LBRACE T_RBRACE { $$ = create_block(NULL); }
    ;

exprStmt
    : expression T_SEMICOLON { $$ = $1; }
    ;

/* Expressions */
expression
    : assignment { $$ = $1; }
    ;

assignment
    : logicalOr { $$ = $1; }
    | logicalOr T_ASSIGNOP assignment 
        { $$ = create_binary_op(OP_ASSIGN, $1, $3); }
    ;

logicalOr
    : logicalAnd { $$ = $1; }
    | logicalOr T_OR logicalAnd 
        { $$ = create_binary_op(OP_OR, $1, $3); }
    ;

logicalAnd
    : bitwiseOr { $$ = $1; }
    | logicalAnd T_AND bitwiseOr 
        { $$ = create_binary_op(OP_AND, $1, $3); }
    ;

bitwiseOr
    : bitwiseXor { $$ = $1; }
    | bitwiseOr T_BITWISE_OR bitwiseXor 
        { $$ = create_binary_op(OP_BIT_OR, $1, $3); }
    ;

bitwiseXor
    : bitwiseAnd { $$ = $1; }
    | bitwiseXor T_BITWISE_XOR bitwiseAnd 
        { $$ = create_binary_op(OP_BIT_XOR, $1, $3); }
    ;

bitwiseAnd
    : equality { $$ = $1; }
    | bitwiseAnd T_BITWISE_AND equality 
        { $$ = create_binary_op(OP_BIT_AND, $1, $3); }
    ;

equality
    : comparison { $$ = $1; }
    | equality T_EQUALOP comparison 
        { $$ = create_binary_op(OP_EQ, $1, $3); }
    | equality T_NE comparison 
        { $$ = create_binary_op(OP_NE, $1, $3); }
    ;

comparison
    : shift { $$ = $1; }
    | comparison T_LT shift 
        { $$ = create_binary_op(OP_LT, $1, $3); }
    | comparison T_GT shift 
        { $$ = create_binary_op(OP_GT, $1, $3); }
    | comparison T_LE shift 
        { $$ = create_binary_op(OP_LE, $1, $3); }
    | comparison T_GE shift 
        { $$ = create_binary_op(OP_GE, $1, $3); }
    ;

shift
    : term { $$ = $1; }
    | shift T_LEFT_SHIFT term 
        { $$ = create_binary_op(OP_LSHIFT, $1, $3); }
    | shift T_RIGHT_SHIFT term 
        { $$ = create_binary_op(OP_RSHIFT, $1, $3); }
    ;

term
    : factor { $$ = $1; }
    | term T_PLUS factor 
        { $$ = create_binary_op(OP_ADD, $1, $3); }
    | term T_MINUS factor 
        { $$ = create_binary_op(OP_SUB, $1, $3); }
    ;

factor
    : unary { $$ = $1; }
    | factor T_MULTIPLY unary 
        { $$ = create_binary_op(OP_MUL, $1, $3); }
    | factor T_DIVIDE unary 
        { $$ = create_binary_op(OP_DIV, $1, $3); }
    | factor T_MODULO unary 
        { $$ = create_binary_op(OP_MOD, $1, $3); }
    ;

unary
    : postfix { $$ = $1; }
    | T_PLUS unary %prec UNARY_PREC 
        { $$ = create_unary_op(OP_PLUS_UNARY, $2); }
    | T_MINUS unary %prec UNARY_PREC 
        { $$ = create_unary_op(OP_MINUS_UNARY, $2); }
    | T_NOT unary 
        { $$ = create_unary_op(OP_NOT, $2); }
    | T_BITWISE_NOT unary 
        { $$ = create_unary_op(OP_BIT_NOT, $2); }
    | T_INCREMENT unary 
        { $$ = create_unary_op(OP_PRE_INC, $2); }
    | T_DECREMENT unary 
        { $$ = create_unary_op(OP_PRE_DEC, $2); }
    ;

postfix
    : call { $$ = $1; }
    | call T_INCREMENT 
        { $$ = create_unary_op(OP_POST_INC, $1); }
    | call T_DECREMENT 
        { $$ = create_unary_op(OP_POST_DEC, $1); }
    ;

call
    : primary { $$ = $1; }
    | primary T_LPAREN arguments T_RPAREN 
        { if ($1->type == NODE_IDENTIFIER) {
              $$ = create_func_call($1->data.identifier.name, $3);
              free_ast($1);
          } else {
              $$ = $1;
          } }
    | primary T_LPAREN T_RPAREN 
        { if ($1->type == NODE_IDENTIFIER) {
              $$ = create_func_call($1->data.identifier.name, NULL);
              free_ast($1);
          } else {
              $$ = $1;
          } }
    ;

arguments
    : expression { $$ = create_list(NODE_ARG_LIST); list_append($$, $1); }
    | arguments T_COMMA expression { $$ = $1; list_append($$, $3); }
    ;

primary
    : literal { $$ = $1; }
    | T_IDENTIFIER { $$ = create_identifier($1); free($1); }
    | T_LPAREN expression T_RPAREN { $$ = $2; }
    | T_INCREMENT unary { $$ = create_unary_op(OP_PRE_INC, $2); }
    | T_DECREMENT unary { $$ = create_unary_op(OP_PRE_DEC, $2); }
    ;

literal
    : T_INTLIT { $$ = create_literal_int($1); }
    | T_FLOATLIT { $$ = create_literal_float($1); }
    | T_BOOLLIT { $$ = create_literal_bool($1); }
    | T_STRINGLIT { $$ = create_literal_string($1); free($1); }
    | T_CHARLIT { $$ = create_literal_char($1); }
    ;

operator
    : T_PLUS { $$ = create_identifier("+"); }
    | T_MINUS { $$ = create_identifier("-"); }
    | T_MULTIPLY { $$ = create_identifier("*"); }
    | T_DIVIDE { $$ = create_identifier("/"); }
    | T_MODULO { $$ = create_identifier("%"); }
    | T_ASSIGNOP { $$ = create_identifier("="); }
    | T_EQUALOP { $$ = create_identifier("=="); }
    | T_NE { $$ = create_identifier("!="); }
    | T_LT { $$ = create_identifier("<"); }
    | T_GT { $$ = create_identifier(">"); }
    | T_LE { $$ = create_identifier("<="); }
    | T_GE { $$ = create_identifier(">="); }
    | T_AND { $$ = create_identifier("&&"); }
    | T_OR { $$ = create_identifier("||"); }
    | T_NOT { $$ = create_identifier("!"); }
    | T_BITWISE_AND { $$ = create_identifier("&"); }
    | T_BITWISE_OR { $$ = create_identifier("|"); }
    | T_BITWISE_XOR { $$ = create_identifier("^"); }
    | T_BITWISE_NOT { $$ = create_identifier("~"); }
    | T_LEFT_SHIFT { $$ = create_identifier("<<"); }
    | T_RIGHT_SHIFT { $$ = create_identifier(">>"); }
    | T_INCREMENT { $$ = create_identifier("++"); }
    | T_DECREMENT { $$ = create_identifier("--"); }
    | T_LPAREN { $$ = create_identifier("("); }
    | T_RPAREN { $$ = create_identifier(")"); }
    | T_LBRACE { $$ = create_identifier("{"); }
    | T_RBRACE { $$ = create_identifier("}"); }
    | T_SEMICOLON { $$ = create_identifier(";"); }
    | T_COMMA { $$ = create_identifier(","); }
    | T_DOT { $$ = create_identifier("."); }
    | T_LBRACKET { $$ = create_identifier("["); }
    | T_RBRACKET { $$ = create_identifier("]"); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d, column %d: %s\n", 
            line_number, column_number, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    }
    
    printf("Starting parse...\n");
    int result = yyparse();
    
    if (result == 0) {
        printf("Parse successful!\n\n");
        printf("==================== ABSTRACT SYNTAX TREE ====================\n\n");
        print_ast(root, 0);
        printf("\n==============================================================\n");
        free_ast(root);
    } else {
        printf("Parse failed!\n");
    }
    
    if (argc > 1) {
        fclose(yyin);
    }
    
    return result;
}
