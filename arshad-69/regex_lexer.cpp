#include <iostream>
#include <regex>
#include <string>
#include <vector>
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

vector<pair<regex, TokenType>> tokenPatterns = {
    {regex("^fn"), T_FUNCTION},
    {regex("^int"), T_INT},
    {regex("^float"), T_FLOAT},
    {regex("^string"), T_STRING},
    {regex("^bool"), T_BOOL},
    {regex("^return"), T_RETURN},
    {regex("^[a-zA-Z_][a-zA-Z0-9_]*"), T_IDENTIFIER},
    {regex("^[0-9]+"), T_INTLIT},
    {regex("^\"(\\\\.|[^\"])*\""), T_STRINGLIT},
    {regex("^=="), T_EQUALSOP},
    {regex("^="), T_ASSIGNOP},
    {regex("^,"), T_COMMA},
    {regex("^;"), T_SEMICOLON},
    {regex("^\\("), T_PARENL},
    {regex("^\\)"), T_PARENR},
    {regex("^\\{"), T_BRACEL},
    {regex("^\\}"), T_BRACER}
};

vector<Token> tokenize(const string &src) {
    vector<Token> tokens;
    string s = src;

    while (!s.empty()) {
        // trim whitespace
        if (isspace(s[0])) {
            s.erase(0,1);
            continue;
        }

        bool matched = false;
        for (auto &p : tokenPatterns) {
            smatch m;
            if (regex_search(s, m, p.first)) {
                string val = m.str();
                tokens.push_back({p.second, val});
                s = m.suffix(); // remaining string
                matched = true;
                break;
            }
        }
        if (!matched) {
            tokens.push_back({T_UNKNOWN, string(1, s[0])});
            s.erase(0,1);
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
