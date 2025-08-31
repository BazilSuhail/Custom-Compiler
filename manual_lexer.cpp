#include <iostream>
#include <string>
#include <cstring>
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

// Simple hash table for keywords
struct KeywordTable {
    struct KeywordEntry {
        string word;
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
        addKeyword("const", T_CONST);
        addKeyword("static", T_STATIC);
        addKeyword("signed", T_SIGNED);
        addKeyword("unsigned", T_UNSIGNED);
        addKeyword("short", T_SHORT);
        addKeyword("long", T_LONG);
        addKeyword("enum", T_ENUM);
        addKeyword("typedef", T_TYPEDEF);
        addKeyword("true", T_BOOLLIT);
        addKeyword("false", T_BOOLLIT);
        addKeyword("#include", T_INCLUDE);
        addKeyword("#define", T_DEFINE);
    }
    
    void addKeyword(const string& word, TokenType type) {
        if (count < 64) {
            entries[count].word = word;
            entries[count].type = type;
            count++;
        }
    }
    
    TokenType findKeyword(const string& word) {
        for (int i = 0; i < count; i++) {
            if (entries[i].word == word) {
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
    state.inputLength = strlen(source);
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
        case ':': return T_COLON;
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
        default: return T_ERROR;
    }
}

bool tryMatchPreprocessor(LexerState& state, Token& token) {
    if (state.input[state.pos] == '#') {
        string directive;
        int startPos = state.pos;
        int startCol = state.column;
        
        // Read the directive
        while (state.pos < state.inputLength && 
               (isAlpha(state.input[state.pos]) || state.input[state.pos] == '#')) {
            directive += state.input[state.pos];
            state.pos++;
            state.column++;
        }
        
        // Check if it's a known preprocessor directive
        if (!directive.empty()) {
            if (directive == "#include") {
                token.type = T_INCLUDE;
            } else if (directive == "#define") {
                token.type = T_DEFINE;
            } else {
                token.type = T_ERROR;
                token.value = "Unknown preprocessor directive: " + directive;
                token.line = state.line;
                token.column = startCol;
                return true;
            }
            
            token.value = directive;
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
            token.value = "==";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '!' && nextCh == '=') {
            token.type = T_NE;
            token.value = "!=";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '<' && nextCh == '=') {
            token.type = T_LE;
            token.value = "<=";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '>' && nextCh == '=') {
            token.type = T_GE;
            token.value = ">=";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '&' && nextCh == '&') {
            token.type = T_AND;
            token.value = "&&";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '|' && nextCh == '|') {
            token.type = T_OR;
            token.value = "||";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '+' && nextCh == '+') {
            token.type = T_INCREMENT;
            token.value = "++";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '-' && nextCh == '-') {
            token.type = T_DECREMENT;
            token.value = "--";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '<' && nextCh == '<') {
            token.type = T_LEFT_SHIFT;
            token.value = "<<";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        } else if (ch == '>' && nextCh == '>') {
            token.type = T_RIGHT_SHIFT;
            token.value = ">>";
            token.line = state.line;
            token.column = state.column;
            state.pos += 2;
            state.column += 2;
            return true;
        }
    }
    
    // Single character operators
    TokenType type = getSingleCharType(ch);
    if (type != T_ERROR) {
        token.type = type;
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
        while (state.pos < state.inputLength && isAlphaNumeric(state.input[state.pos])) {
            value += state.input[state.pos];
            state.pos++;
            state.column++;
        }
        
        // Check if it contains letters (invalid identifier)
        bool hasAlpha = false;
        for (char c : value) {
            if (isAlpha(c)) {
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
    
    // If it starts with alpha or underscore, it's an identifier
    if (isAlpha(state.input[state.pos])) {
        while (state.pos < state.inputLength && isAlphaNumeric(state.input[state.pos])) {
            value += state.input[state.pos];
            state.pos++;
            state.column++;
        }
        
        // Check if it's a keyword
        token.type = state.keywords.findKeyword(value);
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
        case T_COLON: return "T_COLON";
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
        case T_CONST: return "T_CONST";
        case T_STATIC: return "T_STATIC";
        case T_SIGNED: return "T_SIGNED";
        case T_UNSIGNED: return "T_UNSIGNED";
        case T_SHORT: return "T_SHORT";
        case T_LONG: return "T_LONG";
        case T_ENUM: return "T_ENUM";
        case T_TYPEDEF: return "T_TYPEDEF";
        case T_SINGLE_COMMENT: return "T_SINGLE_COMMENT";
        case T_MULTI_COMMENT: return "T_MULTI_COMMENT";
        case T_ERROR: return "T_ERROR";
        default: return "T_UNKNOWN";
    }
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
        // Test 1: Main with comments
    string loopTest = R"(
    
    #include <stdio.h>
    #define MAX 100
    int main() { 
        // this is a comment
        int x = 42;
        float y = 3.14159f;
        char c = '\n';
        char* str = "Hello\tWorld! \"quoted\"";
        
        /* Multi-line comment */
        
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
        }
        return 0;
    }
    
    )"; 
    
    
    testLexer(loopTest, "Loop and Braces Test");
    
    // Test 2: Switch statement
    string switchTest = R"(
    void process(int value) {
        switch(value) {
            case 1:
                printf("One\n");
                break;
            case 2:
                printf("Two\n");
                break;
            default:
                printf("Other\n");
                break;
        }
    }
    )";
    testLexer(switchTest, "Switch Statement Test");

    // Test 3: Invalid identifier (starts with number)
    string invalidIdTest = R"(
    
    int add(int a, int b) {
        return a + b;
    }

    string var1 = "i am  lexer 💕🙌";
    int 123var = 5;
    
    )";
    testLexer(invalidIdTest, "Function and Invalid Identifier Test");

    return 0;
}