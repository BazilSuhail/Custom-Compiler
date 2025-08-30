#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <stdexcept>
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
    string value;
    int line;
    int column;
};

struct LexerState {
    string input;
    size_t pos;
    int line;
    int column;
    map<string, TokenType> keywords;
};

map<string, TokenType> initializeKeywords() {
    map<string, TokenType> keywords;
    
    // Data types
    keywords["int"] = T_INT;
    keywords["float"] = T_FLOAT;
    keywords["double"] = T_DOUBLE;
    keywords["char"] = T_CHAR;
    keywords["void"] = T_VOID;
    keywords["bool"] = T_BOOL;
    
    // Control flow
    keywords["if"] = T_IF;
    keywords["else"] = T_ELSE;
    keywords["while"] = T_WHILE;
    keywords["for"] = T_FOR;
    keywords["return"] = T_RETURN;
    keywords["break"] = T_BREAK;
    keywords["continue"] = T_CONTINUE;
    keywords["switch"] = T_SWITCH;
    keywords["case"] = T_CASE;
    keywords["default"] = T_DEFAULT;
    keywords["do"] = T_DO;
    keywords["goto"] = T_GOTO;
    
    // Storage classes and modifiers
    keywords["const"] = T_CONST;
    keywords["static"] = T_STATIC;
    keywords["extern"] = T_EXTERN;
    keywords["volatile"] = T_VOLATILE;
    keywords["signed"] = T_SIGNED;
    keywords["unsigned"] = T_UNSIGNED;
    keywords["short"] = T_SHORT;
    keywords["long"] = T_LONG;
    
    // Struct/union/enum
    keywords["struct"] = T_STRUCT;
    keywords["union"] = T_UNION;
    keywords["enum"] = T_ENUM;
    keywords["typedef"] = T_TYPEDEF;
    
    // Other keywords
    keywords["sizeof"] = T_SIZEOF;
    keywords["inline"] = T_INLINE;
    keywords["auto"] = T_AUTO;
    keywords["register"] = T_REGISTER;
    
    // Boolean literals
    keywords["true"] = T_BOOLLIT;
    keywords["false"] = T_BOOLLIT;
    
    // Preprocessor
    keywords["#include"] = T_INCLUDE;
    keywords["#define"] = T_DEFINE;
    
    return keywords;
}

LexerState createLexerState(const string& source) {
    LexerState state;
    state.input = source;
    state.pos = 0;
    state.line = 1;
    state.column = 1;
    state.keywords = initializeKeywords();
    return state;
}

void skipWhitespace(LexerState& state) {
    while (state.pos < state.input.length() && isspace(state.input[state.pos])) {
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
    if (state.pos + 1 < state.input.length()) {
        if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '/') {
            // Single-line comment
            while (state.pos < state.input.length() && state.input[state.pos] != '\n') {
                state.pos++;
                state.column++;
            }
        } else if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '*') {
            // Multi-line comment
            state.pos += 2;
            state.column += 2;
            while (state.pos + 1 < state.input.length()) {
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

string extractMatch(const smatch& match) {
    return match.str();
}

TokenType getOperatorType(const string& op) {
    // Comparison operators
    if (op == "==") return T_EQUALOP;
    else if (op == "!=") return T_NE;
    else if (op == "<=") return T_LE;
    else if (op == ">=") return T_GE;
    
    // Logical operators
    else if (op == "&&") return T_AND;
    else if (op == "||") return T_OR;
    
    // Increment/decrement
    else if (op == "++") return T_INCREMENT;
    else if (op == "--") return T_DECREMENT;
    
    // Bitwise operators
    else if (op == "&") return T_BITWISE_AND;
    else if (op == "|") return T_BITWISE_OR;
    else if (op == "^") return T_BITWISE_XOR;
    else if (op == "~") return T_BITWISE_NOT;
    else if (op == "<<") return T_LEFT_SHIFT;
    else if (op == ">>") return T_RIGHT_SHIFT;
    
    // Assignment operators
    else if (op == "=") return T_ASSIGNOP;
    
    return T_UNKNOWN;
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
        case '"': return T_UNKNOWN; // Handle separately
        case '\'': return T_UNKNOWN; // Handle separately
        default: return T_UNKNOWN;
    }
}

bool tryMatchPreprocessor(LexerState& state, vector<Token>& tokens) {
    if (state.input[state.pos] == '#') {
        regex preprocessorPattern(R"(#(?:include|define|ifdef|ifndef|endif|if|else|elif|undef|pragma|error|line))");
        string remaining = state.input.substr(state.pos);
        smatch match;
        
        if (regex_search(remaining, match, preprocessorPattern) && match.position() == 0) {
            string directive = extractMatch(match);
            TokenType type = T_PREPROCESSOR;
            
            if (directive == "#include") type = T_INCLUDE;
            else if (directive == "#define") type = T_DEFINE;
            
            tokens.push_back({type, directive, state.line, state.column});
            state.pos += directive.length();
            state.column += (int)directive.length();
            return true;
        }
    }
    return false;
}

bool tryMatchOperator(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    
    // Multi-character operators first
    regex multiCharPattern(R"(==|!=|<=|>=|&&|\|\||\+\+|--|<<|>>|\+=|-=|\*=|/=|%=|&=|\|=|\^=)");
    smatch match;
    
    if (regex_search(remaining, match, multiCharPattern) && match.position() == 0) {
        string op = extractMatch(match);
        TokenType type = getOperatorType(op);
        tokens.push_back({type, op, state.line, state.column});
        state.pos += op.length();
        state.column += (int)op.length();
        return true;
    }
    
    // Single character operators
    char ch = state.input[state.pos];
    TokenType type = getSingleCharType(ch);
    if (type != T_UNKNOWN) {
        tokens.push_back({type, string(1, ch), state.line, state.column});
        state.pos++;
        state.column++;
        return true;
    }
    
    return false;
}

bool tryMatchString(LexerState& state, vector<Token>& tokens) {
    if (state.input[state.pos] != '"') return false;
    
    string str = "\"";
    state.pos++;
    state.column++;
    
    while (state.pos < state.input.length() && state.input[state.pos] != '"') {
        if (state.input[state.pos] == '\\') {
            // Handle escape sequences
            str += state.input[state.pos];
            state.column++;
            state.pos++;
            
            if (state.pos < state.input.length()) {
                str += state.input[state.pos];
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
            str += state.input[state.pos];
            state.pos++;
        }
    }
    
    if (state.pos < state.input.length()) {
        str += '"';
        state.pos++;
        state.column++;
        tokens.push_back({T_STRINGLIT, str, state.line, state.column - (int)str.length()});
        return true;
    } else {
        // Unterminated string
        tokens.push_back({T_ERROR, "Unterminated string literal", state.line, state.column});
        return true;
    }
}

bool tryMatchCharLiteral(LexerState& state, vector<Token>& tokens) {
    if (state.input[state.pos] != '\'') return false;
    
    string charLit = "'";
    state.pos++;
    state.column++;
    
    if (state.pos < state.input.length()) {
        if (state.input[state.pos] == '\\') {
            // Escape sequence
            charLit += state.input[state.pos];
            state.column++;
            state.pos++;
            
            if (state.pos < state.input.length()) {
                charLit += state.input[state.pos];
                state.column++;
                state.pos++;
            }
        } else {
            charLit += state.input[state.pos];
            state.column++;
            state.pos++;
        }
    }
    
    if (state.pos < state.input.length() && state.input[state.pos] == '\'') {
        charLit += '\'';
        state.pos++;
        state.column++;
        tokens.push_back({T_CHARLIT, charLit, state.line, state.column - (int)charLit.length()});
        return true;
    } else {
        // Unterminated character literal
        tokens.push_back({T_ERROR, "Unterminated character literal", state.line, state.column});
        return true;
    }
}

bool tryMatchFloat(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    regex floatPattern(R"(\d+\.\d+([eE][+-]?\d+)?)");
    smatch match;
    
    if (regex_search(remaining, match, floatPattern) && match.position() == 0) {
        string num = extractMatch(match);
        tokens.push_back({T_FLOATLIT, num, state.line, state.column});
        state.pos += num.length();
        state.column += (int)num.length();
        return true;
    }
    
    return false;
}

bool tryMatchInteger(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    regex intPattern(R"(\d+([uU]?[lL]{0,2}|[lL]{0,2}[uU]?)?)");
    smatch match;
    
    if (regex_search(remaining, match, intPattern) && match.position() == 0) {
        string num = extractMatch(match);
        tokens.push_back({T_INTLIT, num, state.line, state.column});
        state.pos += num.length();
        state.column += (int)num.length();
        return true;
    }
    
    return false;
}

bool tryMatchIdentifier(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    // Updated to support Unicode identifiers (basic support)
    regex identifierPattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    smatch match;
    
    if (regex_search(remaining, match, identifierPattern) && match.position() == 0) {
        string identifier = extractMatch(match);
        
        // Check if it's a keyword
        TokenType type = T_IDENTIFIER;
        auto keywordIt = state.keywords.find(identifier);
        if (keywordIt != state.keywords.end()) {
            type = keywordIt->second;
        }
        
        tokens.push_back({type, identifier, state.line, state.column});
        state.pos += identifier.length();
        state.column += (int)identifier.length();
        return true;
    }
    
    return false;
}

vector<Token> tokenize(const string& source) {
    vector<Token> tokens;
    LexerState state = createLexerState(source);
    
    while (state.pos < state.input.length()) {
        skipWhitespace(state);
        skipComments(state);
        
        if (state.pos >= state.input.length()) break;
        
        // Check for preprocessor directives first
        if (tryMatchPreprocessor(state, tokens)) continue;
        
        // Try to match various token types
        if (tryMatchString(state, tokens)) continue;
        if (tryMatchCharLiteral(state, tokens)) continue;
        if (tryMatchOperator(state, tokens)) continue;
        if (tryMatchFloat(state, tokens)) continue;
        if (tryMatchInteger(state, tokens)) continue;
        if (tryMatchIdentifier(state, tokens)) continue;
        
        // If we get here, it's an unknown character
        tokens.push_back({T_UNKNOWN, string(1, state.input[state.pos]), state.line, state.column});
        state.pos++;
        state.column++;
    }
    
    tokens.push_back({T_EOF, "", state.line, state.column});
    return tokens;
}

string tokenTypeToString(TokenType type) {
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
    string code = R"(
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
    
    try {
        vector<Token> tokens = tokenize(code);
        cout << "Tokenized output (Enhanced C Lexer):\n";
        for (const Token& token : tokens) {
            if (token.type == T_EOF) continue;
            if (token.type == T_ERROR) {
                cout << "ERROR: " << token.value << " at line " << token.line << ", column " << token.column << "\n";
            } else {
                cout << tokenTypeToString(token.type) << "(\"" << token.value << "\"), ";
            }
        }
        cout << "\n";
    } catch (const exception& e) {
        cerr << "Error during tokenization: " << e.what() << "\n";
    }
    
    return 0;
}
