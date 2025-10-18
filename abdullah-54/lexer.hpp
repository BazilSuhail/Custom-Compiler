#ifndef LEXER_HPP
#define LEXER_HPP
#include "parser.tab.hpp"
#include <string>

// Define TokenType enum
enum TokenType
{
    // Types
    T_INT,
    T_FLOAT,
    T_DOUBLE,
    T_CHAR,
    T_VOID,
    T_BOOL,
    T_STRING,

    // Literals
    T_IDENTIFIER,
    T_INTLIT,
    T_FLOATLIT,
    T_STRINGLIT,
    T_CHARLIT,
    T_BOOLLIT,

    // Brackets & Separators
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_LBRACKET,
    T_RBRACKET,
    T_SEMICOLON,
    T_COMMA,
    T_DOT,
    T_COLON,

    // Operators
    T_ASSIGNOP,
    T_EQUALOP,
    T_NE,
    T_LT,
    T_GT,
    T_LE,
    T_GE,
    T_PLUS,
    T_MINUS,
    T_MULTIPLY,
    T_DIVIDE,
    T_MODULO,
    T_INCREMENT,
    T_DECREMENT,
    T_AND,
    T_OR,
    T_NOT,

    // Keywords
    T_IF,
    T_ELSE,
    T_WHILE,
    T_RETURN,
    T_PRINT,
    T_MAIN,
    T_CONST,
    T_ENUM,
    T_DO,
    T_SWITCH,
    T_BREAK,
    T_FOR,
    T_DEFAULT,
    T_CASE,
    T_INCLUDE,

    // Comments
    T_SINGLE_COMMENT,
    T_MULTI_COMMENT,

    // Error / EOF
    T_ERROR,
    T_EOF
};

struct Token
{
    TokenType type;
    std::string value;
    int line;
    int column;
};

struct LexerState;

int yylex(yy::parser::semantic_type *yylval);
LexerState createLexerState(const char *filename);

#endif