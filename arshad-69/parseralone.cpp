#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
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
    T_IF, T_ELSE, T_WHILE, T_RETURN, T_PRINT,
    T_MAIN,
    T_SINGLE_COMMENT, T_MULTI_COMMENT,
    T_ERROR, T_EOF
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
};

// === Parse Error Types ===
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
    ExpectedSemicolon,
    ExpectedRightParen,
    ExpectedLeftBrace,
    ExpectedRightBrace
};

// === AST Node Types ===
enum class NodeType {
    Program,
    FunctionDecl,
    Parameter,
    Block,
    VarDecl,
    Assignment,
    IfStmt,
    WhileStmt,
    ReturnStmt,
    PrintStmt,
    ExprStmt,
    BinaryOp,
    UnaryOp,
    FunctionCall,
    Identifier,
    IntLiteral,
    FloatLiteral,
    StringLiteral,
    CharLiteral,
    BoolLiteral
};

// === AST Node Base Class ===
class ASTNode {
public:
    NodeType nodeType;
    int line, column;
    
    ASTNode(NodeType type, int l = 0, int c = 0) : nodeType(type), line(l), column(c) {}
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
    
protected:
    void printIndent(int indent) const {
        for (int i = 0; i < indent; i++) cout << "  ";
    }
};

// === Specific AST Node Classes ===
class Program : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> declarations;
    
    Program() : ASTNode(NodeType::Program) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Program [\n";
        for (const auto& decl : declarations) {
            decl->print(indent + 1);
        }
        printIndent(indent);
        cout << "]\n";
    }
};

class Parameter : public ASTNode {
public:
    string type;
    string name;
    
    Parameter(const string& t, const string& n) : ASTNode(NodeType::Parameter), type(t), name(n) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Param { type: " << type << ", name: \"" << name << "\" }\n";
    }
};

class FunctionDecl : public ASTNode {
public:
    string returnType;
    string name;
    vector<unique_ptr<Parameter>> parameters;
    unique_ptr<ASTNode> body;
    
    FunctionDecl(const string& retType, const string& funcName) 
        : ASTNode(NodeType::FunctionDecl), returnType(retType), name(funcName) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Fn(\n";
        printIndent(indent + 1);
        cout << "FnDecl {\n";
        printIndent(indent + 2);
        cout << "type_tok: " << returnType << ",\n";
        printIndent(indent + 2);
        cout << "ident: \"" << name << "\",\n";
        printIndent(indent + 2);
        cout << "params: [\n";
        for (const auto& param : parameters) {
            param->print(indent + 3);
        }
        printIndent(indent + 2);
        cout << "],\n";
        printIndent(indent + 2);
        cout << "block: [\n";
        if (body) body->print(indent + 3);
        printIndent(indent + 2);
        cout << "]\n";
        printIndent(indent + 1);
        cout << "}\n";
        printIndent(indent);
        cout << ")\n";
    }
};

class Block : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> statements;
    
    Block() : ASTNode(NodeType::Block) {}
    
    void print(int indent = 0) const override {
        for (const auto& stmt : statements) {
            stmt->print(indent);
        }
    }
};

class VarDecl : public ASTNode {
public:
    string type;
    string name;
    unique_ptr<ASTNode> initializer;
    
    VarDecl(const string& t, const string& n) : ASTNode(NodeType::VarDecl), type(t), name(n) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Var(\n";
        printIndent(indent + 1);
        cout << "VarDecl {\n";
        printIndent(indent + 2);
        cout << "type_tok: " << type << ",\n";
        printIndent(indent + 2);
        cout << "ident: \"" << name << "\",\n";
        printIndent(indent + 2);
        cout << "expr: ";
        if (initializer) {
            cout << "Some(\n";
            initializer->print(indent + 3);
            printIndent(indent + 2);
            cout << ")\n";
        } else {
            cout << "None\n";
        }
        printIndent(indent + 1);
        cout << "}\n";
        printIndent(indent);
        cout << ")\n";
    }
};

class Assignment : public ASTNode {
public:
    string variable;
    unique_ptr<ASTNode> expression;
    
    Assignment(const string& var) : ASTNode(NodeType::Assignment), variable(var) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Assign(\"" << variable << "\", ";
        if (expression) expression->print(0);
        cout << ")\n";
    }
};

class BinaryOp : public ASTNode {
public:
    string operator_;
    unique_ptr<ASTNode> left;
    unique_ptr<ASTNode> right;
    
    BinaryOp(const string& op) : ASTNode(NodeType::BinaryOp), operator_(op) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "::" << getOpName() << "(" << operator_ << ")\n";
        if (left) left->print(indent + 1);
        if (right) right->print(indent + 1);
    }
    
private:
    string getOpName() const {
        if (operator_ == "+") return "Add";
        if (operator_ == "-") return "Sub";
        if (operator_ == "*") return "Mul";
        if (operator_ == "/") return "Div";
        if (operator_ == "==") return "Comp";
        if (operator_ == "!=") return "Comp";
        if (operator_ == "<") return "Comp";
        if (operator_ == ">") return "Comp";
        if (operator_ == "<=") return "Comp";
        if (operator_ == ">=") return "Comp";
        return "BinOp";
    }
};

class UnaryOp : public ASTNode {
public:
    string operator_;
    unique_ptr<ASTNode> operand;
    
    UnaryOp(const string& op) : ASTNode(NodeType::UnaryOp), operator_(op) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Unary(" << operator_ << ")\n";
        if (operand) operand->print(indent + 1);
    }
};

class FunctionCall : public ASTNode {
public:
    string functionName;
    vector<unique_ptr<ASTNode>> arguments;
    
    FunctionCall(const string& name) : ASTNode(NodeType::FunctionCall), functionName(name) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Call(FnCall {\n";
        printIndent(indent + 1);
        cout << "ident: \"" << functionName << "\",\n";
        printIndent(indent + 1);
        cout << "args: [\n";
        for (const auto& arg : arguments) {
            printIndent(indent + 2);
            cout << "Some(\n";
            arg->print(indent + 3);
            printIndent(indent + 2);
            cout << "),\n";
        }
        printIndent(indent + 1);
        cout << "]\n";
        printIndent(indent);
        cout << "})\n";
    }
};

class Identifier : public ASTNode {
public:
    string name;
    
    Identifier(const string& n) : ASTNode(NodeType::Identifier), name(n) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "\"" << name << "\"\n";
    }
};

class IntLiteral : public ASTNode {
public:
    int value;
    
    IntLiteral(int v) : ASTNode(NodeType::IntLiteral), value(v) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << value << "\n";
    }
};

class FloatLiteral : public ASTNode {
public:
    float value;
    
    FloatLiteral(float v) : ASTNode(NodeType::FloatLiteral), value(v) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << value << "\n";
    }
};

class StringLiteral : public ASTNode {
public:
    string value;
    
    StringLiteral(const string& v) : ASTNode(NodeType::StringLiteral), value(v) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "\"" << value << "\"\n";
    }
};

class ReturnStmt : public ASTNode {
public:
    unique_ptr<ASTNode> expression;
    
    ReturnStmt() : ASTNode(NodeType::ReturnStmt) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Ret(\n";
        printIndent(indent + 1);
        cout << "ExprStmt {\n";
        printIndent(indent + 2);
        cout << "expr: ";
        if (expression) {
            cout << "Some(\n";
            expression->print(indent + 3);
            printIndent(indent + 2);
            cout << ")\n";
        } else {
            cout << "None\n";
        }
        printIndent(indent + 1);
        cout << "}\n";
        printIndent(indent);
        cout << ")\n";
    }
};

class IfStmt : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    unique_ptr<ASTNode> thenBranch;
    unique_ptr<ASTNode> elseBranch;
    
    IfStmt() : ASTNode(NodeType::IfStmt) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "If(\n";
        printIndent(indent + 1);
        cout << "IfStmt {\n";
        printIndent(indent + 2);
        cout << "cond: ";
        if (condition) {
            cout << "Some(\n";
            condition->print(indent + 3);
            printIndent(indent + 2);
            cout << "),\n";
        } else {
            cout << "None,\n";
        }
        printIndent(indent + 2);
        cout << "if_block: [\n";
        if (thenBranch) thenBranch->print(indent + 3);
        printIndent(indent + 2);
        cout << "],\n";
        printIndent(indent + 2);
        cout << "else_block: [\n";
        if (elseBranch) elseBranch->print(indent + 3);
        printIndent(indent + 2);
        cout << "]\n";
        printIndent(indent + 1);
        cout << "}\n";
        printIndent(indent);
        cout << ")\n";
    }
};

class WhileStmt : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    unique_ptr<ASTNode> body;
    
    WhileStmt() : ASTNode(NodeType::WhileStmt) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "While(\n";
        printIndent(indent + 1);
        cout << "WhileStmt {\n";
        printIndent(indent + 2);
        cout << "cond: ";
        if (condition) {
            cout << "Some(\n";
            condition->print(indent + 3);
            printIndent(indent + 2);
            cout << "),\n";
        }
        printIndent(indent + 2);
        cout << "body: [\n";
        if (body) body->print(indent + 3);
        printIndent(indent + 2);
        cout << "]\n";
        printIndent(indent + 1);
        cout << "}\n";
        printIndent(indent);
        cout << ")\n";
    }
};

class PrintStmt : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> arguments;
    
    PrintStmt() : ASTNode(NodeType::PrintStmt) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        cout << "Print([\n";
        for (const auto& arg : arguments) {
            arg->print(indent + 1);
        }
        printIndent(indent);
        cout << "])\n";
    }
};

// === Parser Class ===
class Parser {
private:
    vector<Token> tokens;
    size_t current;
    
public:
    Parser(const vector<Token>& tokenList) : tokens(tokenList), current(0) {}
    
    unique_ptr<Program> parse() {
        auto program = make_unique<Program>();
        
        while (!isAtEnd()) {
            if (peek().type == T_EOF) break;
            
            try {
                auto decl = parseDeclaration();
                if (decl) {
                    program->declarations.push_back(move(decl));
                }
            } catch (const exception& e) {
                cerr << "Parse error: " << e.what() << endl;
                synchronize();
            }
        }
        
        return program;
    }

private:
    // === Utility Methods ===
    bool isAtEnd() const {
        return current >= tokens.size() || peek().type == T_EOF;
    }
    
    Token peek() const {
        if (current < tokens.size()) {
            return tokens[current];
        }
        return Token{T_EOF, "", 0, 0};
    }
    
    Token previous() const {
        if (current > 0 && current <= tokens.size()) {
            return tokens[current - 1];
        }
        return Token{T_EOF, "", 0, 0};
    }
    
    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }
    
    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }
    
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
    
    bool match(const vector<TokenType>& types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }
    
    Token consume(TokenType type, const string& message) {
        if (check(type)) return advance();
        throw runtime_error(message + " at line " + to_string(peek().line));
    }
    
    void synchronize() {
        advance();
        while (!isAtEnd()) {
            if (previous().type == T_SEMICOLON) return;
            
            switch (peek().type) {
                case T_IF:
                case T_WHILE:
                case T_RETURN:
                case T_INT:
                case T_FLOAT:
                case T_DOUBLE:
                case T_CHAR:
                case T_VOID:
                case T_BOOL:
                    return;
                default:
                    break;
            }
            advance();
        }
    }
    
    bool isTypeToken(TokenType type) const {
        return type == T_INT || type == T_FLOAT || type == T_DOUBLE || 
               type == T_CHAR || type == T_VOID || type == T_BOOL;
    }
    
    // === Parsing Methods ===
    unique_ptr<ASTNode> parseDeclaration() {
        if (isTypeToken(peek().type)) {
            Token typeToken = advance();
            
            if (!check(T_IDENTIFIER)) {
                throw runtime_error("Expected identifier after type");
            }
            
            Token identifier = advance();
            
            if (match(T_LPAREN)) {
                return parseFunctionDeclaration(typeToken.value, identifier.value);
            } else {
                return parseVariableDeclaration(typeToken.value, identifier.value);
            }
        }
        
        return parseStatement();
    }
    
    unique_ptr<FunctionDecl> parseFunctionDeclaration(const string& returnType, const string& name) {
        auto func = make_unique<FunctionDecl>(returnType, name);
        
        // Parse parameters
        if (!check(T_RPAREN)) {
            do {
                if (!isTypeToken(peek().type)) {
                    throw runtime_error("Expected parameter type");
                }
                Token paramType = advance();
                
                if (!check(T_IDENTIFIER)) {
                    throw runtime_error("Expected parameter name");
                }
                Token paramName = advance();
                
                func->parameters.push_back(make_unique<Parameter>(paramType.value, paramName.value));
            } while (match(T_COMMA));
        }
        
        consume(T_RPAREN, "Expected ')' after parameters");
        consume(T_LBRACE, "Expected '{' before function body");
        
        func->body = parseBlock();
        
        consume(T_RBRACE, "Expected '}' after function body");
        
        return func;
    }
    
    unique_ptr<VarDecl> parseVariableDeclaration(const string& type, const string& name) {
        auto varDecl = make_unique<VarDecl>(type, name);
        
        if (match(T_ASSIGNOP)) {
            varDecl->initializer = parseExpression();
        }
        
        consume(T_SEMICOLON, "Expected ';' after variable declaration");
        return varDecl;
    }
    
    unique_ptr<ASTNode> parseStatement() {
        if (match(T_IF)) return parseIfStatement();
        if (match(T_WHILE)) return parseWhileStatement();
        if (match(T_RETURN)) return parseReturnStatement();
        if (match(T_PRINT)) return parsePrintStatement();
        if (match(T_LBRACE)) return parseBlock();
        
        return parseExpressionStatement();
    }
    
    unique_ptr<Block> parseBlock() {
        auto block = make_unique<Block>();
        
        while (!check(T_RBRACE) && !isAtEnd()) {
            auto stmt = parseDeclaration();
            if (stmt) {
                block->statements.push_back(move(stmt));
            }
        }
        
        return block;
    }
    
    unique_ptr<IfStmt> parseIfStatement() {
        auto ifStmt = make_unique<IfStmt>();
        
        consume(T_LPAREN, "Expected '(' after 'if'");
        ifStmt->condition = parseExpression();
        consume(T_RPAREN, "Expected ')' after if condition");
        
        ifStmt->thenBranch = parseStatement();
        
        if (match(T_ELSE)) {
            ifStmt->elseBranch = parseStatement();
        }
        
        return ifStmt;
    }
    
    unique_ptr<WhileStmt> parseWhileStatement() {
        auto whileStmt = make_unique<WhileStmt>();
        
        consume(T_LPAREN, "Expected '(' after 'while'");
        whileStmt->condition = parseExpression();
        consume(T_RPAREN, "Expected ')' after while condition");
        
        whileStmt->body = parseStatement();
        
        return whileStmt;
    }
    
    unique_ptr<ReturnStmt> parseReturnStatement() {
        auto returnStmt = make_unique<ReturnStmt>();
        
        if (!check(T_SEMICOLON)) {
            returnStmt->expression = parseExpression();
        }
        
        consume(T_SEMICOLON, "Expected ';' after return statement");
        return returnStmt;
    }
    
    unique_ptr<PrintStmt> parsePrintStatement() {
        auto printStmt = make_unique<PrintStmt>();
        
        consume(T_LPAREN, "Expected '(' after 'print'");
        
        if (!check(T_RPAREN)) {
            do {
                printStmt->arguments.push_back(parseExpression());
            } while (match(T_COMMA));
        }
        
        consume(T_RPAREN, "Expected ')' after print arguments");
        consume(T_SEMICOLON, "Expected ';' after print statement");
        
        return printStmt;
    }
    
    unique_ptr<ASTNode> parseExpressionStatement() {
        auto expr = parseExpression();
        consume(T_SEMICOLON, "Expected ';' after expression");
        return expr;
    }
    
    unique_ptr<ASTNode> parseExpression() {
        return parseAssignment();
    }
    
    unique_ptr<ASTNode> parseAssignment() {
        auto expr = parseLogicalOr();
        
        if (match(T_ASSIGNOP)) {
            if (expr->nodeType == NodeType::Identifier) {
                auto identifier = static_cast<Identifier*>(expr.get());
                auto assignment = make_unique<Assignment>(identifier->name);
                assignment->expression = parseAssignment();
                expr.release(); // Release the identifier since we're replacing it
                return move(assignment);
            } else {
                throw runtime_error("Invalid assignment target");
            }
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parseLogicalOr() {
        auto expr = parseLogicalAnd();
        
        while (match(T_OR)) {
            string op = previous().value;
            auto right = parseLogicalAnd();
            auto binary = make_unique<BinaryOp>(op);
            binary->left = move(expr);
            binary->right = move(right);
            expr = move(binary);
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parseLogicalAnd() {
        auto expr = parseEquality();
        
        while (match(T_AND)) {
            string op = previous().value;
            auto right = parseEquality();
            auto binary = make_unique<BinaryOp>(op);
            binary->left = move(expr);
            binary->right = move(right);
            expr = move(binary);
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parseEquality() {
        auto expr = parseComparison();
        
        while (match({T_EQUALOP, T_NE})) {
            string op = previous().value;
            auto right = parseComparison();
            auto binary = make_unique<BinaryOp>(op);
            binary->left = move(expr);
            binary->right = move(right);
            expr = move(binary);
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parseComparison() {
        auto expr = parseTerm();
        
        while (match({T_GT, T_GE, T_LT, T_LE})) {
            string op = previous().value;
            auto right = parseTerm();
            auto binary = make_unique<BinaryOp>(op);
            binary->left = move(expr);
            binary->right = move(right);
            expr = move(binary);
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parseTerm() {
        auto expr = parseFactor();
        
        while (match({T_MINUS, T_PLUS})) {
            string op = previous().value;
            auto right = parseFactor();
            auto binary = make_unique<BinaryOp>(op);
            binary->left = move(expr);
            binary->right = move(right);
            expr = move(binary);
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parseFactor() {
        auto expr = parseUnary();
        
        while (match({T_DIVIDE, T_MULTIPLY, T_MODULO})) {
            string op = previous().value;
            auto right = parseUnary();
            auto binary = make_unique<BinaryOp>(op);
            binary->left = move(expr);
            binary->right = move(right);
            expr = move(binary);
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parseUnary() {
        if (match({T_NOT, T_MINUS})) {
            string op = previous().value;
            auto right = parseUnary();
            auto unary = make_unique<UnaryOp>(op);
            unary->operand = move(right);
            return move(unary);
        }
        
        return parseCall();
    }
    
    unique_ptr<ASTNode> parseCall() {
        auto expr = parsePrimary();
        
        while (true) {
            if (match(T_LPAREN)) {
                if (expr->nodeType == NodeType::Identifier) {
                    auto identifier = static_cast<Identifier*>(expr.get());
                    auto call = make_unique<FunctionCall>(identifier->name);
                    
                    if (!check(T_RPAREN)) {
                        do {
                            call->arguments.push_back(parseExpression());
                        } while (match(T_COMMA));
                    }
                    
                    consume(T_RPAREN, "Expected ')' after arguments");
                    expr.release(); // Release the identifier
                    expr = move(call);
                } else {
                    throw runtime_error("Only identifiers can be called");
                }
            } else {
                break;
            }
        }
        
        return expr;
    }
    
    unique_ptr<ASTNode> parsePrimary() {
        if (match(T_BOOLLIT)) {
            // Handle boolean literals if needed
            return make_unique<IntLiteral>(previous().value == "true" ? 1 : 0);
        }
        
        if (match(T_INTLIT)) {
            return make_unique<IntLiteral>(stoi(previous().value));
        }
        
        if (match(T_FLOATLIT)) {
            return make_unique<FloatLiteral>(stof(previous().value));
        }
        
        if (match(T_STRINGLIT)) {
            return make_unique<StringLiteral>(previous().value);
        }
        
        if (match(T_IDENTIFIER)) {
            return make_unique<Identifier>(previous().value);
        }
        
        if (match(T_LPAREN)) {
            auto expr = parseExpression();
            consume(T_RPAREN, "Expected ')' after expression");
            return expr;
        }
        
        throw runtime_error("Expected expression at line " + to_string(peek().line));
    }
};

// === Lexer Integration (simplified version of your lexer) ===
static const map<string, TokenType> keywords = {
    {"int", T_INT}, {"float", T_FLOAT}, {"double", T_DOUBLE},
    {"char", T_CHAR}, {"void", T_VOID}, {"bool", T_BOOL},
    {"true", T_BOOLLIT}, {"false", T_BOOLLIT},
    {"if", T_IF}, {"else", T_ELSE}, {"while", T_WHILE}, {"return", T_RETURN},
    {"print", T_PRINT}, {"main", T_MAIN}
};

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case T_INT: return "T_INT";
        case T_FLOAT: return "T_FLOAT";
        case T_IDENTIFIER: return "T_IDENTIFIER";
        case T_INTLIT: return "T_INTLIT";
        case T_FLOATLIT: return "T_FLOATLIT";
        case T_STRINGLIT: return "T_STRINGLIT";
        case T_LPAREN: return "T_LPAREN";
        case T_RPAREN: return "T_RPAREN";
        case T_LBRACE: return "T_LBRACE";
        case T_RBRACE: return "T_RBRACE";
        case T_SEMICOLON: return "T_SEMICOLON";
        case T_COMMA: return "T_COMMA";
        case T_ASSIGNOP: return "T_ASSIGNOP";
        case T_EQUALOP: return "T_EQUALOP";
        case T_LT: return "T_LT";
        case T_GT: return "T_GT";
        case T_PLUS: return "T_PLUS";
        case T_MINUS: return "T_MINUS";
        case T_MULTIPLY: return "T_MULTIPLY";
        case T_DIVIDE: return "T_DIVIDE";
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_RETURN: return "T_RETURN";
        case T_PRINT: return "T_PRINT";
        case T_EOF: return "T_EOF";
        default: return "T_UNKNOWN";
    }
}

// Function to parse tokens from your lexer's output file
vector<Token> loadTokensFromFile(const string& filename) {
    vector<Token> tokens;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open " << filename << endl;
        return tokens;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // Parse format: TOKEN_TYPE(value),line,column
        size_t parenPos = line.find('(');
        size_t closeParenPos = line.find(')');
        size_t firstCommaPos = line.find(',', closeParenPos);
        size_t secondCommaPos = line.find(',', firstCommaPos + 1);
        
        if (parenPos == string::npos || closeParenPos == string::npos || 
            firstCommaPos == string::npos || secondCommaPos == string::npos) {
            continue;
        }
        
        string tokenTypeStr = line.substr(0, parenPos);
        string value = line.substr(parenPos + 1, closeParenPos - parenPos - 1);
        int lineNum = stoi(line.substr(firstCommaPos + 1, secondCommaPos - firstCommaPos - 1));
        int column = stoi(line.substr(secondCommaPos + 1));
        
        // Convert string to TokenType
        TokenType type = T_EOF;
        if (tokenTypeStr == "T_INT") type = T_INT;
        else if (tokenTypeStr == "T_FLOAT") type = T_FLOAT;
        else if (tokenTypeStr == "T_IDENTIFIER") type = T_IDENTIFIER;
        else if (tokenTypeStr == "T_INTLIT") type = T_INTLIT;
        else if (tokenTypeStr == "T_FLOATLIT") type = T_FLOATLIT;
        else if (tokenTypeStr == "T_STRINGLIT") type = T_STRINGLIT;
        else if (tokenTypeStr == "T_LPAREN") type = T_LPAREN;
        else if (tokenTypeStr == "T_RPAREN") type = T_RPAREN;
        else if (tokenTypeStr == "T_LBRACE") type = T_LBRACE;
        else if (tokenTypeStr == "T_RBRACE") type = T_RBRACE;
        else if (tokenTypeStr == "T_SEMICOLON") type = T_SEMICOLON;
        else if (tokenTypeStr == "T_COMMA") type = T_COMMA;
        else if (tokenTypeStr == "T_ASSIGNOP") type = T_ASSIGNOP;
        else if (tokenTypeStr == "T_EQUALOP") type = T_EQUALOP;
        else if (tokenTypeStr == "T_LT") type = T_LT;
        else if (tokenTypeStr == "T_GT") type = T_GT;
        else if (tokenTypeStr == "T_LE") type = T_LE;
        else if (tokenTypeStr == "T_GE") type = T_GE;
        else if (tokenTypeStr == "T_NE") type = T_NE;
        else if (tokenTypeStr == "T_PLUS") type = T_PLUS;
        else if (tokenTypeStr == "T_MINUS") type = T_MINUS;
        else if (tokenTypeStr == "T_MULTIPLY") type = T_MULTIPLY;
        else if (tokenTypeStr == "T_DIVIDE") type = T_DIVIDE;
        else if (tokenTypeStr == "T_MODULO") type = T_MODULO;
        else if (tokenTypeStr == "T_AND") type = T_AND;
        else if (tokenTypeStr == "T_OR") type = T_OR;
        else if (tokenTypeStr == "T_NOT") type = T_NOT;
        else if (tokenTypeStr == "T_IF") type = T_IF;
        else if (tokenTypeStr == "T_ELSE") type = T_ELSE;
        else if (tokenTypeStr == "T_WHILE") type = T_WHILE;
        else if (tokenTypeStr == "T_RETURN") type = T_RETURN;
        else if (tokenTypeStr == "T_PRINT") type = T_PRINT;
        else if (tokenTypeStr == "T_BOOLLIT") type = T_BOOLLIT;
        else if (tokenTypeStr == "T_CHARLIT") type = T_CHARLIT;
        else continue; // Skip unknown tokens
        
        tokens.push_back({type, value, lineNum, column});
    }
    
    // Add EOF token
    tokens.push_back({T_EOF, "", 0, 0});
    return tokens;
}

int main() {
    try {
        // Load tokens from the lexer output file
        vector<Token> tokens = loadTokensFromFile("tester/tokens.txt");
        
        if (tokens.empty()) {
            cerr << "No tokens found or could not read tokens file." << endl;
            return 1;
        }
        
        cout << "Loaded " << tokens.size() << " tokens from lexer output." << endl;
        
        // Create parser and parse the tokens
        Parser parser(tokens);
        auto ast = parser.parse();
        
        // Open output file for AST
        ofstream outputFile("tester/ast.txt");
        if (!outputFile.is_open()) {
            cerr << "Could not create ast.txt output file." << endl;
            return 1;
        }
        
        // Redirect cout to the output file
        streambuf* coutBuf = cout.rdbuf();
        cout.rdbuf(outputFile.rdbuf());
        
        // Print the AST
        cout << "=== Abstract Syntax Tree ===" << endl;
        ast->print();
        
        // Restore cout
        cout.rdbuf(coutBuf);
        outputFile.close();
        
        // Also print to console for immediate feedback
        cout << "Parsing completed successfully!" << endl;
        cout << "AST written to tester/ast.txt" << endl;
        
        // Print a preview of the AST to console
        cout << "\n=== AST Preview ===" << endl;
        ast->print();
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}