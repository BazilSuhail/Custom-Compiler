#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <unordered_map>

using namespace std;

// === Token Types (from your lexer) ===
enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_VOID, T_BOOL,
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT, T_CHARLIT, T_BOOLLIT,
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_SEMICOLON, T_COMMA, T_DOT,  // Keep all tokens for completeness
    T_ASSIGNOP, T_EQUALOP, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,
    T_INCREMENT, T_DECREMENT,
    T_AND, T_OR, T_NOT,
    T_IF, T_ELSE, T_WHILE, T_RETURN, T_PRINT, T_MAIN,
    T_SINGLE_COMMENT, T_MULTI_COMMENT,
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
    InvalidCallTarget,   // NEW: only identifiers can be called
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

    string getTokenName(TokenType t) {
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
            for (const auto& arg : args) {
                arg->print(indent + 4);
            }
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
        if (initializer) {
            initializer->print(indent + 2);
        }
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
        for (const auto& stmt : body) {
            stmt->print(indent + 2);
        }
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
        for (const auto& stmt : body) {
            stmt->print(indent + 4);
        }
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
        for (const auto& stmt : body) {
            stmt->print(indent + 2);
        }
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
        for (const auto& stmt : ifBody) {
            stmt->print(indent + 4);
        }
        if (!elseBody.empty()) {
            cout << string(indent + 2, ' ') << "ElseBody:\n";
            for (const auto& stmt : elseBody) {
                stmt->print(indent + 4);
            }
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
        for (const auto& stmt : body) {
            stmt->print(indent + 4);
        }
    }
};

struct ReturnStmt : public ASTNode {
    ASTPtr value;
    ReturnStmt(ASTPtr val = nullptr) : value(move(val)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ReturnStmt\n";
        if (value) {
            value->print(indent + 2);
        }
    }
};

struct PrintStmt : public ASTNode {
    vector<ASTPtr> args;
    PrintStmt(vector<ASTPtr> a) : args(move(a)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "PrintStmt\n";
        for (const auto& arg : args) {
            arg->print(indent + 2);
        }
    }
};

struct ExpressionStmt : public ASTNode {
    ASTPtr expr;
    ExpressionStmt(ASTPtr e) : expr(move(e)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ExpressionStmt\n";
        if (expr) {
            expr->print(indent + 2);
        }
    }
};

// === Pratt Parser Implementation ===
class Parser {
private:
    vector<Token> tokens;
    size_t current;
    Token currentToken;

    // Precedence levels
    enum Precedence {
        LOWEST = 0,
        ASSIGNMENT = 1,      // =
        LOGICAL_OR = 2,      // ||
        LOGICAL_AND = 3,     // &&
        EQUALITY = 4,        // == !=
        COMPARISON = 5,      // < > <= >=
        TERM = 6,            // + -
        FACTOR = 7,          // * / %
        UNARY = 8,           // ! -
        CALL = 9,            // function()
    };

    // Map tokens to precedence
    int getPrecedence(TokenType type) {
        switch (type) {
            case T_ASSIGNOP: return ASSIGNMENT;
            case T_OR: return LOGICAL_OR;
            case T_AND: return LOGICAL_AND;
            case T_EQUALOP: case T_NE: return EQUALITY;
            case T_LT: case T_GT: case T_LE: case T_GE: return COMPARISON;
            case T_PLUS: case T_MINUS: return TERM;
            case T_MULTIPLY: case T_DIVIDE: case T_MODULO: return FACTOR;
            case T_LPAREN: return CALL;   // allow ( to act as call operator
            default: return LOWEST;
        }
    }


    // Check if token is a type
    bool isTypeToken(TokenType type) {
        return type == T_INT || type == T_FLOAT || type == T_DOUBLE || 
               type == T_CHAR || type == T_BOOL || type == T_VOID;
    }

    // Check if token is a type or function keyword
    bool isDeclarationStart(TokenType type) {
        return isTypeToken(type) || type == T_MAIN || type == T_PRINT;
    }

    // Get next token
    void advance() {
        if (current < tokens.size()) {
            current++;
        }
        if (current < tokens.size()) {
            currentToken = tokens[current];
        } else {
            currentToken = {T_EOF, "EOF", -1, -1};
        }
    }

    // Check current token
    bool check(TokenType type) {
        return currentToken.type == type;
    }

    // Consume token if matches
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    // Expect a specific token
    Token expect(TokenType type) {
        if (currentToken.type == type) {
            Token t = currentToken;
            advance();
            return t;
        }
        throw ParseError(ParseErrorType::FailedToFindToken, currentToken);
    }

    // Pratt parsing methods
    ASTPtr parseExpression(int precedence = LOWEST) {
    ASTPtr left = parsePrefix();

    while (getPrecedence(currentToken.type) > precedence) { // FIX: > instead of >=
        left = parseInfix(move(left), getPrecedence(currentToken.type));
    }

    return left;
}

    ASTPtr parsePrefix() {
        switch (currentToken.type) {
            case T_INTLIT:
                return parseIntLiteral();
            case T_FLOATLIT:
                return parseFloatLiteral();
            case T_STRINGLIT:
                return parseStringLiteral();
            case T_CHARLIT:
                return parseCharLiteral();
            case T_BOOLLIT:
                return parseBoolLiteral();
            case T_IDENTIFIER:
                return parseIdentifier();
            case T_LPAREN:
                return parseGroupedExpression();
            case T_MINUS:
            case T_NOT:
                return parseUnaryExpression();
            default:
                throw ParseError(ParseErrorType::ExpectedExpr, currentToken);
        }
    }

    ASTPtr parseInfix(ASTPtr left, int precedence) {
        switch (currentToken.type) {
            case T_ASSIGNOP:
            case T_PLUS:
            case T_MINUS:
            case T_MULTIPLY:
            case T_DIVIDE:
            case T_MODULO:
            case T_EQUALOP:
            case T_NE:
            case T_LT:
            case T_GT:
            case T_LE:
            case T_GE:
            case T_AND:
            case T_OR:
                return parseBinaryExpression(move(left), precedence);

            case T_LPAREN: {
                // Only identifiers shpuld be called
                if (dynamic_cast<Identifier*>(left.get())) {
                    return parseCallExpression(move(left));
                }
                throw ParseError(ParseErrorType::InvalidCallTarget, currentToken);
            }

            default:
                throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
        }
    }


    // Literal parsers
    ASTPtr parseIntLiteral() {
        Token t = currentToken;
        advance();
        return make_unique<IntLiteral>(stoi(t.value));
    }

    ASTPtr parseFloatLiteral() {
        Token t = currentToken;
        advance();
        return make_unique<FloatLiteral>(stod(t.value));
    }

    ASTPtr parseStringLiteral() {
        Token t = currentToken;
        advance();
        // Remove quotes from string
        string s = t.value;
        if (s.length() >= 2 && s[0] == '"' && s[s.length()-1] == '"') {
            s = s.substr(1, s.length() - 2);
        }
        return make_unique<StringLiteral>(s);
    }

    ASTPtr parseCharLiteral() {
        Token t = currentToken;
        advance();
        // Remove quotes and get char
        char c = t.value[1]; // assuming format: 'x'
        return make_unique<CharLiteral>(c);
    }

    ASTPtr parseBoolLiteral() {
        Token t = currentToken;
        advance();
        return make_unique<BoolLiteral>(t.value == "true");
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

        // Restrict assignment: left must be Identifier
        if (op.type == T_ASSIGNOP) {
            if (dynamic_cast<Identifier*>(left.get()) == nullptr) {
                throw ParseError(ParseErrorType::UnexpectedToken, op);
            }
        }

        return make_unique<BinaryExpr>(op.type, move(left), move(right));
    }


    // main syntax

    ASTPtr parseIncludeStatement() {
        Token ident = expect(T_IDENTIFIER); // "include"
        expect(T_LT);
        Token header = expect(T_MAIN); // e.g., "main"
        expect(T_GT);
        return make_unique<IncludeStmt>(header.value);
    }

   ASTPtr parseCallExpression(ASTPtr callee) {
        // Only identifiers are valid callees
        if (!dynamic_cast<Identifier*>(callee.get())) {
            throw ParseError(ParseErrorType::InvalidCallTarget, currentToken);
        }

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



    // Statement parsers
    ASTPtr parseStatement() {
        if (isTypeToken(currentToken.type)) {
            return parseVariableDeclaration();
        } else if (check(T_PRINT)) {
            return parsePrintStatement();
        } else if (check(T_IF)) {
            return parseIfStatement();
        } else if (check(T_WHILE)) {
            return parseWhileStatement();
        } else if (check(T_RETURN)) {
            return parseReturnStatement();
        } else if (check(T_LBRACE)) {
            return parseBlockStatement();
        } else if (check(T_MAIN)) {
            return parseMainDeclaration();
        } else {
            // Expression statement
            ASTPtr expr = parseExpression();
            //expect(T_SEMICOLON);  // Consume the semicolon
            if (!match(T_SEMICOLON)) {
                throw ParseError(ParseErrorType::MissingSemicolon, currentToken);
            }           
            return make_unique<ExpressionStmt>(move(expr));
        }
    }

    ASTPtr parseVariableDeclaration() {
        TokenType type = currentToken.type;
        advance();
        
        Token identToken = expect(T_IDENTIFIER);
        string name = identToken.value;
        
        ASTPtr initializer = nullptr;
        if (match(T_ASSIGNOP)) {
            initializer = parseExpression();
        }
        
        // expect(T_SEMICOLON);  // Consume the semicolon
        if (!match(T_SEMICOLON)) {
            throw ParseError(ParseErrorType::MissingSemicolon, currentToken);
        }

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
        
        //expect(T_SEMICOLON);  // Consume the semicolon
        if (!match(T_SEMICOLON)) {
            throw ParseError(ParseErrorType::MissingSemicolon, currentToken);
        }
        return make_unique<PrintStmt>(move(args));
    }

    ASTPtr parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        ASTPtr condition = parseExpression();
        expect(T_RPAREN);
        
        vector<ASTPtr> ifBody = parseBlock();
        
        vector<ASTPtr> elseBody;
        if (match(T_ELSE)) {
            elseBody = parseBlock();
        }
        
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

    ASTPtr parseReturnStatement() {
        expect(T_RETURN);
        ASTPtr value = nullptr;
        if (!check(T_SEMICOLON) && !check(T_RBRACE)) {  
            value = parseExpression();
        }

        // Consume the semicolon
        // expect(T_SEMICOLON);  
        if (!match(T_SEMICOLON)) {
            throw ParseError(ParseErrorType::MissingSemicolon, currentToken);
        }
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
        if (check(T_EOF)) {
            throw ParseError(ParseErrorType::UnclosedBlock, currentToken);
        }
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
        
        Token nameToken = expect(T_IDENTIFIER);
        string name = nameToken.value;
        
        expect(T_LPAREN);
        
        vector<pair<TokenType, string>> params;
        if (!check(T_RPAREN)) {
            do {
                TokenType paramType = currentToken.type;
                advance();
                Token paramName = expect(T_IDENTIFIER);
                params.push_back({paramType, paramName.value});
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);
        
        vector<ASTPtr> body = parseBlock();
        
        return make_unique<FunctionDecl>(returnType, name, move(params), move(body));
    }

public:
    Parser(const vector<Token>& toks) : tokens(toks), current(0) {
        if (!tokens.empty()) {
            currentToken = tokens[0];
        } else {
            currentToken = {T_EOF, "EOF", -1, -1};
        }
    }

    vector<ASTPtr> parseProgram() {
        vector<ASTPtr> declarations;
        
        while (currentToken.type != T_EOF) {
            if (check(T_IDENTIFIER) && currentToken.value == "include") {
                // handle include statement
                declarations.push_back(parseIncludeStatement());
            }
            else if (isTypeToken(currentToken.type)) {
                size_t next_pos = current + 1;
                if (next_pos < tokens.size() && tokens[next_pos].type == T_IDENTIFIER) {
                    declarations.push_back(parseFunctionDeclaration());
                } else {
                    declarations.push_back(parseStatement());
                }
            }
            else if (check(T_MAIN)) {
                declarations.push_back(parseMainDeclaration());
            }
            else {
                declarations.push_back(parseStatement());
            }
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

        // Skip lexer error messages
        if (line.rfind("ERROR", 0) == 0) {
            cerr << "Skipping lexer error: " << line << endl;
            continue;
        }

        // 1. Read token type (before '(')
        size_t openParen = line.find('(');
        size_t closeParen = line.find(')');
        if (openParen == string::npos || closeParen == string::npos) {
            cerr << "Malformed token line: " << line << endl;
            continue;
        }

        string typeStr = line.substr(0, openParen);

        // 2. Lexeme between '(' and ')'
        string value = line.substr(openParen + 1, closeParen - openParen - 1);

        // 3. After ")", expect ",line,column"
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

        // Trim possible whitespace
        auto trim = [](string &s) {
            while (!s.empty() && isspace(s.back())) s.pop_back();
            while (!s.empty() && isspace(s.front())) s.erase(s.begin());
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

    // Push EOF at the end
    tokens.push_back({T_EOF, "EOF", -1, -1});
    return tokens;
}

int main() {
    try {
        vector<Token> tokens = loadTokens("tester/tokens.txt");  // load from file

        Parser parser(tokens);
        auto ast = parser.parseProgram();
        
        cout << "=== Parsed AST ===\n";
        for (const auto& node : ast) {
            node->print();
        }
        cout << "\n=== Parsing Successful ===\n";
    } catch (const ParseError& e) {
        cout << "Parse Error: " << e.message 
             << " at line " << e.token.line 
             << ", column " << e.token.column << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

