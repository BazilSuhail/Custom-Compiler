#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

enum TokenType {
    T_FUNCTION, T_INT, T_FLOAT, T_STRING, T_BOOL, T_RETURN,
    T_IDENTIFIER, T_INTLIT, T_STRINGLIT,
    T_ASSIGNOP, T_EQUALSOP, T_COMMA, T_SEMICOLON,
    T_PARENL, T_PARENR, T_BRACEL, T_BRACER,
    T_UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

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
        if (isspace(c)) {
            continue;
        }

        // identifiers and keywords
        if (isalpha(c) || c == '_') {
            cur = "";
            while (i < src.size() && (isalnum(src[i]) || src[i] == '_')) {
                cur += src[i++];
            }
            i--; // adjust
            if (isKeyword(cur))
                tokens.push_back({keywordType(cur), cur});
            else
                tokens.push_back({T_IDENTIFIER, cur});
        }
        // numbers
        else if (isdigit(c)) {
            cur = "";
            while (i < src.size() && isdigit(src[i])) cur += src[i++];
            i--;
            tokens.push_back({T_INTLIT, cur});
        }
        // strings
        else if (c == '"') {
            cur = "";
            i++;
            while (i < src.size() && src[i] != '"') {
                if (src[i] == '\\') { // escaped chars
                    cur += src[i++];
                }
                if (i < src.size()) cur += src[i];
                i++;
            }
            tokens.push_back({T_STRINGLIT, cur});
        }
        // operators & punctuation
        else {
            switch (c) {
                case '=':
                    if (i + 1 < src.size() && src[i+1] == '=') { 
                        tokens.push_back({T_EQUALSOP, "=="}); 
                        i++; 
                    }
                    else tokens.push_back({T_ASSIGNOP, "="});
                    break;
                case ',': tokens.push_back({T_COMMA, ","}); break;
                case ';': tokens.push_back({T_SEMICOLON, ";"}); break;
                case '(': tokens.push_back({T_PARENL, "("}); break;
                case ')': tokens.push_back({T_PARENR, ")"}); break;
                case '{': tokens.push_back({T_BRACEL, "{"}); break;
                case '}': tokens.push_back({T_BRACER, "}"}); break;
                default: tokens.push_back({T_UNKNOWN, string(1, c)});
            }
        }
    }
    return tokens;
}

int main() {
    string src = R"(fn int my_fn(int x, float y) {
        string my_str = "hmm";
        bool my_bool = x == 40;
        return x;
    })";

    vector<Token> tokens = tokenize(src);
    for (auto &t : tokens) {
        cout << "[" << t.type << ", " << t.value << "]\n";
    }
}
