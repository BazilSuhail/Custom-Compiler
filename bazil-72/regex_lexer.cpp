#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

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
    std::string value;
    int line;
    int column;
};

struct LexerState {
    std::string input;
    size_t pos;
    int line;
    int column;
    std::map<std::string, TokenType> keywords;
};

// Initialize keywords map
std::map<std::string, TokenType> initializeKeywords() {
    std::map<std::string, TokenType> keywords;
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

// Create initial lexer state
LexerState createLexerState(const std::string& source) {
    LexerState state;
    state.input = source;
    state.pos = 0;
    state.line = 1;
    state.column = 1;
    state.keywords = initializeKeywords();
    return state;
}

// Skip whitespace and update position
void skipWhitespace(LexerState& state) {
    while (state.pos < state.input.length() && std::isspace(state.input[state.pos])) {
        if (state.input[state.pos] == '\n') {
            state.line++;
            state.column = 1;
        } else {
            state.column++;
        }
        state.pos++;
    }
}

// Extract match from regex result
std::string extractMatch(const std::smatch& match) {
    return match.str();
}

// Get token type for operator
TokenType getOperatorType(const std::string& op) {
    if (op == "==") return T_EQUALOP;
    else if (op == "!=") return T_NE;
    else if (op == "<=") return T_LE;
    else if (op == ">=") return T_GE;
    else if (op == "&&") return T_AND;
    else if (op == "||") return T_OR;
    return T_UNKNOWN;
}

// Get token type for single character
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

// Try to match multi-character operators
bool tryMatchOperator(LexerState& state, std::vector<Token>& tokens) {
    std::string remaining = state.input.substr(state.pos);
    std::regex operatorPattern(R"(==|!=|<=|>=|&&|\|\||->)");
    std::smatch match;
    
    if (std::regex_search(remaining, match, operatorPattern) && match.position() == 0) {
        std::string op = extractMatch(match);
        TokenType type = getOperatorType(op);
        
        tokens.push_back({type, op, state.line, state.column});
        state.pos += op.length();
        state.column += op.length();
        return true;
    }
    return false;
}

// Try to match string literals
bool tryMatchString(LexerState& state, std::vector<Token>& tokens) {
    std::string remaining = state.input.substr(state.pos);
    std::regex stringPattern(R"("([^"\\]|\\.)*")");
    std::smatch match;
    
    if (std::regex_search(remaining, match, stringPattern) && match.position() == 0) {
        std::string str = extractMatch(match);
        tokens.push_back({T_STRINGLIT, str, state.line, state.column});
        state.pos += str.length();
        state.column += str.length();
        return true;
    }
    return false;
}

// Try to match float literals
bool tryMatchFloat(LexerState& state, std::vector<Token>& tokens) {
    std::string remaining = state.input.substr(state.pos);
    std::regex floatPattern(R"(\d+\.\d+)");
    std::smatch match;
    
    if (std::regex_search(remaining, match, floatPattern) && match.position() == 0) {
        std::string num = extractMatch(match);
        tokens.push_back({T_FLOATLIT, num, state.line, state.column});
        state.pos += num.length();
        state.column += num.length();
        return true;
    }
    return false;
}

// Try to match integer literals
bool tryMatchInteger(LexerState& state, std::vector<Token>& tokens) {
    std::string remaining = state.input.substr(state.pos);
    std::regex intPattern(R"(\d+)");
    std::smatch match;
    
    if (std::regex_search(remaining, match, intPattern) && match.position() == 0) {
        std::string num = extractMatch(match);
        tokens.push_back({T_INTLIT, num, state.line, state.column});
        state.pos += num.length();
        state.column += num.length();
        return true;
    }
    return false;
}

// Try to match identifiers and keywords
bool tryMatchIdentifier(LexerState& state, std::vector<Token>& tokens) {
    std::string remaining = state.input.substr(state.pos);
    std::regex identifierPattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    std::smatch match;
    
    if (std::regex_search(remaining, match, identifierPattern) && match.position() == 0) {
        std::string identifier = extractMatch(match);
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

// Try to match single character tokens
bool tryMatchSingleChar(LexerState& state, std::vector<Token>& tokens) {
    if (state.pos >= state.input.length()) return false;
    
    char ch = state.input[state.pos];
    TokenType type = getSingleCharType(ch);
    
    tokens.push_back({type, std::string(1, ch), state.line, state.column});
    state.pos++;
    state.column++;
    return true;
}

// Main tokenization function
std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;
    LexerState state = createLexerState(source);
    
    while (state.pos < state.input.length()) {
        skipWhitespace(state);
        
        if (state.pos >= state.input.length()) break;
        
        // Try to match different token types in order of precedence
        if (tryMatchOperator(state, tokens)) continue;
        if (tryMatchString(state, tokens)) continue;
        if (tryMatchFloat(state, tokens)) continue;
        if (tryMatchInteger(state, tokens)) continue;
        if (tryMatchIdentifier(state, tokens)) continue;
        if (tryMatchSingleChar(state, tokens)) continue;
        
        // If nothing matches, it's an unknown character
        tokens.push_back({T_UNKNOWN, std::string(1, state.input[state.pos]), state.line, state.column});
        state.pos++;
        state.column++;
    }
    
    tokens.push_back({T_EOF, "", state.line, state.column});
    return tokens;
}

std::string tokenTypeToString(TokenType type) {
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
    std::string code = R"(
fn my_fn(int x, float y) {
    string my_str = "hmm";
    bool my_bool = x == 40;
    return x;
}
)";
    
    std::vector<Token> tokens = tokenize(code);
    
    std::cout << "Tokenized output (Functional Regex Version):\n";
    for (const Token& token : tokens) {
        if (token.type != T_EOF) {
            std::cout << tokenTypeToString(token.type) << "(\"" << token.value << "\"), ";
        }
    }
    std::cout << std::endl;
    
    return 0;
}