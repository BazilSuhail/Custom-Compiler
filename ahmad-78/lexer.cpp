#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <cwctype>
#include <cstdint>
using namespace std;

enum TokenType {
    // Data types
    T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_VOID, T_BOOL,
    
    // Literals
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT, T_CHARLIT, T_BOOLLIT,
    
    // Brackets and separators
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_SEMICOLON, T_COMMA, T_DOT, T_COLON,
    
    // Operators
    T_ASSIGNOP, T_EQUALOP, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,
    T_INCREMENT, T_DECREMENT,
    T_AND, T_OR, T_NOT,
    T_BITWISE_AND, T_BITWISE_OR, T_BITWISE_XOR, T_BITWISE_NOT,
    T_LEFT_SHIFT, T_RIGHT_SHIFT,
    
    // Preprocessor
    T_INCLUDE, T_DEFINE,
    
    // Keywords
    T_IF, T_ELSE, T_WHILE, T_FOR, T_RETURN, T_BREAK, T_CONTINUE,
    T_SWITCH, T_CASE, T_DEFAULT, T_DO,
    T_CONST, T_STATIC, T_SIGNED, T_UNSIGNED,
    T_SHORT, T_LONG, T_ENUM, T_TYPEDEF,
    
    // Comments
    T_SINGLE_COMMENT, T_MULTI_COMMENT,
    
    // Special
    T_ERROR
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
};

// ✅ Static keyword map (built once, faster than re-creating every call)
static const map<string, TokenType> keywords = {
    {"int", T_INT}, {"float", T_FLOAT}, {"double", T_DOUBLE},
    {"char", T_CHAR}, {"void", T_VOID}, {"bool", T_BOOL},
    {"if", T_IF}, {"else", T_ELSE}, {"while", T_WHILE},
    {"for", T_FOR}, {"return", T_RETURN}, {"break", T_BREAK},
    {"continue", T_CONTINUE}, {"switch", T_SWITCH}, {"case", T_CASE},
    {"default", T_DEFAULT}, {"do", T_DO}, {"const", T_CONST},
    {"static", T_STATIC}, {"signed", T_SIGNED}, {"unsigned", T_UNSIGNED},
    {"short", T_SHORT}, {"long", T_LONG}, {"enum", T_ENUM},
    {"typedef", T_TYPEDEF}, {"true", T_BOOLLIT}, {"false", T_BOOLLIT},
    {"#include", T_INCLUDE}, {"#define", T_DEFINE}
};

// ✅ Static single-char tokens
static const map<char, TokenType> singleChars = {
    {'(', T_LPAREN}, {')', T_RPAREN}, {'{', T_LBRACE}, {'}', T_RBRACE},
    {'[', T_LBRACKET}, {']', T_RBRACKET}, {';', T_SEMICOLON}, {',', T_COMMA},
    {'.', T_DOT}, {':', T_COLON}, {'+', T_PLUS}, {'-', T_MINUS},
    {'*', T_MULTIPLY}, {'/', T_DIVIDE}, {'%', T_MODULO}, {'<', T_LT},
    {'>', T_GT}, {'!', T_NOT}, {'&', T_BITWISE_AND}, {'|', T_BITWISE_OR},
    {'^', T_BITWISE_XOR}, {'~', T_BITWISE_NOT}, {'=', T_ASSIGNOP}
};

// ✅ Static two-character operators
static const map<string, TokenType> twoCharOps = {
    {"==", T_EQUALOP}, {"!=", T_NE}, {"<=", T_LE}, {">=", T_GE},
    {"&&", T_AND}, {"||", T_OR}, {"++", T_INCREMENT}, {"--", T_DECREMENT},
    {"<<", T_LEFT_SHIFT}, {">>", T_RIGHT_SHIFT}
};

struct LexerState {
    const char* input;
    int pos;
    int line;
    int column;
    int inputLength;
};

// Create lexer state
LexerState createLexerState(const char* source) {
    LexerState state;
    state.input = source;
    state.pos = 0;
    state.line = 1;
    state.column = 1;
    state.inputLength = strlen(source);
    return state;
}

// --- Utility functions ---
inline bool isAsciiWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}
inline bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}
inline bool isAsciiAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}
inline bool isAsciiAlphaNumeric(char ch) {
    return isAsciiAlpha(ch) || isDigit(ch);
}
inline bool isUnicodeChar(const char* str, int pos, int len) {
    return pos < len && (str[pos] & 0x80) != 0; // Non-ASCII byte
}

// Helper to build a token
inline void makeToken(Token& token, TokenType type, const string& value, const LexerState& state, int startCol) {
    token.type = type;
    token.value = value;
    token.line = state.line;
    token.column = startCol;
}

// Skip spaces/tabs/newlines
void skipWhitespace(LexerState& state) {
    while (state.pos < state.inputLength && isAsciiWhitespace(state.input[state.pos])) {
        if (state.input[state.pos] == '\n') {
            state.line++;
            state.column = 1;
        } else {
            state.column++;
        }
        state.pos++;
    }
}

// --- Token matchers ---

// Comments
bool tryMatchComments(LexerState& state, Token& token) {
    if (state.pos + 1 >= state.inputLength) return false;
    int startCol = state.column;

    if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '/') {
        string comment = "//"; state.pos += 2; state.column += 2;
        while (state.pos < state.inputLength && state.input[state.pos] != '\n')
            comment += state.input[state.pos++], state.column++;
        if (state.pos < state.inputLength && state.input[state.pos] == '\n')
            state.line++, state.column = 1, state.pos++;
        makeToken(token, T_SINGLE_COMMENT, comment, state, startCol);
        return true;
    }
    if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '*') {
        string comment = "/*"; state.pos += 2; state.column += 2;
        while (state.pos + 1 < state.inputLength) {
            if (state.input[state.pos] == '*' && state.input[state.pos + 1] == '/') {
                comment += "*/"; state.pos += 2; state.column += 2; break;
            }
            if (state.input[state.pos] == '\n') state.line++, state.column = 1;
            else state.column++;
            comment += state.input[state.pos++];
        }
        makeToken(token, T_MULTI_COMMENT, comment, state, startCol);
        return true;
    }
    return false;
}

// Preprocessor directives (#include, #define)
bool tryMatchPreprocessor(LexerState& state, Token& token) {
    if (state.input[state.pos] != '#') return false;
    int startCol = state.column;
    string directive;
    while (state.pos < state.inputLength &&
           (isAsciiAlpha(state.input[state.pos]) || state.input[state.pos] == '#'))
        directive += state.input[state.pos++], state.column++;

    auto it = keywords.find(directive);
    if (it != keywords.end()) {
        makeToken(token, it->second, directive, state, startCol);
    } else {
        makeToken(token, T_ERROR, "Unknown directive: " + directive, state, startCol);
    }
    return true;
}

// String or char literals (unified)
bool tryMatchQuoted(LexerState& state, Token& token, char delimiter, TokenType type) {
    if (state.input[state.pos] != delimiter) return false;
    int startCol = state.column;
    string value(1, delimiter); state.pos++; state.column++;

    while (state.pos < state.inputLength && state.input[state.pos] != delimiter) {
        if (state.input[state.pos] == '\\' && state.pos + 1 < state.inputLength) {
            value += state.input[state.pos++]; value += state.input[state.pos++];
            state.column += 2;
        } else {
            if (state.input[state.pos] == '\n') state.line++, state.column = 1;
            else state.column++;
            value += state.input[state.pos++];
        }
    }
    if (state.pos < state.inputLength) {
        value += delimiter; state.pos++; state.column++;
        makeToken(token, type, value, state, startCol);
    } else {
        makeToken(token, T_ERROR, "Unterminated literal", state, startCol);
    }
    return true;
}

// Operators
bool tryMatchOperator(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength) return false;
    char ch = state.input[state.pos];
    char nextCh = (state.pos + 1 < state.inputLength) ? state.input[state.pos + 1] : '\0';
    string twoChar = string(1, ch) + nextCh;
    int startCol = state.column;

    auto it = twoCharOps.find(twoChar);
    if (it != twoCharOps.end()) {
        makeToken(token, it->second, twoChar, state, startCol);
        state.pos += 2; state.column += 2;
        return true;
    }
    auto singleIt = singleChars.find(ch);
    if (singleIt != singleChars.end()) {
        makeToken(token, singleIt->second, string(1, ch), state, startCol);
        state.pos++; state.column++;
        return true;
    }
    return false;
}

// Identifiers or numbers
bool tryMatchIdentifierOrNumber(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength) return false;
    int startCol = state.column;
    string value;

    // Numbers
    if (isDigit(state.input[state.pos])) {
        while (state.pos < state.inputLength && isAsciiAlphaNumeric(state.input[state.pos]))
            value += state.input[state.pos++], state.column++;

        bool hasAlpha = false;
        for (char c : value) if (isAsciiAlpha(c)) hasAlpha = true;
        if (hasAlpha) { makeToken(token, T_ERROR, "Invalid identifier: " + value, state, startCol); return true; }

        if (state.pos < state.inputLength &&
            (state.input[state.pos] == '.' || state.input[state.pos] == 'e' || state.input[state.pos] == 'E')) {
            bool hasDot = false, hasExp = false;
            while (state.pos < state.inputLength &&
                   (isDigit(state.input[state.pos]) || state.input[state.pos] == '.' ||
                    state.input[state.pos] == 'e' || state.input[state.pos] == 'E' ||
                    state.input[state.pos] == '+' || state.input[state.pos] == '-')) {
                if (state.input[state.pos] == '.') { if (hasDot) break; hasDot = true; }
                if (state.input[state.pos] == 'e' || state.input[state.pos] == 'E') { if (hasExp) break; hasExp = true; }
                value += state.input[state.pos++]; state.column++;
            }
            makeToken(token, T_FLOATLIT, value, state, startCol);
            return true;
        }
        makeToken(token, T_INTLIT, value, state, startCol);
        return true;
    }

    // Identifiers/keywords (including Unicode start)
    if (isAsciiAlpha(state.input[state.pos]) || isUnicodeChar(state.input, state.pos, state.inputLength)) {
        while (state.pos < state.inputLength &&
              (isAsciiAlphaNumeric(state.input[state.pos]) || isUnicodeChar(state.input, state.pos, state.inputLength)))
            value += state.input[state.pos++], state.column++;

        auto it = keywords.find(value);
        makeToken(token, it != keywords.end() ? it->second : T_IDENTIFIER, value, state, startCol);
        return true;
    }
    return false;
}

// Get next token
bool getNextToken(LexerState& state, Token& token) {
    while (state.pos < state.inputLength) {
        skipWhitespace(state);
        if (state.pos >= state.inputLength) break;
        if (tryMatchComments(state, token)) return true;
        if (tryMatchPreprocessor(state, token)) return true;
        if (tryMatchQuoted(state, token, '"', T_STRINGLIT)) return true;
        if (tryMatchQuoted(state, token, '\'', T_CHARLIT)) return true;
        if (tryMatchOperator(state, token)) return true;
        if (tryMatchIdentifierOrNumber(state, token)) return true;

        // Unknown character
        makeToken(token, T_ERROR, string("Unknown character: ") + state.input[state.pos], state, state.column);
        state.pos++; state.column++;
        return true;
    }
    return false;
}

// Map token type to string
const char* tokenTypeToString(TokenType type) {
    static const char* names[] = {
        "T_INT", "T_FLOAT", "T_DOUBLE", "T_CHAR", "T_VOID", "T_BOOL",
        "T_IDENTIFIER", "T_INTLIT", "T_FLOATLIT", "T_STRINGLIT", "T_CHARLIT", "T_BOOLLIT",
        "T_LPAREN", "T_RPAREN", "T_LBRACE", "T_RBRACE", "T_LBRACKET", "T_RBRACKET",
        "T_SEMICOLON", "T_COMMA", "T_DOT", "T_COLON",
        "T_ASSIGNOP", "T_EQUALOP", "T_NE", "T_LT", "T_GT", "T_LE", "T_GE",
        "T_PLUS", "T_MINUS", "T_MULTIPLY", "T_DIVIDE", "T_MODULO",
        "T_INCREMENT", "T_DECREMENT",
        "T_AND", "T_OR", "T_NOT",
        "T_BITWISE_AND", "T_BITWISE_OR", "T_BITWISE_XOR", "T_BITWISE_NOT",
        "T_LEFT_SHIFT", "T_RIGHT_SHIFT",
        "T_INCLUDE", "T_DEFINE",
        "T_IF", "T_ELSE", "T_WHILE", "T_FOR", "T_RETURN", "T_BREAK", "T_CONTINUE",
        "T_SWITCH", "T_CASE", "T_DEFAULT", "T_DO",
        "T_CONST", "T_STATIC", "T_SIGNED", "T_UNSIGNED",
        "T_SHORT", "T_LONG", "T_ENUM", "T_TYPEDEF",
        "T_SINGLE_COMMENT", "T_MULTI_COMMENT",
        "T_ERROR"
    };
    if (type < 0 || type >= (int)(sizeof(names)/sizeof(names[0]))) return "T_UNKNOWN";
    return names[type];
}

// Test driver
void testLexer(const string& code, const string& testName) {
    cout << "=== " << testName << " ===\n";
    LexerState state = createLexerState(code.c_str());
    Token token;
    while (getNextToken(state, token)) {
        if (token.type == T_ERROR)
            cout << "ERROR: " << token.value << " at line " << token.line << ", column " << token.column << "\n";
        else
            cout << tokenTypeToString(token.type) << "(\"" << token.value << "\"), ";
    }
    cout << "\n\n";
}

