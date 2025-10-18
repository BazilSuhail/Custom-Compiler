#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
using namespace std;

// Include Bison-generated header
#include "parser.tab.hpp"

// === Token structure ===
struct Token {
    int type;
    string value;
    int line;
    int column;
};

// === Keyword map ===
static const map<string, int> keywords = {
    {"int", T_INT}, {"float", T_FLOAT}, {"double", T_DOUBLE},
    {"char", T_CHAR}, {"void", T_VOID}, {"bool", T_BOOL},
    {"true", T_BOOLLIT}, {"false", T_BOOLLIT},
    {"if", T_IF}, {"else", T_ELSE}, {"while", T_WHILE}, {"return", T_RETURN},
    {"print", T_PRINT}, {"main", T_MAIN}, {"string", T_STRING}, {"do", T_DO},
    {"switch", T_SWITCH}, {"break", T_BREAK}, {"for", T_FOR},
    {"default", T_DEFAULT}, {"case", T_CASE}, {"const", T_CONST},
    {"enum", T_ENUM}
};

// === Single-character tokens ===
static const map<char, int> singleChars = {
    {'(', T_LPAREN}, {')', T_RPAREN}, {'{', T_LBRACE}, {'}', T_RBRACE},
    {'[', T_LBRACKET}, {']', T_RBRACKET}, {';', T_SEMICOLON},
    {',', T_COMMA}, {'.', T_DOT}, {'+', T_PLUS}, {'-', T_MINUS},
    {':', T_COLON}, {'*', T_MULTIPLY}, {'/', T_DIVIDE}, {'%', T_MODULO},
    {'=', T_ASSIGNOP}, {'!', T_NOT}, {'<', T_LT}, {'>', T_GT}
};

// === Two-character operators ===
static const map<string, int> twoCharOps = {
    {"==", T_EQUALOP}, {"!=", T_NE}, {"<=", T_LE}, {">=", T_GE},
    {"&&", T_AND}, {"||", T_OR}, {"++", T_INCREMENT}, {"--", T_DECREMENT}
};

// === Lexer state ===
struct LexerState {
    const char* input;
    int pos;
    int line;
    int column;
    int inputLength;
};

LexerState createLexerState(const char* source) {
    LexerState s{source, 0, 1, 1, (int)strlen(source)};
    return s;
}

inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
inline bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
inline bool isAlnum(char c) { return isAlpha(c) || isDigit(c); }

void skipSpace(LexerState& s) {
    while (s.pos < s.inputLength && isSpace(s.input[s.pos])) {
        if (s.input[s.pos] == '\n') {
            s.line++;
            s.column = 1;
        } else {
            s.column++;
        }
        s.pos++;
    }
}

void makeToken(Token& t, int type, const string& val, const LexerState& s, int col) {
    t.type = type;
    t.value = val;
    t.line = s.line;
    t.column = col;
}

// === Match comments ===
bool tryComment(LexerState& s, Token& t) {
    if (s.pos + 1 >= s.inputLength) return false;
    int c = s.column;

    if (s.input[s.pos] == '/' && s.input[s.pos + 1] == '/') {
        string text = "//";
        s.pos += 2;
        s.column += 2;
        while (s.pos < s.inputLength && s.input[s.pos] != '\n') {
            text += s.input[s.pos++];
            s.column++;
        }
        makeToken(t, T_SINGLE_COMMENT, text, s, c);
        return true;
    }

    if (s.input[s.pos] == '/' && s.input[s.pos + 1] == '*') {
        string text = "/*";
        s.pos += 2;
        s.column += 2;
        while (s.pos + 1 < s.inputLength) {
            if (s.input[s.pos] == '*' && s.input[s.pos + 1] == '/') {
                text += "*/";
                s.pos += 2;
                s.column += 2;
                makeToken(t, T_MULTI_COMMENT, text, s, c);
                return true;
            }
            if (s.input[s.pos] == '\n') {
                s.line++;
                s.column = 1;
            } else {
                s.column++;
            }
            text += s.input[s.pos++];
        }
        makeToken(t, T_ERROR, "Unterminated comment", s, c);
        return true;
    }
    return false;
}

// === Match quoted literals ===
bool tryQuoted(LexerState& s, Token& t) {
    if (s.pos >= s.inputLength) return false;
    char q = s.input[s.pos];
    if (q != '"' && q != '\'') return false;

    int c = s.column;
    string val(1, q);
    s.pos++;
    s.column++;

    while (s.pos < s.inputLength && s.input[s.pos] != q) {
        if (s.input[s.pos] == '\\' && s.pos + 1 < s.inputLength) {
            val += s.input[s.pos++];
            val += s.input[s.pos++];
            s.column += 2;
        } else {
            val += s.input[s.pos++];
            s.column++;
        }
    }

    if (s.pos < s.inputLength) {
        val += q;
        s.pos++;
        s.column++;
        makeToken(t, q == '"' ? T_STRINGLIT : T_CHARLIT, val, s, c);
    } else {
        makeToken(t, T_ERROR, "Unterminated literal", s, c);
    }

    return true;
}

// === Match operators ===
bool tryOperator(LexerState& s, Token& t) {
    if (s.pos >= s.inputLength) return false;
    int c = s.column;

    string two;
    two += s.input[s.pos];
    if (s.pos + 1 < s.inputLength) two += s.input[s.pos + 1];

    auto it2 = twoCharOps.find(two);
    if (it2 != twoCharOps.end()) {
        makeToken(t, it2->second, two, s, c);
        s.pos += 2;
        s.column += 2;
        return true;
    }

    auto it1 = singleChars.find(s.input[s.pos]);
    if (it1 != singleChars.end()) {
        makeToken(t, it1->second, string(1, s.input[s.pos]), s, c);
        s.pos++;
        s.column++;
        return true;
    }

    return false;
}

// === Match identifier or number ===
bool tryIdOrNum(LexerState& s, Token& t) {
    if (s.pos >= s.inputLength) return false;
    int c = s.column;
    string v;

    if (isDigit(s.input[s.pos])) {
        while (s.pos < s.inputLength && (isDigit(s.input[s.pos]) || s.input[s.pos] == '.')) {
            v += s.input[s.pos++];
            s.column++;
        }
        makeToken(t, (v.find('.') != string::npos) ? T_FLOATLIT : T_INTLIT, v, s, c);
        return true;
    }

    if (isAlpha(s.input[s.pos])) {
        while (s.pos < s.inputLength && isAlnum(s.input[s.pos])) {
            v += s.input[s.pos++];
            s.column++;
        }
        auto it = keywords.find(v);
        makeToken(t, (it != keywords.end()) ? it->second : T_IDENTIFIER, v, s, c);
        return true;
    }

    return false;
}

// === Get next token ===
bool getNextToken(LexerState& s, Token& t) {
    skipSpace(s);
    if (s.pos >= s.inputLength) {
        t.type = T_EOF;
        t.value = "";
        return false;
    }

    int c = s.column;
    if (tryComment(s, t)) return true;
    if (tryQuoted(s, t)) return true;
    if (tryOperator(s, t)) return true;
    if (tryIdOrNum(s, t)) return true;

    string err = "Unexpected char: " + string(1, s.input[s.pos]);
    makeToken(t, T_ERROR, err, s, c);
    s.pos++;
    s.column++;
    return true;
}

// === Globals for Bison ===
extern YYSTYPE yylval;
static LexerState lexState;
static bool initialized = false;

void initLexer(const string& code) {
    lexState = createLexerState(code.c_str());
    initialized = true;
}

int yylex() {
    if (!initialized) return 0;
    Token t;
    if (!getNextToken(lexState, t)) return 0;
    yylval.str = strdup(t.value.c_str());
    return t.type;
}

int main() {
    ifstream file("test.code");
    if (!file.is_open()) {
        cerr << "Could not open test.code" << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();

    initLexer(code);
    cout << "Starting parse..." << endl;

    if (yyparse() == 0)
        cout << "Parsing completed successfully." << endl;
    else
        cout << "Parsing failed." << endl;

    return 0;
}
