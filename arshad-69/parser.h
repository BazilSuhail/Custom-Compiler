#ifndef PARSER_H
#define PARSER_H

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
    
    ASTNode(NodeType type, int l = 0, int c = 0);
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
    
protected:
    void printIndent(int indent) const;
};

// === Specific AST Node Classes ===
class Program : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> declarations;
    
    Program();
    void print(int indent = 0) const override;
};

class Parameter : public ASTNode {
public:
    string type;
    string name;
    
    Parameter(const string& t, const string& n);
    void print(int indent = 0) const override;
};

class FunctionDecl : public ASTNode {
public:
    string returnType;
    string name;
    vector<unique_ptr<Parameter>> parameters;
    unique_ptr<ASTNode> body;
    
    FunctionDecl(const string& retType, const string& funcName);
    void print(int indent = 0) const override;
};

class Block : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> statements;
    
    Block();
    void print(int indent = 0) const override;
};

class VarDecl : public ASTNode {
public:
    string type;
    string name;
    unique_ptr<ASTNode> initializer;
    
    VarDecl(const string& t, const string& n);
    void print(int indent = 0) const override;
};

class Assignment : public ASTNode {
public:
    string variable;
    unique_ptr<ASTNode> expression;
    
    Assignment(const string& var);
    void print(int indent = 0) const override;
};

class BinaryOp : public ASTNode {
public:
    string operator_;
    unique_ptr<ASTNode> left;
    unique_ptr<ASTNode> right;
    
    BinaryOp(const string& op);
    void print(int indent = 0) const override;
    
private:
    string getOpName() const;
};

class UnaryOp : public ASTNode {
public:
    string operator_;
    unique_ptr<ASTNode> operand;
    
    UnaryOp(const string& op);
    void print(int indent = 0) const override;
};

class FunctionCall : public ASTNode {
public:
    string functionName;
    vector<unique_ptr<ASTNode>> arguments;
    
    FunctionCall(const string& name);
    void print(int indent = 0) const override;
};

class Identifier : public ASTNode {
public:
    string name;
    
    Identifier(const string& n);
    void print(int indent = 0) const override;
};

class IntLiteral : public ASTNode {
public:
    int value;
    
    IntLiteral(int v);
    void print(int indent = 0) const override;
};

class FloatLiteral : public ASTNode {
public:
    float value;
    
    FloatLiteral(float v);
    void print(int indent = 0) const override;
};

class StringLiteral : public ASTNode {
public:
    string value;
    
    StringLiteral(const string& v);
    void print(int indent = 0) const override;
};

class ReturnStmt : public ASTNode {
public:
    unique_ptr<ASTNode> expression;
    
    ReturnStmt();
    void print(int indent = 0) const override;
};

class IfStmt : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    unique_ptr<ASTNode> thenBranch;
    unique_ptr<ASTNode> elseBranch;
    
    IfStmt();
    void print(int indent = 0) const override;
};

class WhileStmt : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    unique_ptr<ASTNode> body;
    
    WhileStmt();
    void print(int indent = 0) const override;
};

class PrintStmt : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> arguments;
    
    PrintStmt();
    void print(int indent = 0) const override;
};

// === Parser Class ===
class Parser {
private:
    vector<Token> tokens;
    size_t current;
    
    // Utility Methods
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(const vector<TokenType>& types);
    Token consume(TokenType type, const string& message);
    void synchronize();
    bool isTypeToken(TokenType type) const;
    
    // Parsing Methods
    unique_ptr<ASTNode> parseDeclaration();
    unique_ptr<FunctionDecl> parseFunctionDeclaration(const string& returnType, const string& name);
    unique_ptr<VarDecl> parseVariableDeclaration(const string& type, const string& name);
    unique_ptr<ASTNode> parseStatement();
    unique_ptr<Block> parseBlock();
    unique_ptr<IfStmt> parseIfStatement();
    unique_ptr<WhileStmt> parseWhileStatement();
    unique_ptr<ReturnStmt> parseReturnStatement();
    unique_ptr<PrintStmt> parsePrintStatement();
    unique_ptr<ASTNode> parseExpressionStatement();
    unique_ptr<ASTNode> parseExpression();
    unique_ptr<ASTNode> parseAssignment();
    unique_ptr<ASTNode> parseLogicalOr();
    unique_ptr<ASTNode> parseLogicalAnd();
    unique_ptr<ASTNode> parseEquality();
    unique_ptr<ASTNode> parseComparison();
    unique_ptr<ASTNode> parseTerm();
    unique_ptr<ASTNode> parseFactor();
    unique_ptr<ASTNode> parseUnary();
    unique_ptr<ASTNode> parseCall();
    unique_ptr<ASTNode> parsePrimary();
    
public:
    Parser(const vector<Token>& tokenList);
    unique_ptr<Program> parse();
};

// === Lexer Integration ===
const char* tokenTypeToString(TokenType type);
vector<Token> loadTokensFromFile(const string& filename);

#endif // PARSER_H