#include <iostream>
#include <cstring>

using namespace std;

enum TokenType
{
    // Data types / keywords
    T_INT,
    T_FLOAT,
    T_DOUBLE,
    T_CHAR,
    T_VOID,
    T_BOOL,
    // Literals & identifiers
    T_IDENTIFIER,
    T_INTLIT,
    T_FLOATLIT,
    T_STRINGLIT,
    T_CHARLIT,
    T_BOOLLIT,
    // Punctuation
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_LBRACKET,
    T_RBRACKET,
    T_SEMICOLON,
    T_COMMA,
    T_DOT,
    // Operators
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
    // Preprocessor
    T_INCLUDE,
    T_DEFINE,
    T_PREPROCESSOR,
    // Control-flow & storage
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
    char value[512];
    int line;
    int column;
};

static const int MAX_TOKENS = 2000;

struct Keyword
{
    const char *word;
    TokenType type;
};

static const Keyword keywords[] = {
    {"int", T_INT}, {"float", T_FLOAT}, {"double", T_DOUBLE}, {"char", T_CHAR}, {"void", T_VOID}, {"bool", T_BOOL}, {"if", T_IF}, {"else", T_ELSE}, {"while", T_WHILE}, {"for", T_FOR}, {"return", T_RETURN}, {"break", T_BREAK}, {"continue", T_CONTINUE}};

bool isWhitespace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f';
}
bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }
bool isAsciiAlpha(char ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

bool isIdentifierStart(unsigned char ch)
{
    return ch == '_' || isAsciiAlpha((char)ch) || ch >= 128;
}
bool isIdentifierPart(unsigned char ch)
{
    return ch == '_' || isAsciiAlpha((char)ch) || isDigit((char)ch) || ch >= 128;
}

struct LexerState
{
    const char *src;
    int pos;
    int length;
    int line;
    int column;
};

LexerState makeState(const char *s)
{
    LexerState st;
    st.src = s;
    st.pos = 0;
    st.length = (int)strlen(s);
    st.line = 1;
    st.column = 1;
    return st;
}

void advance(LexerState &st, int n = 1)
{
    for (int i = 0; i < n && st.pos < st.length; i++)
    {
        if (st.src[st.pos] == '\n')
        {
            st.line++;
            st.column = 1;
        }
        else
            st.column++;
        st.pos++;
    }
}

void skipWhitespace(LexerState &st)
{
    while (st.pos < st.length && isWhitespace(st.src[st.pos]))
        advance(st);
}

void skipComments(LexerState &st)
{
    if (st.pos + 1 >= st.length)
        return;
    if (st.src[st.pos] == '/' && st.src[st.pos + 1] == '/')
    {
        // single-line
        while (st.pos < st.length && st.src[st.pos] != '\n')
            advance(st);
    }
    else if (st.src[st.pos] == '/' && st.src[st.pos + 1] == '*')
    {
        advance(st, 2);
        while (st.pos + 1 < st.length)
        {
            if (st.src[st.pos] == '*' && st.src[st.pos + 1] == '/')
            {
                advance(st, 2);
                return;
            }
            advance(st);
        }
    }
}

Token makeToken(TokenType t, const char *val, int line, int col)
{
    Token tk;
    tk.type = t;
    tk.line = line;
    tk.column = col;
    tk.value[0] = '\0';
    if (val)
        strncpy(tk.value, val, sizeof(tk.value) - 1);
    return tk;
}

TokenType checkKeyword(const char *ident)
{
    for (auto &k : keywords)
    {
        if (strcmp(k.word, ident) == 0)
            return k.type;
    }
    return T_IDENTIFIER;
}

bool startsWith(LexerState &st, const char *s)
{
    int len = (int)strlen(s);
    if (st.pos + len > st.length)
        return false;
    return strncmp(st.src + st.pos, s, len) == 0;
}

bool tryPreprocessor(LexerState &st, Token &out)
{
    if (st.src[st.pos] != '#')
        return false;
    int startPos = st.pos;
    int startCol = st.column;
    // read directive up to whitespace or newline
    int i = st.pos;
    while (i < st.length && !isWhitespace(st.src[i]))
        i++;
    int len = i - st.pos;
    string tmp(st.src + st.pos, st.src + st.pos + len);
    if (tmp == "#include")
        out = makeToken(T_INCLUDE, tmp.c_str(), st.line, startCol);
    else if (tmp == "#define")
        out = makeToken(T_DEFINE, tmp.c_str(), st.line, startCol);
    else
        out = makeToken(T_PREPROCESSOR, tmp.c_str(), st.line, startCol);
    advance(st, len);
    return true;
}

bool tryStringLiteral(LexerState &st, Token &out)
{
    if (st.src[st.pos] != '"')
        return false;
    int startLine = st.line, startCol = st.column;
    string acc;
    acc.push_back('"');
    advance(st); // consume opening "
    while (st.pos < st.length)
    {
        char c = st.src[st.pos];
        if (c == '\\')
        {
            // keep escape sequence
            if (st.pos + 1 < st.length)
            {
                acc.push_back('\\');
                acc.push_back(st.src[st.pos + 1]);
                advance(st, 2);
            }
            else
            {
                // truncated escape
                advance(st);
            }
        }
        else if (c == '"')
        {
            acc.push_back('"');
            advance(st);
            out = makeToken(T_STRINGLIT, acc.c_str(), startLine, startCol);
            return true;
        }
        else
        {
            acc.push_back(c);
            advance(st);
        }
    }
    // if we reach here => unterminated string
    out = makeToken(T_ERROR, "Unterminated string literal", startLine, startCol);
    return true;
}

bool tryCharLiteral(LexerState &st, Token &out)
{
    if (st.src[st.pos] != '\'')
        return false;
    int startLine = st.line, startCol = st.column;
    string acc;
    acc.push_back('\'');
    advance(st); // consume '
    if (st.pos < st.length)
    {
        if (st.src[st.pos] == '\\')
        {
            // escape
            acc.push_back('\\');
            if (st.pos + 1 < st.length)
            {
                acc.push_back(st.src[st.pos + 1]);
                advance(st, 2);
            }
            else
                advance(st);
        }
        else
        {
            acc.push_back(st.src[st.pos]);
            advance(st);
        }
    }
    if (st.pos < st.length && st.src[st.pos] == '\'')
    {
        acc.push_back('\'');
        advance(st);
        out = makeToken(T_CHARLIT, acc.c_str(), startLine, startCol);
    }
    else
    {
        out = makeToken(T_ERROR, "Unterminated char literal", startLine, startCol);
    }
    return true;
}

bool tryOperatorOrPunct(LexerState &st, Token &out)
{
    if (st.pos >= st.length)
        return false;
    int startLine = st.line, startCol = st.column;
    // two-char
    if (st.pos + 1 < st.length)
    {
        string two = string() + st.src[st.pos] + st.src[st.pos + 1];
        if (two == "==")
        {
            out = makeToken(T_EQUALOP, "==", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "!=")
        {
            out = makeToken(T_NE, "!=", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "<=")
        {
            out = makeToken(T_LE, "<=", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == ">=")
        {
            out = makeToken(T_GE, ">=", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "&&")
        {
            out = makeToken(T_AND, "&&", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "||")
        {
            out = makeToken(T_OR, "||", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "++")
        {
            out = makeToken(T_INCREMENT, "++", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "--")
        {
            out = makeToken(T_DECREMENT, "--", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "<<")
        {
            out = makeToken(T_LEFT_SHIFT, "<<", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == ">>")
        {
            out = makeToken(T_RIGHT_SHIFT, ">>", startLine, startCol);
            advance(st, 2);
            return true;
        }
        if (two == "+=" || two == "-=" || two == "*=" || two == "/=" || two == "%=")
        {
            out = makeToken(T_ASSIGNOP, two.c_str(), startLine, startCol);
            advance(st, 2);
            return true;
        }
    }
    // single-char
    char c = st.src[st.pos];
    switch (c)
    {
    case '(':
        out = makeToken(T_LPAREN, "(", startLine, startCol);
        advance(st);
        return true;
    case ')':
        out = makeToken(T_RPAREN, ")", startLine, startCol);
        advance(st);
        return true;
    case '{':
        out = makeToken(T_LBRACE, "{", startLine, startCol);
        advance(st);
        return true;
    case '}':
        out = makeToken(T_RBRACE, "}", startLine, startCol);
        advance(st);
        return true;
    case '[':
        out = makeToken(T_LBRACKET, "[", startLine, startCol);
        advance(st);
        return true;
    case ']':
        out = makeToken(T_RBRACKET, "]", startLine, startCol);
        advance(st);
        return true;
    case ';':
        out = makeToken(T_SEMICOLON, ";", startLine, startCol);
        advance(st);
        return true;
    case ',':
        out = makeToken(T_COMMA, ",", startLine, startCol);
        advance(st);
        return true;
    case '.':
        out = makeToken(T_DOT, ".", startLine, startCol);
        advance(st);
        return true;
    case '+':
        out = makeToken(T_PLUS, "+", startLine, startCol);
        advance(st);
        return true;
    case '-':
        out = makeToken(T_MINUS, "-", startLine, startCol);
        advance(st);
        return true;
    case '*':
        out = makeToken(T_MULTIPLY, "*", startLine, startCol);
        advance(st);
        return true;
    case '/':
        out = makeToken(T_DIVIDE, "/", startLine, startCol);
        advance(st);
        return true;
    case '%':
        out = makeToken(T_MODULO, "%", startLine, startCol);
        advance(st);
        return true;
    case '<':
        out = makeToken(T_LT, "<", startLine, startCol);
        advance(st);
        return true;
    case '>':
        out = makeToken(T_GT, ">", startLine, startCol);
        advance(st);
        return true;
    case '!':
        out = makeToken(T_NOT, "!", startLine, startCol);
        advance(st);
        return true;
    case '&':
        out = makeToken(T_BITWISE_AND, "&", startLine, startCol);
        advance(st);
        return true;
    case '|':
        out = makeToken(T_BITWISE_OR, "|", startLine, startCol);
        advance(st);
        return true;
    case '^':
        out = makeToken(T_BITWISE_XOR, "^", startLine, startCol);
        advance(st);
        return true;
    case '~':
        out = makeToken(T_BITWISE_NOT, "~", startLine, startCol);
        advance(st);
        return true;
    case '=':
        out = makeToken(T_ASSIGNOP, "=", startLine, startCol);
        advance(st);
        return true;
    default:
        return false;
    }
}

bool tryNumberOrInvalidIdent(LexerState &st, Token &out)
{
    // If starts with digit: check whether the run of [0-9]+ is immediately followed by a letter/underscore/utf8 start -> that's invalid identifier (error).
    if (!isDigit(st.src[st.pos]))
        return false;
    int start = st.pos, startLine = st.line, startCol = st.column;
    // consume digits
    while (st.pos < st.length && isDigit(st.src[st.pos]))
        advance(st);
    // If next is identifier-start (letter/underscore/utf8), then that's an invalid identifier like "123abc"
    if (st.pos < st.length && isIdentifierStart((unsigned char)st.src[st.pos]))
    {
        // consume until a non-identifier-part
        int errStart = start;
        while (st.pos < st.length && isIdentifierPart((unsigned char)st.src[st.pos]))
            advance(st);
        // produce error token with the full bad token
        int len = st.pos - errStart;
        string bad(st.src + errStart, st.src + errStart + len);
        out = makeToken(T_ERROR, bad.c_str(), startLine, startCol);
        return true;
    }
    // else produce integer token (we could also detect floats here)
    int len = st.pos - start;
    string num(st.src + start, st.src + start + len);
    out = makeToken(T_INTLIT, num.c_str(), startLine, startCol);
    return true;
}

bool tryFloat(LexerState &st, Token &out)
{
    // simple float detection: digits '.' digits [exponent optional]
    if (st.pos >= st.length || !isDigit(st.src[st.pos]))
        return false;
    int saved = st.pos;
    int startLine = st.line, startCol = st.column;
    // lead digits
    while (st.pos < st.length && isDigit(st.src[st.pos]))
        advance(st);
    if (st.pos < st.length && st.src[st.pos] == '.')
    {
        advance(st); // consume dot
        bool anyAfterDot = false;
        while (st.pos < st.length && isDigit(st.src[st.pos]))
        {
            anyAfterDot = true;
            advance(st);
        }
        // optional exponent
        if (st.pos < st.length && (st.src[st.pos] == 'e' || st.src[st.pos] == 'E'))
        {
            advance(st);
            if (st.pos < st.length && (st.src[st.pos] == '+' || st.src[st.pos] == '-'))
                advance(st);
            while (st.pos < st.length && isDigit(st.src[st.pos]))
                advance(st);
        }
        int len = st.pos - saved;
        string num(st.src + saved, st.src + saved + len);
        out = makeToken(T_FLOATLIT, num.c_str(), startLine, startCol);
        return true;
    }
    else
    {
        // restore (it was integer) — but tryNumberOrInvalidIdent handled integers first, so we won't usually reach here
        st.pos = saved; // restore
        return false;
    }
}

bool tryIdentifier(LexerState &st, Token &out)
{
    if (!isIdentifierStart((unsigned char)st.src[st.pos]))
        return false;
    int start = st.pos, startLine = st.line, startCol = st.column;
    // consume identifier bytes (basic UTF-8 friendliness)
    while (st.pos < st.length && isIdentifierPart((unsigned char)st.src[st.pos]))
        advance(st);
    int len = st.pos - start;
    string ident(st.src + start, st.src + start + len);
    TokenType k = checkKeyword(ident.c_str());
    out = makeToken(k, ident.c_str(), startLine, startCol);
    return true;
}

const char *tokenName(TokenType t)
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
    const char *code = R"(
#include <stdio.h>
#define MAX 100

        int main() {
            // This is a comment
            int x = 42;
            float y = 3.14159f;
            char c = '\n';
            char *str = "Hello\tWorld! \"quoted\"";

            /* Multi-line
               comment */

            if (x > 0 && x <= MAX)
            {
                printf("Value: %d\n", x);
            }
            else if (x == 0 || x >= MAX)
            {
                printf("Boundary value\n");
            }

            for (int i = 0; i < 10; i++)
            {
                x += i;
                x++;
                x--;
            }

            while (x > 0)
            {
                x >>= 1;   // Right shift
                x &= 0xFF; // Bitwise AND
            }

            return 0;
        })";

    LexerState st = makeState(code);
    Token output;
    cout << "Tokenized output (manual lexer improved):\n";

    while (st.pos < st.length)
    {
        skipWhitespace(st);
        skipComments(st);
        if (st.pos >= st.length)
            break;

        if (tryPreprocessor(st, output))
        {
            cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
            continue;
        }
        if (tryStringLiteral(st, output))
        {
            cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
            continue;
        }
        if (tryCharLiteral(st, output))
        {
            cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
            continue;
        }
        if (tryOperatorOrPunct(st, output))
        {
            cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
            continue;
        }
        if (tryFloat(st, output))
        {
            cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
            continue;
        }
        if (tryNumberOrInvalidIdent(st, output))
        {
            cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
            continue;
        }
        if (tryIdentifier(st, output))
        {
            cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
            continue;
        }

        // unknown single char
        output = makeToken(T_UNKNOWN, string(1, st.src[st.pos]).c_str(), st.line, st.column);
        cout << tokenName(output.type) << "(\"" << output.value << "\"), ";
        advance(st);
    }

    cout << endl;
    system("pause");
    return 0;
}
