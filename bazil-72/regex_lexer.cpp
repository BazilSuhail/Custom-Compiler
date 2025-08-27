#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
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
    keywords["fn"] = T_FUNCTION;
    keywords["int"] = T_INT;
    keywords["float"] = T_FLOAT;
    keywords["bool"] = T_BOOL;
    keywords["string"] = T_STRING;
    keywords["void"] = T_VOID;
    keywords["if"] = T_IF;
    keywords["else"] = T_ELSE;
    keywords["while"] = T_WHILE;
    keywords["for"] = T_FOR;
    keywords["return"] = T_RETURN;
    keywords["true"] = T_BOOLLIT;
    keywords["false"] = T_BOOLLIT;
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

string extractMatch(const smatch& match) {
    return match.str();
}

TokenType getOperatorType(const string& op) {
    if (op == "==") return T_EQUALOP;
    else if (op == "!=") return T_NE;
    else if (op == "<=") return T_LE;
    else if (op == ">=") return T_GE;
    else if (op == "&&") return T_AND;
    else if (op == "||") return T_OR;
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
        case '=': return T_ASSIGNOP;
        case '+': return T_PLUS;
        case '-': return T_MINUS;
        case '*': return T_MULTIPLY;
        case '/': return T_DIVIDE;
        case '%': return T_MODULO;
        case '<': return T_LT;
        case '>': return T_GT;
        case '!': return T_NOT;
        case '"': return T_QUOTES;
        default: return T_UNKNOWN;
    }
}

bool tryMatchOperator(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    regex operatorPattern(R"(==|!=|<=|>=|&&|\|\||->)");
    smatch match;
    if (regex_search(remaining, match, operatorPattern) && match.position() == 0) {
        string op = extractMatch(match);
        TokenType type = getOperatorType(op);
        tokens.push_back({type, op, state.line, state.column});
        state.pos += op.length();
        state.column += op.length();
        return true;
    }
    return false;
}

bool tryMatchString(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    regex stringPattern(R"("([^"\\]|\\.)*")");
    smatch match;
    if (regex_search(remaining, match, stringPattern) && match.position() == 0) {
        string str = extractMatch(match);
        tokens.push_back({T_STRINGLIT, str, state.line, state.column});
        state.pos += str.length();
        state.column += str.length();
        return true;
    }
    return false;
}

bool tryMatchFloat(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    regex floatPattern(R"(\d+\.\d+)");
    smatch match;
    if (regex_search(remaining, match, floatPattern) && match.position() == 0) {
        string num = extractMatch(match);
        tokens.push_back({T_FLOATLIT, num, state.line, state.column});
        state.pos += num.length();
        state.column += num.length();
        return true;
    }
    return false;
}

bool tryMatchInteger(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    regex intPattern(R"(\d+)");
    smatch match;
    if (regex_search(remaining, match, intPattern) && match.position() == 0) {
        string num = extractMatch(match);
        tokens.push_back({T_INTLIT, num, state.line, state.column});
        state.pos += num.length();
        state.column += num.length();
        return true;
    }
    return false;
}

bool tryMatchIdentifier(LexerState& state, vector<Token>& tokens) {
    string remaining = state.input.substr(state.pos);
    regex identifierPattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    smatch match;
    if (regex_search(remaining, match, identifierPattern) && match.position() == 0) {
        string identifier = extractMatch(match);
        TokenType type = T_IDENTIFIER;
        auto keywordIt = state.keywords.find(identifier);
        if (keywordIt != state.keywords.end()) {
            type = keywordIt->second;
        }
        tokens.push_back({type, identifier, state.line, state.column});
        state.pos += identifier.length();
        state.column += identifier.length();
        return true;
    }
    return false;
}

bool tryMatchSingleChar(LexerState& state, vector<Token>& tokens) {
    if (state.pos >= state.input.length()) return false;
    char ch = state.input[state.pos];
    TokenType type = getSingleCharType(ch);
    tokens.push_back({type, string(1, ch), state.line, state.column});
    state.pos++;
    state.column++;
    return true;
}

vector<Token> tokenize(const string& source) {
    vector<Token> tokens;
    LexerState state = createLexerState(source);
    while (state.pos < state.input.length()) {
        skipWhitespace(state);
        if (state.pos >= state.input.length()) break;
        if (tryMatchOperator(state, tokens)) continue;
        if (tryMatchString(state, tokens)) continue;
        if (tryMatchFloat(state, tokens)) continue;
        if (tryMatchInteger(state, tokens)) continue;
        if (tryMatchIdentifier(state, tokens)) continue;
        if (tryMatchSingleChar(state, tokens)) continue;
        tokens.push_back({T_UNKNOWN, string(1, state.input[state.pos]), state.line, state.column});
        state.pos++;
        state.column++;
    }
    tokens.push_back({T_EOF, "", state.line, state.column});
    return tokens;
}

string tokenTypeToString(TokenType type) {
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

int main() {
    string code = R"(
fn my_fn(int x, float y) {
    string my_str = "hmm";
    bool my_bool = x == 40;
    return x;
}
)";
    vector<Token> tokens = tokenize(code);
    cout << "Tokenized output (Functional Regex Version):\n";
    for (const Token& token : tokens) {
        if (token.type != T_EOF) {
            cout << tokenTypeToString(token.type) << "(\"" << token.value << "\"), ";
        }
    }
    cout << endl;
    return 0;
}
