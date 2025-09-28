#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <cctype>

using namespace std;

// === Token Types (from your lexer) ===
enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_VOID, T_BOOL,
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT, T_CHARLIT, T_BOOLLIT,
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_SEMICOLON, T_COMMA, T_DOT,
    T_ASSIGNOP, T_EQUALOP, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,
    T_INCREMENT, T_DECREMENT,
    T_AND, T_OR, T_NOT,
    T_IF, T_ELSE, T_WHILE, T_RETURN, T_PRINT, T_MAIN,
    T_SINGLE_COMMENT, T_MULTI_COMMENT,
    // new
    T_DO, T_FOR, T_SWITCH, T_CASE, T_DEFAULT, T_BREAK, T_COLON,
    T_ERROR, T_EOF
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
};

// === Parse Errors ===
enum class ParseErrorType {
    UnexpectedEOF,
    FailedToFindToken,
    ExpectedTypeToken,
    ExpectedIdentifier,
    UnexpectedToken,
    ExpectedFloatLit,
    ExpectedIntLit,
    ExpectedStringLit,
    ExpectedBoolLit,
    ExpectedExpr,
    InvalidCallTarget,
    MissingSemicolon,
    UnclosedBlock
};

struct ParseError {
    ParseErrorType type;
    Token token;
    string message;

    ParseError(ParseErrorType t, const Token& tok) : type(t), token(tok) {
        switch (t) {
            case ParseErrorType::UnexpectedEOF:
                message = "Unexpected end of file";
                break;
            case ParseErrorType::FailedToFindToken:
                message = "Failed to find expected token: " + getTokenName(tok.type);
                break;
            case ParseErrorType::ExpectedTypeToken:
                message = "Expected type token (int, float, etc.)";
                break;
            case ParseErrorType::ExpectedIdentifier:
                message = "Expected identifier";
                break;
            case ParseErrorType::UnexpectedToken:
                message = "Unexpected token: " + getTokenName(tok.type) + " (" + tok.value + ")";
                break;
            case ParseErrorType::ExpectedFloatLit:
                message = "Expected float literal";
                break;
            case ParseErrorType::ExpectedIntLit:
                message = "Expected int literal";
                break;
            case ParseErrorType::ExpectedStringLit:
                message = "Expected string literal";
                break;
            case ParseErrorType::ExpectedBoolLit:
                message = "Expected bool literal";
                break;
            case ParseErrorType::InvalidCallTarget:
                message = "Invalid function call target: only identifiers can be called";
                break;
            case ParseErrorType::MissingSemicolon:
                message = "Missing semicolon at end of statement";
                break;
            case ParseErrorType::UnclosedBlock:
                message = "Unclosed block, expected '}' before end of file";
                break;
            case ParseErrorType::ExpectedExpr:
                message = "Expected expression";
                break;
        }
    }

    string getTokenName(TokenType t) const {
        switch (t) {
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
            case T_LPAREN: return "T_LPAREN";
            case T_RPAREN: return "T_RPAREN";
            case T_LBRACE: return "T_LBRACE";
            case T_RBRACE: return "T_RBRACE";
            case T_SEMICOLON: return "T_SEMICOLON";
            case T_COMMA: return "T_COMMA";
            case T_DOT: return "T_DOT";
            case T_ASSIGNOP: return "T_ASSIGNOP";
            case T_PLUS: return "T_PLUS";
            case T_MINUS: return "T_MINUS";
            case T_MULTIPLY: return "T_MULTIPLY";
            case T_DIVIDE: return "T_DIVIDE";
            case T_IF: return "T_IF";
            case T_ELSE: return "T_ELSE";
            case T_WHILE: return "T_WHILE";
            case T_RETURN: return "T_RETURN";
            case T_PRINT: return "T_PRINT";
            // new tokens
            case T_DO: return "T_DO";
            case T_FOR: return "T_FOR";
            case T_SWITCH: return "T_SWITCH";
            case T_CASE: return "T_CASE";
            case T_DEFAULT: return "T_DEFAULT";
            case T_BREAK: return "T_BREAK";
            case T_COLON: return "T_COLON";

            case T_MAIN: return "T_MAIN";
            case T_EOF: return "T_EOF";
            default: return "T_UNKNOWN";
        }
    }
};

// === AST Node Types ===
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};
using ASTPtr = unique_ptr<ASTNode>;

// === Literal Types ===
struct IntLiteral : public ASTNode {
    int value;
    IntLiteral(int v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "IntLiteral(" << value << ")\n";
    }
};

struct FloatLiteral : public ASTNode {
    double value;
    FloatLiteral(double v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "FloatLiteral(" << value << ")\n";
    }
};

struct StringLiteral : public ASTNode {
    string value;
    StringLiteral(const string& v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "StringLiteral(\"" << value << "\")\n";
    }
};

struct CharLiteral : public ASTNode {
    char value;
    CharLiteral(char v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "CharLiteral('" << value << "')\n";
    }
};

struct BoolLiteral : public ASTNode {
    bool value;
    BoolLiteral(bool v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BoolLiteral(" << (value ? "true" : "false") << ")\n";
    }
};

struct Identifier : public ASTNode {
    string name;
    Identifier(const string& n) : name(n) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Identifier(\"" << name << "\")\n";
    }
};

// === Expression Types ===
struct BinaryExpr : public ASTNode {
    TokenType op;
    ASTPtr left;
    ASTPtr right;
    BinaryExpr(TokenType o, ASTPtr l, ASTPtr r) : op(o), left(move(l)), right(move(r)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BinaryExpr(";
        printOp(op);
        cout << ")\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
    void printOp(TokenType t) const {
        switch (t) {
            case T_PLUS: cout << "+"; break;
            case T_MINUS: cout << "-"; break;
            case T_MULTIPLY: cout << "*"; break;
            case T_DIVIDE: cout << "/"; break;
            case T_MODULO: cout << "%"; break;
            case T_EQUALOP: cout << "=="; break;
            case T_NE: cout << "!="; break;
            case T_LT: cout << "<"; break;
            case T_GT: cout << ">"; break;
            case T_LE: cout << "<="; break;
            case T_GE: cout << ">="; break;
            case T_AND: cout << "&&"; break;
            case T_OR: cout << "||"; break;
            case T_ASSIGNOP: cout << "="; break;
            default: cout << "op_" << (int)t; break;
        }
    }
};

struct UnaryExpr : public ASTNode {
    TokenType op;
    ASTPtr operand;
    UnaryExpr(TokenType o, ASTPtr opd) : op(o), operand(move(opd)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "UnaryExpr(";
        if (op == T_MINUS) cout << "-";
        else if (op == T_NOT) cout << "!";
        else cout << "unary_op";
        cout << ")\n";
        operand->print(indent + 2);
    }
};

// ============= language syntax
struct IncludeStmt : public ASTNode {
    string header;
    IncludeStmt(string h) : header(move(h)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "IncludeStmt(\"" << header << "\")\n";
    }
};

struct CallExpr : public ASTNode {
    ASTPtr callee;
    vector<ASTPtr> args;
    CallExpr(ASTPtr c, vector<ASTPtr> a) : callee(move(c)), args(move(a)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "CallExpr\n";
        cout << string(indent + 2, ' ') << "Callee:\n";
        callee->print(indent + 4);
        if (!args.empty()) {
            cout << string(indent + 2, ' ') << "Args:\n";
            for (const auto& arg : args) arg->print(indent + 4);
        }
    }
};

// === Statement Types ===
struct VarDecl : public ASTNode {
    TokenType type;
    string name;
    ASTPtr initializer;
    VarDecl(TokenType t, const string& n, ASTPtr init = nullptr)
        : type(t), name(n), initializer(move(init)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "VarDecl(";
        printType(type);
        cout << ", \"" << name << "\")\n";
        if (initializer) initializer->print(indent + 2);
    }
    void printType(TokenType t) const {
        switch (t) {
            case T_INT: cout << "int"; break;
            case T_FLOAT: cout << "float"; break;
            case T_DOUBLE: cout << "double"; break;
            case T_CHAR: cout << "char"; break;
            case T_BOOL: cout << "bool"; break;
            case T_VOID: cout << "void"; break;
            default: cout << "unknown_type"; break;
        }
    }
};

struct BlockStmt : public ASTNode {
    vector<ASTPtr> body;
    BlockStmt(vector<ASTPtr> b) : body(move(b)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BlockStmt\n";
        for (const auto& stmt : body) stmt->print(indent + 2);
    }
};

struct FunctionDecl : public ASTNode {
    TokenType returnType;
    string name;
    vector<pair<TokenType, string>> params;
    vector<ASTPtr> body;
    FunctionDecl(TokenType rt, const string& n,
                 vector<pair<TokenType, string>> p,
                 vector<ASTPtr> b)
        : returnType(rt), name(n), params(move(p)), body(move(b)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "FunctionDecl(";
        printType(returnType);
        cout << ", \"" << name << "\")\n";
        for (const auto& param : params) {
            cout << string(indent + 2, ' ') << "Param: ";
            switch (param.first) {
                case T_INT: cout << "int"; break;
                case T_FLOAT: cout << "float"; break;
                case T_DOUBLE: cout << "double"; break;
                case T_CHAR: cout << "char"; break;
                case T_BOOL: cout << "bool"; break;
                case T_VOID: cout << "void"; break;
                default: cout << "unknown"; break;
            }
            cout << " " << param.second << "\n";
        }
        cout << string(indent + 2, ' ') << "Body:\n";
        for (const auto& stmt : body) stmt->print(indent + 4);
    }
    void printType(TokenType t) const {
        switch (t) {
            case T_INT: cout << "int"; break;
            case T_FLOAT: cout << "float"; break;
            case T_DOUBLE: cout << "double"; break;
            case T_CHAR: cout << "char"; break;
            case T_BOOL: cout << "bool"; break;
            case T_VOID: cout << "void"; break;
            default: cout << "unknown_type"; break;
        }
    }
};

struct MainDecl : public ASTNode {
    vector<ASTPtr> body;
    MainDecl(vector<ASTPtr> b) : body(move(b)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "MainDecl\n";
        for (const auto& stmt : body) stmt->print(indent + 2);
    }
};

struct IfStmt : public ASTNode {
    ASTPtr condition;
    vector<ASTPtr> ifBody;
    vector<ASTPtr> elseBody;
    IfStmt(ASTPtr cond, vector<ASTPtr> ifb, vector<ASTPtr> elseb)
        : condition(move(cond)), ifBody(move(ifb)), elseBody(move(elseb)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "IfStmt\n";
        cout << string(indent + 2, ' ') << "Condition:\n";
        condition->print(indent + 4);
        cout << string(indent + 2, ' ') << "IfBody:\n";
        for (const auto& stmt : ifBody) stmt->print(indent + 4);
        if (!elseBody.empty()) {
            cout << string(indent + 2, ' ') << "ElseBody:\n";
            for (const auto& stmt : elseBody) stmt->print(indent + 4);
        }
    }
};

struct WhileStmt : public ASTNode {
    ASTPtr condition;
    vector<ASTPtr> body;
    WhileStmt(ASTPtr cond, vector<ASTPtr> b)
        : condition(move(cond)), body(move(b)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "WhileStmt\n";
        cout << string(indent + 2, ' ') << "Condition:\n";
        condition->print(indent + 4);
        cout << string(indent + 2, ' ') << "Body:\n";
        for (const auto& stmt : body) stmt->print(indent + 4);
    }
};

struct DoWhileStmt : ASTNode {
    ASTPtr body;
    ASTPtr condition;
    DoWhileStmt(ASTPtr b, ASTPtr c) : body(move(b)), condition(move(c)) {}
    void print(int indent=0) const override {
        cout << string(indent, ' ') << "DoWhileStmt\n";
        cout << string(indent+2, ' ') << "Body:\n";
        body->print(indent+4);
        cout << string(indent+2, ' ') << "Condition:\n";
        condition->print(indent+4);
    }
};

struct ForStmt : ASTNode {
    ASTPtr init;
    ASTPtr condition;
    ASTPtr update;
    ASTPtr body;
    ForStmt(ASTPtr i, ASTPtr c, ASTPtr u, ASTPtr b)
        : init(move(i)), condition(move(c)), update(move(u)), body(move(b)) {}
    void print(int indent=0) const override {
        cout << string(indent, ' ') << "ForStmt\n";
        if (init) { cout << string(indent+2, ' ') << "Init:\n"; init->print(indent+4); }
        if (condition) { cout << string(indent+2, ' ') << "Condition:\n"; condition->print(indent+4); }
        if (update) { cout << string(indent+2, ' ') << "Update:\n"; update->print(indent+4); }
        cout << string(indent+2, ' ') << "Body:\n";
        body->print(indent+4);
    }
};

struct CaseBlock : ASTNode {
    ASTPtr value;
    vector<ASTPtr> body;
    CaseBlock(ASTPtr v, vector<ASTPtr> b) : value(move(v)), body(move(b)) {}
    void print(int indent=0) const override {
        cout << string(indent, ' ') << "CaseBlock\n";
        cout << string(indent+2, ' ') << "Value:\n";
        value->print(indent+4);
        cout << string(indent+2, ' ') << "Body:\n";
        for (auto& stmt : body) stmt->print(indent+4);
    }
};

struct SwitchStmt : ASTNode {
    ASTPtr expression;
    vector<ASTPtr> cases;
    vector<ASTPtr> defaultBody;
    SwitchStmt(ASTPtr e, vector<ASTPtr> c, vector<ASTPtr> d)
        : expression(move(e)), cases(move(c)), defaultBody(move(d)) {}
    void print(int indent=0) const override {
        cout << string(indent, ' ') << "SwitchStmt\n";
        cout << string(indent+2, ' ') << "Expression:\n";
        expression->print(indent+4);
        cout << string(indent+2, ' ') << "Cases:\n";
        for (auto& c : cases) c->print(indent+4);
        if (!defaultBody.empty()) {
            cout << string(indent+2, ' ') << "Default:\n";
            for (auto& stmt : defaultBody) stmt->print(indent+4);
        }
    }
};


struct ReturnStmt : public ASTNode {
    ASTPtr value;
    ReturnStmt(ASTPtr val = nullptr) : value(move(val)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ReturnStmt\n";
        if (value) value->print(indent + 2);
    }
};

struct PrintStmt : public ASTNode {
    vector<ASTPtr> args;
    PrintStmt(vector<ASTPtr> a) : args(move(a)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "PrintStmt\n";
        for (const auto& arg : args) arg->print(indent + 2);
    }
};

struct ExpressionStmt : public ASTNode {
    ASTPtr expr;
    ExpressionStmt(ASTPtr e) : expr(move(e)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ExpressionStmt\n";
        if (expr) expr->print(indent + 2);
    }
};

// === Parser (refactored helpers) ===
class Parser {
private:
    vector<Token> tokens;
    size_t current;
    Token currentToken;

    enum Precedence {
        LOWEST = 0,
        ASSIGNMENT = 1,
        LOGICAL_OR = 2,
        LOGICAL_AND = 3,
        EQUALITY = 4,
        COMPARISON = 5,
        TERM = 6,
        FACTOR = 7,
        UNARY = 8,
        CALL = 9,
    };

    // Convenience helpers
    const Token& peek(size_t offset = 0) const {
        size_t idx = current + offset;
        if (idx < tokens.size()) return tokens[idx];
        static Token eofTok = {T_EOF, "EOF", -1, -1};
        return eofTok;
    }

    bool isAtEnd() const { return currentToken.type == T_EOF; }

    void advance() {
        if (current < tokens.size()) current++;
        if (current < tokens.size()) currentToken = tokens[current];
        else currentToken = {T_EOF, "EOF", -1, -1};
    }

    bool check(TokenType type) const { return currentToken.type == type; }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    Token expect(TokenType type, ParseErrorType errType = ParseErrorType::FailedToFindToken) {
        if (currentToken.type == type) {
            Token t = currentToken;
            advance();
            return t;
        }
        throw ParseError(errType, currentToken);
    }

    void consumeSemicolon() {
        if (!match(T_SEMICOLON)) throw ParseError(ParseErrorType::MissingSemicolon, currentToken);
    }

    // Precedence mapping
    int getPrecedence(TokenType type) const {
        switch (type) {
            case T_ASSIGNOP: return ASSIGNMENT;
            case T_OR: return LOGICAL_OR;
            case T_AND: return LOGICAL_AND;
            case T_EQUALOP: case T_NE: return EQUALITY;
            case T_LT: case T_GT: case T_LE: case T_GE: return COMPARISON;
            case T_PLUS: case T_MINUS: return TERM;
            case T_MULTIPLY: case T_DIVIDE: case T_MODULO: return FACTOR;
            case T_LPAREN: return CALL;
            default: return LOWEST;
        }
    }

    bool isTypeToken(TokenType type) const {
        return type == T_INT || type == T_FLOAT || type == T_DOUBLE ||
               type == T_CHAR || type == T_BOOL || type == T_VOID;
    }

    bool isDeclarationStart(TokenType type) const {
        return isTypeToken(type) || type == T_MAIN || type == T_PRINT;
    }

    // Small utility to check if ASTPtr is Identifier
    bool isIdentifierNode(const ASTPtr& node) const {
        return node && dynamic_cast<Identifier*>(node.get()) != nullptr;
    }

    // ---- Pratt parser core ----
    ASTPtr parseExpression(int precedence = LOWEST) {
        ASTPtr left = parsePrefix();

        while (!isAtEnd() && getPrecedence(currentToken.type) > precedence) {
            left = parseInfix(move(left), getPrecedence(currentToken.type));
        }

        return left;
    }

    ASTPtr parsePrefix() {
        switch (currentToken.type) {
            case T_INTLIT:    return parseIntLiteral();
            case T_FLOATLIT:  return parseFloatLiteral();
            case T_STRINGLIT: return parseStringLiteral();
            case T_CHARLIT:   return parseCharLiteral();
            case T_BOOLLIT:   return parseBoolLiteral();
            case T_IDENTIFIER: return parseIdentifier();
            case T_LPAREN:    return parseGroupedExpression();
            case T_MINUS:
            case T_NOT:
                return parseUnaryExpression();
            default:
                throw ParseError(ParseErrorType::ExpectedExpr, currentToken);
        }
    }

    ASTPtr parseInfix(ASTPtr left, int precedence) {
        switch (currentToken.type) {
            case T_ASSIGNOP: case T_PLUS: case T_MINUS: case T_MULTIPLY:
            case T_DIVIDE: case T_MODULO: case T_EQUALOP: case T_NE:
            case T_LT: case T_GT: case T_LE: case T_GE: case T_AND: case T_OR:
                return parseBinaryExpression(move(left), precedence);

            case T_LPAREN:
                if (isIdentifierNode(left)) return parseCallExpression(move(left));
                throw ParseError(ParseErrorType::InvalidCallTarget, currentToken);

            default:
                throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
        }
    }

    // ---- Literal parsers with small helpers ----
    ASTPtr makeIntLiteralFromToken(const Token& t) {
        return make_unique<IntLiteral>(stoi(t.value));
    }
    ASTPtr makeFloatLiteralFromToken(const Token& t) {
        return make_unique<FloatLiteral>(stod(t.value));
    }
    ASTPtr makeStringLiteralFromToken(const Token& t) {
        string s = t.value;
        if (s.length() >= 2 && s.front() == '"' && s.back() == '"') s = s.substr(1, s.length() - 2);
        return make_unique<StringLiteral>(s);
    }
    ASTPtr makeCharLiteralFromToken(const Token& t) {
        char c = '\0';
        if (t.value.size() >= 3 && t.value.front() == '\'' && t.value.back() == '\'') {
            c = t.value[1];
        } else if (!t.value.empty()) {
            c = t.value[0];
        }
        return make_unique<CharLiteral>(c);
    }
    ASTPtr makeBoolLiteralFromToken(const Token& t) {
        return make_unique<BoolLiteral>(t.value == "true");
    }

    ASTPtr parseIntLiteral() {
        Token t = currentToken;
        advance();
        return makeIntLiteralFromToken(t);
    }
    ASTPtr parseFloatLiteral() {
        Token t = currentToken;
        advance();
        return makeFloatLiteralFromToken(t);
    }
    ASTPtr parseStringLiteral() {
        Token t = currentToken;
        advance();
        return makeStringLiteralFromToken(t);
    }
    ASTPtr parseCharLiteral() {
        Token t = currentToken;
        advance();
        return makeCharLiteralFromToken(t);
    }
    ASTPtr parseBoolLiteral() {
        Token t = currentToken;
        advance();
        return makeBoolLiteralFromToken(t);
    }

    ASTPtr parseIdentifier() {
        Token t = currentToken;
        advance();
        return make_unique<Identifier>(t.value);
    }

    ASTPtr parseGroupedExpression() {
        expect(T_LPAREN);
        ASTPtr expr = parseExpression();
        expect(T_RPAREN);
        return expr;
    }

    ASTPtr parseUnaryExpression() {
        Token op = currentToken;
        advance();
        ASTPtr right = parseExpression(UNARY);
        return make_unique<UnaryExpr>(op.type, move(right));
    }

    ASTPtr parseBinaryExpression(ASTPtr left, int precedence) {
        Token op = currentToken;
        int nextPrecedence = getPrecedence(op.type);
        advance();
        ASTPtr right = parseExpression(nextPrecedence);

        if (op.type == T_ASSIGNOP) {
            if (!isIdentifierNode(left)) {
                throw ParseError(ParseErrorType::UnexpectedToken, op);
            }
        }

        return make_unique<BinaryExpr>(op.type, move(left), move(right));
    }

    // ---- Call expression ----
    ASTPtr parseCallExpression(ASTPtr callee) {
        if (!isIdentifierNode(callee)) throw ParseError(ParseErrorType::InvalidCallTarget, currentToken);

        expect(T_LPAREN);
        vector<ASTPtr> args;
        if (!check(T_RPAREN)) {
            do {
                args.push_back(parseExpression());
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);
        return make_unique<CallExpr>(move(callee), move(args));
    }

    // ---- Statements and declarations ----
    ASTPtr parseStatement() {
        if (isTypeToken(currentToken.type)) return parseVariableDeclaration();
        if (check(T_PRINT)) return parsePrintStatement();
        if (check(T_IF)) return parseIfStatement();
        if (check(T_WHILE)) return parseWhileStatement();
        if (match(T_DO)) return parseDoWhileStatement();
        if (match(T_FOR)) return parseForStatement();
        if (match(T_SWITCH)) return parseSwitchStatement();
        if (check(T_RETURN)) return parseReturnStatement();
        if (check(T_LBRACE)) return parseBlockStatement();
        if (check(T_MAIN)) return parseMainDeclaration();
        if (match(T_BREAK)) {
            expect(T_SEMICOLON);
            return make_unique<Identifier>("break"); // simple placeholder for BreakStmt
        }

        ASTPtr expr = parseExpression();
        consumeSemicolon();
        return make_unique<ExpressionStmt>(move(expr));
    }

    ASTPtr parseVariableDeclaration() {
        TokenType type = currentToken.type;
        advance();

        Token identToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
        string name = identToken.value;

        ASTPtr initializer = nullptr;
        if (match(T_ASSIGNOP)) initializer = parseExpression();

        consumeSemicolon();
        return make_unique<VarDecl>(type, name, move(initializer));
    }

    ASTPtr parsePrintStatement() {
        expect(T_PRINT);
        expect(T_LPAREN);
        vector<ASTPtr> args;
        if (!check(T_RPAREN)) {
            do {
                args.push_back(parseExpression());
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);
        consumeSemicolon();
        return make_unique<PrintStmt>(move(args));
    }

    ASTPtr parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        ASTPtr condition = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> ifBody = parseBlock();

        vector<ASTPtr> elseBody;
        if (match(T_ELSE)) elseBody = parseBlock();

        return make_unique<IfStmt>(move(condition), move(ifBody), move(elseBody));
    }

    ASTPtr parseWhileStatement() {
        expect(T_WHILE);
        expect(T_LPAREN);
        ASTPtr condition = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> body = parseBlock();
        return make_unique<WhileStmt>(move(condition), move(body));
    }

    // Replace the do-while method
    ASTPtr parseDoWhileStatement() {
        vector<ASTPtr> bodyVec = parseBlock(); // Parse as vector first
        ASTPtr body = make_unique<BlockStmt>(move(bodyVec)); // Wrap in BlockStmt
        expect(T_WHILE);
        expect(T_LPAREN);
        ASTPtr cond = parseExpression();
        expect(T_RPAREN);
        expect(T_SEMICOLON);
        return make_unique<DoWhileStmt>(move(body), move(cond));
    }

    // Replace the for method
    ASTPtr parseForStatement() {
        expect(T_LPAREN);

        // init
        ASTPtr init = nullptr;
        if (!check(T_SEMICOLON)) {
            if (isTypeToken(currentToken.type)) {
                // Parse variable declaration directly here
                TokenType type = currentToken.type;
                advance();
                Token identToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
                string name = identToken.value;
                ASTPtr initializer = nullptr;
                if (match(T_ASSIGNOP)) initializer = parseExpression();
                consumeSemicolon();
                init = make_unique<VarDecl>(type, name, move(initializer));
            } else {
                init = parseExpression();
                expect(T_SEMICOLON);
            }
        } else advance(); // skip ;

        // condition
        ASTPtr cond = nullptr;
        if (!check(T_SEMICOLON)) cond = parseExpression();
        expect(T_SEMICOLON);

        // update
        ASTPtr update = nullptr;
        if (!check(T_RPAREN)) update = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> bodyVec = parseBlock(); // Parse as vector first
        ASTPtr body = make_unique<BlockStmt>(move(bodyVec)); // Wrap in BlockStmt
        return make_unique<ForStmt>(move(init), move(cond), move(update), move(body));
    }

    ASTPtr parseSwitchStatement() {
        expect(T_LPAREN);
        ASTPtr expr = parseExpression();
        expect(T_RPAREN);
        expect(T_LBRACE);

        vector<ASTPtr> cases;
        vector<ASTPtr> defaultBody;

        while (!check(T_RBRACE) && !check(T_EOF)) {
            if (match(T_CASE)) {
                ASTPtr val = parseExpression();
                // Expect block statement directly after case value (no colon)
                vector<ASTPtr> caseBody = parseBlock(); // This will parse the { ... } block
                cases.push_back(make_unique<CaseBlock>(move(val), move(caseBody)));
            } 
            else if (match(T_DEFAULT)) {
                // Expect block statement directly after default (no colon)
                defaultBody = parseBlock(); // This will parse the { ... } block
            } 
            else {
                advance(); // skip unknown tokens
            }
        }
        expect(T_RBRACE);

        return make_unique<SwitchStmt>(move(expr), move(cases), move(defaultBody));
    }

    ASTPtr parseReturnStatement() {
        expect(T_RETURN);
        ASTPtr value = nullptr;
        if (!check(T_SEMICOLON) && !check(T_RBRACE)) value = parseExpression();
        consumeSemicolon();
        return make_unique<ReturnStmt>(move(value));
    }

    ASTPtr parseBlockStatement() {
        vector<ASTPtr> body = parseBlock();
        return make_unique<BlockStmt>(move(body));
    }

    vector<ASTPtr> parseBlock() {
        expect(T_LBRACE);
        vector<ASTPtr> statements;
        while (!check(T_RBRACE) && !check(T_EOF)) {
            statements.push_back(parseStatement());
        }
        if (check(T_EOF)) throw ParseError(ParseErrorType::UnclosedBlock, currentToken);
        expect(T_RBRACE);
        return statements;
    }

    ASTPtr parseMainDeclaration() {
        expect(T_MAIN);
        vector<ASTPtr> body = parseBlock();
        return make_unique<MainDecl>(move(body));
    }

    ASTPtr parseFunctionDeclaration() {
        TokenType returnType = currentToken.type;
        advance();

        Token nameToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
        string name = nameToken.value;

        expect(T_LPAREN);
        vector<pair<TokenType, string>> params;
        if (!check(T_RPAREN)) {
            do {
                TokenType paramType = currentToken.type;
                advance();
                Token paramName = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
                params.push_back({paramType, paramName.value});
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);

        vector<ASTPtr> body = parseBlock();
        return make_unique<FunctionDecl>(returnType, name, move(params), move(body));
    }

    
    ASTPtr parseIncludeStatement() {
    Token kw = expect(T_IDENTIFIER, ParseErrorType::UnexpectedToken);
    if (kw.value != "include") {
        throw ParseError(ParseErrorType::UnexpectedToken, kw);
    }

    // include <main>
    if (match(T_LT)) {
        if (!check(T_MAIN)) {
            throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
        }
        advance(); // consume T_MAIN
        expect(T_GT);
        return make_unique<IncludeStmt>("main");
    }

    // include "header"
    if (check(T_STRINGLIT)) {
        Token headerTok = currentToken;
        advance();
        string header = headerTok.value;
        if (header.size() >= 2 && header.front() == '"' && header.back() == '"') {
            header = header.substr(1, header.size() - 2);
        }
        return make_unique<IncludeStmt>(header);
    }

    // include identifier fallback
    if (check(T_IDENTIFIER)) {
        Token headerTok = currentToken;
        advance();
        return make_unique<IncludeStmt>(headerTok.value);
    }

    throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
}


public:
    Parser(const vector<Token>& toks) : tokens(toks), current(0) {
        if (!tokens.empty()) currentToken = tokens[0];
        else currentToken = {T_EOF, "EOF", -1, -1};
    }

    
    vector<ASTPtr> parseProgram() {
    vector<ASTPtr> declarations;

    // Enforce first token must be include<main>
    if (!(check(T_IDENTIFIER) && currentToken.value == "include")) {
        throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
    }
    declarations.push_back(parseIncludeStatement());

    // Continue with the rest
    while (currentToken.type != T_EOF) {
        if (check(T_IDENTIFIER) && currentToken.value == "include") {
            declarations.push_back(parseIncludeStatement());
            continue;
        }
        if (isTypeToken(currentToken.type)) {
            const Token& next = peek(1);
            if (next.type == T_IDENTIFIER) {
                declarations.push_back(parseFunctionDeclaration());
            } else {
                declarations.push_back(parseStatement());
            }
            continue;
        }
        if (check(T_MAIN)) {
            declarations.push_back(parseMainDeclaration());
            continue;
        }
        declarations.push_back(parseStatement());
    }
    return declarations;
}

};

// ==========================================================
// Map string names in file -> enum TokenType
TokenType tokenTypeFromString(const string& s) {
    static unordered_map<string, TokenType> mapping = {
        {"T_INT", T_INT}, {"T_FLOAT", T_FLOAT}, {"T_DOUBLE", T_DOUBLE},
        {"T_CHAR", T_CHAR}, {"T_VOID", T_VOID}, {"T_BOOL", T_BOOL},
        {"T_IDENTIFIER", T_IDENTIFIER}, {"T_INTLIT", T_INTLIT},
        {"T_FLOATLIT", T_FLOATLIT}, {"T_STRINGLIT", T_STRINGLIT},
        {"T_CHARLIT", T_CHARLIT}, {"T_BOOLLIT", T_BOOLLIT},
        {"T_LPAREN", T_LPAREN}, {"T_RPAREN", T_RPAREN},
        {"T_LBRACE", T_LBRACE}, {"T_RBRACE", T_RBRACE},
        {"T_LBRACKET", T_LBRACKET}, {"T_RBRACKET", T_RBRACKET},
        {"T_SEMICOLON", T_SEMICOLON}, {"T_COMMA", T_COMMA},
        {"T_DOT", T_DOT}, {"T_ASSIGNOP", T_ASSIGNOP},
        {"T_EQUALOP", T_EQUALOP}, {"T_NE", T_NE},
        {"T_LT", T_LT}, {"T_GT", T_GT}, {"T_LE", T_LE}, {"T_GE", T_GE},
        {"T_PLUS", T_PLUS}, {"T_MINUS", T_MINUS}, {"T_MULTIPLY", T_MULTIPLY},
        {"T_DIVIDE", T_DIVIDE}, {"T_MODULO", T_MODULO},
        {"T_INCREMENT", T_INCREMENT}, {"T_DECREMENT", T_DECREMENT},
        {"T_AND", T_AND}, {"T_OR", T_OR}, {"T_NOT", T_NOT},
        {"T_IF", T_IF}, {"T_ELSE", T_ELSE}, {"T_WHILE", T_WHILE},
        {"T_RETURN", T_RETURN}, {"T_PRINT", T_PRINT}, {"T_MAIN", T_MAIN},
        {"T_SINGLE_COMMENT", T_SINGLE_COMMENT}, {"T_MULTI_COMMENT", T_MULTI_COMMENT},
        {"T_DO", T_DO}, {"T_FOR", T_FOR}, {"T_SWITCH", T_SWITCH},
        {"T_CASE", T_CASE}, {"T_DEFAULT", T_DEFAULT},
        {"T_BREAK", T_BREAK}, {"T_COLON", T_COLON},
        {"T_ERROR", T_ERROR}, {"T_EOF", T_EOF}
    };
    auto it = mapping.find(s);
    if (it != mapping.end()) return it->second;
    return T_ERROR;
}

vector<Token> loadTokens(const string& filename) {
    vector<Token> tokens;
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("Cannot open file: " + filename);

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.rfind("ERROR", 0) == 0) {
            cerr << "Skipping lexer error: " << line << endl;
            continue;
        }

        size_t openParen = line.find('(');
        size_t closeParen = line.find(')');
        if (openParen == string::npos || closeParen == string::npos) {
            cerr << "Malformed token line: " << line << endl;
            continue;
        }

        string typeStr = line.substr(0, openParen);
        string value = line.substr(openParen + 1, closeParen - openParen - 1);

        size_t firstComma = line.find(',', closeParen);
        if (firstComma == string::npos) {
            cerr << "Missing line number: " << line << endl;
            continue;
        }

        size_t secondComma = line.find(',', firstComma + 1);
        if (secondComma == string::npos) {
            cerr << "Missing column number: " << line << endl;
            continue;
        }

        string lineStr = line.substr(firstComma + 1, secondComma - firstComma - 1);
        string colStr  = line.substr(secondComma + 1);

        auto trim = [](string &s) {
            while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
            while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin());
        };
        trim(lineStr);
        trim(colStr);

        int lineNum = -1, colNum = -1;
        try {
            lineNum = stoi(lineStr);
            colNum  = stoi(colStr);
        } catch (...) {
            cerr << "Invalid line/col number: " << line << endl;
            continue;
        }

        tokens.push_back({tokenTypeFromString(typeStr), value, lineNum, colNum});
    }

    tokens.push_back({T_EOF, "EOF", -1, -1});
    return tokens;
}

int main() {
    try {
        vector<Token> tokens = loadTokens("tester/tokens.txt");

        Parser parser(tokens);
        auto ast = parser.parseProgram();

        cout << "=== Parsed AST ===\n";
        for (const auto& node : ast) node->print();
        cout << "\n=== Parsing Successful ===\n";
    } catch (const ParseError& e) {
        cout << "Parse Error: " << e.message
             << " at line " << e.token.line
             << ", column " << e.token.column << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}
