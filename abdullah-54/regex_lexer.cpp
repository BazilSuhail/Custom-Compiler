// regex_lexer_improved.cpp
// Regex-based lexer (improved)
// Compile: g++ -std=c++17 regex_lexer_improved.cpp -o regex_lexer -pthread
// Note: When using <regex> some implementations require linking with -pthread on older toolchains.
// The code still uses additional manual checks for invalid identifiers starting with digit.

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
using namespace std;

enum TokenType
{
    T_INT,
    T_FLOAT,
    T_DOUBLE,
    T_CHAR,
    T_VOID,
    T_BOOL,
    T_IDENTIFIER,
    T_INTLIT,
    T_FLOATLIT,
    T_STRINGLIT,
    T_CHARLIT,
    T_BOOLLIT,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_LBRACKET,
    T_RBRACKET,
    T_SEMICOLON,
    T_COMMA,
    T_DOT,
    T_ASSIGNOP,
    T_EQUALOP,
    T_NE,
    T_LT,
    T_GT,
    T_LE,
    T_GE,
    T_PLUS,
    T_MINUS,
    T_MULTIPLY,
    T_DIVIDE,
    T_MODULO,
    T_INCREMENT,
    T_DECREMENT,
    T_AND,
    T_OR,
    T_NOT,
    T_BITWISE_AND,
    T_BITWISE_OR,
    T_BITWISE_XOR,
    T_BITWISE_NOT,
    T_LEFT_SHIFT,
    T_RIGHT_SHIFT,
    T_INCLUDE,
    T_DEFINE,
    T_PREPROCESSOR,
    T_IF,
    T_ELSE,
    T_WHILE,
    T_FOR,
    T_RETURN,
    T_BREAK,
    T_CONTINUE,
    T_EOF,
    T_UNKNOWN,
    T_ERROR
};

struct Token
{
    TokenType type;
    string value;
    int line;
    int column;
};

map<string, TokenType> initializeKeywords()
{
    map<string, TokenType> k;
    k["int"] = T_INT;
    k["float"] = T_FLOAT;
    k["double"] = T_DOUBLE;
    k["char"] = T_CHAR;
    k["void"] = T_VOID;
    k["bool"] = T_BOOL;
    k["if"] = T_IF;
    k["else"] = T_ELSE;
    k["while"] = T_WHILE;
    k["for"] = T_FOR;
    k["return"] = T_RETURN;
    k["break"] = T_BREAK;
    k["continue"] = T_CONTINUE;
    k["true"] = T_BOOLLIT;
    k["false"] = T_BOOLLIT;
    k["#include"] = T_INCLUDE;
    k["#define"] = T_DEFINE;
    return k;
}

struct LexerState
{
    string input;
    size_t pos;
    int line;
    int column;
    map<string, TokenType> keywords;
    LexerState(const string &s = "") : input(s), pos(0), line(1), column(1)
    {
        keywords = initializeKeywords();
    }
};

void advance(LexerState &s, size_t n = 1)
{
    for (size_t i = 0; i < n && s.pos < s.input.size(); ++i)
    {
        if (s.input[s.pos] == '\n')
        {
            s.line++;
            s.column = 1;
        }
        else
            s.column++;
        s.pos++;
    }
}

void skipWhitespace(LexerState &s)
{
    while (s.pos < s.input.size() && isspace((unsigned char)s.input[s.pos]))
        advance(s);
}

void skipComments(LexerState &s)
{
    if (s.pos + 1 >= s.input.size())
        return;
    if (s.input[s.pos] == '/' && s.input[s.pos + 1] == '/')
    {
        while (s.pos < s.input.size() && s.input[s.pos] != '\n')
            advance(s);
    }
    else if (s.input[s.pos] == '/' && s.input[s.pos + 1] == '*')
    {
        advance(s, 2);
        while (s.pos + 1 < s.input.size())
        {
            if (s.input[s.pos] == '*' && s.input[s.pos + 1] == '/')
            {
                advance(s, 2);
                return;
            }
            advance(s);
        }
    }
}

bool startsWith(const string &s, size_t pos, const string &pat)
{
    return pos + pat.size() <= s.size() && s.compare(pos, pat.size(), pat) == 0;
}

bool tryPreprocessor(LexerState &s, vector<Token> &out)
{
    if (s.input[s.pos] != '#')
        return false;
    // read until whitespace or newline
    size_t i = s.pos;
    while (i < s.input.size() && !isspace((unsigned char)s.input[i]))
        ++i;
    string dir = s.input.substr(s.pos, i - s.pos);
    TokenType t = T_PREPROCESSOR;
    if (dir == "#include")
        t = T_INCLUDE;
    else if (dir == "#define")
        t = T_DEFINE;
    out.push_back({t, dir, s.line, s.column});
    advance(s, dir.size());
    return true;
}

bool tryString(LexerState &s, vector<Token> &out)
{
    if (s.input[s.pos] != '"')
        return false;
    int startLine = s.line, startCol = s.column;
    string acc;
    acc.push_back('"');
    advance(s);
    while (s.pos < s.input.size())
    {
        char c = s.input[s.pos];
        if (c == '\\')
        {
            if (s.pos + 1 < s.input.size())
            {
                acc.push_back('\\');
                acc.push_back(s.input[s.pos + 1]);
                advance(s, 2);
            }
            else
            {
                advance(s);
            }
        }
        else if (c == '"')
        {
            acc.push_back('"');
            advance(s);
            out.push_back({T_STRINGLIT, acc, startLine, startCol});
            return true;
        }
        else
        {
            acc.push_back(c);
            advance(s);
        }
    }
    out.push_back({T_ERROR, "Unterminated string literal", startLine, startCol});
    return true;
}

bool tryChar(LexerState &s, vector<Token> &out)
{
    if (s.input[s.pos] != '\'')
        return false;
    int startLine = s.line, startCol = s.column;
    string acc;
    acc.push_back('\'');
    advance(s);
    if (s.pos < s.input.size())
    {
        if (s.input[s.pos] == '\\')
        {
            acc.push_back('\\');
            if (s.pos + 1 < s.input.size())
            {
                acc.push_back(s.input[s.pos + 1]);
                advance(s, 2);
            }
            else
                advance(s);
        }
        else
        {
            acc.push_back(s.input[s.pos]);
            advance(s);
        }
    }
    if (s.pos < s.input.size() && s.input[s.pos] == '\'')
    {
        acc.push_back('\'');
        advance(s);
        out.push_back({T_CHARLIT, acc, startLine, startCol});
    }
    else
    {
        out.push_back({T_ERROR, "Unterminated char literal", startLine, startCol});
    }
    return true;
}

bool tryOperatorsAndPunct(LexerState &s, vector<Token> &out)
{
    static const vector<pair<string, TokenType>> twoChar = {
        {"==", T_EQUALOP}, {"!=", T_NE}, {"<=", T_LE}, {">=", T_GE}, {"&&", T_AND}, {"||", T_OR}, {"++", T_INCREMENT}, {"--", T_DECREMENT}, {"<<", T_LEFT_SHIFT}, {">>", T_RIGHT_SHIFT}, {"+=", T_ASSIGNOP}, {"-=", T_ASSIGNOP}, {"*=", T_ASSIGNOP}, {"/=", T_ASSIGNOP}, {"%=", T_ASSIGNOP}};
    for (auto &p : twoChar)
    {
        if (startsWith(s.input, s.pos, p.first))
        {
            out.push_back({p.second, p.first, s.line, s.column});
            advance(s, p.first.size());
            return true;
        }
    }
    char c = s.input[s.pos];
    switch (c)
    {
    case '(':
        out.push_back({T_LPAREN, "(", s.line, s.column});
        advance(s);
        return true;
    case ')':
        out.push_back({T_RPAREN, ")", s.line, s.column});
        advance(s);
        return true;
    case '{':
        out.push_back({T_LBRACE, "{", s.line, s.column});
        advance(s);
        return true;
    case '}':
        out.push_back({T_RBRACE, "}", s.line, s.column});
        advance(s);
        return true;
    case '[':
        out.push_back({T_LBRACKET, "[", s.line, s.column});
        advance(s);
        return true;
    case ']':
        out.push_back({T_RBRACKET, "]", s.line, s.column});
        advance(s);
        return true;
    case ';':
        out.push_back({T_SEMICOLON, ";", s.line, s.column});
        advance(s);
        return true;
    case ',':
        out.push_back({T_COMMA, ",", s.line, s.column});
        advance(s);
        return true;
    case '.':
        out.push_back({T_DOT, ".", s.line, s.column});
        advance(s);
        return true;
    case '+':
        out.push_back({T_PLUS, "+", s.line, s.column});
        advance(s);
        return true;
    case '-':
        out.push_back({T_MINUS, "-", s.line, s.column});
        advance(s);
        return true;
    case '*':
        out.push_back({T_MULTIPLY, "*", s.line, s.column});
        advance(s);
        return true;
    case '/':
        out.push_back({T_DIVIDE, "/", s.line, s.column});
        advance(s);
        return true;
    case '%':
        out.push_back({T_MODULO, "%", s.line, s.column});
        advance(s);
        return true;
    case '<':
        out.push_back({T_LT, "<", s.line, s.column});
        advance(s);
        return true;
    case '>':
        out.push_back({T_GT, ">", s.line, s.column});
        advance(s);
        return true;
    case '!':
        out.push_back({T_NOT, "!", s.line, s.column});
        advance(s);
        return true;
    case '&':
        out.push_back({T_BITWISE_AND, "&", s.line, s.column});
        advance(s);
        return true;
    case '|':
        out.push_back({T_BITWISE_OR, "|", s.line, s.column});
        advance(s);
        return true;
    case '^':
        out.push_back({T_BITWISE_XOR, "^", s.line, s.column});
        advance(s);
        return true;
    case '~':
        out.push_back({T_BITWISE_NOT, "~", s.line, s.column});
        advance(s);
        return true;
    case '=':
        out.push_back({T_ASSIGNOP, "=", s.line, s.column});
        advance(s);
        return true;
    default:
        return false;
    }
}

// Helper: scan number token or invalid identifier like "123abc"
bool tryNumberOrInvalidIdentifier(LexerState &s, vector<Token> &out)
{
    size_t start = s.pos;
    if (!isdigit((unsigned char)s.input[s.pos]))
        return false;
    // consume digits
    while (s.pos < s.input.size() && isdigit((unsigned char)s.input[s.pos]))
        advance(s);
    // if immediately followed by identifier-start (letter, underscore or utf8), treat as invalid identifier
    if (s.pos < s.input.size())
    {
        unsigned char c = (unsigned char)s.input[s.pos];
        bool idStart = (c == '_') || isalpha(c) || c >= 128;
        if (idStart)
        {
            size_t badStart = start;
            while (s.pos < s.input.size())
            {
                unsigned char c2 = (unsigned char)s.input[s.pos];
                bool idPart = (c2 == '_') || isalpha(c2) || isdigit(c2) || c2 >= 128;
                if (!idPart)
                    break;
                advance(s);
            }
            string bad = s.input.substr(badStart, s.pos - badStart);
            out.push_back({T_ERROR, bad, s.line, s.column - (int)bad.size()});
            return true;
        }
    }
    // otherwise integer (simple)
    string num = s.input.substr(start, s.pos - start);
    out.push_back({T_INTLIT, num, s.line, s.column - (int)num.size()});
    return true;
}

bool tryFloat(LexerState &s, vector<Token> &out)
{
    // simple float: digits '.' digits [exponent]
    size_t saved = s.pos;
    if (!(s.pos < s.input.size() && isdigit((unsigned char)s.input[s.pos])))
        return false;
    while (s.pos < s.input.size() && isdigit((unsigned char)s.input[s.pos]))
        advance(s);
    if (s.pos < s.input.size() && s.input[s.pos] == '.')
    {
        advance(s);
        while (s.pos < s.input.size() && isdigit((unsigned char)s.input[s.pos]))
            advance(s);
        if (s.pos < s.input.size() && (s.input[s.pos] == 'e' || s.input[s.pos] == 'E'))
        {
            advance(s);
            if (s.pos < s.input.size() && (s.input[s.pos] == '+' || s.input[s.pos] == '-'))
                advance(s);
            while (s.pos < s.input.size() && isdigit((unsigned char)s.input[s.pos]))
                advance(s);
        }
        string num = s.input.substr(saved, s.pos - saved);
        out.push_back({T_FLOATLIT, num, s.line, s.column - (int)num.size()});
        return true;
    }
    s.pos = saved; // revert
    return false;
}

bool tryIdentifier(LexerState &s, vector<Token> &out)
{
    if (s.pos >= s.input.size())
        return false;
    unsigned char c = (unsigned char)s.input[s.pos];
    bool startOk = (c == '_') || isalpha(c) || c >= 128;
    if (!startOk)
        return false;
    size_t start = s.pos;
    advance(s);
    while (s.pos < s.input.size())
    {
        unsigned char c2 = (unsigned char)s.input[s.pos];
        bool partOk = (c2 == '_') || isalpha(c2) || isdigit(c2) || c2 >= 128;
        if (!partOk)
            break;
        advance(s);
    }
    string ident = s.input.substr(start, s.pos - start);
    auto it = s.keywords.find(ident);
    TokenType tt = (it != s.keywords.end()) ? it->second : T_IDENTIFIER;
    out.push_back({tt, ident, s.line, s.column - (int)ident.size()});
    return true;
}

vector<Token> tokenize(const string &source)
{
    vector<Token> tokens;
    LexerState st(source);
    while (st.pos < st.input.size())
    {
        skipWhitespace(st);
        skipComments(st);
        if (st.pos >= st.input.size())
            break;
        if (tryPreprocessor(st, tokens))
            continue;
        if (tryString(st, tokens))
            continue;
        if (tryChar(st, tokens))
            continue;
        if (tryOperatorsAndPunct(st, tokens))
            continue;
        if (tryFloat(st, tokens))
            continue;
        if (tryNumberOrInvalidIdentifier(st, tokens))
            continue;
        if (tryIdentifier(st, tokens))
            continue;
        // unknown single char fallback
        string s(1, st.input[st.pos]);
        tokens.push_back({T_UNKNOWN, s, st.line, st.column});
        advance(st);
    }
    tokens.push_back({T_EOF, "", st.line, st.column});
    return tokens;
}

string tokenName(TokenType t)
{
    switch (t)
    {
    case T_INT:
        return "T_INT";
    case T_FLOAT:
        return "T_FLOAT";
    case T_DOUBLE:
        return "T_DOUBLE";
    case T_CHAR:
        return "T_CHAR";
    case T_VOID:
        return "T_VOID";
    case T_BOOL:
        return "T_BOOL";
    case T_IDENTIFIER:
        return "T_IDENTIFIER";
    case T_INTLIT:
        return "T_INTLIT";
    case T_FLOATLIT:
        return "T_FLOATLIT";
    case T_STRINGLIT:
        return "T_STRINGLIT";
    case T_CHARLIT:
        return "T_CHARLIT";
    case T_BOOLLIT:
        return "T_BOOLLIT";
    case T_LPAREN:
        return "T_LPAREN";
    case T_RPAREN:
        return "T_RPAREN";
    case T_LBRACE:
        return "T_LBRACE";
    case T_RBRACE:
        return "T_RBRACE";
    case T_LBRACKET:
        return "T_LBRACKET";
    case T_RBRACKET:
        return "T_RBRACKET";
    case T_SEMICOLON:
        return "T_SEMICOLON";
    case T_COMMA:
        return "T_COMMA";
    case T_DOT:
        return "T_DOT";
    case T_ASSIGNOP:
        return "T_ASSIGNOP";
    case T_EQUALOP:
        return "T_EQUALOP";
    case T_NE:
        return "T_NE";
    case T_LT:
        return "T_LT";
    case T_GT:
        return "T_GT";
    case T_LE:
        return "T_LE";
    case T_GE:
        return "T_GE";
    case T_PLUS:
        return "T_PLUS";
    case T_MINUS:
        return "T_MINUS";
    case T_MULTIPLY:
        return "T_MULTIPLY";
    case T_DIVIDE:
        return "T_DIVIDE";
    case T_MODULO:
        return "T_MODULO";
    case T_INCREMENT:
        return "T_INCREMENT";
    case T_DECREMENT:
        return "T_DECREMENT";
    case T_AND:
        return "T_AND";
    case T_OR:
        return "T_OR";
    case T_NOT:
        return "T_NOT";
    case T_BITWISE_AND:
        return "T_BITWISE_AND";
    case T_BITWISE_OR:
        return "T_BITWISE_OR";
    case T_BITWISE_XOR:
        return "T_BITWISE_XOR";
    case T_BITWISE_NOT:
        return "T_BITWISE_NOT";
    case T_LEFT_SHIFT:
        return "T_LEFT_SHIFT";
    case T_RIGHT_SHIFT:
        return "T_RIGHT_SHIFT";
    case T_INCLUDE:
        return "T_INCLUDE";
    case T_DEFINE:
        return "T_DEFINE";
    case T_PREPROCESSOR:
        return "T_PREPROCESSOR";
    case T_IF:
        return "T_IF";
    case T_ELSE:
        return "T_ELSE";
    case T_WHILE:
        return "T_WHILE";
    case T_FOR:
        return "T_FOR";
    case T_RETURN:
        return "T_RETURN";
    case T_BREAK:
        return "T_BREAK";
    case T_CONTINUE:
        return "T_CONTINUE";
    case T_EOF:
        return "T_EOF";
    case T_UNKNOWN:
        return "T_UNKNOWN";
    case T_ERROR:
        return "T_ERROR";
    default:
        return "T_UNKNOWN";
    }
}

int main()
{
    string code = R"(
fn int my_fn(int x, float y) {
    string my_str = "hmm";
    bool my_bool = x == 40;
    return x;
}
)";
    vector<Token> tokens = tokenize(code);
    cout << "Tokenized output (regex lexer improved):\n";
    for (auto &tk : tokens)
    {
        if (tk.type == T_EOF)
            continue;
        if (tk.type == T_ERROR)
            cout << "ERROR: " << tk.value << " at " << tk.line << ":" << tk.column << "\n";
        else
            cout << tokenName(tk.type) << "(\"" << tk.value << "\"), ";
    }
    cout << endl;
    system("pause");
    return 0;
}
