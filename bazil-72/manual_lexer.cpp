#include <iostream>
#include <string>

using namespace std;

enum TokenType {
    T_FUNCTION, T_INT, T_FLOAT, T_BOOL, T_STRING, T_VOID,
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT, T_BOOLLIT,
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_SEMICOLON, T_COMMA, T_QUOTES, T_ASSIGNOP, T_EQUALOP,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,
    T_AND, T_OR, T_NOT, T_LT, T_GT, T_LE, T_GE, T_NE,
    T_IF, T_ELSE, T_WHILE, T_FOR, T_RETURN,
    T_UNKNOWN, T_EOF
};

enum State {
    START,
    IN_IDENTIFIER,
    IN_NUMBER,
    IN_FLOAT,
    IN_STRING,
    IN_STRING_ESCAPE,
    SINGLE_CHAR,
    MAYBE_EQUAL,
    MAYBE_NOT_EQUAL,
    MAYBE_LESS_EQUAL,
    MAYBE_GREATER_EQUAL,
    MAYBE_AND,
    MAYBE_OR,
    ACCEPT,
    ERROR
};

struct Token {
    TokenType type;
    char* value;
    int line;
    int column;
    
    Token() : type(T_UNKNOWN), value(nullptr), line(0), column(0) {}
    
    void cleanup() {
        if (value != nullptr) {
            delete[] value;
            value = nullptr;
        }
    }
};

struct TokenArray {
    Token* tokens;
    int size;
    int capacity;
    
    TokenArray() : tokens(nullptr), size(0), capacity(0) {}
    
    void push_back(const Token& token) {
        if (size >= capacity) {
            int newCapacity = capacity == 0 ? 1 : capacity * 2;
            Token* newTokens = new Token[newCapacity];
            
            for (int i = 0; i < size; i++) {
                newTokens[i] = tokens[i];
            }
            
            if (tokens != nullptr) {
                delete[] tokens;
            }
            
            tokens = newTokens;
            capacity = newCapacity;
        }
        
        tokens[size] = token;
        size++;
    }
    
    void cleanup() {
        for (int i = 0; i < size; i++) {
            tokens[i].cleanup();
        }
        if (tokens != nullptr) {
            delete[] tokens;
            tokens = nullptr;
        }
        size = 0;
        capacity = 0;
    }
};

struct LexerState {
    const char* input;
    int pos;
    int length;
    int line;
    int column;
    State currentState;
    char* currentToken;
    int tokenLength;
    int tokenCapacity;
    
    LexerState(const char* source) {
        input = source;
        pos = 0;
        length = stringLength(source);
        line = 1;
        column = 1;
        currentState = START;
        currentToken = nullptr;
        tokenLength = 0;
        tokenCapacity = 0;
    }
    
    void cleanup() {
        if (currentToken != nullptr) {
            delete[] currentToken;
            currentToken = nullptr;
        }
    }
    
    int stringLength(const char* str) {
        int len = 0;
        while (str[len] != '\0') {
            len++;
        }
        return len;
    }
};

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAlphaNum(char c) {
    return isAlpha(c) || isDigit(c);
}

bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool stringEquals(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return str1[i] == '\0' && str2[i] == '\0';
}

char* copyString(const char* src, int length) {
    if (length == 0) {
        while (src[length] != '\0') {
            length++;
        }
    }
    
    char* dest = new char[length + 1];
    for (int i = 0; i < length; i++) {
        dest[i] = src[i];
    }
    dest[length] = '\0';
    return dest;
}

void addToToken(LexerState& state, char c) {
    if (state.tokenLength >= state.tokenCapacity) {
        int newCapacity = state.tokenCapacity == 0 ? 8 : state.tokenCapacity * 2;
        char* newToken = new char[newCapacity];
        
        for (int i = 0; i < state.tokenLength; i++) {
            newToken[i] = state.currentToken[i];
        }
        
        if (state.currentToken != nullptr) {
            delete[] state.currentToken;
        }
        
        state.currentToken = newToken;
        state.tokenCapacity = newCapacity;
    }
    
    state.currentToken[state.tokenLength] = c;
    state.tokenLength++;
}

void resetToken(LexerState& state) {
    state.tokenLength = 0;
}

char* getCurrentToken(LexerState& state) {
    if (state.tokenLength == 0) {
        char* empty = new char[1];
        empty[0] = '\0';
        return empty;
    }
    
    char* result = new char[state.tokenLength + 1];
    for (int i = 0; i < state.tokenLength; i++) {
        result[i] = state.currentToken[i];
    }
    result[state.tokenLength] = '\0';
    return result;
}

void advance(LexerState& state) {
    if (state.pos < state.length) {
        if (state.input[state.pos] == '\n') {
            state.line++;
            state.column = 1;
        } else {
            state.column++;
        }
        state.pos++;
    }
}

char currentChar(const LexerState& state) {
    if (state.pos >= state.length) {
        return '\0';
    }
    return state.input[state.pos];
}

char peekChar(const LexerState& state, int offset = 1) {
    if (state.pos + offset >= state.length) {
        return '\0';
    }
    return state.input[state.pos + offset];
}

TokenType getKeywordType(const char* identifier) {
    if (stringEquals(identifier, "fn")) return T_FUNCTION;
    if (stringEquals(identifier, "int")) return T_INT;
    if (stringEquals(identifier, "float")) return T_FLOAT;
    if (stringEquals(identifier, "bool")) return T_BOOL;
    if (stringEquals(identifier, "string")) return T_STRING;
    if (stringEquals(identifier, "void")) return T_VOID;
    if (stringEquals(identifier, "if")) return T_IF;
    if (stringEquals(identifier, "else")) return T_ELSE;
    if (stringEquals(identifier, "while")) return T_WHILE;
    if (stringEquals(identifier, "for")) return T_FOR;
    if (stringEquals(identifier, "return")) return T_RETURN;
    if (stringEquals(identifier, "true")) return T_BOOLLIT;
    if (stringEquals(identifier, "false")) return T_BOOLLIT;
    return T_IDENTIFIER;
}

bool containsDecimal(const char* number) {
    int i = 0;
    while (number[i] != '\0') {
        if (number[i] == '.') {
            return true;
        }
        i++;
    }
    return false;
}

TokenType getSingleCharType(char c) {
    switch (c) {
        case '(': return T_LPAREN;
        case ')': return T_RPAREN;
        case '{': return T_LBRACE;
        case '}': return T_RBRACE;
        case '[': return T_LBRACKET;
        case ']': return T_RBRACKET;
        case ';': return T_SEMICOLON;
        case ',': return T_COMMA;
        case '+': return T_PLUS;
        case '-': return T_MINUS;
        case '*': return T_MULTIPLY;
        case '/': return T_DIVIDE;
        case '%': return T_MODULO;
        default: return T_UNKNOWN;
    }
}

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case T_FUNCTION: return "T_FUNCTION";
        case T_INT: return "T_INT";
        case T_FLOAT: return "T_FLOAT";
        case T_BOOL: return "T_BOOL";
        case T_STRING: return "T_STRING";
        case T_VOID: return "T_VOID";
        case T_IDENTIFIER: return "T_IDENTIFIER";
        case T_INTLIT: return "T_INTLIT";
        case T_FLOATLIT: return "T_FLOATLIT";
        case T_STRINGLIT: return "T_STRINGLIT";
        case T_BOOLLIT: return "T_BOOLLIT";
        case T_LPAREN: return "T_LPAREN";
        case T_RPAREN: return "T_RPAREN";
        case T_LBRACE: return "T_LBRACE";
        case T_RBRACE: return "T_RBRACE";
        case T_LBRACKET: return "T_LBRACKET";
        case T_RBRACKET: return "T_RBRACKET";
        case T_SEMICOLON: return "T_SEMICOLON";
        case T_COMMA: return "T_COMMA";
        case T_QUOTES: return "T_QUOTES";
        case T_ASSIGNOP: return "T_ASSIGNOP";
        case T_EQUALOP: return "T_EQUALOP";
        case T_PLUS: return "T_PLUS";
        case T_MINUS: return "T_MINUS";
        case T_MULTIPLY: return "T_MULTIPLY";
        case T_DIVIDE: return "T_DIVIDE";
        case T_MODULO: return "T_MODULO";
        case T_AND: return "T_AND";
        case T_OR: return "T_OR";
        case T_NOT: return "T_NOT";
        case T_LT: return "T_LT";
        case T_GT: return "T_GT";
        case T_LE: return "T_LE";
        case T_GE: return "T_GE";
        case T_NE: return "T_NE";
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_FOR: return "T_FOR";
        case T_RETURN: return "T_RETURN";
        case T_EOF: return "T_EOF";
        default: return "T_UNKNOWN";
    }
}

Token createToken(TokenType type, const char* value, int line, int column) {
    Token token;
    token.type = type;
    int len = 0;
    while (value[len] != '\0') {
        len++;
    }
    token.value = copyString(value, len);
    token.line = line;
    token.column = column;
    return token;
}

State getNextState(State currentState, char input) {
    switch (currentState) {
        case START:
            if (isWhitespace(input)) return START;
            if (isAlpha(input) || input == '_') return IN_IDENTIFIER;
            if (isDigit(input)) return IN_NUMBER;
            if (input == '"') return IN_STRING;
            if (input == '=') return MAYBE_EQUAL;
            if (input == '!') return MAYBE_NOT_EQUAL;
            if (input == '<') return MAYBE_LESS_EQUAL;
            if (input == '>') return MAYBE_GREATER_EQUAL;
            if (input == '&') return MAYBE_AND;
            if (input == '|') return MAYBE_OR;
            if (input == '(' || input == ')' || input == '{' || input == '}' ||
                input == '[' || input == ']' || input == ';' || input == ',' ||
                input == '+' || input == '-' || input == '*' || input == '/' ||
                input == '%') return SINGLE_CHAR;
            return ERROR;
            
        case IN_IDENTIFIER:
            if (isAlphaNum(input) || input == '_') return IN_IDENTIFIER;
            return ACCEPT;
            
        case IN_NUMBER:
            if (isDigit(input)) return IN_NUMBER;
            if (input == '.') return IN_FLOAT;
            return ACCEPT;
            
        case IN_FLOAT:
            if (isDigit(input)) return IN_FLOAT;
            return ACCEPT;
            
        case IN_STRING:
            if (input == '"') return ACCEPT;
            if (input == '\\') return IN_STRING_ESCAPE;
            if (input == '\0') return ERROR;
            return IN_STRING;
            
        case IN_STRING_ESCAPE:
            return IN_STRING;
            
        case MAYBE_EQUAL:
            if (input == '=') return ACCEPT;
            return ACCEPT;
            
        case MAYBE_NOT_EQUAL:
            if (input == '=') return ACCEPT;
            return ACCEPT;
            
        case MAYBE_LESS_EQUAL:
            if (input == '=') return ACCEPT;
            return ACCEPT;
            
        case MAYBE_GREATER_EQUAL:
            if (input == '=') return ACCEPT;
            return ACCEPT;
            
        case MAYBE_AND:
            if (input == '&') return ACCEPT;
            return ERROR;
            
        case MAYBE_OR:
            if (input == '|') return ACCEPT;
            return ERROR;
            
        default:
            return ERROR;
    }
}

TokenArray tokenize(const char* source) {
    TokenArray tokens;
    LexerState state(source);
    
    while (state.pos <= state.length) {
        char c = currentChar(state);
        
        if (state.currentState == START && isWhitespace(c)) {
            advance(state);
            continue;
        }
        
        if (c == '\0' && state.currentState == START) {
            Token eofToken = createToken(T_EOF, "", state.line, state.column);
            tokens.push_back(eofToken);
            break;
        }
        
        if (c == '\0' && state.currentState != START) {
            TokenType tokenType = T_UNKNOWN;
            char* tokenValue = getCurrentToken(state);
            
            switch (state.currentState) {
                case IN_IDENTIFIER:
                    tokenType = getKeywordType(tokenValue);
                    break;
                case IN_NUMBER:
                    tokenType = T_INTLIT;
                    break;
                case IN_FLOAT:
                    tokenType = T_FLOATLIT;
                    break;
                default:
                    tokenType = T_UNKNOWN;
                    break;
            }
            
            Token token = createToken(tokenType, tokenValue, state.line, state.column);
            tokens.push_back(token);
            delete[] tokenValue;
            
            Token eofToken = createToken(T_EOF, "", state.line, state.column);
            tokens.push_back(eofToken);
            break;
        }
        
        State nextState = getNextState(state.currentState, c);
        
        if (nextState == ACCEPT) {
            TokenType tokenType = T_UNKNOWN;
            char* tokenValue = nullptr;
            int tokenLine = state.line;
            int tokenColumn = state.column - state.tokenLength;
            
            switch (state.currentState) {
                case IN_IDENTIFIER: {
                    tokenValue = getCurrentToken(state);
                    tokenType = getKeywordType(tokenValue);
                    break;
                }
                case IN_NUMBER: {
                    tokenValue = getCurrentToken(state);
                    tokenType = T_INTLIT;
                    break;
                }
                case IN_FLOAT: {
                    tokenValue = getCurrentToken(state);
                    tokenType = T_FLOATLIT;
                    break;
                }
                case IN_STRING: {
                    addToToken(state, c);
                    advance(state);
                    tokenValue = getCurrentToken(state);
                    tokenType = T_STRINGLIT;
                    break;
                }
                case MAYBE_EQUAL: {
                    if (c == '=') {
                        addToToken(state, c);
                        advance(state);
                        tokenValue = getCurrentToken(state);
                        tokenType = T_EQUALOP;
                    } else {
                        tokenValue = getCurrentToken(state);
                        tokenType = T_ASSIGNOP;
                    }
                    break;
                }
                case MAYBE_NOT_EQUAL: {
                    if (c == '=') {
                        addToToken(state, c);
                        advance(state);
                        tokenValue = getCurrentToken(state);
                        tokenType = T_NE;
                    } else {
                        tokenValue = getCurrentToken(state);
                        tokenType = T_NOT;
                    }
                    break;
                }
                case MAYBE_LESS_EQUAL: {
                    if (c == '=') {
                        addToToken(state, c);
                        advance(state);
                        tokenValue = getCurrentToken(state);
                        tokenType = T_LE;
                    } else {
                        tokenValue = getCurrentToken(state);
                        tokenType = T_LT;
                    }
                    break;
                }
                case MAYBE_GREATER_EQUAL: {
                    if (c == '=') {
                        addToToken(state, c);
                        advance(state);
                        tokenValue = getCurrentToken(state);
                        tokenType = T_GE;
                    } else {
                        tokenValue = getCurrentToken(state);
                        tokenType = T_GT;
                    }
                    break;
                }
                case MAYBE_AND: {
                    addToToken(state, c);
                    advance(state);
                    tokenValue = getCurrentToken(state);
                    tokenType = T_AND;
                    break;
                }
                case MAYBE_OR: {
                    addToToken(state, c);
                    advance(state);
                    tokenValue = getCurrentToken(state);
                    tokenType = T_OR;
                    break;
                }
                case SINGLE_CHAR: {
                    tokenValue = getCurrentToken(state);
                    tokenType = getSingleCharType(state.currentToken[0]);
                    break;
                }
            }
            
            if (tokenValue != nullptr) {
                Token token = createToken(tokenType, tokenValue, tokenLine, tokenColumn);
                tokens.push_back(token);
                delete[] tokenValue;
            }
            
            state.currentState = START;
            resetToken(state);
            continue;
        }
        
        if (nextState == ERROR) {
            if (state.currentState == START) {
                char errorStr[2] = {c, '\0'};
                TokenType singleType = getSingleCharType(c);
                if (singleType != T_UNKNOWN) {
                    Token token = createToken(singleType, errorStr, state.line, state.column);
                    tokens.push_back(token);
                } else {
                    Token errorToken = createToken(T_UNKNOWN, errorStr, state.line, state.column);
                    tokens.push_back(errorToken);
                }
                advance(state);
                continue;
            } else {
                TokenType tokenType = T_UNKNOWN;
                char* tokenValue = getCurrentToken(state);
                
                switch (state.currentState) {
                    case IN_IDENTIFIER:
                        tokenType = getKeywordType(tokenValue);
                        break;
                    case IN_NUMBER:
                        tokenType = T_INTLIT;
                        break;
                    case IN_FLOAT:
                        tokenType = T_FLOATLIT;
                        break;
                }
                
                if (tokenValue != nullptr) {
                    Token token = createToken(tokenType, tokenValue, state.line, state.column - state.tokenLength);
                    tokens.push_back(token);
                    delete[] tokenValue;
                }
                
                state.currentState = START;
                resetToken(state);
                continue;
            }
        }
        
        if (nextState != START) {
            addToToken(state, c);
        }
        
        state.currentState = nextState;
        advance(state);
    }
    
    state.cleanup();
    return tokens;
}

int main() {
    const char* code = 
        "fn my_fn(int x, float y) {\n"
        "    string my_str = \"hmm\";\n"
        "    bool my_bool = x == 40;\n"
        "    return x;\n"
        "}\n";
    
    TokenArray tokens = tokenize(code);
    
    cout << "Tokenized output (State Machine Version):\n";
    for (int i = 0; i < tokens.size; i++) {
        if (tokens.tokens[i].type != T_EOF) {
            cout << tokenTypeToString(tokens.tokens[i].type) 
                      << "(\"" << tokens.tokens[i].value << "\"), ";
        }
    }
    cout << endl;
    
    tokens.cleanup();
    return 0;
}