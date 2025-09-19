#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <initializer_list>
#include <stdexcept>
#include <map>
using namespace std;

// ============================================================================
// TOKEN STRUCTURE (MATCHING YOUR LEXER)
// ============================================================================

struct Token {
    string type;
    string value;
    int line;
    int column;
    
    Token(string t, string v, int ln, int col) : type(t), value(v), line(ln), column(col) {}
};

// ============================================================================
// AST NODES
// ============================================================================

struct AstNode {
    virtual ~AstNode() = default;
    virtual string toString() const = 0;
};

struct FnDecl : AstNode {
    string returnType;
    string name;
    vector<pair<string, string>> params;
    vector<unique_ptr<AstNode>> body;
    
    string toString() const override {
        string result = "Fn(FnDecl { returnType: \"" + returnType + "\", name: \"" + name + "\", params: [";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) result += ", ";
            result += "(\"" + params[i].first + "\", \"" + params[i].second + "\")";
        }
        result += "], block: [";
        for (size_t i = 0; i < body.size(); ++i) {
            if (i > 0) result += ", ";
            result += body[i]->toString();
        }
        result += "] })";
        return result;
    }
};

struct VarDecl : AstNode {
    string type;
    string name;
    unique_ptr<AstNode> initializer;
    
    string toString() const override {
        string initStr = initializer ? initializer->toString() : "null";
        return "Var(VarDecl { type: \"" + type + "\", name: \"" + name + "\", expr: " + initStr + " })";
    }
};

struct AssignStmt : AstNode {
    string identifier;
    unique_ptr<AstNode> expr;
    
    string toString() const override {
        return "Assign(AssignStmt { identifier: \"" + identifier + "\", expr: " + 
               (expr ? expr->toString() : "null") + " })";
    }
};

struct ReturnStmt : AstNode {
    unique_ptr<AstNode> expr;
    
    string toString() const override {
        return "Ret(ExprStmt { expr: " + (expr ? expr->toString() : "null") + " })";
    }
};

struct IfStmt : AstNode {
    unique_ptr<AstNode> condition;
    vector<unique_ptr<AstNode>> thenBranch;
    vector<unique_ptr<AstNode>> elseBranch;
    
    string toString() const override {
        string thenStr = "[";
        for (size_t i = 0; i < thenBranch.size(); ++i) {
            if (i > 0) thenStr += ", ";
            thenStr += thenBranch[i]->toString();
        }
        thenStr += "]";
        
        string elseStr = "[";
        for (size_t i = 0; i < elseBranch.size(); ++i) {
            if (i > 0) elseStr += ", ";
            elseStr += elseBranch[i]->toString();
        }
        elseStr += "]";
        
        return "If(IfStmt { cond: " + (condition ? condition->toString() : "null") + 
               ", if_block: " + thenStr + ", else_block: " + elseStr + " })";
    }
};

struct ForStmt : AstNode {
    unique_ptr<AstNode> initializer;
    unique_ptr<AstNode> condition;
    unique_ptr<AstNode> increment;
    vector<unique_ptr<AstNode>> body;
    
    string toString() const override {
        string initStr = initializer ? initializer->toString() : "null";
        string condStr = condition ? condition->toString() : "null";
        string incrStr = increment ? increment->toString() : "null";
        
        string bodyStr = "[";
        for (size_t i = 0; i < body.size(); ++i) {
            if (i > 0) bodyStr += ", ";
            bodyStr += body[i]->toString();
        }
        bodyStr += "]";
        
        return "For(ForStmt { init: " + initStr + ", cond: " + condStr + 
               ", updt: " + incrStr + ", block: " + bodyStr + " })";
    }
};

struct BreakStmt : AstNode {
    string toString() const override {
        return "Break";
    }
};

struct ExprStmt : AstNode {
    unique_ptr<AstNode> expr;
    
    string toString() const override {
        return "ExprStmt { expr: " + (expr ? expr->toString() : "null") + " }";
    }
};

struct BinaryExpr : AstNode {
    unique_ptr<AstNode> left;
    string op;
    unique_ptr<AstNode> right;
    
    string toString() const override {
        return "::" + string(op == "+" ? "Add" : op == "-" ? "Sub" : op == "*" ? "Mul" : 
                            op == "/" ? "Div" : op == "==" ? "Comp(EqualsOp)" : 
                            op == "!=" ? "Comp(NotEqualsOp)" : op == "<" ? "Comp(LessThan)" : 
                            op == ">" ? "Comp(GreaterThan)" : op == "<=" ? "Comp(LessEqual)" : 
                            op == ">=" ? "Comp(GreaterEqual)" : "Unknown") + 
               "(" + op + ") " + (left ? left->toString() : "null") + " " + 
               (right ? right->toString() : "null");
    }
};

struct UnaryExpr : AstNode {
    string op;
    unique_ptr<AstNode> expr;
    bool isPostfix = false;  // NEW: to handle ++i vs i++
    
    string toString() const override {
        if (isPostfix) {
            return "Postfix(" + op + ") " + (expr ? expr->toString() : "null");
        } else {
            return "Unary(" + op + ") " + (expr ? expr->toString() : "null");
        }
    }
};

struct CallExpr : AstNode {
    string callee;
    vector<unique_ptr<AstNode>> arguments;
    
    string toString() const override {
        string argsStr = "[";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) argsStr += ", ";
            argsStr += arguments[i] ? arguments[i]->toString() : "null";
        }
        argsStr += "]";
        return "Call(FnCall { ident: \"" + callee + "\", args: " + argsStr + " })";
    }
};

struct LiteralExpr : AstNode {
    string type;
    string value;
    
    string toString() const override {
        return value;
    }
};

struct IdentifierExpr : AstNode {
    string name;
    
    string toString() const override {
        return "\"" + name + "\"";
    }
};

// ============================================================================
// PARSER CLASS
// ============================================================================

enum class ParseError {
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
};

class Parser {
private:
    vector<Token> tokens;
    int current = 0;

public:
    Parser(vector<Token> tokens) : tokens(tokens) {}

    vector<unique_ptr<AstNode>> parse();

private:
    unique_ptr<AstNode> declaration();
    unique_ptr<AstNode> varDecl();
    unique_ptr<AstNode> statement();
    unique_ptr<AstNode> exprStmt();
    unique_ptr<AstNode> assignStmt();
    unique_ptr<AstNode> ifStmt();
    unique_ptr<AstNode> forStmt();
    unique_ptr<AstNode> returnStmt();
    unique_ptr<AstNode> breakStmt();

    unique_ptr<AstNode> expression();    // assignment
    unique_ptr<AstNode> assignment();
    unique_ptr<AstNode> equality();
    unique_ptr<AstNode> comparison();
    unique_ptr<AstNode> term();
    unique_ptr<AstNode> factor();
    unique_ptr<AstNode> unary();
    unique_ptr<AstNode> postfix();
    unique_ptr<AstNode> call();
    unique_ptr<AstNode> primary();

    // Helper methods
    bool match(initializer_list<string> types);
    Token consume(string type, string message);
    bool check(string type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    
    void error(ParseError err, const string& message);
};

// ============================================================================
// PARSER IMPLEMENTATION
// ============================================================================

vector<unique_ptr<AstNode>> Parser::parse() {
    vector<unique_ptr<AstNode>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

unique_ptr<AstNode> Parser::declaration() {
    if (check("T_INT") || check("T_FLOAT") || check("T_DOUBLE") || check("T_CHAR") || check("T_BOOL")) {
        advance(); // CONSUME THE TYPE TOKEN
        return varDecl();
    } else {
        return statement();
    }
}

unique_ptr<AstNode> Parser::varDecl() {
    string type = previous().value;  // GET TYPE FROM PREVIOUS TOKEN
    Token name = consume("T_IDENTIFIER", "Expected variable name");
    
    auto varDecl = make_unique<VarDecl>();
    varDecl->type = type;
    varDecl->name = name.value;
    
    if (match({"T_ASSIGNOP"})) {
        varDecl->initializer = expression();
    }
    
    consume("T_SEMICOLON", "Expected ';' after variable declaration");
    return varDecl;
}

unique_ptr<AstNode> Parser::statement() {
    if (check("T_RETURN")) {
        return returnStmt();
    } else if (check("T_IF")) {
        return ifStmt();
    } else if (check("T_FOR")) {
        return forStmt();
    } else if (check("T_BREAK")) {
        return breakStmt();
    } else if (check("T_IDENTIFIER") && tokens[current + 1].type == "T_ASSIGNOP") {
        return assignStmt();
    } else {
        return exprStmt();
    }
}

unique_ptr<AstNode> Parser::exprStmt() {
    auto expr = expression();
    consume("T_SEMICOLON", "Expected ';' after expression");
    
    auto exprStmt = make_unique<ExprStmt>();
    exprStmt->expr = move(expr);
    return exprStmt;
}

unique_ptr<AstNode> Parser::assignStmt() {
    Token name = consume("T_IDENTIFIER", "Expected identifier");
    consume("T_ASSIGNOP", "Expected '='");
    auto expr = expression();
    consume("T_SEMICOLON", "Expected ';' after assignment");
    
    auto assignStmt = make_unique<AssignStmt>();
    assignStmt->identifier = name.value;
    assignStmt->expr = move(expr);
    return assignStmt;
}

unique_ptr<AstNode> Parser::ifStmt() {
    consume("T_IF", "Expected 'if'");
    consume("T_LPAREN", "Expected '(' after 'if'");
    
    auto condition = expression();
    consume("T_RPAREN", "Expected ')' after condition");
    
    consume("T_LBRACE", "Expected '{' before if body");
    
    auto ifStmt = make_unique<IfStmt>();
    ifStmt->condition = move(condition);
    
    while (!check("T_RBRACE") && !isAtEnd()) {
        ifStmt->thenBranch.push_back(declaration());
    }
    
    consume("T_RBRACE", "Expected '}' after if body");
    
    if (match({"T_ELSE"})) {
        consume("T_LBRACE", "Expected '{' before else body");
        
        while (!check("T_RBRACE") && !isAtEnd()) {
            ifStmt->elseBranch.push_back(declaration());
        }
        
        consume("T_RBRACE", "Expected '}' after else body");
    }
    
    return ifStmt;
}

unique_ptr<AstNode> Parser::forStmt() {
    consume("T_FOR", "Expected 'for'");
    consume("T_LPAREN", "Expected '(' after 'for'");
    
    auto forStmt = make_unique<ForStmt>();
    
    // Initializer
    if (check("T_INT") || check("T_FLOAT") || check("T_DOUBLE") || check("T_CHAR") || check("T_BOOL")) {
        advance(); // CONSUME THE TYPE TOKEN
        forStmt->initializer = varDecl();
    } else if (!check("T_SEMICOLON")) {
        forStmt->initializer = exprStmt();
    } else {
        advance(); // consume ';'
    }
    
    // Condition
    if (!check("T_SEMICOLON")) {
        forStmt->condition = expression();
    }
    consume("T_SEMICOLON", "Expected ';' in for loop");
    
    // Increment
    if (!check("T_RPAREN")) {
        forStmt->increment = expression();  // CHANGED: parse as expression instead of special cases
    }
    
    consume("T_RPAREN", "Expected ')' after for clauses");
    consume("T_LBRACE", "Expected '{' before for body");
    
    while (!check("T_RBRACE") && !isAtEnd()) {
        forStmt->body.push_back(declaration());
    }
    
    consume("T_RBRACE", "Expected '}' after for body");
    
    return forStmt;
}

unique_ptr<AstNode> Parser::returnStmt() {
    consume("T_RETURN", "Expected 'return'");
    
    auto returnStmt = make_unique<ReturnStmt>();
    
    if (!check("T_SEMICOLON")) {
        returnStmt->expr = expression();
    }
    
    consume("T_SEMICOLON", "Expected ';' after return value");
    return returnStmt;
}

unique_ptr<AstNode> Parser::breakStmt() {
    consume("T_BREAK", "Expected 'break'");
    consume("T_SEMICOLON", "Expected ';' after break");
    return make_unique<BreakStmt>();
}

// Expression parsing with precedence
unique_ptr<AstNode> Parser::expression() {
    return assignment();
}

unique_ptr<AstNode> Parser::assignment() {
    auto expr = equality();
    
    if (match({"T_ASSIGNOP"})) {
        string op = previous().value;
        auto right = assignment();
        auto assign = make_unique<BinaryExpr>();
        assign->left = move(expr);
        assign->op = op;
        assign->right = move(right);
        return assign;
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::equality() {
    auto expr = comparison();
    
    while (match({"T_EQUALOP", "T_NE"})) {
        string op = previous().value;
        auto right = comparison();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::comparison() {
    auto expr = term();
    
    while (match({"T_LT", "T_GT", "T_LE", "T_GE"})) {
        string op = previous().value;
        auto right = term();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::term() {
    auto expr = factor();
    
    while (match({"T_PLUS", "T_MINUS"})) {
        string op = previous().value;
        auto right = factor();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::factor() {
    auto expr = unary();
    
    while (match({"T_MULTIPLY", "T_DIVIDE"})) {
        string op = previous().value;
        auto right = unary();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::unary() {
    if (match({"T_MINUS", "T_PLUS"})) {
        string op = previous().value;
        auto expr = unary();
        auto unary = make_unique<UnaryExpr>();
        unary->op = op;
        unary->expr = move(expr);
        return unary;
    }
    
    return postfix();
}

unique_ptr<AstNode> Parser::postfix() {
    auto expr = call();
    
    // Handle postfix increment/decrement
    if (match({"T_INCREMENT", "T_DECREMENT"})) {
        string op = previous().value;
        auto unary = make_unique<UnaryExpr>();
        unary->op = op;
        unary->expr = move(expr);
        unary->isPostfix = true;
        return unary;
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::call() {
    auto expr = primary();
    
    if (match({"T_LPAREN"})) {
        auto call = make_unique<CallExpr>();
        
        if (auto identExpr = dynamic_cast<IdentifierExpr*>(expr.get())) {
            call->callee = identExpr->name;
        }
        
        if (!check("T_RPAREN")) {
            do {
                call->arguments.push_back(expression());
            } while (match({"T_COMMA"}));
        }
        
        consume("T_RPAREN", "Expected ')' after arguments");
        return call;
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::primary() {
    if (match({"T_INTLIT", "T_FLOATLIT", "T_BOOLLIT", "T_STRINGLIT", "T_CHARLIT"})) {
        auto literal = make_unique<LiteralExpr>();
        literal->type = previous().type;
        literal->value = previous().value;
        return literal;
    }
    
    if (match({"T_IDENTIFIER"})) {
        auto ident = make_unique<IdentifierExpr>();
        ident->name = previous().value;
        return ident;
    }
    
    if (match({"T_LPAREN"})) {
        auto expr = expression();
        consume("T_RPAREN", "Expected ')' after expression");
        return expr;
    }
    
    // Handle prefix increment/decrement
    if (match({"T_INCREMENT", "T_DECREMENT"})) {
        string op = previous().value;
        auto expr = unary();
        auto unary = make_unique<UnaryExpr>();
        unary->op = op;
        unary->expr = move(expr);
        unary->isPostfix = false;
        return unary;
    }
    
    error(ParseError::ExpectedExpr, "Expected expression");
    return nullptr;
}

// Helper methods
bool Parser::match(initializer_list<string> types) {
    for (const string& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(string type, string message) {
    if (check(type)) return advance();
    error(ParseError::UnexpectedToken, message);
    // Return a dummy token to avoid compiler warnings
    return Token("", "", 0, 0);
}

bool Parser::check(string type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == "EOF";
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

void Parser::error(ParseError err, const string& message) {
    cerr << "Parse Error: " << message << " at line " << peek().line << ", column " << peek().column << endl;
    throw runtime_error(message);
}

// ============================================================================
// TEST DRIVER - USING YOUR LEXER TOKENS
// ============================================================================

// Create test tokens that match your lexer output
vector<Token> createTestTokens() {
    vector<Token> tokens = {
        Token("T_INT", "int", 1, 1),
        Token("T_IDENTIFIER", "a", 1, 5),
        Token("T_ASSIGNOP", "=", 1, 7),
        Token("T_INTLIT", "5", 1, 9),
        Token("T_SEMICOLON", ";", 1, 10),
        
        Token("T_INT", "int", 2, 1),
        Token("T_IDENTIFIER", "b", 2, 5),
        Token("T_ASSIGNOP", "=", 2, 7),
        Token("T_IDENTIFIER", "a", 2, 9),
        Token("T_PLUS", "+", 2, 11),
        Token("T_INTLIT", "3", 2, 13),
        Token("T_SEMICOLON", ";", 2, 14),
        
        Token("T_IF", "if", 3, 1),
        Token("T_LPAREN", "(", 3, 4),
        Token("T_IDENTIFIER", "a", 3, 5),
        Token("T_GT", ">", 3, 7),
        Token("T_INTLIT", "0", 3, 9),
        Token("T_RPAREN", ")", 3, 10),
        Token("T_LBRACE", "{", 3, 12),
        Token("T_RETURN", "return", 4, 5),
        Token("T_IDENTIFIER", "a", 4, 12),
        Token("T_SEMICOLON", ";", 4, 13),
        Token("T_RBRACE", "}", 5, 1),
        Token("T_ELSE", "else", 5, 3),
        Token("T_LBRACE", "{", 5, 8),
        Token("T_RETURN", "return", 6, 5),
        Token("T_INTLIT", "0", 6, 12),
        Token("T_SEMICOLON", ";", 6, 13),
        Token("T_RBRACE", "}", 7, 1),
        
        Token("T_FOR", "for", 8, 1),
        Token("T_LPAREN", "(", 8, 5),
        Token("T_INT", "int", 8, 6),
        Token("T_IDENTIFIER", "i", 8, 10),
        Token("T_ASSIGNOP", "=", 8, 12),
        Token("T_INTLIT", "0", 8, 14),
        Token("T_SEMICOLON", ";", 8, 15),
        Token("T_IDENTIFIER", "i", 8, 17),
        Token("T_LT", "<", 8, 19),
        Token("T_INTLIT", "10", 8, 21),
        Token("T_SEMICOLON", ";", 8, 23),
        Token("T_IDENTIFIER", "i", 8, 25),
        Token("T_INCREMENT", "++", 8, 26),  // FIXED: This should be T_INCREMENT
        Token("T_RPAREN", ")", 8, 28),
        Token("T_LBRACE", "{", 8, 30),
        Token("T_IDENTIFIER", "a", 9, 5),
        Token("T_ASSIGNOP", "=", 9, 7),
        Token("T_IDENTIFIER", "a", 9, 9),
        Token("T_PLUS", "+", 9, 11),
        Token("T_IDENTIFIER", "i", 9, 13),
        Token("T_SEMICOLON", ";", 9, 14),
        Token("T_RBRACE", "}", 10, 1),
        
        Token("T_RETURN", "return", 11, 1),
        Token("T_IDENTIFIER", "a", 11, 8),
        Token("T_SEMICOLON", ";", 11, 9),
        
        Token("EOF", "", 12, 1)
    };
    return tokens;
}

int main() {
    try {
        cout << "=== Parsing Test Code ===" << endl;
        cout << "Source code equivalent:" << endl;
        cout << "int a = 5;" << endl;
        cout << "int b = a + 3;" << endl;
        cout << "if (a > 0) {" << endl;
        cout << "    return a;" << endl;
        cout << "} else {" << endl;
        cout << "    return 0;" << endl;
        cout << "}" << endl;
        cout << "for (int i = 0; i < 10; i++) {" << endl;
        cout << "    a = a + i;" << endl;
        cout << "}" << endl;
        cout << "return a;" << endl << endl;
        
        vector<Token> tokens = createTestTokens();
        Parser parser(tokens);
        vector<unique_ptr<AstNode>> ast = parser.parse();
        
        cout << "AST Output:" << endl;
        cout << "[" << endl;
        for (size_t i = 0; i < ast.size(); ++i) {
            if (i > 0) cout << "," << endl;
            cout << "    " << ast[i]->toString();
        }
        cout << endl << "]" << endl;
        
    } catch (const exception& e) {
        cerr << "Parsing failed: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}