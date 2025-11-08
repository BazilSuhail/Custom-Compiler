#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
 
#include <vector> 
#include <memory>
#include <stdexcept> 
#include <unordered_map>
#include <cctype>
#include <variant>
#include <functional>

using namespace std;

enum TokenType {
    // Types
    T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_VOID, T_BOOL, T_ENUM,

    // Literals
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT, T_CHARLIT, T_BOOLLIT,

    // Brackets & Separators
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    T_SEMICOLON, T_COMMA, T_DOT,

    // Operators
    T_ASSIGNOP, T_EQUALOP, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE, T_MODULO,
    T_INCREMENT, T_DECREMENT,
    T_AND, T_OR, T_NOT,

    // Keywords
    T_IF, T_ELSE, T_WHILE, T_RETURN, T_PRINT,
    T_MAIN, T_INCLUDE,

    // New keywords
    T_STRING, T_DO, T_SWITCH, T_BREAK, T_FOR, T_DEFAULT, T_CASE, T_COLON,

    // Comments
    T_SINGLE_COMMENT, T_MULTI_COMMENT,

    // Error & EOF
    T_ERROR, T_EOF
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
};
// === Scope Analysis Errors ===
enum class ScopeErrorType {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition,
    ConflictingFunctionDefinition,
    ConflictingDeclaration,
    ParameterRedefinition,
    InvalidForwardReference,
    InvalidStorageClassUsage,
    EnumRedefinition,
    EnumVariantRedefinition,
};

struct ScopeError {
    ScopeErrorType type;
    string name;
    int line;
    int column;
    string message;

    ScopeError(ScopeErrorType t, const string& n, int l, int c) 
        : type(t), name(n), line(l), column(c) {
        switch (t) {
            case ScopeErrorType::UndeclaredVariableAccessed:
                message = "Undeclared variable accessed: '" + n + "'";
                break;
            case ScopeErrorType::UndefinedFunctionCalled:
                message = "Undefined function called: '" + n + "'";
                break;
            case ScopeErrorType::VariableRedefinition:
                message = "Variable redefinition: '" + n + "'";
                break;
            case ScopeErrorType::FunctionPrototypeRedefinition:
                message = "Function prototype redefinition: '" + n + "'";
                break;
            case ScopeErrorType::ConflictingFunctionDefinition:
                message = "Conflicting function definition: '" + n + "'";
                break;
            case ScopeErrorType::ConflictingDeclaration:
                message = "Conflicting declaration: '" + n + "'";
                break;
            case ScopeErrorType::ParameterRedefinition:
                message = "Parameter redefinition: '" + n + "'";
                break;
            case ScopeErrorType::InvalidForwardReference:
                message = "Invalid forward reference: '" + n + "'";
                break;
            case ScopeErrorType::InvalidStorageClassUsage:
                message = "Invalid storage class usage for: '" + n + "'";
                break;
            case ScopeErrorType::EnumRedefinition:
                message = "Enum redefinition: '" + n + "'";
                break;
            case ScopeErrorType::EnumVariantRedefinition:
                message = "Enum variant redefinition: '" + n + "'";
                break;
        }
    }
};

// === AST Node Types (using std::variant) ===
struct IntLiteral {
    int value;
    IntLiteral(int v) : value(v) {}
};

struct FloatLiteral {
    double value;
    FloatLiteral(double v) : value(v) {}
};

struct StringLiteral {
    string value;
    StringLiteral(const string& v) : value(v) {}
};

struct CharLiteral {
    char value;
    CharLiteral(char v) : value(v) {}
};

struct BoolLiteral {
    bool value;
    BoolLiteral(bool v) : value(v) {}
};

struct Identifier {
    string name;
    Identifier(const string& n) : name(n) {}
};

// === Expression Types ===
struct BinaryExpr {
    TokenType op;
    unique_ptr<struct ASTNode> left;
    unique_ptr<struct ASTNode> right;
    BinaryExpr(TokenType o, unique_ptr<struct ASTNode> l, unique_ptr<struct ASTNode> r) : op(o), left(move(l)), right(move(r)) {}
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

struct UnaryExpr {
    TokenType op;
    unique_ptr<struct ASTNode> operand;
    UnaryExpr(TokenType o, unique_ptr<struct ASTNode> opd) : op(o), operand(move(opd)) {}
};

// ============= language syntax
struct IncludeStmt {
    string header;
    IncludeStmt(string h) : header(move(h)) {}
};

// Enum
struct EnumValueList {
    vector<string> values;
    EnumValueList(vector<string> v) : values(move(v)) {}
};

struct EnumDecl {
    string name;
    unique_ptr<struct ASTNode> values; // Will hold EnumValueList
    EnumDecl(const string& n, unique_ptr<struct ASTNode> v) : name(n), values(move(v)) {}
};

struct CallExpr {
    unique_ptr<struct ASTNode> callee;
    vector<unique_ptr<struct ASTNode>> args;
    CallExpr(unique_ptr<struct ASTNode> c, vector<unique_ptr<struct ASTNode>> a) : callee(move(c)), args(move(a)) {}
};

// === Statement Types ===
struct VarDecl {
    TokenType type;
    string name;
    unique_ptr<struct ASTNode> initializer;
    VarDecl(TokenType t, const string& n, unique_ptr<struct ASTNode> init = nullptr)
        : type(t), name(n), initializer(move(init)) {}
    void printType(TokenType t) const {
        switch (t) {
            case T_INT: cout << "int"; break;
            case T_FLOAT: cout << "float"; break;
            case T_DOUBLE: cout << "double"; break;
            case T_CHAR: cout << "char"; break;
            case T_BOOL: cout << "bool"; break;
            case T_VOID: cout << "void"; break;
            case T_STRING: cout << "string"; break;
            default: cout << "unknown_type"; break;
        }
    }
};

struct BlockStmt {
    vector<unique_ptr<struct ASTNode>> body;
    BlockStmt(vector<unique_ptr<struct ASTNode>> b) : body(move(b)) {}
};

// Add this struct to your header file after FunctionDecl
struct FunctionProto {
    TokenType returnType;
    string name;
    vector<pair<TokenType, string>> params;
    FunctionProto(TokenType rt, const string& n, vector<pair<TokenType, string>> p)
        : returnType(rt), name(n), params(move(p)) {}
    void printType(TokenType t) const {
        switch (t) {
            case T_INT: cout << "int"; break;
            case T_FLOAT: cout << "float"; break;
            case T_DOUBLE: cout << "double"; break;
            case T_CHAR: cout << "char"; break;
            case T_BOOL: cout << "bool"; break;
            case T_VOID: cout << "void"; break;
            case T_STRING: cout << "string"; break;
            default: cout << "unknown_type"; break;
        }
    }
};

struct FunctionDecl {
    TokenType returnType;
    string name;
    vector<pair<TokenType, string>> params;
    vector<unique_ptr<struct ASTNode>> body;
    FunctionDecl(TokenType rt, const string& n,
                 vector<pair<TokenType, string>> p,
                 vector<unique_ptr<struct ASTNode>> b)
        : returnType(rt), name(n), params(move(p)), body(move(b)) {}
    void printType(TokenType t) const {
        switch (t) {
            case T_INT: cout << "int"; break;
            case T_FLOAT: cout << "float"; break;
            case T_DOUBLE: cout << "double"; break;
            case T_CHAR: cout << "char"; break;
            case T_BOOL: cout << "bool"; break;
            case T_VOID: cout << "void"; break;
            case T_STRING: cout << "string"; break;
            default: cout << "unknown_type"; break;
        }
    }
};

struct MainDecl {
    vector<unique_ptr<struct ASTNode>> body;
    MainDecl(vector<unique_ptr<struct ASTNode>> b) : body(move(b)) {}
};

struct IfStmt {
    unique_ptr<struct ASTNode> condition;
    vector<unique_ptr<struct ASTNode>> ifBody;
    vector<unique_ptr<struct ASTNode>> elseBody;
    IfStmt(unique_ptr<struct ASTNode> cond, vector<unique_ptr<struct ASTNode>> ifb, vector<unique_ptr<struct ASTNode>> elseb)
        : condition(move(cond)), ifBody(move(ifb)), elseBody(move(elseb)) {}
};

struct WhileStmt {
    unique_ptr<struct ASTNode> condition;
    vector<unique_ptr<struct ASTNode>> body;
    WhileStmt(unique_ptr<struct ASTNode> cond, vector<unique_ptr<struct ASTNode>> b)
        : condition(move(cond)), body(move(b)) {}
};

struct DoWhileStmt {
    unique_ptr<struct ASTNode> body;
    unique_ptr<struct ASTNode> condition;
    DoWhileStmt(unique_ptr<struct ASTNode> b, unique_ptr<struct ASTNode> c) : body(move(b)), condition(move(c)) {}
};

struct ForStmt {
    unique_ptr<struct ASTNode> init;
    unique_ptr<struct ASTNode> condition;
    unique_ptr<struct ASTNode> update;
    unique_ptr<struct ASTNode> body;
    ForStmt(unique_ptr<struct ASTNode> i, unique_ptr<struct ASTNode> c, unique_ptr<struct ASTNode> u, unique_ptr<struct ASTNode> b)
        : init(move(i)), condition(move(c)), update(move(u)), body(move(b)) {}
};

struct CaseBlock {
    unique_ptr<struct ASTNode> value;
    vector<unique_ptr<struct ASTNode>> body;
    CaseBlock(unique_ptr<struct ASTNode> v, vector<unique_ptr<struct ASTNode>> b) : value(move(v)), body(move(b)) {}
};

struct SwitchStmt {
    unique_ptr<struct ASTNode> expression;
    vector<unique_ptr<struct ASTNode>> cases;
    vector<unique_ptr<struct ASTNode>> defaultBody;
    SwitchStmt(unique_ptr<struct ASTNode> e, vector<unique_ptr<struct ASTNode>> c, vector<unique_ptr<struct ASTNode>> d)
        : expression(move(e)), cases(move(c)), defaultBody(move(d)) {}
};

struct ReturnStmt {
    unique_ptr<struct ASTNode> value;
    ReturnStmt(unique_ptr<struct ASTNode> val = nullptr) : value(move(val)) {}
};

struct PrintStmt {
    vector<unique_ptr<struct ASTNode>> args;
    PrintStmt(vector<unique_ptr<struct ASTNode>> a) : args(move(a)) {}
};

struct ExpressionStmt {
    unique_ptr<struct ASTNode> expr;
    ExpressionStmt(unique_ptr<struct ASTNode> e) : expr(move(e)) {}
};

using ASTNodeVariant = variant<
    IntLiteral,
    FloatLiteral,
    StringLiteral,
    CharLiteral,
    BoolLiteral,
    Identifier,
    BinaryExpr,
    UnaryExpr,
    IncludeStmt,
    EnumValueList,
    EnumDecl,
    CallExpr,
    VarDecl,
    BlockStmt,
    FunctionProto,
    FunctionDecl,
    MainDecl,
    IfStmt,
    WhileStmt,
    DoWhileStmt,
    ForStmt,
    CaseBlock,
    SwitchStmt,
    ReturnStmt,
    PrintStmt,
    ExpressionStmt
>;

struct ASTNode {
    ASTNodeVariant node;
    
    template<typename T>
    ASTNode(T&& t) : node(std::forward<T>(t)) {}
};

using ASTPtr = unique_ptr<ASTNode>;



// ********************************* FUNCTION DECLARATIONS ******************************************

vector<Token> lexAndDumpToFile(const string& inputFilename, const string& outputFilename);
vector<unique_ptr<ASTNode>> parseFromFile(const vector<Token>& tokens);
//vector<ScopeError> performScopeAnalysis(const vector<ASTPtr>& ast, const vector<Token>& tokens)
void performScopeAnalysis(const vector<ASTPtr>& ast, const vector<Token>& tokens);
#endif