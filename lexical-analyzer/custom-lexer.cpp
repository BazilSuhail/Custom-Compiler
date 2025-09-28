#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
using namespace std;

// === Token Types ===
enum TokenType {
    // Types
    T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_VOID, T_BOOL,

    // Literals
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT, T_CHARLIT, T_BOOLLIT,

    // Brackets & Separators
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_SEMICOLON, T_COMMA, T_DOT,

    // Operators
    T_ASSIGNOP, T_EQUALOP, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,
    T_INCREMENT, T_DECREMENT,
    T_AND, T_OR, T_NOT,

    // Keywords
    T_IF, T_ELSE, T_WHILE, T_RETURN, T_PRINT,
    T_MAIN,

    // New keywords
    T_STRING, T_DO, T_SWITCH, T_BREAK, T_FOR, T_DEFAULT, T_CASE, T_COLON,

    // Comments
    T_SINGLE_COMMENT, T_MULTI_COMMENT,

    // Error & EOF
    T_ERROR, T_EOF
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
};

// === Static Keyword Map ===
static const map<string, TokenType> keywords = {
    {"int", T_INT}, {"float", T_FLOAT}, {"double", T_DOUBLE},
    {"char", T_CHAR}, {"void", T_VOID}, {"bool", T_BOOL},
    {"true", T_BOOLLIT}, {"false", T_BOOLLIT},
    {"if", T_IF}, {"else", T_ELSE}, {"while", T_WHILE}, {"return", T_RETURN},
    {"print", T_PRINT}, {"main", T_MAIN},

    // Newly added keywords
    {"string", T_STRING}, {"do", T_DO}, {"switch", T_SWITCH}, 
    {"break", T_BREAK}, {"for", T_FOR}, {"default", T_DEFAULT}, {"case", T_CASE}
};


// === Single-character tokens ===
static const map<char, TokenType> singleChars = {
    {'(', T_LPAREN}, {')', T_RPAREN}, {'{', T_LBRACE}, {'}', T_RBRACE},
    {'[', T_LBRACKET}, {']', T_RBRACKET}, {';', T_SEMICOLON},
    {',', T_COMMA}, {'.', T_DOT}, {'+', T_PLUS}, {'-', T_MINUS},{':', T_COLON},   // <-- added here
    {'*', T_MULTIPLY}, {'/', T_DIVIDE}, {'%', T_MODULO}, {'=', T_ASSIGNOP},
    {'!', T_NOT}, {'<', T_LT}, {'>', T_GT}
};

// === Two-character operators ===
static const map<string, TokenType> twoCharOps = {
    {"==", T_EQUALOP}, {"!=", T_NE}, {"<=", T_LE}, {">=", T_GE},
    {"&&", T_AND}, {"||", T_OR}, {"++", T_INCREMENT}, {"--", T_DECREMENT}
};

// === Lexer State ===
struct LexerState {
    const char* input;
    int pos;
    int line;
    int column;
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

// === Utility Functions ===
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

// Check if byte starts a UTF-8 sequence (non-ASCII)
inline bool isUnicodeLeadByte(char ch) {
    return (ch & 0x80) != 0; // Non-ASCII
}

// Skip whitespace including newlines
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

// Make token helper
void makeToken(Token& token, TokenType type, const string& value, const LexerState& state, int startCol) {
    token.type = type;
    token.value = value;
    token.line = state.line;
    token.column = startCol;
}

// === Match Comments ===
bool tryMatchComments(LexerState& state, Token& token) {
    if (state.pos + 1 >= state.inputLength) return false;
    int startCol = state.column;

    // Single-line comment //
    if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '/') {
        string comment = "//";
        state.pos += 2;
        state.column += 2;
        while (state.pos < state.inputLength && state.input[state.pos] != '\n') {
            comment += state.input[state.pos++];
            state.column++;
        }
        makeToken(token, T_SINGLE_COMMENT, comment, state, startCol);
        return true;
    }

    // Multi-line comment /* ... */
    if (state.input[state.pos] == '/' && state.input[state.pos + 1] == '*') {
        string comment = "/*";
        state.pos += 2;
        state.column += 2;
        while (state.pos + 1 < state.inputLength) {
            if (state.input[state.pos] == '*' && state.input[state.pos + 1] == '/') {
                comment += "*/";
                state.pos += 2;
                state.column += 2;
                makeToken(token, T_MULTI_COMMENT, comment, state, startCol);
                return true;
            }
            if (state.input[state.pos] == '\n') {
                state.line++;
                state.column = 1;
            } else {
                state.column++;
            }
            comment += state.input[state.pos++];
        }
        makeToken(token, T_ERROR, "Unterminated multi-line comment", state, startCol);
        return true;
    }
    return false;
}

// === Match String or Char Literal (with escape support) ===
bool tryMatchQuoted(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength) return false;
    char quote = state.input[state.pos];
    if (quote != '"' && quote != '\'') return false;

    int startCol = state.column;
    string literal(1, quote);
    state.pos++; state.column++;

    while (state.pos < state.inputLength && state.input[state.pos] != quote) {
        if (state.input[state.pos] == '\\') {
            if (state.pos + 1 >= state.inputLength) {
                literal += state.input[state.pos++];
                state.column++;
                break;
            }
            literal += state.input[state.pos++]; // '\'
            literal += state.input[state.pos++]; // escaped char
            state.column += 2;
        } else {
            if (state.input[state.pos] == '\n') {
                state.line++;
                state.column = 1;
            } else {
                state.column++;
            }
            literal += state.input[state.pos++];
        }
    }

    if (state.pos < state.inputLength) {
        literal += quote;
        state.pos++; state.column++;
        makeToken(token, quote == '"' ? T_STRINGLIT : T_CHARLIT, literal, state, startCol);
        return true;
    } else {
        makeToken(token, T_ERROR, "Unterminated literal", state, startCol);
        return true;
    }
}

// === Match Operators (two-char first) ===
bool tryMatchOperator(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength) return false;
    int startCol = state.column;
    char ch = state.input[state.pos];
    char next = (state.pos + 1 < state.inputLength) ? state.input[state.pos + 1] : '\0';
    string two = string(1, ch) + next;

    auto it = twoCharOps.find(two);
    if (it != twoCharOps.end()) {
        makeToken(token, it->second, two, state, startCol);
        state.pos += 2;
        state.column += 2;
        return true;
    }

    auto singleIt = singleChars.find(ch);
    if (singleIt != singleChars.end()) {
        makeToken(token, singleIt->second, string(1, ch), state, startCol);
        state.pos++;
        state.column++;
        return true;
    }

    return false;
}

// === Match Identifier or Number ===
bool tryMatchIdOrNumber(LexerState& state, Token& token) {
    if (state.pos >= state.inputLength) return false;

    int startCol = state.column;
    string value;

    char current = state.input[state.pos];

    // Start with digit → number
    if (isDigit(current)) {
        while (state.pos < state.inputLength &&
               (isDigit(state.input[state.pos]) ||
                state.input[state.pos] == '.' ||
                state.input[state.pos] == 'e' || state.input[state.pos] == 'E' ||
                state.input[state.pos] == '+' || state.input[state.pos] == '-')) {

            char c = state.input[state.pos];
            if (!isDigit(c) && c != '.' && c != 'e' && c != 'E' && c != '+' && c != '-') {
                break;
            }

            value += state.input[state.pos++];
            state.column++;
        }

        // After number, check for invalid identifier suffix like 123abc
        if (state.pos < state.inputLength) {
            char next = state.input[state.pos];
            if (isAsciiAlpha(next) || isUnicodeLeadByte(next)) {
                string badIdent = value;
                while (state.pos < state.inputLength) {
                    char c = state.input[state.pos];
                    if (isAsciiAlphaNumeric(c) || isUnicodeLeadByte(c) || ((c & 0xC0) == 0x80)) {
                        badIdent += c;
                        state.pos++;
                        state.column++;
                    } else {
                        break;
                    }
                }
                makeToken(token, T_ERROR, "Invalid numeric literal followed by identifier: '" + badIdent + "'", state, startCol);
                return true;
            }
        }

        if (value.find('.') != string::npos || value.find('e') != string::npos || value.find('E') != string::npos) {
            makeToken(token, T_FLOATLIT, value, state, startCol);
        } else {
            makeToken(token, T_INTLIT, value, state, startCol);
        }
        return true;
    }

    // Identifier: starts with alpha/_/Unicode
    if (isAsciiAlpha(current) || isUnicodeLeadByte(current)) {
        while (state.pos < state.inputLength) {
            char c = state.input[state.pos];
            if (isAsciiAlphaNumeric(c) || isUnicodeLeadByte(c)) {
                value += c;
                state.pos++;
                state.column++;
            } else if ((c & 0xC0) == 0x80 || (c & 0xC0) == 0xC0) {
                // UTF-8 continuation bytes
                value += c;
                state.pos++;
                state.column++;
            } else {
                break;
            }
        }

        auto it = keywords.find(value);
        TokenType type = (it != keywords.end()) ? it->second : T_IDENTIFIER;
        makeToken(token, type, value, state, startCol);
        return true;
    }

    return false;
}

// === Get Next Token ===
bool getNextToken(LexerState& state, Token& token) {
    skipWhitespace(state);
    if (state.pos >= state.inputLength) {
        token.type = T_EOF;
        token.value = "";
        token.line = state.line;
        token.column = state.column;
        return false;
    }

    int startCol = state.column;

    if (tryMatchComments(state, token)) return true;
    if (tryMatchQuoted(state, token)) return true;
    if (tryMatchOperator(state, token)) return true;
    if (tryMatchIdOrNumber(state, token)) return true;

    // Unknown character
    string err = string("Unexpected character: '") + state.input[state.pos] + "'";
    makeToken(token, T_ERROR, err, state, startCol);
    state.pos++;
    state.column++;
    return true;
}

// === Token Type to String ===
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
        case T_STRING: return "T_STRING";
        case T_DO: return "T_DO";
        case T_SWITCH: return "T_SWITCH";
        case T_BREAK: return "T_BREAK";
        case T_FOR: return "T_FOR";
        case T_DEFAULT: return "T_DEFAULT";
        case T_CASE: return "T_CASE";
        case T_COLON: return "T_COLON";
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
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_RETURN: return "T_RETURN";
        case T_PRINT: return "T_PRINT";
        case T_MAIN: return "T_MAIN";
        case T_SINGLE_COMMENT: return "T_SINGLE_COMMENT";
        case T_MULTI_COMMENT: return "T_MULTI_COMMENT";
        case T_ERROR: return "T_ERROR";
        case T_EOF: return "T_EOF";
        default: return "T_UNKNOWN";
    }
}

// =========================================================================

void testLexer(const string& code, ostream& out) {
    LexerState state = createLexerState(code.c_str());
    Token token;
    while (getNextToken(state, token)) {
        if (token.type == T_ERROR) {
            out << "ERROR(line " << token.line << ", col " << token.column << "): " << token.value << "\n";
        } 
        else if (token.type != T_SINGLE_COMMENT && token.type != T_MULTI_COMMENT) {
            //out << tokenTypeToString(token.type) << "(" << token.value << "), " << "line: " << token.line << ", col: " << token.column <<"\n";
            out << tokenTypeToString(token.type) << "(" << token.value << ")," << token.line << "," << token.column <<"\n";
        }
    }
    out << endl;
}

int main() {
    ifstream inputFile("tester/sample.txt");
    if (!inputFile.is_open()) {
        cerr << "Failed to open input.txt" << endl;
        return 1;
    }

    stringstream buffer;
    buffer << inputFile.rdbuf();
    string code = buffer.str();
    inputFile.close();

    ofstream outputFile("tester/tokens.txt", ios::out | ios::trunc);
    if (!outputFile.is_open()) {
        cerr << "Failed to open tester/tokens.txt" << endl;
        return 1;
    }

    testLexer(code, outputFile);

    outputFile.close();
    cout << "Lexing completed successfully. Results written to tester/tokens.txt" << endl;

    return 0;
}

// // === Test Function ===
// void testLexer(const string& code, const string& testName) {
//     cout << "=== " << testName << " ===\n";
//     LexerState state = createLexerState(code.c_str());
//     Token token;
//     while (getNextToken(state, token)) {
//         if (token.type == T_ERROR) {
//             cout << "ERROR(line " << token.line << ", col " << token.column << "): " << token.value << "\n";
//         } 
//         else if (token.type != T_SINGLE_COMMENT && token.type != T_MULTI_COMMENT) {
//             cout << tokenTypeToString(token.type) << "(" << token.value << ")\n";
//         }
//         /*
//         else if (token.type != T_SINGLE_COMMENT && token.type != T_MULTI_COMMENT) {
//             cout << "Line " << token.line << ", Col " << token.column << ": "
//                  << tokenTypeToString(token.type) << "(\"" << token.value << "\")\n";
//         }
//         */
//     }
//     cout << endl;
// }

// // === Example Input Code (with print and main) ===
// int main() {
//     string code = R"(
//     include<main>
//     // Functions
//     int add(int a, int b) {
//         return a + b;
//     }

//     // Main entry point
//     main {
//         int x = 42;
//         float y = 3.14159;

//         // Unicode identifiers
//         int π = 3;
//         int 🌍 = 9;
//         float résumé = 2.5;
//         char 你好 = 'A';

//         // Print statements
//         print("Hello World! 🌍✨\n";)
//         print("Value of pi: ", π, "\n");

//         if (π > 0) {
//             print("Positive pi\n");
//         } else {
//             return x;
//         }

//         while (x > 0) {
//             x = x - 1;
//             print "x = ", x, "\n";
//         }

//         // Invalid variable name
//         int 123val = 55;

//         return 0;
//     }
//     )";

//     testLexer(code, "Final Lexer with PRINT and MAIN");
//     return 0;
// }