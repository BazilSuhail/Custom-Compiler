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
#include <variant>
#include <functional>

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
    T_AND, T_OR, T_NOT,T_STRING,
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
            case T_STRING: return "T_STRING";
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

// struct Identifier {
//     string name;
//     Identifier(const string& n) : name(n) {}
// };
struct Identifier {
    string name;
    int line; // Add line and column for error reporting
    int column;
    Identifier(const string& n, int l = -1, int c = -1) : name(n), line(l), column(c) {} // Default values for backward compatibility if needed elsewhere
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

struct CallExpr {
    unique_ptr<struct ASTNode> callee;
    vector<unique_ptr<struct ASTNode>> args;
    CallExpr(unique_ptr<struct ASTNode> c, vector<unique_ptr<struct ASTNode>> a) : callee(move(c)), args(move(a)) {}
};

// === Statement Types ===
// struct VarDecl {
//     TokenType type;
//     string name;
//     unique_ptr<struct ASTNode> initializer;
//     VarDecl(TokenType t, const string& n, unique_ptr<struct ASTNode> init = nullptr)
//         : type(t), name(n), initializer(move(init)) {}
//     void printType(TokenType t) const {
//         switch (t) {
//             case T_INT: cout << "int"; break;
//             case T_FLOAT: cout << "float"; break;
//             case T_DOUBLE: cout << "double"; break;
//             case T_CHAR: cout << "char"; break;
//             case T_BOOL: cout << "bool"; break;
//             case T_VOID: cout << "void"; break;
//             case T_STRING: cout << "string"; break;
//             default: cout << "unknown_type"; break;
//         }
//     }
// };
struct VarDecl {
    TokenType type;
    string name;
    unique_ptr<struct ASTNode> initializer;
    int line; // Add line and column for error reporting
    int column;
    VarDecl(TokenType t, const string& n, unique_ptr<struct ASTNode> init = nullptr, int l = -1, int c = -1)
        : type(t), name(n), initializer(move(init)), line(l), column(c) {}
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

// struct FunctionDecl {
//     TokenType returnType;
//     string name;
//     vector<pair<TokenType, string>> params;
//     vector<unique_ptr<struct ASTNode>> body;
//     FunctionDecl(TokenType rt, const string& n,
//                  vector<pair<TokenType, string>> p,
//                  vector<unique_ptr<struct ASTNode>> b)
//         : returnType(rt), name(n), params(move(p)), body(move(b)) {}
//     void printType(TokenType t) const {
//         switch (t) {
//             case T_INT: cout << "int"; break;
//             case T_FLOAT: cout << "float"; break;
//             case T_DOUBLE: cout << "double"; break;
//             case T_CHAR: cout << "char"; break;
//             case T_BOOL: cout << "bool"; break;
//             case T_VOID: cout << "void"; break;
//             case T_STRING: cout << "string"; break;
//             default: cout << "unknown_type"; break;
//         }
//     }
// };
struct FunctionDecl {
    TokenType returnType;
    string name;
    vector<pair<TokenType, string>> params;
    vector<unique_ptr<struct ASTNode>> body;
    int line; // Add line and column for error reporting
    int column;
    FunctionDecl(TokenType rt, const string& n,
                 vector<pair<TokenType, string>> p,
                 vector<unique_ptr<struct ASTNode>> b,
                 int l = -1, int c = -1) // Add default values
        : returnType(rt), name(n), params(move(p)), body(move(b)), line(l), column(c) {}
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
    CallExpr,
    VarDecl,
    BlockStmt,
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

// ---------------v------------------ SCOPE ANALYSIS ------------------------------
// ---------------v----------------------------------------------------------------
enum class ScopeErrorType {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition,
};

// Forward declarations for print functions
void printASTNode(const ASTNodeVariant& node, int indent = 0);

void printIntLiteral(const IntLiteral& node, int indent) {
    cout << string(indent, ' ') << "IntLiteral(" << node.value << ")\n";
}

void printFloatLiteral(const FloatLiteral& node, int indent) {
    cout << string(indent, ' ') << "FloatLiteral(" << node.value << ")\n";
}

void printStringLiteral(const StringLiteral& node, int indent) {
    cout << string(indent, ' ') << "StringLiteral(\"" << node.value << "\")\n";
}

void printCharLiteral(const CharLiteral& node, int indent) {
    cout << string(indent, ' ') << "CharLiteral('" << node.value << "')\n";
}

void printBoolLiteral(const BoolLiteral& node, int indent) {
    cout << string(indent, ' ') << "BoolLiteral(" << (node.value ? "true" : "false") << ")\n";
}

void printIdentifier(const Identifier& node, int indent) {
    cout << string(indent, ' ') << "Identifier(\"" << node.name << "\")\n";
}

void printBinaryExpr(const BinaryExpr& node, int indent) {
    cout << string(indent, ' ') << "BinaryExpr(";
    node.printOp(node.op);
    cout << ")\n";
    if (node.left) printASTNode(node.left->node, indent + 2);
    if (node.right) printASTNode(node.right->node, indent + 2);
}

void printUnaryExpr(const UnaryExpr& node, int indent) {
    cout << string(indent, ' ') << "UnaryExpr(";
    if (node.op == T_MINUS) cout << "-";
    else if (node.op == T_NOT) cout << "!";
    else cout << "unary_op";
    cout << ")\n";
    if (node.operand) printASTNode(node.operand->node, indent + 2);
}

void printIncludeStmt(const IncludeStmt& node, int indent) {
    cout << string(indent, ' ') << "IncludeStmt(\"" << node.header << "\")\n";
}

void printCallExpr(const CallExpr& node, int indent) {
    cout << string(indent, ' ') << "CallExpr\n";
    cout << string(indent + 2, ' ') << "Callee:\n";
    if (node.callee) printASTNode(node.callee->node, indent + 4);
    if (!node.args.empty()) {
        cout << string(indent + 2, ' ') << "Args:\n";
        for (const auto& arg : node.args) {
            if (arg) printASTNode(arg->node, indent + 4);
        }
    }
}

void printVarDecl(const VarDecl& node, int indent) {
    cout << string(indent, ' ') << "VarDecl(";
    node.printType(node.type);
    cout << ", \"" << node.name << "\")\n";
    if (node.initializer) printASTNode(node.initializer->node, indent + 2);
}

void printBlockStmt(const BlockStmt& node, int indent) {
    cout << string(indent, ' ') << "BlockStmt\n";
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 2);
    }
}

void printFunctionDecl(const FunctionDecl& node, int indent) {
    cout << string(indent, ' ') << "FunctionDecl(";
    node.printType(node.returnType);
    cout << ", \"" << node.name << "\")\n";
    for (const auto& param : node.params) {
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
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 4);
    }
}

void printMainDecl(const MainDecl& node, int indent) {
    cout << string(indent, ' ') << "MainDecl\n";
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 2);
    }
}

void printIfStmt(const IfStmt& node, int indent) {
    cout << string(indent, ' ') << "IfStmt\n";
    cout << string(indent + 2, ' ') << "Condition:\n";
    if (node.condition) printASTNode(node.condition->node, indent + 4);
    cout << string(indent + 2, ' ') << "IfBody:\n";
    for (const auto& stmt : node.ifBody) {
        if (stmt) printASTNode(stmt->node, indent + 4);
    }
    if (!node.elseBody.empty()) {
        cout << string(indent + 2, ' ') << "ElseBody:\n";
        for (const auto& stmt : node.elseBody) {
            if (stmt) printASTNode(stmt->node, indent + 4);
        }
    }
}

void printWhileStmt(const WhileStmt& node, int indent) {
    cout << string(indent, ' ') << "WhileStmt\n";
    cout << string(indent + 2, ' ') << "Condition:\n";
    if (node.condition) printASTNode(node.condition->node, indent + 4);
    cout << string(indent + 2, ' ') << "Body:\n";
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 4);
    }
}

void printDoWhileStmt(const DoWhileStmt& node, int indent) {
    cout << string(indent, ' ') << "DoWhileStmt\n";
    cout << string(indent+2, ' ') << "Body:\n";
    if (node.body) printASTNode(node.body->node, indent+4);
    cout << string(indent+2, ' ') << "Condition:\n";
    if (node.condition) printASTNode(node.condition->node, indent+4);
}

void printForStmt(const ForStmt& node, int indent) {
    cout << string(indent, ' ') << "ForStmt\n";
    if (node.init) { cout << string(indent+2, ' ') << "Init:\n"; printASTNode(node.init->node, indent+4); }
    if (node.condition) { cout << string(indent+2, ' ') << "Condition:\n"; printASTNode(node.condition->node, indent+4); }
    if (node.update) { cout << string(indent+2, ' ') << "Update:\n"; printASTNode(node.update->node, indent+4); }
    cout << string(indent+2, ' ') << "Body:\n";
    if (node.body) printASTNode(node.body->node, indent+4);
}

void printCaseBlock(const CaseBlock& node, int indent) {
    cout << string(indent, ' ') << "CaseBlock\n";
    cout << string(indent+2, ' ') << "Value:\n";
    if (node.value) printASTNode(node.value->node, indent+4);
    cout << string(indent+2, ' ') << "Body:\n";
    for (auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent+4);
    }
}

void printSwitchStmt(const SwitchStmt& node, int indent) {
    cout << string(indent, ' ') << "SwitchStmt\n";
    cout << string(indent+2, ' ') << "Expression:\n";
    if (node.expression) printASTNode(node.expression->node, indent+4);
    cout << string(indent+2, ' ') << "Cases:\n";
    for (auto& c : node.cases) {
        if (c) printASTNode(c->node, indent+4);
    }
    if (!node.defaultBody.empty()) {
        cout << string(indent+2, ' ') << "Default:\n";
        for (auto& stmt : node.defaultBody) {
            if (stmt) printASTNode(stmt->node, indent+4);
        }
    }
}

void printReturnStmt(const ReturnStmt& node, int indent) {
    cout << string(indent, ' ') << "ReturnStmt\n";
    if (node.value) printASTNode(node.value->node, indent + 2);
}

void printPrintStmt(const PrintStmt& node, int indent) {
    cout << string(indent, ' ') << "PrintStmt\n";
    for (const auto& arg : node.args) {
        if (arg) printASTNode(arg->node, indent + 2);
    }
}

void printExpressionStmt(const ExpressionStmt& node, int indent) {
    cout << string(indent, ' ') << "ExpressionStmt\n";
    if (node.expr) printASTNode(node.expr->node, indent + 2);
}

void printASTNode(const ASTNodeVariant& node, int indent) {
    visit([indent](const auto& n) { 
        using T = std::decay_t<decltype(n)>;
        if constexpr (std::is_same_v<T, IntLiteral>) printIntLiteral(n, indent);
        else if constexpr (std::is_same_v<T, FloatLiteral>) printFloatLiteral(n, indent);
        else if constexpr (std::is_same_v<T, StringLiteral>) printStringLiteral(n, indent);
        else if constexpr (std::is_same_v<T, CharLiteral>) printCharLiteral(n, indent);
        else if constexpr (std::is_same_v<T, BoolLiteral>) printBoolLiteral(n, indent);
        else if constexpr (std::is_same_v<T, Identifier>) printIdentifier(n, indent);
        else if constexpr (std::is_same_v<T, BinaryExpr>) printBinaryExpr(n, indent);
        else if constexpr (std::is_same_v<T, UnaryExpr>) printUnaryExpr(n, indent);
        else if constexpr (std::is_same_v<T, IncludeStmt>) printIncludeStmt(n, indent);
        else if constexpr (std::is_same_v<T, CallExpr>) printCallExpr(n, indent);
        else if constexpr (std::is_same_v<T, VarDecl>) printVarDecl(n, indent);
        else if constexpr (std::is_same_v<T, BlockStmt>) printBlockStmt(n, indent);
        else if constexpr (std::is_same_v<T, FunctionDecl>) printFunctionDecl(n, indent);
        else if constexpr (std::is_same_v<T, MainDecl>) printMainDecl(n, indent);
        else if constexpr (std::is_same_v<T, IfStmt>) printIfStmt(n, indent);
        else if constexpr (std::is_same_v<T, WhileStmt>) printWhileStmt(n, indent);
        else if constexpr (std::is_same_v<T, DoWhileStmt>) printDoWhileStmt(n, indent);
        else if constexpr (std::is_same_v<T, ForStmt>) printForStmt(n, indent);
        else if constexpr (std::is_same_v<T, CaseBlock>) printCaseBlock(n, indent);
        else if constexpr (std::is_same_v<T, SwitchStmt>) printSwitchStmt(n, indent);
        else if constexpr (std::is_same_v<T, ReturnStmt>) printReturnStmt(n, indent);
        else if constexpr (std::is_same_v<T, PrintStmt>) printPrintStmt(n, indent);
        else if constexpr (std::is_same_v<T, ExpressionStmt>) printExpressionStmt(n, indent);
    }, node);
}

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
               type == T_CHAR || type == T_BOOL || type == T_VOID  || type == T_STRING;
    }

    bool isDeclarationStart(TokenType type) const {
        return isTypeToken(type) || type == T_MAIN || type == T_PRINT;
    }

    // Small utility to check if ASTPtr is Identifier
    bool isIdentifierNode(const ASTPtr& node) const {
        if (!node) return false;
        return holds_alternative<Identifier>(node->node);
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
        return make_unique<ASTNode>(IntLiteral(stoi(t.value)));
    }
    ASTPtr makeFloatLiteralFromToken(const Token& t) {
        return make_unique<ASTNode>(FloatLiteral(stod(t.value)));
    }
    ASTPtr makeStringLiteralFromToken(const Token& t) {
        string s = t.value;
        if (s.length() >= 2 && s.front() == '"' && s.back() == '"') s = s.substr(1, s.length() - 2);
        return make_unique<ASTNode>(StringLiteral(s));
    }
    ASTPtr makeCharLiteralFromToken(const Token& t) {
        char c = '\0';
        if (t.value.size() >= 3 && t.value.front() == '\'' && t.value.back() == '\'') {
            c = t.value[1];
        } else if (!t.value.empty()) {
            c = t.value[0];
        }
        return make_unique<ASTNode>(CharLiteral(c));
    }
    ASTPtr makeBoolLiteralFromToken(const Token& t) {
        return make_unique<ASTNode>(BoolLiteral(t.value == "true"));
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

    // ASTPtr parseIdentifier() {
    //     Token t = currentToken;
    //     advance();
    //     return make_unique<ASTNode>(Identifier(t.value));
    // }

    ASTPtr parseIdentifier() {
        Token t = currentToken;
        advance();
        return make_unique<ASTNode>(Identifier(t.value, t.line, t.column)); // Pass line/column to Identifier
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
        return make_unique<ASTNode>(UnaryExpr(op.type, move(right)));
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

        return make_unique<ASTNode>(BinaryExpr(op.type, move(left), move(right)));
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
        return make_unique<ASTNode>(CallExpr(move(callee), move(args)));
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
            return make_unique<ASTNode>(Identifier("break")); // simple placeholder for BreakStmt
        }

        ASTPtr expr = parseExpression();
        consumeSemicolon();
        return make_unique<ASTNode>(ExpressionStmt(move(expr)));
    }

    // ASTPtr parseVariableDeclaration() {
    //     TokenType type = currentToken.type;
    //     advance();

    //     Token identToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
    //     string name = identToken.value;

    //     ASTPtr initializer = nullptr;
    //     if (match(T_ASSIGNOP)) initializer = parseExpression();

    //     consumeSemicolon();
    //     return make_unique<ASTNode>(VarDecl(type, name, move(initializer)));
    // }
    ASTPtr parseVariableDeclaration() {
        TokenType type = currentToken.type;
        advance();
        Token identToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
        string name = identToken.value;
        ASTPtr initializer = nullptr;
        if (match(T_ASSIGNOP)) initializer = parseExpression();
        consumeSemicolon();
        return make_unique<ASTNode>(VarDecl(type, name, move(initializer), identToken.line, identToken.column)); // Pass line/column to VarDecl
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
        return make_unique<ASTNode>(PrintStmt(move(args)));
    }

    ASTPtr parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        ASTPtr condition = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> ifBody = parseBlock();

        vector<ASTPtr> elseBody;
        if (match(T_ELSE)) elseBody = parseBlock();

        return make_unique<ASTNode>(IfStmt(move(condition), move(ifBody), move(elseBody)));
    }

    ASTPtr parseWhileStatement() {
        expect(T_WHILE);
        expect(T_LPAREN);
        ASTPtr condition = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> body = parseBlock();
        return make_unique<ASTNode>(WhileStmt(move(condition), move(body)));
    }

    // Replace the do-while method
    ASTPtr parseDoWhileStatement() {
        vector<ASTPtr> bodyVec = parseBlock(); // Parse as vector first
        ASTPtr body = make_unique<ASTNode>(BlockStmt(move(bodyVec))); // Wrap in BlockStmt
        expect(T_WHILE);
        expect(T_LPAREN);
        ASTPtr cond = parseExpression();
        expect(T_RPAREN);
        expect(T_SEMICOLON);
        return make_unique<ASTNode>(DoWhileStmt(move(body), move(cond)));
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
                init = make_unique<ASTNode>(VarDecl(type, name, move(initializer)));
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
        ASTPtr body = make_unique<ASTNode>(BlockStmt(move(bodyVec))); // Wrap in BlockStmt
        return make_unique<ASTNode>(ForStmt(move(init), move(cond), move(update), move(body)));
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
                cases.push_back(make_unique<ASTNode>(CaseBlock(move(val), move(caseBody))));
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

        return make_unique<ASTNode>(SwitchStmt(move(expr), move(cases), move(defaultBody)));
    }

    ASTPtr parseReturnStatement() {
        expect(T_RETURN);
        ASTPtr value = nullptr;
        if (!check(T_SEMICOLON) && !check(T_RBRACE)) value = parseExpression();
        consumeSemicolon();
        return make_unique<ASTNode>(ReturnStmt(move(value)));
    }

    ASTPtr parseBlockStatement() {
        vector<ASTPtr> body = parseBlock();
        return make_unique<ASTNode>(BlockStmt(move(body)));
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
        return make_unique<ASTNode>(MainDecl(move(body)));
    }

    // ASTPtr parseFunctionDeclaration() {
    //     TokenType returnType = currentToken.type;
    //     advance();

    //     Token nameToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
    //     string name = nameToken.value;

    //     expect(T_LPAREN);
    //     vector<pair<TokenType, string>> params;
    //     if (!check(T_RPAREN)) {
    //         do {
    //             TokenType paramType = currentToken.type;
    //             advance();
    //             Token paramName = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
    //             params.push_back({paramType, paramName.value});
    //         } while (match(T_COMMA));
    //     }
    //     expect(T_RPAREN);

    //     vector<ASTPtr> body = parseBlock();
    //     return make_unique<ASTNode>(FunctionDecl(returnType, name, move(params), move(body)));
    // }
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
        return make_unique<ASTNode>(FunctionDecl(returnType, name, move(params), move(body), nameToken.line, nameToken.column)); // Pass line/column to FunctionDecl
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
        return make_unique<ASTNode>(IncludeStmt("main"));
    }

    // include "header"
    if (check(T_STRINGLIT)) {
        Token headerTok = currentToken;
        advance();
        string header = headerTok.value;
        if (header.size() >= 2 && header.front() == '"' && header.back() == '"') {
            header = header.substr(1, header.size() - 2);
        }
        return make_unique<ASTNode>(IncludeStmt(header));
    }

    // include identifier fallback
    if (check(T_IDENTIFIER)) {
        Token headerTok = currentToken;
        advance();
        return make_unique<ASTNode>(IncludeStmt(headerTok.value));
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

struct ScopeError {
    ScopeErrorType type;
    string name; // The name causing the error (e.g., undefined variable name)
    int line;    // The line number where the error occurred
    int column;  // The column number where the error occurred
    string message; // Human-readable message

    ScopeError(ScopeErrorType t, const string& n, int l, int c) : type(t), name(n), line(l), column(c) {
        switch (t) {
            case ScopeErrorType::UndeclaredVariableAccessed:
                message = "Undeclared variable accessed: '" + name + "'";
                break;
            case ScopeErrorType::UndefinedFunctionCalled:
                message = "Undefined function called: '" + name + "'";
                break;
            case ScopeErrorType::VariableRedefinition:
                message = "Variable redefinition: '" + name + "'";
                break;
            case ScopeErrorType::FunctionPrototypeRedefinition:
                message = "Function redefinition: '" + name + "'";
                break;
        }
    }
};

class ScopeAnalyzer {
private:
    // --- Spaghetti Stack ---
    // Use a struct to represent items on the stack
    struct StackItem {
        string name;
        bool isVariable; // true for variable, false for function
        int line;        // line where declared
        int column;      // column where declared
        StackItem(const string& n, bool v, int l, int c) : name(n), isVariable(v), line(l), column(c) {}
    };
    vector<variant<StackItem, nullptr_t>> stack; // nullptr acts as a scope marker

    // Helper to check current scope for redefinition
    bool isDeclaredInCurrentScope(const string& name) {
        for (int i = stack.size() - 1; i >= 0; --i) {
            if (stack[i].index() == 1) { // Found a nullptr marker
                break; // Stop searching in the current scope
            }
            auto& item = get<StackItem>(stack[i]);
            if (item.name == name) {
                return true;
            }
        }
        return false;
    }

    // Helper to check global scope for function redefinition
    bool isFunctionDefinedGlobally(const string& name) {
        for (int i = 0; i < stack.size(); ++i) {
            if (stack[i].index() == 1) { // Found a nullptr marker
                continue; // Skip markers, they don't count as definitions
            }
            auto& item = get<StackItem>(stack[i]);
            if (item.name == name && !item.isVariable) { // Found a function with the same name
                return true;
            }
        }
        return false;
    }

    // --- Visitor Methods (Recursive AST Traversal) ---
    void visitProgram(const vector<ASTPtr>& program) {
        // Global scope is entered implicitly at the start
        // Process include statements if needed (they don't add names to our scope stack)
        for (size_t i = 1; i < program.size(); ++i) { // Start from 1, skipping include
            if (holds_alternative<FunctionDecl>(program[i]->node)) {
                visitFunctionDecl(get<FunctionDecl>(program[i]->node));
            } else if (holds_alternative<MainDecl>(program[i]->node)) {
                visitMainDecl(get<MainDecl>(program[i]->node));
            } else {
                // Handle other top-level declarations/statements if any (e.g., global vars)
                // For now, assume only functions and main are top-level after include
            }
        }
    }

    void visitNode(const ASTNodeVariant& node) {
        visit([this](const auto& n) {
            using T = std::decay_t<decltype(n)>;
            if constexpr (std::is_same_v<T, IntLiteral> || std::is_same_v<T, FloatLiteral> ||
                          std::is_same_v<T, StringLiteral> || std::is_same_v<T, CharLiteral> ||
                          std::is_same_v<T, BoolLiteral>) {
                // Literals do not require scope analysis
            }
            else if constexpr (std::is_same_v<T, Identifier>) {
                visitIdentifier(n);
            }
            else if constexpr (std::is_same_v<T, BinaryExpr>) {
                visitBinaryExpr(n);
            }
            else if constexpr (std::is_same_v<T, UnaryExpr>) {
                visitUnaryExpr(n);
            }
            else if constexpr (std::is_same_v<T, CallExpr>) {
                visitCallExpr(n);
            }
            else if constexpr (std::is_same_v<T, VarDecl>) {
                visitVarDecl(n);
            }
            else if constexpr (std::is_same_v<T, BlockStmt>) {
                visitBlock(n);
            }
            else if constexpr (std::is_same_v<T, FunctionDecl>) {
                visitFunctionDecl(n);
            }
            else if constexpr (std::is_same_v<T, MainDecl>) {
                visitMainDecl(n);
            }
            else if constexpr (std::is_same_v<T, IfStmt>) {
                visitIfStmt(n);
            }
            else if constexpr (std::is_same_v<T, WhileStmt>) {
                visitWhileStmt(n);
            }
            else if constexpr (std::is_same_v<T, DoWhileStmt>) {
                visitDoWhileStmt(n);
            }
            else if constexpr (std::is_same_v<T, ForStmt>) {
                visitForStmt(n);
            }
            else if constexpr (std::is_same_v<T, SwitchStmt>) {
                visitSwitchStmt(n);
            }
            else if constexpr (std::is_same_v<T, ReturnStmt>) {
                visitReturnStmt(n);
            }
            else if constexpr (std::is_same_v<T, PrintStmt>) {
                visitPrintStmt(n);
            }
            else if constexpr (std::is_same_v<T, ExpressionStmt>) {
                visitExpressionStmt(n);
            }
            else if constexpr (std::is_same_v<T, IncludeStmt>) {
                // Include statements don't affect local scopes directly for this analysis
            }
            else if constexpr (std::is_same_v<T, CaseBlock>) {
                // Case blocks are handled within SwitchStmt
            }
        }, node);
    }

    void visitIdentifier(const Identifier& id) {
        // Search the stack for the name
        for (int i = stack.size() - 1; i >= 0; --i) {
            if (stack[i].index() == 1) { // Found a nullptr marker
                continue; // Skip the marker itself
            }
            auto& item = get<StackItem>(stack[i]);
            if (item.name == id.name) {
                // Found the identifier
                return; // Success, found within scope
            }
        }
        // If loop finishes without returning, the identifier was not found
        throw ScopeError(ScopeErrorType::UndeclaredVariableAccessed, id.name, id.line, id.column);
    }

    void visitBinaryExpr(const BinaryExpr& expr) {
        if (expr.left) visitNode(expr.left->node);
        if (expr.right) visitNode(expr.right->node);
    }

    void visitUnaryExpr(const UnaryExpr& expr) {
        if (expr.operand) visitNode(expr.operand->node);
    }

    void visitCallExpr(const CallExpr& call) {
        // The callee should be an identifier (as per parser logic)
        if (holds_alternative<Identifier>(call.callee->node)) {
            const auto& calleeId = get<Identifier>(call.callee->node);
            // Check if the function being called is defined globally
            bool found = false;
            for (int i = 0; i < stack.size(); ++i) {
                if (stack[i].index() == 1) { // Found a nullptr marker (end of global scope items)
                    continue; // Continue searching in global scope
                }
                auto& item = get<StackItem>(stack[i]);
                if (item.name == calleeId.name && !item.isVariable) { // Found a function
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw ScopeError(ScopeErrorType::UndefinedFunctionCalled, calleeId.name, calleeId.line, calleeId.column);
            }
        } else {
            // If callee is not an identifier, it's likely an error from the parser
            // or a more complex expression, which shouldn't happen per the grammar.
            // For robustness, we could visit the callee expression anyway.
            if (call.callee) visitNode(call.callee->node);
        }
        // Visit arguments
        for (const auto& arg : call.args) {
            if (arg) visitNode(arg->node);
        }
    }

    void visitVarDecl(const VarDecl& decl) {
        // Check for redefinition in the *current* scope (between top of stack and last marker)
        if (isDeclaredInCurrentScope(decl.name)) {
            throw ScopeError(ScopeErrorType::VariableRedefinition, decl.name, decl.line, decl.column); // Note: line/col might need adjustment if VarDecl doesn't have them
        }
        // Add the variable to the stack
        stack.emplace_back(StackItem(decl.name, true, decl.line, decl.column)); // Note: line/col might need adjustment

        // Visit the initializer expression (if present) *after* declaring the variable
        // This allows the initializer to potentially reference the variable being declared (e.g., x = x + 1; is valid in some contexts, but not here as x is uninitialized)
        // For this analysis, we assume the variable is declared before its initializer is checked.
        if (decl.initializer) {
            visitNode(decl.initializer->node);
        }
    }

    void visitBlock(const BlockStmt& block) {
        enterScope();
        for (const auto& stmt : block.body) {
            if (stmt) visitNode(stmt->node);
        }
        exitScope();
    }

    void visitFunctionDecl(const FunctionDecl& decl) {
        // Check for function redefinition in the *global* scope
        if (isFunctionDefinedGlobally(decl.name)) {
             throw ScopeError(ScopeErrorType::FunctionPrototypeRedefinition, decl.name, decl.line, decl.column); // Note: line/col might need adjustment
        }
        // Add the function to the *global* scope stack (top level, before any scope markers)
        stack.emplace_back(StackItem(decl.name, false, decl.line, decl.column)); // isVariable = false, line/col might need adjustment

        enterScope(); // Enter function's local scope

        // Add parameters to the function's local scope
        for (const auto& param : decl.params) {
            const string& paramName = param.second;
            int paramLine = -1; // We don't have exact line/column for params from parser, use -1 or find a way to pass it
            int paramCol = -1;
            if (isDeclaredInCurrentScope(paramName)) {
                 throw ScopeError(ScopeErrorType::VariableRedefinition, paramName, paramLine, paramCol);
            }
            stack.emplace_back(StackItem(paramName, true, paramLine, paramCol)); // isVariable = true
        }

        // Visit the function body statements
        for (const auto& stmt : decl.body) {
            if (stmt) visitNode(stmt->node);
        }

        exitScope(); // Exit function's local scope
    }

    void visitMainDecl(const MainDecl& decl) {
        enterScope(); // Enter main's local scope
        for (const auto& stmt : decl.body) {
            if (stmt) visitNode(stmt->node);
        }
        exitScope(); // Exit main's local scope
    }

    void visitIfStmt(const IfStmt& stmt) {
        if (stmt.condition) visitNode(stmt.condition->node);

        enterScope(); // Enter if-block scope
        for (const auto& s : stmt.ifBody) {
            if (s) visitNode(s->node);
        }
        exitScope(); // Exit if-block scope

        if (!stmt.elseBody.empty()) {
            enterScope(); // Enter else-block scope
            for (const auto& s : stmt.elseBody) {
                if (s) visitNode(s->node);
            }
            exitScope(); // Exit else-block scope
        }
    }

    void visitWhileStmt(const WhileStmt& stmt) {
        if (stmt.condition) visitNode(stmt.condition->node);

        enterScope(); // Enter while-block scope
        for (const auto& s : stmt.body) {
            if (s) visitNode(s->node);
        }
        exitScope(); // Exit while-block scope
    }

    void visitDoWhileStmt(const DoWhileStmt& stmt) {
        enterScope(); // Enter do-while-block scope
        if (stmt.body) visitNode(stmt.body->node);
        exitScope(); // Exit do-while-block scope

        if (stmt.condition) visitNode(stmt.condition->node); // Condition is evaluated after the block
    }

    void visitForStmt(const ForStmt& stmt) {
        enterScope(); // Enter for-loop scope

        // Visit init (VarDecl or ExpressionStmt)
        if (stmt.init) visitNode(stmt.init->node);

        // Visit condition
        if (stmt.condition) visitNode(stmt.condition->node);

        // Visit update
        if (stmt.update) visitNode(stmt.update->node);

        // Visit body
        if (stmt.body) visitNode(stmt.body->node);

        exitScope(); // Exit for-loop scope
    }

    void visitSwitchStmt(const SwitchStmt& stmt) {
        if (stmt.expression) visitNode(stmt.expression->node);

        for (const auto& caseBlockNode : stmt.cases) {
            if (holds_alternative<CaseBlock>(caseBlockNode->node)) {
                const auto& caseBlock = get<CaseBlock>(caseBlockNode->node);
                if (caseBlock.value) visitNode(caseBlock.value->node); // Visit the case value expression

                enterScope(); // Enter case block scope (as per parser's AST structure)
                for (const auto& s : caseBlock.body) {
                    if (s) visitNode(s->node);
                }
                exitScope(); // Exit case block scope
            }
        }

        // Visit default block if present
        if (!stmt.defaultBody.empty()) {
            enterScope(); // Enter default block scope
            for (const auto& s : stmt.defaultBody) {
                if (s) visitNode(s->node);
            }
            exitScope(); // Exit default block scope
        }
    }

    void visitReturnStmt(const ReturnStmt& stmt) {
        if (stmt.value) visitNode(stmt.value->node);
    }

    void visitPrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) {
            if (arg) visitNode(arg->node);
        }
    }

    void visitExpressionStmt(const ExpressionStmt& stmt) {
        if (stmt.expr) visitNode(stmt.expr->node);
    }

    // Helper to enter/exit scopes
    void enterScope() {
        stack.push_back(nullptr); // Push a marker for the new scope
    }

    void exitScope() {
        while (!stack.empty() && stack.back().index() != 1) { // While top is not a nullptr marker
            stack.pop_back(); // Pop items until we find the marker
        }
        if (!stack.empty()) { // Pop the marker itself
            stack.pop_back();
        }
    }

public:
    void analyze(const vector<ASTPtr>& ast) {
        visitProgram(ast);
    }
};


// ==========================================================
// Map string names in file -> enum TokenType
TokenType tokenTypeFromString(const string& s) {
    static unordered_map<string, TokenType> mapping = {
        {"T_INT", T_INT}, {"T_FLOAT", T_FLOAT}, {"T_DOUBLE", T_DOUBLE},
        {"T_CHAR", T_CHAR}, {"T_VOID", T_VOID}, {"T_BOOL", T_BOOL},{"T_STRING", T_STRING},
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
        for (const auto& node : ast) {
            if (node) printASTNode(node->node);
        }
        cout << "\n=== Parsing Successful ===\n";

        // --- Add Scope Analysis Here ---
        ScopeAnalyzer scopeAnalyzer;
        scopeAnalyzer.analyze(ast); // <-- Pass the AST here
        cout << "=== Scope Analysis Successful ===";

    } // <-- This is the closing brace of the try block, place the scope analyzer call just before the first catch
    catch (const ParseError& e) {
        cout << "Parse Error: " << e.message
             << " at line " << e.token.line
             << ", column " << e.token.column << endl;
    } catch (const ScopeError& e) { // Catch the new scope errors
        cout << "Scope Error: " << e.message
             << " at line " << e.line
             << ", column " << e.column << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}