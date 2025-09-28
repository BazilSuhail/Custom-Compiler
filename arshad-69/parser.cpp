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