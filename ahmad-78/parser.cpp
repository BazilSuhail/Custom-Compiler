#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <initializer_list>
#include <stdexcept>
#include <map>
using namespace std;

#include "lexer.cpp" // Assuming lexer.cpp defines Token and TokenType

// ============================================================================
// TOKEN STRUCTURE (MATCHING YOUR LEXER)
// ============================================================================

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
    unique_ptr<AstNode> fnDecl();
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
    bool match(initializer_list<TokenType> types);
    Token consume(TokenType type, string message);
    bool check(TokenType type);
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
    if (check(T_INT) || check(T_FLOAT) || check(T_DOUBLE) || check(T_CHAR) || check(T_BOOL) || check(T_VOID)) {
        advance(); // CONSUME THE TYPE TOKEN
        
        // Look ahead to see if this is a function declaration
        if (current < tokens.size() && tokens[current].type == T_IDENTIFIER && 
            current + 1 < tokens.size() && tokens[current + 1].type == T_LPAREN) {
            return fnDecl();
        } else {
            return varDecl();
        }
    } else {
        return statement();
    }
}

unique_ptr<AstNode> Parser::varDecl() {
    string type = previous().value;  // GET TYPE FROM PREVIOUS TOKEN
    Token name = consume(T_IDENTIFIER, "Expected variable name");
    
    auto varDecl = make_unique<VarDecl>();
    varDecl->type = type;
    varDecl->name = name.value;
    
    if (match({T_ASSIGNOP})) {
        varDecl->initializer = expression();
    }
    
    consume(T_SEMICOLON, "Expected ';' after variable declaration");
    return varDecl;
}

unique_ptr<AstNode> Parser::fnDecl() {
    string returnType = previous().value;  // GET TYPE FROM PREVIOUS TOKEN
    Token name = consume(T_IDENTIFIER, "Expected function name");
    
    auto fnDecl = make_unique<FnDecl>();
    fnDecl->returnType = returnType;
    fnDecl->name = name.value;
    
    consume(T_LPAREN, "Expected '(' after function name");
    
    // Parse parameters
    if (!check(T_RPAREN)) {
        do {
            // Parse parameter type
            if (!(check(T_INT) || check(T_FLOAT) || check(T_DOUBLE) || check(T_CHAR) || check(T_BOOL) || check(T_VOID))) {
                error(ParseError::ExpectedTypeToken, "Expected parameter type");
            }
            string paramType = advance().value;
            
            // Parse parameter name
            Token paramName = consume(T_IDENTIFIER, "Expected parameter name");
            
            fnDecl->params.push_back(make_pair(paramType, paramName.value));
            
        } while (match({T_COMMA}));
    }
    
    consume(T_RPAREN, "Expected ')' after parameters");
    consume(T_LBRACE, "Expected '{' before function body");
    
    // Parse function body
    while (!check(T_RBRACE) && !isAtEnd()) {
        fnDecl->body.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after function body");
    return fnDecl;
}

unique_ptr<AstNode> Parser::statement() {
    if (check(T_RETURN)) {
        return returnStmt();
    } else if (check(T_IF)) {
        return ifStmt();
    } else if (check(T_FOR)) {
        return forStmt();
    } else if (check(T_BREAK)) {
        return breakStmt();
    } else if (check(T_IDENTIFIER) && current + 1 < tokens.size() && tokens[current + 1].type == T_ASSIGNOP) {
        return assignStmt();
    } else {
        return exprStmt();
    }
}

unique_ptr<AstNode> Parser::exprStmt() {
    auto expr = expression();
    consume(T_SEMICOLON, "Expected ';' after expression");
    
    auto exprStmt = make_unique<ExprStmt>();
    exprStmt->expr = move(expr);
    return exprStmt;
}

unique_ptr<AstNode> Parser::assignStmt() {
    Token name = consume(T_IDENTIFIER, "Expected identifier");
    consume(T_ASSIGNOP, "Expected '='");
    auto expr = expression();
    consume(T_SEMICOLON, "Expected ';' after assignment");
    
    auto assignStmt = make_unique<AssignStmt>();
    assignStmt->identifier = name.value;
    assignStmt->expr = move(expr);
    return assignStmt;
}

unique_ptr<AstNode> Parser::ifStmt() {
    consume(T_IF, "Expected 'if'");
    consume(T_LPAREN, "Expected '(' after 'if'");
    
    auto condition = expression();
    consume(T_RPAREN, "Expected ')' after condition");
    
    consume(T_LBRACE, "Expected '{' before if body");
    
    auto ifStmt = make_unique<IfStmt>();
    ifStmt->condition = move(condition);
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        ifStmt->thenBranch.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after if body");
    
    if (match({T_ELSE})) {
        consume(T_LBRACE, "Expected '{' before else body");
        
        while (!check(T_RBRACE) && !isAtEnd()) {
            ifStmt->elseBranch.push_back(declaration());
        }
        
        consume(T_RBRACE, "Expected '}' after else body");
    }
    
    return ifStmt;
}

unique_ptr<AstNode> Parser::forStmt() {
    consume(T_FOR, "Expected 'for'");
    consume(T_LPAREN, "Expected '(' after 'for'");
    
    auto forStmt = make_unique<ForStmt>();
    
    // Initializer
    if (check(T_INT) || check(T_FLOAT) || check(T_DOUBLE) || check(T_CHAR) || check(T_BOOL)) {
        advance(); // CONSUME THE TYPE TOKEN
        forStmt->initializer = varDecl();
    } else if (!check(T_SEMICOLON)) {
        forStmt->initializer = exprStmt();
    } else {
        advance(); // consume ';'
    }
    
    // Condition
    if (!check(T_SEMICOLON)) {
        forStmt->condition = expression();
    }
    consume(T_SEMICOLON, "Expected ';' in for loop");
    
    // Increment
    if (!check(T_RPAREN)) {
        forStmt->increment = expression();  // CHANGED: parse as expression instead of special cases
    }
    
    consume(T_RPAREN, "Expected ')' after for clauses");
    consume(T_LBRACE, "Expected '{' before for body");
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        forStmt->body.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after for body");
    
    return forStmt;
}

unique_ptr<AstNode> Parser::returnStmt() {
    consume(T_RETURN, "Expected 'return'");
    
    auto returnStmt = make_unique<ReturnStmt>();
    
    if (!check(T_SEMICOLON)) {
        returnStmt->expr = expression();
    }
    
    consume(T_SEMICOLON, "Expected ';' after return value");
    return returnStmt;
}

unique_ptr<AstNode> Parser::breakStmt() {
    consume(T_BREAK, "Expected 'break'");
    consume(T_SEMICOLON, "Expected ';' after break");
    return make_unique<BreakStmt>();
}

// Expression parsing with precedence
unique_ptr<AstNode> Parser::expression() {
    return assignment();
}

unique_ptr<AstNode> Parser::assignment() {
    auto expr = equality();
    
    if (match({T_ASSIGNOP})) {
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
    
    while (match({T_EQUALOP, T_NE})) {
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
    
    while (match({T_LT, T_GT, T_LE, T_GE})) {
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
    
    while (match({T_PLUS, T_MINUS})) {
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
    
    while (match({T_MULTIPLY, T_DIVIDE})) {
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
    if (match({T_MINUS, T_PLUS})) {
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
    if (match({T_INCREMENT, T_DECREMENT})) {
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
    
    if (match({T_LPAREN})) {
        auto call = make_unique<CallExpr>();
        
        if (auto identExpr = dynamic_cast<IdentifierExpr*>(expr.get())) {
            call->callee = identExpr->name;
        }
        
        if (!check(T_RPAREN)) {
            do {
                call->arguments.push_back(expression());
            } while (match({T_COMMA}));
        }
        
        consume(T_RPAREN, "Expected ')' after arguments");
        return call;
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::primary() {
    if (match({T_INTLIT, T_FLOATLIT, T_BOOLLIT, T_STRINGLIT, T_CHARLIT})) {
        auto literal = make_unique<LiteralExpr>();
        literal->type = tokenTypeToString(previous().type);
        literal->value = previous().value;
        return literal;
    }
    
    if (match({T_IDENTIFIER})) {
        auto ident = make_unique<IdentifierExpr>();
        ident->name = previous().value;
        return ident;
    }
    
    if (match({T_LPAREN})) {
        auto expr = expression();
        consume(T_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    // Handle prefix increment/decrement
    if (match({T_INCREMENT, T_DECREMENT})) {
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
bool Parser::match(initializer_list<TokenType> types) {
    for (const TokenType& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, string message) {
    if (check(type)) return advance();
    error(ParseError::UnexpectedToken, message);
    // Return a dummy token to avoid compiler warnings
    return Token();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return current >= tokens.size() || peek().type == T_ERROR;
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

vector<Token> tokenizeWithLexer(const string& code) {
    vector<Token> tokens;
    LexerState state = createLexerState(code.c_str());
    Token token;
    while (getNextToken(state, token)) {
        if (token.type == T_ERROR) {
            cerr << "Lexer error: " << token.value << " at line " << token.line << ", column " << token.column << endl;
            continue;
        }
        // Skip comments but include other tokens
        if (token.type != T_SINGLE_COMMENT && token.type != T_MULTI_COMMENT && 
            token.type != T_INCLUDE && token.type != T_DEFINE) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int main() {
    try {
        cout << "=== Testing Simple Variable Declaration ===" << endl;
        string testCode1 = R"(
int x = 42;
float y = 3.14;
)";
        
        cout << "Source code:" << endl << testCode1 << endl;
        
        vector<Token> tokens1 = tokenizeWithLexer(testCode1);
        cout << "Tokens:" << endl;
        for (const auto& tok : tokens1) {
            cout << tokenTypeToString(tok.type) << "('" << tok.value << "') ";
        }
        cout << endl;
        cout << "Total tokens: " << tokens1.size() << endl;
        
        Parser parser1(tokens1);
        vector<unique_ptr<AstNode>> ast1 = parser1.parse();
        
        cout << "AST Output:" << endl;
        cout << "[" << endl;
        for (size_t i = 0; i < ast1.size(); ++i) {
            if (i > 0) cout << "," << endl;
            cout << "    " << ast1[i]->toString();
        }
        cout << endl << "]" << endl << endl;
        
        cout << "=== Testing For Loop ===" << endl;
        string testCode4 = R"(
for (int i = 0; i < 10; i++) {
    x = x + i;
}
)";
        
        cout << "Source code:" << endl << testCode4 << endl;
        
        vector<Token> tokens4 = tokenizeWithLexer(testCode4);
        cout << "Tokens:" << endl;
        for (const auto& tok : tokens4) {
            cout << tokenTypeToString(tok.type) << "('" << tok.value << "') ";
        }
        cout << endl;
        cout << "Total tokens: " << tokens4.size() << endl;
        
        Parser parser4(tokens4);
        vector<unique_ptr<AstNode>> ast4 = parser4.parse();
        
        cout << "AST Output:" << endl;
        cout << "[" << endl;
        for (size_t i = 0; i < ast4.size(); ++i) {
            if (i > 0) cout << "," << endl;
            cout << "    " << ast4[i]->toString();
        }
        cout << endl << "]" << endl << endl;

        cout << "=== Testing Function ===" << endl;
        string testCode5 = R"(
int result(int a, int b, int c, int d) {
        return (a + b) * c - d / 2;
}
)";

        cout << "Source code:" << endl << testCode5 << endl;
        
        vector<Token> tokens5 = tokenizeWithLexer(testCode5);
        cout << "Tokens:" << endl;
        for (const auto& tok : tokens5) {
            cout << tokenTypeToString(tok.type) << "('" << tok.value << "') ";
        }
        cout << endl;
        cout << "Total tokens: " << tokens5.size() << endl;
        
        Parser parser5(tokens5);
        vector<unique_ptr<AstNode>> ast5 = parser5.parse();
        
        cout << "AST Output:" << endl;
        cout << "[" << endl;
        for (size_t i = 0; i < ast5.size(); ++i) {
            if (i > 0) cout << "," << endl;
            cout << "    " << ast5[i]->toString();
        }
        cout << endl << "]" << endl;
        
    } catch (const exception& e) {
        cerr << "Parsing failed: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}