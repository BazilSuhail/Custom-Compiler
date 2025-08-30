#include <iostream>
#include <string>
using namespace std;

enum TokenType {
    // Data types
    T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_VOID, T_BOOL,
    
    // Literals
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT, T_CHARLIT, T_BOOLLIT,
    
    // Brackets and separators
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_SEMICOLON, T_COMMA, T_DOT,
    
    // Operators
    T_ASSIGNOP, T_EQUALOP, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,
    T_INCREMENT, T_DECREMENT,
    T_AND, T_OR, T_NOT,
    T_BITWISE_AND, T_BITWISE_OR, T_BITWISE_XOR, T_BITWISE_NOT,
    T_LEFT_SHIFT, T_RIGHT_SHIFT,
    
    // Preprocessor
    T_INCLUDE, T_DEFINE, T_PREPROCESSOR,
    
    // Keywords
    T_IF, T_ELSE, T_WHILE, T_FOR, T_RETURN, T_BREAK, T_CONTINUE,
    T_SWITCH, T_CASE, T_DEFAULT, T_DO, T_GOTO,
    T_CONST, T_STATIC, T_EXTERN, T_VOLATILE, T_SIGNED, T_UNSIGNED,
    T_SHORT, T_LONG, T_STRUCT, T_UNION, T_ENUM, T_TYPEDEF,
    T_SIZEOF, T_INLINE, T_AUTO, T_REGISTER,
    
    // Special
    T_EOF, T_UNKNOWN, T_ERROR
};

struct Token {
    TokenType type;
    char value[256];  // Fixed size array instead of string
    int line;
    int column;
};

// Simple fixed-size array implementation
template<typename T, int SIZE>
struct SimpleArray {
    T data[SIZE];
    int size;
    
    SimpleArray() : size(0) {}
    
    void push_back(const T& item) {
        if (size < SIZE) {
            data[size] = item;
            size++;
        }
    }
    
    T& operator[](int index) {
        return data[index];
    }
    
    int getSize() const {
        return size;
    }
};

// Simple hash table for keywords
struct KeywordTable {
    struct KeywordEntry {
        char word[32];
        TokenType type;
    };
    
    KeywordEntry entries[64];
    int count;
    
    KeywordTable() : count(0) {
        addKeyword("int", T_INT);
        addKeyword("float", T_FLOAT);
        addKeyword("double", T_DOUBLE);
        addKeyword("char", T_CHAR);
        addKeyword("void", T_VOID);
        addKeyword("bool", T_BOOL);
        addKeyword("if", T_IF);
        addKeyword("else", T_ELSE);
        addKeyword("while", T_WHILE);
        addKeyword("for", T_FOR);
        addKeyword("return", T_RETURN);
        addKeyword("break", T_BREAK);
        addKeyword("continue", T_CONTINUE);
        addKeyword("switch", T_SWITCH);
        addKeyword("case", T_CASE);
        addKeyword("default", T_DEFAULT);
        addKeyword("do", T_DO);
        addKeyword("goto", T_GOTO);
        addKeyword("const", T_CONST);
        addKeyword("static", T_STATIC);
        addKeyword("extern", T_EXTERN);
        addKeyword("volatile", T_VOLATILE);
        addKeyword("signed", T_SIGNED);
        addKeyword("unsigned", T_UNSIGNED);
        addKeyword("short", T_SHORT);
        addKeyword("long", T_LONG);
        addKeyword("struct", T_STRUCT);
        addKeyword("union", T_UNION);
        addKeyword("enum", T_ENUM);
        addKeyword("typedef", T_TYPEDEF);
        addKeyword("sizeof", T_SIZEOF);
        addKeyword("inline", T_INLINE);
        addKeyword("auto", T_AUTO);
        addKeyword("register", T_REGISTER);
        addKeyword("true", T_BOOLLIT);
        addKeyword("false", T_BOOLLIT);
        addKeyword("#include", T_INCLUDE);
        addKeyword("#define", T_DEFINE);
    }
    
    void addKeyword(const char* word, TokenType type) {
        if (count < 64) {
            int i = 0;
            while (word[i] != '\0' && i < 31) {
                entries[count].word[i] = word[i];
                i++;
            }
            entries[count].word[i] = '\0';
            entries[count].type = type;
            count++;
        }
    }
    
    TokenType findKeyword(const char* word) {
        for (int i = 0; i < count; i++) {
            bool match = true;
            int j = 0;
            while (word[j] != '\0' && entries[i].word[j] != '\0') {
                if (word[j] != entries[i].word[j]) {
                    match = false;
                    break;
                }
                j++;
            }
            if (match && word[j] == '\0' && entries[i].word[j] == '\0') {
                return entries[i].type;
            }
        }
        return T_IDENTIFIER;
    }
};

struct LexerState {
    const char* input;
    int pos;
    int line;
    int column;
    KeywordTable keywords;
    int inputLength;
};

LexerState createLexerState(const char* source) {
    LexerState state;
    state.input = source;
    state.pos = 0;
    state.line = 1;
    state.column = 1;
    state.inputLength = 0;
    
    // Calculate input length
    while (source[state.inputLength] != '\0') {
        state.inputLength++;
    }
    
    return state;
}

bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool isAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool isAlphaNumeric(char ch) {
    return isAlpha(ch) || isDigit(ch);
}

void skipWhitespace(LexerState& state) {
    while (state.pos < state.inputLength && isWhitespace(state.input[state.pos])) {
        if (state.input[state.pos] == '\n') {
            state.line++;
            state.column = 1;
        } else {
            state.column++;
        }
        state.pos++;
    }
}

void skipComments(LexerState& state) {
    if (state.pos + 1 < state.inputLength) {
        if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '/') {
            // Single-line comment
            while (state.pos < state.inputLength && state.input[state.pos] != '\n') {
                state.pos++;
                state.column++;
            }
        } else if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '*') {
            // Multi-line comment
            state.pos += 2;
            state.column += 2;
            while (state.pos + 1 < state.inputLength) {
                if (state.input[state.pos] == '*' && state.input[state.pos + 1] == '/') {
                    state.pos += 2;
                    state.column += 2;
                    return;
                }
                if (state.input[state.pos] == '\n') {
                    state.line++;
                    state.column = 1;
                } else {
                    state.column++;
                }
                state.pos++;
            }
        }
    }
}

TokenType getSingleCharType(char ch) {
    switch (ch) {
        case '(': return T_LPAREN;
        case ')': return T_RPAREN;
        case '{': return T_LBRACE;
        case '}': return T_RBRACE;
        case '[': return T_LBRACKET;
        case ']': return T_RBRACKET;
        case ';': return T_SEMICOLON;
        case ',': return T_COMMA;
        case '.': return T_DOT;
        case '+': return T_PLUS;
        case '-': return T_MINUS;
        case '*': return T_MULTIPLY;
        case '/': return T_DIVIDE;
        case '%': return T_MODULO;
        case '<': return T_LT;
        case '>': return T_GT;
        case '!': return T_NOT;
        case '&': return T_BITWISE_AND;
        case '|': return T_BITWISE_OR;
        case '^': return T_BITWISE_XOR;
        case '~': return T_BITWISE_NOT;
        case '=': return T_ASSIGNOP;
        default: return T_UNKNOWN;
    }
}

bool tryMatchPreprocessor(LexerState& state, Token& token) {
    if (state.input[state.pos] == '#') {
        char directive[32];
        int len = 0;
        int startPos = state.pos;
        int startCol = state.column;
        
        // Read the directive
        while (state.pos < state.inputLength && 
               (isAlpha(state.input[state.pos]) || state.input[state.pos] == '#') &&
               len < 31) {
            directive[len] = state.input[state.pos];
            len++;
            state.pos++;
            state.column++;
        }
        directive[len] = '\0';
        
        // Check if it's a known preprocessor directive
        if (len > 0) {
            if (len >= 8 && directive[1] == 'i' && directive[2] == 'n' && directive[3] == 'c' && 
                directive[4] == 'l' && directive[5] == 'u' && directive[6] == 'd' && directive[7] == 'e') {
                token.type = T_INCLUDE;
            } else if (len >= 7 && directive[1] == 'd' && directive[2] == 'e' && directive[3] == 'f' && 
                      directive[4] == 'i' && directive[5] == 'n' && directive[6] == 'e') {
                token.type = T_DEFINE;
            } else {
                token.type = T_PREPROCESSOR;
            }
            
            // Copy directive to token value
            for (int i = 0; i < len && i < 255; i++) {
                token.value[i] = directive[i];
            }
            token.value[len] = '\0';
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
    if (nextCh != '\0') {
        if (ch == '=' && nextCh == '=') {
            token.type = T_EQUALOP;
            token.value[0] = '=';
            token.value[1] = '=';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '!' && nextCh == '=') {
            token.type = T_NE;
            token.value[0] = '!';
            token.value[1] = '=';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '<' && nextCh == '=') {
            token.type = T_LE;
            token.value[0] = '<';
            token.value[1] = '=';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '>' && nextCh == '=') {
            token.type = T_GE;
            token.value[0] = '>';
            token.value[1] = '=';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '&' && nextCh == '&') {
            token.type = T_AND;
            token.value[0] = '&';
            token.value[1] = '&';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '|' && nextCh == '|') {
            token.type = T_OR;
            token.value[0] = '|';
            token.value[1] = '|';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '+' && nextCh == '+') {
            token.type = T_INCREMENT;
            token.value[0] = '+';
            token.value[1] = '+';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '-' && nextCh == '-') {
            token.type = T_DECREMENT;
            token.value[0] = '-';
            token.value[1] = '-';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '<' && nextCh == '<') {
            token.type = T_LEFT_SHIFT;
            token.value[0] = '<';
            token.value[1] = '<';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '>' && nextCh == '>') {
            token.type = T_RIGHT_SHIFT;
            token.value[0] = '>';
            token.value[1] = '>';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '+' && nextCh == '=') {
            token.type = T_ASSIGNOP; // Actually += but we'll keep it simple
            token.value[0] = '+';
            token.value[1] = '=';
            token.value[2] = '\0';
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        }
    }
    
    // Single character operators
    TokenType type = getSingleCharType(ch);
    if (type != T_UNKNOWN) {
        token.type = type;
        token.value[0] = ch;
        token.value[1] = '\0';
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
    
    int startPos = state.pos;
    int startCol = state.column;
    int valuePos = 0;
    
    token.value[valuePos] = '"';
    valuePos++;
    state.pos++;
    state.column++;
    
    while (state.pos < state.inputLength && state.input[state.pos] != '"' && valuePos < 254) {
        if (state.input[state.pos] == '\\') {
            // Handle escape sequences
            token.value[valuePos] = state.input[state.pos];
            valuePos++;
            state.column++;
            state.pos++;
            
            if (state.pos < state.inputLength && valuePos < 254) {
                token.value[valuePos] = state.input[state.pos];
                valuePos++;
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
            token.value[valuePos] = state.input[state.pos];
            valuePos++;
            state.pos++;
        }
    }
    
    if (state.pos < state.inputLength && valuePos < 254) {
        token.value[valuePos] = '"';
        valuePos++;
        state.pos++;
        state.column++;
        token.value[valuePos] = '\0';
        token.type = T_STRINGLIT;
        token.line = state.line;
        token.column = startCol;
        return true;
    } else {
        // Unterminated string
        token.type = T_ERROR;
        token.value[0] = 'U';
        token.value[1] = 'n';
        token.value[2] = 't';
        token.value[3] = 'e';
        token.value[4] = 'r';
        token.value[5] = 'm';
        token.value[6] = 'i';
        token.value[7] = 'n';
        token.value[8] = 'a';
        token.value[9] = 't';
        token.value[10] = 'e';
        token.value[11] = 'd';
        token.value[12] = ' ';
        token.value[13] = 's';
        token.value[14] = 't';
        token.value[15] = 'r';
        token.value[16] = 'i';
        token.value[17] = 'n';
        token.value[18] = 'g';
        token.value[19] = '\0';
        token.line = state.line;
        token.column = startCol;
        return true;
    }
}

bool tryMatchCharLiteral(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength || state.input[state.pos] != '\'') return false;
    
    int startCol = state.column;
    int valuePos = 0;
    
    token.value[valuePos] = '\'';
    valuePos++;
    state.pos++;
    state.column++;
    
    if (state.pos < state.inputLength && valuePos < 254) {
        if (state.input[state.pos] == '\\') {
            // Escape sequence
            token.value[valuePos] = state.input[state.pos];
            valuePos++;
            state.column++;
            state.pos++;
            
            if (state.pos < state.inputLength && valuePos < 254) {
                token.value[valuePos] = state.input[state.pos];
                valuePos++;
                state.column++;
                state.pos++;
            }
        } else {
            token.value[valuePos] = state.input[state.pos];
            valuePos++;
            state.column++;
            state.pos++;
        }
    }
    
    if (state.pos < state.inputLength && state.input[state.pos] == '\'' && valuePos < 253) {
        token.value[valuePos] = '\'';
        valuePos++;
        state.pos++;
        state.column++;
        token.value[valuePos] = '\0';
        token.type = T_CHARLIT;
        token.line = state.line;
        token.column = startCol;
        return true;
    } else {
        // Unterminated character literal
        token.type = T_ERROR;
        token.value[0] = 'U';
        token.value[1] = 'n';
        token.value[2] = 't';
        token.value[3] = 'e';
        token.value[4] = 'r';
        token.value[5] = 'm';
        token.value[6] = 'i';
        token.value[7] = 'n';
        token.value[8] = 'a';
        token.value[9] = 't';
        token.value[10] = 'e';
        token.value[11] = 'd';
        token.value[12] = ' ';
        token.value[13] = 'c';
        token.value[14] = 'h';
        token.value[15] = 'a';
        token.value[16] = 'r';
        token.value[17] = '\0';
        token.line = state.line;
        token.column = startCol;
        return true;
    }
}

bool tryMatchFloat(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength || !isDigit(state.input[state.pos])) return false;
    
    int startPos = state.pos;
    int startCol = state.column;
    bool hasDot = false;
    bool hasExponent = false;
    
    // Check if it's a float (contains . or e/E)
    int tempPos = state.pos;
    while (tempPos < state.inputLength && 
           (isDigit(state.input[tempPos]) || 
            state.input[tempPos] == '.' || 
            state.input[tempPos] == 'e' || 
            state.input[tempPos] == 'E' ||
            state.input[tempPos] == '+' || 
            state.input[tempPos] == '-')) {
        if (state.input[tempPos] == '.') hasDot = true;
        if (state.input[tempPos] == 'e' || state.input[tempPos] == 'E') hasExponent = true;
        tempPos++;
    }
    
    // If it doesn't have dot or exponent, it's not a float
    if (!hasDot && !hasExponent) return false;
    
    // Copy the float literal
    int valuePos = 0;
    while (state.pos < tempPos && valuePos < 254) {
        token.value[valuePos] = state.input[state.pos];
        valuePos++;
        state.pos++;
        state.column++;
    }
    token.value[valuePos] = '\0';
    token.type = T_FLOATLIT;
    token.line = state.line;
    token.column = startCol;
    return true;
}

bool tryMatchInteger(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength || !isDigit(state.input[state.pos])) return false;
    
    int startPos = state.pos;
    int startCol = state.column;
    
    // Read digits
    while (state.pos < state.inputLength && isDigit(state.input[state.pos])) {
        state.pos++;
        state.column++;
    }
    
    // Copy integer literal
    int valuePos = 0;
    int tempPos = startPos;
    while (tempPos < state.pos && valuePos < 254) {
        token.value[valuePos] = state.input[tempPos];
        valuePos++;
        tempPos++;
    }
    token.value[valuePos] = '\0';
    token.type = T_INTLIT;
    token.line = state.line;
    token.column = startCol;
    return true;
}

bool tryMatchIdentifier(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength || !isAlpha(state.input[state.pos])) return false;
    
    int startPos = state.pos;
    int startCol = state.column;
    
    // Read identifier
    while (state.pos < state.inputLength && isAlphaNumeric(state.input[state.pos])) {
        state.pos++;
        state.column++;
    }
    
    // Copy identifier
    int valuePos = 0;
    int tempPos = startPos;
    while (tempPos < state.pos && valuePos < 254) {
        token.value[valuePos] = state.input[tempPos];
        valuePos++;
        tempPos++;
    }
    token.value[valuePos] = '\0';
    
    // Check if it's a keyword
    token.type = state.keywords.findKeyword(token.value);
    token.line = state.line;
    token.column = startCol;
    return true;
}

bool getNextToken(LexerState& state, Token& token) {
    while (state.pos < state.inputLength) {
        skipWhitespace(state);
        skipComments(state);
        
        if (state.pos >= state.inputLength) break;
        
        // Try to match various token types
        if (tryMatchPreprocessor(state, token)) return true;
        if (tryMatchString(state, token)) return true;
        if (tryMatchCharLiteral(state, token)) return true;
        if (tryMatchOperator(state, token)) return true;
        if (tryMatchFloat(state, token)) return true;
        if (tryMatchInteger(state, token)) return true;
        if (tryMatchIdentifier(state, token)) return true;
        
        // If we get here, it's an unknown character
        token.type = T_UNKNOWN;
        token.value[0] = state.input[state.pos];
        token.value[1] = '\0';
        token.line = state.line;
        token.column = state.column;
        state.pos++;
        state.column++;
        return true;
    }
    
    // End of file
    token.type = T_EOF;
    token.value[0] = '\0';
    token.line = state.line;
    token.column = state.column;
    return true;
}

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case T_INT: return "T_INT";
        case T_FLOAT: return "T_FLOAT";
        case T_DOUBLE: return "T_DOUBLE";
        case T_CHAR: return "T_CHAR";
        case T_VOID: return "T_VOID";
        case T_BOOL: return "T_BOOL";
        case T_IDENTIFIER: return "T_IDENTIFIER";
        case T_INTLIT: return "T_INTLIT";
        case T_FLOATLIT: return "T_FLOATLIT";
        case T_STRINGLIT: return "T_STRINGLIT";
        case T_CHARLIT: return "T_CHARLIT";
        case T_BOOLLIT: return "T_BOOLLIT";
        case T_LPAREN: return "T_LPAREN";
        case T_RPAREN: return "T_RPAREN";
        case T_LBRACE: return "T_LBRACE";
        case T_RBRACE: return "T_RBRACE";
        case T_LBRACKET: return "T_LBRACKET";
        case T_RBRACKET: return "T_RBRACKET";
        case T_SEMICOLON: return "T_SEMICOLON";
        case T_COMMA: return "T_COMMA";
        case T_DOT: return "T_DOT";
        case T_ASSIGNOP: return "T_ASSIGNOP";
        case T_EQUALOP: return "T_EQUALOP";
        case T_NE: return "T_NE";
        case T_LT: return "T_LT";
        case T_GT: return "T_GT";
        case T_LE: return "T_LE";
        case T_GE: return "T_GE";
        case T_PLUS: return "T_PLUS";
        case T_MINUS: return "T_MINUS";
        case T_MULTIPLY: return "T_MULTIPLY";
        case T_DIVIDE: return "T_DIVIDE";
        case T_MODULO: return "T_MODULO";
        case T_INCREMENT: return "T_INCREMENT";
        case T_DECREMENT: return "T_DECREMENT";
        case T_AND: return "T_AND";
        case T_OR: return "T_OR";
        case T_NOT: return "T_NOT";
        case T_BITWISE_AND: return "T_BITWISE_AND";
        case T_BITWISE_OR: return "T_BITWISE_OR";
        case T_BITWISE_XOR: return "T_BITWISE_XOR";
        case T_BITWISE_NOT: return "T_BITWISE_NOT";
        case T_LEFT_SHIFT: return "T_LEFT_SHIFT";
        case T_RIGHT_SHIFT: return "T_RIGHT_SHIFT";
        case T_INCLUDE: return "T_INCLUDE";
        case T_DEFINE: return "T_DEFINE";
        case T_PREPROCESSOR: return "T_PREPROCESSOR";
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_FOR: return "T_FOR";
        case T_RETURN: return "T_RETURN";
        case T_BREAK: return "T_BREAK";
        case T_CONTINUE: return "T_CONTINUE";
        case T_SWITCH: return "T_SWITCH";
        case T_CASE: return "T_CASE";
        case T_DEFAULT: return "T_DEFAULT";
        case T_DO: return "T_DO";
        case T_GOTO: return "T_GOTO";
        case T_CONST: return "T_CONST";
        case T_STATIC: return "T_STATIC";
        case T_EXTERN: return "T_EXTERN";
        case T_VOLATILE: return "T_VOLATILE";
        case T_SIGNED: return "T_SIGNED";
        case T_UNSIGNED: return "T_UNSIGNED";
        case T_SHORT: return "T_SHORT";
        case T_LONG: return "T_LONG";
        case T_STRUCT: return "T_STRUCT";
        case T_UNION: return "T_UNION";
        case T_ENUM: return "T_ENUM";
        case T_TYPEDEF: return "T_TYPEDEF";
        case T_SIZEOF: return "T_SIZEOF";
        case T_INLINE: return "T_INLINE";
        case T_AUTO: return "T_AUTO";
        case T_REGISTER: return "T_REGISTER";
        case T_EOF: return "T_EOF";
        case T_ERROR: return "T_ERROR";
        default: return "T_UNKNOWN";
    }
}

int main() {
    const char* code = R"(
#include <stdio.h>
#define MAX 100

int main() {
    // This is a comment
    int x = 42;
    float y = 3.14159f;
    char c = '\n';
    char* str = "Hello\tWorld! \"quoted\"";
    
    /* Multi-line
       comment */
    
    if (x > 0 && x <= MAX) {
        printf("Value: %d\n", x);
    } else if (x == 0 || x >= MAX) {
        printf("Boundary value\n");
    }
    
    for (int i = 0; i < 10; i++) {
        x += i;
        x++;
        x--;
    }
    
    while (x > 0) {
        x >>= 1;  // Right shift
        x &= 0xFF; // Bitwise AND
    }
    
    return 0;
}
)";
    
    LexerState state = createLexerState(code);
    Token token;
    SimpleArray<Token, 500> tokens;
    
    cout << "Tokenized output (DFA/State Machine Version):\n";
    
    while (true) {
        if (!getNextToken(state, token)) break;
        if (token.type == T_EOF) break;
        
        tokens.push_back(token);
        
        if (token.type == T_ERROR) {
            cout << "ERROR: " << token.value << " at line " << token.line << ", column " << token.column << "\n";
        } else {
            cout << tokenTypeToString(token.type) << "(\"" << token.value << "\"), ";
        }
    }
    
    cout << "\n";
    
    return 0;
}
