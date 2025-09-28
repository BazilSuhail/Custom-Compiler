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