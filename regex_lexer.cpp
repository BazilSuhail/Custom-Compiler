#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
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
    T_ERROR, T_EOF
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
};

map<string, TokenType> keywords = {
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

vector<pair<string, TokenType>> tokenPatterns = {
    {"//[^\n]*", T_SINGLE_COMMENT},
    {"/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/", T_MULTI_COMMENT},
    {"#[a-zA-Z_][a-zA-Z0-9_]*", T_INCLUDE},
    {"\\(", T_LPAREN},
    {"\\)", T_RPAREN},
    {"\\{", T_LBRACE},
    {"\\}", T_RBRACE},
    {"\\[", T_LBRACKET},
    {"\\]", T_RBRACKET},
    {";", T_SEMICOLON},
    {",", T_COMMA},
    {"\\.", T_DOT},
    {":", T_COLON},
    {"\\+\\+", T_INCREMENT},
    {"--", T_DECREMENT},
    {"==", T_EQUALOP},
    {"!=", T_NE},
    {"<=", T_LE},
    {">=", T_GE},
    {"&&", T_AND},
    {"\\|\\|", T_OR},
    {"<<", T_LEFT_SHIFT},
    {">>", T_RIGHT_SHIFT},
    {"=", T_ASSIGNOP},
    {"\\+", T_PLUS},
    {"-", T_MINUS},
    {"\\*", T_MULTIPLY},
    {"/", T_DIVIDE},
    {"%", T_MODULO},
    {"<", T_LT},
    {">", T_GT},
    {"!", T_NOT},
    {"&", T_BITWISE_AND},
    {"\\|", T_BITWISE_OR},
    {"\\^", T_BITWISE_XOR},
    {"~", T_BITWISE_NOT},
    {"[0-9]+\\.[0-9]+([eE][+-]?[0-9]+)?", T_FLOATLIT},
    {"[0-9]+[eE][+-]?[0-9]+", T_FLOATLIT},
    {"[a-zA-Z_][a-zA-Z0-9_]*", T_IDENTIFIER},
    {"[0-9]+", T_INTLIT},
    {"'([^'\\\\]|\\\\.)'", T_CHARLIT},
    {"\"([^\"\\\\]|\\\\.)*\"", T_STRINGLIT}
};

int getLine(const string& input, size_t pos) {
    int line = 1;
    for (size_t i = 0; i < pos; i++) {
        if (input[i] == '\n') line++;
    }
    return line;
}

int getColumn(const string& input, size_t pos) {
    int col = 1;
    for (size_t i = pos; i > 0 && input[i-1] != '\n'; i--) {
        col++;
    }
    return col;
}

vector<Token> tokenize(const string& input) {
    vector<Token> tokens;
    size_t pos = 0;
    
    while (pos < input.length()) {
        // Skip whitespace
        while (pos < input.length() && isspace(input[pos])) {
            pos++;
        }
        
        if (pos >= input.length()) break;
        
        bool matched = false;
        for (const auto& pattern : tokenPatterns) {
            regex tokenRegex("^" + pattern.first);
            smatch match;
            string remaining = input.substr(pos);
            
            if (regex_search(remaining, match, tokenRegex)) {
                string value = match.str();
                TokenType type = pattern.second;
                
                // Special handling for identifiers that start with digits
                if (type == T_INTLIT || type == T_IDENTIFIER) {
                    // Check if it's an identifier that starts with a digit
                    if (!value.empty() && isdigit(value[0])) {
                        // Look ahead to see if it's followed by letters
                        regex combinedPattern("^[0-9]+[a-zA-Z_][a-zA-Z0-9_]*");
                        smatch combinedMatch;
                        if (regex_search(remaining, combinedMatch, combinedPattern)) {
                            // This is an invalid identifier starting with a digit
                            string invalidId = combinedMatch.str();
                            int line = getLine(input, pos);
                            int column = getColumn(input, pos);
                            tokens.push_back({T_ERROR, "Invalid identifier: " + invalidId, line, column});
                            pos += invalidId.length();
                            matched = true;
                            break;
                        }
                        // Otherwise it's a valid integer
                        type = T_INTLIT;
                    } else {
                        // It's an identifier, check if it's a keyword
                        if (keywords.find(value) != keywords.end()) {
                            type = keywords[value];
                        }
                    }
                }
                // Handle preprocessor directives specifically
                else if (type == T_INCLUDE && (value == "#include" || value == "#define")) {
                    type = keywords[value];
                }
                
                if (!matched) {
                    int line = getLine(input, pos);
                    int column = getColumn(input, pos);
                    tokens.push_back({type, value, line, column});
                    pos += value.length();
                    matched = true;
                    break;
                }
            }
        }
        
        if (!matched) {
            int line = getLine(input, pos);
            int column = getColumn(input, pos);
            tokens.push_back({T_ERROR, string(1, input[pos]), line, column});
            pos++;
        }
    }
    
    int line = getLine(input, pos);
    int column = getColumn(input, pos);
    tokens.push_back({T_EOF, "", line, column});
    return tokens;
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
        case T_EOF: return "T_EOF";
        default: return "T_UNKNOWN";
    }
}

void testLexer(const string& code, const string& testName) {
    cout << "=== " << testName << " ===\n";
    vector<Token> tokens = tokenize(code);
    
    for (const Token& token : tokens) {
        if (token.type == T_ERROR) {
            cout << "ERROR: " << token.value << " at line " << token.line << ", column " << token.column << "\n";
        } else if (token.type != T_EOF) {
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