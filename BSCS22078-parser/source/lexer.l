%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int line_number = 1;
int column_number = 1;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = line_number; \
    yylloc.first_column = column_number; \
    yylloc.last_column = column_number + yyleng - 1; \
    column_number += yyleng;

%}

%option noyywrap
%option yylineno

DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
INTLIT      {DIGIT}+
FLOATLIT    {DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?
STRINGLIT   \"([^\\\"]|\\.)*\"
CHARLIT     \'([^\\\']|\\.)?\'
WHITESPACE  [ \t\r]+
NEWLINE     \n

%%

    /* Comments */
"//".*                  { /* Single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/" { 
    /* Multi-line comment */ 
    for (int i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            line_number++;
            column_number = 1;
        }
    }
}

    /* Preprocessor Directives */
"#include"              { return T_INCLUDE; }
"#define"               { return T_DEFINE; }

    /* Keywords - Data Types */
"int"                   { return T_INT; }
"float"                 { return T_FLOAT; }
"double"                { return T_DOUBLE; }
"char"                  { return T_CHAR; }
"void"                  { return T_VOID; }
"bool"                  { return T_BOOL; }

    /* Keywords - Type Qualifiers */
"const"                 { return T_CONST; }
"enum"                  { return T_ENUM; }

    /* Keywords - Control Flow */
"if"                    { return T_IF; }
"else"                  { return T_ELSE; }
"while"                 { return T_WHILE; }
"for"                   { return T_FOR; }
"do"                    { return T_DO; }
"switch"                { return T_SWITCH; }
"case"                  { return T_CASE; }
"default"               { return T_DEFAULT; }
"return"                { return T_RETURN; }
"break"                 { return T_BREAK; }
"continue"              { return T_CONTINUE; }

    /* Boolean Literals */
"true"                  { yylval.boolval = 1; return T_BOOLLIT; }
"false"                 { yylval.boolval = 0; return T_BOOLLIT; }

    /* Operators - Comparison */
"=="                    { return T_EQUALOP; }
"!="                    { return T_NE; }
"<="                    { return T_LE; }
">="                    { return T_GE; }
"<"                     { return T_LT; }
">"                     { return T_GT; }

    /* Operators - Logical */
"&&"                    { return T_AND; }
"||"                    { return T_OR; }
"!"                     { return T_NOT; }

    /* Operators - Bitwise */
"&"                     { return T_BITWISE_AND; }
"|"                     { return T_BITWISE_OR; }
"^"                     { return T_BITWISE_XOR; }
"~"                     { return T_BITWISE_NOT; }
"<<"                    { return T_LEFT_SHIFT; }
">>"                    { return T_RIGHT_SHIFT; }

    /* Operators - Arithmetic */
"++"                    { return T_INCREMENT; }
"--"                    { return T_DECREMENT; }
"+"                     { return T_PLUS; }
"-"                     { return T_MINUS; }
"*"                     { return T_MULTIPLY; }
"/"                     { return T_DIVIDE; }
"%"                     { return T_MODULO; }

    /* Assignment */
"="                     { return T_ASSIGNOP; }

    /* Brackets and Separators */
"("                     { return T_LPAREN; }
")"                     { return T_RPAREN; }
"{"                     { return T_LBRACE; }
"}"                     { return T_RBRACE; }
"["                     { return T_LBRACKET; }
"]"                     { return T_RBRACKET; }
";"                     { return T_SEMICOLON; }
","                     { return T_COMMA; }
"."                     { return T_DOT; }
":"                     { return T_COLON; }

    /* Literals */
{INTLIT}                { yylval.intval = atoi(yytext); return T_INTLIT; }
{FLOATLIT}              { yylval.floatval = atof(yytext); return T_FLOATLIT; }
{STRINGLIT}             { yylval.strval = strdup(yytext); return T_STRINGLIT; }
{CHARLIT}               { yylval.charval = yytext[1]; return T_CHARLIT; }

    /* Identifier */
{IDENTIFIER}            { yylval.strval = strdup(yytext); return T_IDENTIFIER; }

    /* Whitespace */
{WHITESPACE}            { /* Ignore whitespace */ }
{NEWLINE}               { line_number++; column_number = 1; }

    /* Error handling */
.                       { 
    fprintf(stderr, "Lexical error at line %d, column %d: Unexpected character '%s'\n", 
            line_number, column_number, yytext);
    return T_ERROR;
}

%%