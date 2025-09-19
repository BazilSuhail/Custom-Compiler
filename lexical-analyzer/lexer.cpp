#include <iostream>
#include <string>
#include <vector>
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

// Keyword map for O(1) lookup
map<string, TokenType> createKeywordMap() {
    return {
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
}

// Single character token mapping
map<char, TokenType> createSingleCharMap() {
    return {
        {'(', T_LPAREN}, {')', T_RPAREN}, {'{', T_LBRACE}, {'}', T_RBRACE},
        {'[', T_LBRACKET}, {']', T_RBRACKET}, {';', T_SEMICOLON},
        {',', T_COMMA}, {'.', T_DOT}, {':', T_COLON}, {'+', T_PLUS},
        {'-', T_MINUS}, {'*', T_MULTIPLY}, {'/', T_DIVIDE}, {'%', T_MODULO},
        {'<', T_LT}, {'>', T_GT}, {'!', T_NOT}, {'&', T_BITWISE_AND},
        {'|', T_BITWISE_OR}, {'^', T_BITWISE_XOR}, {'~', T_BITWISE_NOT},
        {'=', T_ASSIGNOP}
    };
}

struct LexerState {
    const char* input;
    int pos;
    int line;
    int column;
    int inputLength;
    map<string, TokenType> keywords;
    map<char, TokenType> singleChars;
};

LexerState createLexerState(const char* source) {
    LexerState state;
    state.input = source;
    state.pos = 0;
    state.line = 1;
    state.column = 1;
    state.inputLength = strlen(source);
    state.keywords = createKeywordMap();
    state.singleChars = createSingleCharMap();
    return state;
}

// Check if character is ASCII whitespace
bool isAsciiWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

// Check if character is a digit (ASCII only for now)
bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

// Check if character is ASCII alphabetic or underscore
bool isAsciiAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

// Check if character is alphanumeric (ASCII only for digits)
bool isAsciiAlphaNumeric(char ch) {
    return isAsciiAlpha(ch) || isDigit(ch);
}

// Simplified Unicode support - treat non-ASCII as valid in identifiers
bool isUnicodeChar(const char* str, int pos, int len) {
    if (pos >= len) return false;
    return (str[pos] & 0x80) != 0; // Non-ASCII byte
}

// Updated skipWhitespace: now only skips ASCII whitespace
void skipWhitespace(LexerState& state) {
    while (state.pos < state.inputLength) {
        char ch = state.input[state.pos];

        if (isAsciiWhitespace(ch)) {
            if (ch == '\n') {
                state.line++;
                state.column = 1;
            } else {
                state.column++;
            }
            state.pos++;
            continue;
        }

        // No longer skip all non-ASCII characters
        break;
    }
}

bool tryMatchComments(LexerState& state, Token& token) {
    if (state.pos + 1 >= state.inputLength) return false;
    
    if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '/') {
        // Single-line comment
        int startLine = state.line;
        int startCol = state.column;
        string comment = "//";
        state.pos += 2;
        state.column += 2;
        
        while (state.pos < state.inputLength && state.input[state.pos] != '\n') {
            comment += state.input[state.pos];
            state.pos++;
            state.column++;
        }
        
        // Consume the newline if present
        if (state.pos < state.inputLength && state.input[state.pos] == '\n') {
            state.line++;
            state.column = 1;
            state.pos++;
        }
        
        token.type = T_SINGLE_COMMENT;
        token.value = comment;
        token.line = startLine;
        token.column = startCol;
        return true;
    }
    
    if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '*') {
        // Multi-line comment
        int startLine = state.line;
        int startCol = state.column;
        string comment = "/*";
        state.pos += 2;
        state.column += 2;
        
        while (state.pos + 1 < state.inputLength) {
            if (state.input[state.pos] == '*' && state.input[state.pos + 1] == '/') {
                comment += "*/";
                state.pos += 2;
                state.column += 2;
                break;
            }
            if (state.input[state.pos] == '\n') {
                state.line++;
                state.column = 1;
            } else {
                state.column++;
            }
            comment += state.input[state.pos];
            state.pos++;
        }
        
        token.type = T_MULTI_COMMENT;
        token.value = comment;
        token.line = startLine;
        token.column = startCol;
        return true;
    }
    
    return false;
}

bool tryMatchPreprocessor(LexerState& state, Token& token) {
    if (state.input[state.pos] != '#') return false;
    
    string directive;
    int startPos = state.pos;
    int startCol = state.column;
    
    // Read the directive
    while (state.pos < state.inputLength && 
           (isAsciiAlpha(state.input[state.pos]) || state.input[state.pos] == '#')) {
        directive += state.input[state.pos];
        state.pos++;
        state.column++;
    }
    
    // Check if it's a known preprocessor directive
    if (!directive.empty()) {
        auto it = state.keywords.find(directive);
        if (it != state.keywords.end() && (it->second == T_INCLUDE || it->second == T_DEFINE)) {
            token.type = it->second;
            token.value = directive;
            token.line = state.line;
            token.column = startCol;
            return true;
        } else {
            token.type = T_ERROR;
            token.value = "Unknown preprocessor directive: " + directive;
            token.line = state.line;
            token.column = startCol;
            return true;
        }
    }
    return false;
}

bool tryMatchOperator(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength) return false;
    
    char ch = state.input[state.pos];
    char nextCh = (state.pos + 1 < state.inputLength) ? state.input[state.pos + 1] : '\0';
    
    // Two-character operators
    string twoChar(1, ch);
    twoChar += nextCh;
    
    map<string, TokenType> twoCharOps = {
        {"==", T_EQUALOP}, {"!=", T_NE}, {"<=", T_LE}, {">=", T_GE},
        {"&&", T_AND}, {"||", T_OR}, {"++", T_INCREMENT}, {"--", T_DECREMENT},
        {"<<", T_LEFT_SHIFT}, {">>", T_RIGHT_SHIFT}
    };
    
    auto it = twoCharOps.find(twoChar);
    if (it != twoCharOps.end()) {
        token.type = it->second;
        token.value = twoChar;
        token.line = state.line;
        token.column = state.column;
        state.pos += 2;
        state.column += 2;
        return true;
    }
    
    // Single character operators
    auto singleIt = state.singleChars.find(ch);
    if (singleIt != state.singleChars.end()) {
        token.type = singleIt->second;
        token.value = ch;
        token.line = state.line;
        token.column = state.column;
        state.pos++;
        state.column++;
        return true;
    }
    
    return false;
}

bool tryMatchString(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength || state.input[state.pos] != '"') return false;
    
    int startCol = state.column;
    string value;
    value += '"';
    state.pos++;
    state.column++;
    
    while (state.pos < state.inputLength && state.input[state.pos] != '"') {
        if (state.input[state.pos] == '\\') {
            // Handle escape sequences
            value += state.input[state.pos];
            state.column++;
            state.pos++;
            
            if (state.pos < state.inputLength) {
                value += state.input[state.pos];
                state.column++;
                state.pos++;
            }
        } else {
            if (state.input[state.pos] == '\n') {
                state.line++;
                state.column = 1;
            } else {
                state.column++;
            }
            value += state.input[state.pos];
            state.pos++;
        }
    }
    
    if (state.pos < state.inputLength) {
        value += '"';
        state.pos++;
        state.column++;
        token.value = value;
        token.type = T_STRINGLIT;
        token.line = state.line;
        token.column = startCol;
        return true;
    } else {
        // Unterminated string
        token.type = T_ERROR;
        token.value = "Unterminated string";
        token.line = state.line;
        token.column = startCol;
        return true;
    }
}

bool tryMatchCharLiteral(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength || state.input[state.pos] != '\'') return false;
    
    int startCol = state.column;
    string value;
    value += '\'';
    state.pos++;
    state.column++;
    
    if (state.pos < state.inputLength) {
        if (state.input[state.pos] == '\\') {
            // Escape sequence
            value += state.input[state.pos];
            state.column++;
            state.pos++;
            
            if (state.pos < state.inputLength) {
                value += state.input[state.pos];
                state.column++;
                state.pos++;
            }
        } else {
            value += state.input[state.pos];
            state.column++;
            state.pos++;
        }
    }
    
    if (state.pos < state.inputLength && state.input[state.pos] == '\'') {
        value += '\'';
        state.pos++;
        state.column++;
        token.value = value;
        token.type = T_CHARLIT;
        token.line = state.line;
        token.column = startCol;
        return true;
    } else {
        // Unterminated character literal
        token.type = T_ERROR;
        token.value = "Unterminated char literal";
        token.line = state.line;
        token.column = startCol;
        return true;
    }
}

bool tryMatchIdentifierOrNumber(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength) return false;
    
    int startCol = state.column;
    string value;
    
    // If it starts with a digit, it must be a number
    if (isDigit(state.input[state.pos])) {
        // Read all digits and letters
        while (state.pos < state.inputLength && isAsciiAlphaNumeric(state.input[state.pos])) {
            value += state.input[state.pos];
            state.pos++;
            state.column++;
        }
        
        // Check if it contains letters (invalid identifier)
        bool hasAlpha = false;
        for (char c : value) {
            if (isAsciiAlpha(c)) {
                hasAlpha = true;
                break;
            }
        }
        
        if (hasAlpha) {
            // Invalid identifier starting with digit
            token.type = T_ERROR;
            token.value = "Invalid identifier: " + value;
            token.line = state.line;
            token.column = startCol;
            return true;
        }
        
        // Check if it's a float
        if (state.pos < state.inputLength && 
            (state.input[state.pos] == '.' || 
             state.input[state.pos] == 'e' || 
             state.input[state.pos] == 'E')) {
            // Continue reading float
            bool hasDot = false;
            bool hasExponent = false;
            
            while (state.pos < state.inputLength && 
                   (isDigit(state.input[state.pos]) || 
                    state.input[state.pos] == '.' || 
                    state.input[state.pos] == 'e' || 
                    state.input[state.pos] == 'E' ||
                    state.input[state.pos] == '+' || 
                    state.input[state.pos] == '-')) {
                if (state.input[state.pos] == '.') {
                    if (hasDot) break; // Second dot is invalid
                    hasDot = true;
                }
                if (state.input[state.pos] == 'e' || state.input[state.pos] == 'E') {
                    if (hasExponent) break; // Second exponent is invalid
                    hasExponent = true;
                }
                value += state.input[state.pos];
                state.pos++;
                state.column++;
            }
            
            token.value = value;
            token.type = T_FLOATLIT;
            token.line = state.line;
            token.column = startCol;
            return true;
        }
        
        // It's an integer
        token.value = value;
        token.type = T_INTLIT;
        token.line = state.line;
        token.column = startCol;
        return true;
    }
    
    // If it starts with alpha, underscore, or Unicode, it's an identifier
    if (isAsciiAlpha(state.input[state.pos]) || isUnicodeChar(state.input, state.pos, state.inputLength)) {
        while (state.pos < state.inputLength && 
               (isAsciiAlphaNumeric(state.input[state.pos]) || 
                isUnicodeChar(state.input, state.pos, state.inputLength))) {
            value += state.input[state.pos];
            state.pos++;
            state.column++;
        }
        
        // Check if it's a keyword
        auto it = state.keywords.find(value);
        if (it != state.keywords.end()) {
            token.type = it->second;
        } else {
            token.type = T_IDENTIFIER;
        }
        token.value = value;
        token.line = state.line;
        token.column = startCol;
        return true;
    }
    
    return false;
}

bool getNextToken(LexerState& state, Token& token) {
    while (state.pos < state.inputLength) {
        skipWhitespace(state);
        
        if (state.pos >= state.inputLength) break;
        
        // Try to match comments first
        if (tryMatchComments(state, token)) return true;
        
        // Try to match various token types
        if (tryMatchPreprocessor(state, token)) return true;
        if (tryMatchString(state, token)) return true;
        if (tryMatchCharLiteral(state, token)) return true;
        if (tryMatchOperator(state, token)) return true;
        if (tryMatchIdentifierOrNumber(state, token)) return true;
        
        // If we get here, it's an unknown character
        token.type = T_ERROR;
        token.value = "Unknown character: ";
        token.value += state.input[state.pos];
        token.line = state.line;
        token.column = state.column;
        state.pos++;
        state.column++;
        return true;
    }
    
    return false; // End of input
}

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

    if (type < 0 || type >= (int)(sizeof(names)/sizeof(names[0]))) {
        return "T_UNKNOWN";
    }
    return names[type];
}


void testLexer(const string& code, const string& testName) {
    cout << "=== " << testName << " ===\n";
    LexerState state = createLexerState(code.c_str());
    Token token;
    
    while (getNextToken(state, token)) {
        if (token.type == T_ERROR) {
            cout << "ERROR: " << token.value << " at line " << token.line << ", column " << token.column << "\n";
        } else {
            cout << tokenTypeToString(token.type) << "(\"" << token.value << "\"), ";
        }
    }
    cout << "\n\n";
}

int main() {
    string loopTest = R"(
    
    #include <stdio.h>
    #define MAX 100

    int add(int a, int b) {
        return a + b;
    }

    int main() { 
        int x = 42;
        float y = 3.14159f;

        // Unicode identifiers
        int π = 3.14159;
        int 🌍 = 3.14159;
        float résumé = 2.5f;
        char 你好 = 'A';
        string message = "Hello World! 🌍✨ 💕🙌";
        
        // This is a comment with emojis: 🚀🌟
        /* Multi-line comment with Unicode: Привет мир! 🌎 */
        
        if (π > 0) {
            printf("Value: %f\n", π);
        } else if (x == 0 || x >= MAX) {
            printf("Boundary value\n");
        }
        
        for (int i = 0; i < 10; i++) {
            x += i;
            x++;
        }
        
        while (x > 0) {
            x >>= 1;  // Right shift 
        }

        switch(value) {
            case 1:
                printf("One\n");
                break;
            default:
                printf("Other\n");
                break;
        }
        int 123val = 55;

        return 0;
    }
    )";    
    testLexer(loopTest, "Demo Code");

    return 0;
}