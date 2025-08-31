#include <iostream>
#include <regex>
#include <string>
#include <vector>
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

// Order matters: longer patterns before shorter (<= before <, etc.)
vector<pair<regex, TokenType>> tokenPatterns = {
    {regex("^\\s+"), T_UNKNOWN},                     // whitespace (we'll skip)
    {regex("^//.*"), T_UNKNOWN},                     // single-line comment (skip)
    {regex("^/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/"), T_UNKNOWN}, // block comment (skip)

    {regex("^fn\\b"), T_FUNCTION},
    {regex("^int\\b"), T_INT},
    {regex("^float\\b"), T_FLOAT},
    {regex("^string\\b"), T_STRING},
    {regex("^bool\\b"), T_BOOL},
    {regex("^return\\b"), T_RETURN},

    {regex("^\"(\\\\.|[^\"])*\""), T_STRINGLIT},
    {regex("^[0-9]+"), T_INTLIT},
    {regex("^[a-zA-Z_][a-zA-Z0-9_]*"), T_IDENTIFIER},

    {regex("^=="), T_EQUALSOP},
    {regex("^<="), T_LE},
    {regex("^>="), T_GE},
    {regex("^<"),  T_LT},
    {regex("^>"),  T_GT},
    {regex("^="),  T_ASSIGNOP},

    {regex("^,"), T_COMMA},
    {regex("^;"), T_SEMICOLON},
    {regex("^\\("), T_PARENL},
    {regex("^\\)"), T_PARENR},
    {regex("^\\{"), T_BRACEL},
    {regex("^\\}"), T_BRACER}
};

vector<Token> tokenize(string s) {
    vector<Token> tokens;

    while (!s.empty()) {
        bool matched = false;

        for (auto &p : tokenPatterns) {
            smatch m;
            if (regex_search(s, m, p.first)) {
                string val = m.str();

                // Skip whitespace/comments (classified here as T_UNKNOWN)
                if (p.second != T_UNKNOWN) {
                    tokens.push_back({p.second, val});
                }

                s = m.suffix().str();
                matched = true;
                break;
            }
        }

        if (!matched) {
            // consume one char as unknown to avoid infinite loop
            tokens.push_back({T_UNKNOWN, string(1, s[0])});
            s.erase(0, 1);
        }
    }
    return tokens;
}

int main() {
    string src = R"(fn int my_fn(int x, float y) {
        string my_str = "hmm";
        bool a = x < 100;
        bool b = x >= 40;
        return x;
    })";

    vector<Token> tokens = tokenize(src);
    for (const auto &t : tokens) {
        cout << "[" << tokenName(t.type) << (t.value.empty() ? "" : (", " + t.value)) << "]\n";
    }
    return 0;
}
