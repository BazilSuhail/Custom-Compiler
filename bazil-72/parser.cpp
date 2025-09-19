#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

using namespace std;

// ---------------- Tokens ----------------
enum class TokenType {
    INT, STRING, IDENT, INTLIT, STRLIT,
    ASSIGN, PLUS, SEMICOLON, END
};

struct Token {
    TokenType type;
    string value;
};

// ---------------- AST ----------------
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

using ASTPtr = unique_ptr<ASTNode>;

struct Expr : ASTNode {};
struct Stmt : ASTNode {};

struct IntLit : Expr {
    int value;
    IntLit(int v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "IntLit(" << value << ")\n";
    }
};

struct StrLit : Expr {
    string value;
    StrLit(const string &v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "StrLit(\"" << value << "\")\n";
    }
};

struct Identifier : Expr {
    string name;
    Identifier(const string &n) : name(n) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Identifier(" << name << ")\n";
    }
};

struct BinaryExpr : Expr {
    string op;
    ASTPtr left, right;
    BinaryExpr(const string &o, ASTPtr l, ASTPtr r)
        : op(o), left(move(l)), right(move(r)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BinaryExpr(" << op << ")\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

struct VarDecl : Stmt {
    string type, name;
    ASTPtr expr;
    VarDecl(const string &t, const string &n, ASTPtr e)
        : type(t), name(n), expr(move(e)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "VarDecl(" << type << " " << name << ")\n";
        if (expr) expr->print(indent + 2);
    }
};

// ---------------- Parser ----------------
class Parser {
    vector<Token> tokens;
    size_t pos = 0;

    Token &peek() {
        if (pos >= tokens.size()) throw runtime_error("UnexpectedEOF");
        return tokens[pos];
    }
    Token &consume(TokenType t, const string &err) {
        if (peek().type != t) throw runtime_error(err);
        return tokens[pos++];
    }

    // Parse expressions (only literals, identifiers, +)
    ASTPtr parseExpr() {
        ASTPtr left;
        if (peek().type == TokenType::INTLIT) {
            left = make_unique<IntLit>(stoi(consume(TokenType::INTLIT, "Expected int").value));
        } else if (peek().type == TokenType::STRLIT) {
            left = make_unique<StrLit>(consume(TokenType::STRLIT, "Expected string").value);
        } else if (peek().type == TokenType::IDENT) {
            left = make_unique<Identifier>(consume(TokenType::IDENT, "Expected identifier").value);
        } else {
            throw runtime_error("ExpectedExpr");
        }

        while (peek().type == TokenType::PLUS) {
            consume(TokenType::PLUS, "Expected '+'");
            ASTPtr right = parseExpr();
            left = make_unique<BinaryExpr>("+", move(left), move(right));
        }
        return left;
    }

    // Parse variable declaration
    ASTPtr parseVarDecl() {
        string type;
        if (peek().type == TokenType::INT) {
            type = consume(TokenType::INT, "Expected type").value;
        } else if (peek().type == TokenType::STRING) {
            type = consume(TokenType::STRING, "Expected type").value;
        } else {
            throw runtime_error("ExpectedTypeToken");
        }

        string name = consume(TokenType::IDENT, "Expected identifier").value;

        consume(TokenType::ASSIGN, "Expected '='");

        ASTPtr expr = parseExpr();

        consume(TokenType::SEMICOLON, "Expected ';'");

        return make_unique<VarDecl>(type, name, move(expr));
    }

public:
    Parser(vector<Token> t) : tokens(move(t)) {}
    vector<ASTPtr> parse() {
        vector<ASTPtr> program;
        while (peek().type != TokenType::END) {
            program.push_back(parseVarDecl());
        }
        return program;
    }
};

// ---------------- Main (Demo) ----------------
int main() {
    // Hardcoded token stream: int x = 42; string msg = "hi" + " there";
    vector<Token> tokens = {
        {TokenType::INT, "int"},
        {TokenType::IDENT, "x"},
        {TokenType::ASSIGN, "="},
        {TokenType::INTLIT, "42"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::STRING, "string"},
        {TokenType::IDENT, "msg"},
        {TokenType::ASSIGN, "="},
        {TokenType::STRLIT, "hi"},
        {TokenType::PLUS, "+"},
        {TokenType::STRLIT, " there"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::END, ""}
    };

    try {
        Parser parser(tokens);
        auto ast = parser.parse();

        cout << "=== AST ===\n";
        for (auto &node : ast) {
            node->print();
        }
    } catch (exception &e) {
        cerr << "ParseError: " << e.what() << "\n";
    }

    return 0;
}