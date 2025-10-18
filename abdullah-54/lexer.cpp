#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cctype>
#include "parser.tab.hpp" // Bison-generated header (C++ mode)
#include "lexer.hpp"
using namespace std;
using namespace yy; // Gives us yy::parser, yy::parser::semantic_type, etc.

/* ============================================================
   TOKEN DEFINITIONS
   ============================================================ */

/* ============================================================
   KEYWORDS AND OPERATORS
   ============================================================ */
static const map<string, TokenType> keywords = {
    {"int", T_INT}, {"float", T_FLOAT}, {"double", T_DOUBLE}, {"char", T_CHAR}, {"void", T_VOID}, {"bool", T_BOOL}, {"true", T_BOOLLIT}, {"false", T_BOOLLIT}, {"if", T_IF}, {"else", T_ELSE}, {"while", T_WHILE}, {"return", T_RETURN}, {"print", T_PRINT}, {"main", T_MAIN}, {"string", T_STRING}, {"do", T_DO}, {"switch", T_SWITCH}, {"break", T_BREAK}, {"for", T_FOR}, {"default", T_DEFAULT}, {"case", T_CASE}, {"const", T_CONST}, {"enum", T_ENUM}, {"include", T_INCLUDE}};

static const map<char, TokenType> singleChars = {
    {'(', T_LPAREN}, {')', T_RPAREN}, {'{', T_LBRACE}, {'}', T_RBRACE}, {'[', T_LBRACKET}, {']', T_RBRACKET}, {';', T_SEMICOLON}, {',', T_COMMA}, {'.', T_DOT}, {'+', T_PLUS}, {'-', T_MINUS}, {':', T_COLON}, {'*', T_MULTIPLY}, {'/', T_DIVIDE}, {'%', T_MODULO}, {'=', T_ASSIGNOP}, {'!', T_NOT}, {'<', T_LT}, {'>', T_GT}};

static const map<string, TokenType> twoCharOps = {
    {"==", T_EQUALOP}, {"!=", T_NE}, {"<=", T_LE}, {">=", T_GE}, {"&&", T_AND}, {"||", T_OR}, {"++", T_INCREMENT}, {"--", T_DECREMENT}};

/* ============================================================
   LEXER STATE AND HELPERS
   ============================================================ */

struct LexerState
{
    const char *input;
    int pos;
    int line;
    int column;
    int length;
};

LexerState createLexerState(const char *src)
{
    LexerState s;
    s.input = src;
    s.pos = 0;
    s.line = 1;
    s.column = 1;
    s.length = strlen(src);
    return s;
}

inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
inline bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
inline bool isAlnum(char c) { return isAlpha(c) || isDigit(c); }

void skipWhitespace(LexerState &s)
{
    while (s.pos < s.length && isSpace(s.input[s.pos]))
    {
        if (s.input[s.pos] == '\n')
        {
            s.line++;
            s.column = 1;
        }
        else
            s.column++;
        s.pos++;
    }
}

void makeToken(Token &t, TokenType type, const string &val, const LexerState &s, int startCol)
{
    t.type = type;
    t.value = val;
    t.line = s.line;
    t.column = startCol;
}

/* ============================================================
   TOKEN MATCHING ROUTINES
   ============================================================ */

bool tryComments(LexerState &s, Token &t)
{
    if (s.pos + 1 >= s.length)
        return false;
    int start = s.column;
    if (s.input[s.pos] == '/' && s.input[s.pos + 1] == '/')
    {
        string val = "//";
        s.pos += 2;
        s.column += 2;
        while (s.pos < s.length && s.input[s.pos] != '\n')
        {
            val += s.input[s.pos++];
            s.column++;
        }
        makeToken(t, T_SINGLE_COMMENT, val, s, start);
        return true;
    }
    if (s.input[s.pos] == '/' && s.input[s.pos + 1] == '*')
    {
        string val = "/*";
        s.pos += 2;
        s.column += 2;
        while (s.pos + 1 < s.length)
        {
            if (s.input[s.pos] == '*' && s.input[s.pos + 1] == '/')
            {
                val += "*/";
                s.pos += 2;
                s.column += 2;
                makeToken(t, T_MULTI_COMMENT, val, s, start);
                return true;
            }
            if (s.input[s.pos] == '\n')
            {
                s.line++;
                s.column = 1;
            }
            else
                s.column++;
            val += s.input[s.pos++];
        }
        makeToken(t, T_ERROR, "Unterminated comment", s, start);
        return true;
    }
    return false;
}

bool tryQuoted(LexerState &s, Token &t)
{
    if (s.pos >= s.length)
        return false;
    char q = s.input[s.pos];
    if (q != '"' && q != '\'')
        return false;
    int start = s.column;
    string val(1, q);
    s.pos++;
    s.column++;
    while (s.pos < s.length && s.input[s.pos] != q)
    {
        if (s.input[s.pos] == '\\' && s.pos + 1 < s.length)
        {
            val += s.input[s.pos++];
            val += s.input[s.pos++];
            s.column += 2;
        }
        else
        {
            if (s.input[s.pos] == '\n')
            {
                s.line++;
                s.column = 1;
            }
            else
                s.column++;
            val += s.input[s.pos++];
        }
    }
    if (s.pos < s.length)
    {
        val += q;
        s.pos++;
        s.column++;
        makeToken(t, q == '"' ? T_STRINGLIT : T_CHARLIT, val, s, start);
        return true;
    }
    makeToken(t, T_ERROR, "Unterminated string/char", s, start);
    return true;
}

bool tryOperator(LexerState &s, Token &t)
{
    if (s.pos >= s.length)
        return false;
    int start = s.column;
    char c = s.input[s.pos];
    char n = (s.pos + 1 < s.length) ? s.input[s.pos + 1] : '\0';
    string two;
    two.push_back(c);
    two.push_back(n);
    auto it2 = twoCharOps.find(two);
    if (it2 != twoCharOps.end())
    {
        makeToken(t, it2->second, two, s, start);
        s.pos += 2;
        s.column += 2;
        return true;
    }
    auto it1 = singleChars.find(c);
    if (it1 != singleChars.end())
    {
        makeToken(t, it1->second, string(1, c), s, start);
        s.pos++;
        s.column++;
        return true;
    }
    return false;
}

bool tryIdOrNum(LexerState &s, Token &t)
{
    if (s.pos >= s.length)
        return false;
    int start = s.column;
    char c = s.input[s.pos];
    if (isDigit(c))
    {
        string val;
        while (s.pos < s.length && (isDigit(s.input[s.pos]) || s.input[s.pos] == '.' || s.input[s.pos] == 'e' || s.input[s.pos] == 'E'))
        {
            val += s.input[s.pos++];
            s.column++;
        }
        if (val.find('.') != string::npos)
            makeToken(t, T_FLOATLIT, val, s, start);
        else
            makeToken(t, T_INTLIT, val, s, start);
        return true;
    }
    if (isAlpha(c))
    {
        string val;
        while (s.pos < s.length && isAlnum(s.input[s.pos]))
        {
            val += s.input[s.pos++];
            s.column++;
        }
        auto kw = keywords.find(val);
        makeToken(t, (kw != keywords.end() ? kw->second : T_IDENTIFIER), val, s, start);
        return true;
    }
    return false;
}

bool getNextToken(LexerState &s, Token &t)
{
    skipWhitespace(s);
    if (s.pos >= s.length)
    {
        t.type = T_EOF;
        t.value = "";
        t.line = s.line;
        t.column = s.column;
        return false;
    }
    if (tryComments(s, t))
        return true;
    if (tryQuoted(s, t))
        return true;
    if (tryOperator(s, t))
        return true;
    if (tryIdOrNum(s, t))
        return true;
    string err = "Unexpected char: " + string(1, s.input[s.pos]);
    makeToken(t, T_ERROR, err, s, s.column);
    s.pos++;
    s.column++;
    return true;
}

/* ============================================================
   GLOBAL STATE FOR BISON PARSER
   ============================================================ */

static LexerState gState;
static bool initialized = false;

/* ============================================================
   MAP TOKEN ENUM → BISON TOKEN
   ============================================================ */
int mapToBison(TokenType t)
{
    switch (t)
    {
    case T_INT:
        return yy::parser::token::INT_TYPE;
    case T_FLOAT:
        return yy::parser::token::FLOAT_TYPE;
    case T_DOUBLE:
        return yy::parser::token::DOUBLE_TYPE;
    case T_CHAR:
        return yy::parser::token::CHAR_TYPE;
    case T_VOID:
        return yy::parser::token::VOID_TYPE;
    case T_BOOL:
        return yy::parser::token::BOOL_TYPE;
    case T_STRING:
        return yy::parser::token::STRING_TYPE;
    case T_IDENTIFIER:
        return yy::parser::token::IDENTIFIER;
    case T_INTLIT:
        return yy::parser::token::INTLIT;
    case T_FLOATLIT:
        return yy::parser::token::FLOATLIT;
    case T_STRINGLIT:
        return yy::parser::token::STRINGLIT;
    case T_CHARLIT:
        return yy::parser::token::CHARLIT;
    case T_BOOLLIT:
        return yy::parser::token::TRUE_KW;
    case T_IF:
        return yy::parser::token::IF;
    case T_ELSE:
        return yy::parser::token::ELSE;
    case T_WHILE:
        return yy::parser::token::WHILE;
    case T_RETURN:
        return yy::parser::token::RETURN;
    case T_PRINT:
        return yy::parser::token::PRINT;
    case T_MAIN:
        return yy::parser::token::MAIN;
    case T_CONST:
        return yy::parser::token::CONST;
    case T_ENUM:
        return yy::parser::token::ENUM;
    case T_DO:
        return yy::parser::token::DO;
    case T_SWITCH:
        return yy::parser::token::SWITCH;
    case T_BREAK:
        return yy::parser::token::BREAK;
    case T_FOR:
        return yy::parser::token::FOR;
    case T_DEFAULT:
        return yy::parser::token::DEFAULT;
    case T_CASE:
        return yy::parser::token::CASE;
    case T_INCLUDE:
        return yy::parser::token::INCLUDE;
    default:
        return 0;
    }
}

/* ============================================================
   yylex() ENTRY POINT (CALLED BY BISON)
   ============================================================ */
int yylex(parser::semantic_type * /*yylval*/)
{
    if (!initialized)
    {
        ostringstream buf;
        buf << cin.rdbuf();
        string code = buf.str();
        gState = createLexerState(code.c_str());
        initialized = true;
    }
    Token tok;
    if (!getNextToken(gState, tok))
        return 0; // EOF
    if (tok.type == T_EOF)
        return 0;
    return mapToBison(tok.type);
}

/* ============================================================
   NO MAIN() HERE (BISON PROVIDES ONE)
   ============================================================ */
// end of lexer.cpp
