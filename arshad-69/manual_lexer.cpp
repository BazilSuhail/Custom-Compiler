#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

enum TokenType {
    T_FUNCTION, T_INT, T_FLOAT, T_STRING, T_BOOL, T_RETURN,
    T_IDENTIFIER, T_INTLIT, T_STRINGLIT,
    T_ASSIGNOP, T_EQUALSOP,
    T_LT, T_GT, T_LE, T_GE,        // <-- added
    T_COMMA, T_SEMICOLON,
    T_PARENL, T_PARENR, T_BRACEL, T_BRACER,
    T_UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

const char* tokenName(TokenType t){
    switch(t){
        case T_FUNCTION: return "T_FUNCTION";
        case T_INT: return "T_INT";
        case T_FLOAT: return "T_FLOAT";
        case T_STRING: return "T_STRING";
        case T_BOOL: return "T_BOOL";
        case T_RETURN: return "T_RETURN";
        case T_IDENTIFIER: return "T_IDENTIFIER";
        case T_INTLIT: return "T_INTLIT";
        case T_STRINGLIT: return "T_STRINGLIT";
        case T_ASSIGNOP: return "T_ASSIGNOP";
        case T_EQUALSOP: return "T_EQUALSOP";
        case T_LT: return "T_LT";
        case T_GT: return "T_GT";
        case T_LE: return "T_LE";
        case T_GE: return "T_GE";
        case T_COMMA: return "T_COMMA";
        case T_SEMICOLON: return "T_SEMICOLON";
        case T_PARENL: return "T_PARENL";
        case T_PARENR: return "T_PARENR";
        case T_BRACEL: return "T_BRACEL";
        case T_BRACER: return "T_BRACER";
        default: return "T_UNKNOWN";
    }
}

bool isKeyword(const string &s) {
    return (s == "fn" || s == "int" || s == "float" || s == "string" ||
            s == "bool" || s == "return");
}

TokenType keywordType(const string &s) {
    if (s == "fn") return T_FUNCTION;
    if (s == "int") return T_INT;
    if (s == "float") return T_FLOAT;
    if (s == "string") return T_STRING;
    if (s == "bool") return T_BOOL;
    if (s == "return") return T_RETURN;
    return T_IDENTIFIER;
}

vector<Token> tokenize(const string &src) {
    vector<Token> tokens;
    string cur;

    for (size_t i = 0; i < src.size(); i++) {
        char c = src[i];

        // skip whitespace
        if (isspace(static_cast<unsigned char>(c))) continue;

        // identifiers and keywords
        if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
            cur.clear();
            while (i < src.size() && (isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_')) {
                cur += src[i++];
            }
            i--; // step back to last consumed
            if (isKeyword(cur)) tokens.push_back({keywordType(cur), cur});
            else tokens.push_back({T_IDENTIFIER, cur});
        }
        // numbers
        else if (isdigit(static_cast<unsigned char>(c))) {
            cur.clear();
            while (i < src.size() && isdigit(static_cast<unsigned char>(src[i]))) cur += src[i++];
            i--;
            tokens.push_back({T_INTLIT, cur});
        }
        // string literal with escapes
        else if (c == '"') {
            cur.clear();
            i++;
            while (i < src.size() && src[i] != '"') {
                if (src[i] == '\\' && i + 1 < src.size()) { // keep escape + next char
                    cur += src[i++];
                    cur += src[i++];
                    continue;
                }
                cur += src[i++];
            }
            tokens.push_back({T_STRINGLIT, cur});
        }
        // operators & punctuation
        else {
            switch (c) {
                case '=':
                    if (i + 1 < src.size() && src[i+1] == '=') { tokens.push_back({T_EQUALSOP, "=="}); i++; }
                    else tokens.push_back({T_ASSIGNOP, "="});
                    break;
                case '<':
                    if (i + 1 < src.size() && src[i+1] == '=') { tokens.push_back({T_LE, "<="}); i++; }
                    else tokens.push_back({T_LT, "<"});
                    break;
                case '>':
                    if (i + 1 < src.size() && src[i+1] == '=') { tokens.push_back({T_GE, ">="}); i++; }
                    else tokens.push_back({T_GT, ">"});
                    break;
                case ',': tokens.push_back({T_COMMA, ","}); break;
                case ';': tokens.push_back({T_SEMICOLON, ";"}); break;
                case '(': tokens.push_back({T_PARENL, "("}); break;
                case ')': tokens.push_back({T_PARENR, ")"}); break;
                case '{': tokens.push_back({T_BRACEL, "{"}); break;
                case '}': tokens.push_back({T_BRACER, "}"}); break;
                default:  tokens.push_back({T_UNKNOWN, string(1, c)});
            }
        }
    }
    return tokens;
}

int main() {
    string src = R"(fn int my_fn(int x, float y) {
        string my_str = "hmm";
        bool my_bool = x >= 40;
        if (x < 100) { return x; }
    })";

    vector<Token> tokens = tokenize(src);
    for (const auto &t : tokens) {
        cout << "[" << tokenName(t.type) << (t.value.empty() ? "" : (", " + t.value)) << "]\n";
    }
    return 0;
}
