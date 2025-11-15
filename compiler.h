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

#include <stack>
#include <set>

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

    // Bitwise operators
    T_BITAND, T_BITOR, T_BITXOR, T_BITLSHIFT, T_BITRSHIFT,

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

// === AST Node Types (using std::variant) ===
struct IntLiteral {
    int value;
    int line, column;
    IntLiteral(int v, int l = -1, int c = -1) : value(v), line(l), column(c) {}
};

struct FloatLiteral {
    double value;
    int line, column;
    FloatLiteral(double v, int l = -1, int c = -1) : value(v), line(l), column(c) {}
};

struct StringLiteral {
    string value;
    int line, column;
    StringLiteral(const string& v, int l = -1, int c = -1) : value(v), line(l), column(c) {}
};

struct CharLiteral {
    char value;
    int line, column;
    CharLiteral(char v, int l = -1, int c = -1) : value(v), line(l), column(c) {}
};

struct BoolLiteral {
    bool value;
    int line, column;
    BoolLiteral(bool v, int l = -1, int c = -1) : value(v), line(l), column(c) {}
};

struct Identifier {
    string name;
    int line, column;
    Identifier(const string& n, int l = -1, int c = -1) : name(n), line(l), column(c) {}
};

// === Expression Types ===
struct BinaryExpr {
    TokenType op;
    unique_ptr<struct ASTNode> left;
    unique_ptr<struct ASTNode> right;
    int line, column;
    BinaryExpr(TokenType o, unique_ptr<struct ASTNode> l, unique_ptr<struct ASTNode> r, int l_pos = -1, int c_pos = -1) 
        : op(o), left(move(l)), right(move(r)), line(l_pos), column(c_pos) {}
    void printOp(TokenType t) const {
        switch (t) {
            case T_PLUS: cout << "+"; break;
            case T_MINUS: cout << "-"; break;
            case T_MULTIPLY: cout << "*"; break;
            case T_DIVIDE: cout << "/"; break;
            case T_MODULO: cout << "%"; break;
                
            case T_NOT: cout << "!";
            case T_INCREMENT: cout << "++"; break;
            case T_DECREMENT: cout << "--"; break;

            case T_EQUALOP: cout << "=="; break;
            case T_NE: cout << "!="; break;
            case T_LT: cout << "<"; break;
            case T_GT: cout << ">"; break;
            case T_LE: cout << "<="; break;
            case T_GE: cout << ">="; break;
            case T_AND: cout << "&&"; break;
            case T_OR: cout << "||"; break;
            case T_ASSIGNOP: cout << "="; break;
            case T_BITAND: cout << "&"; break;
            case T_BITOR: cout << "|"; break;
            case T_BITXOR: cout << "^"; break;
            case T_BITLSHIFT: cout << "<<"; break;
            case T_BITRSHIFT: cout << ">>"; break;
            default: cout << "op_" << (int)t; break;
        }
    }
};

// struct UnaryExpr {
//     TokenType op;
//     unique_ptr<struct ASTNode> operand;
//     int line, column;
//     UnaryExpr(TokenType o, unique_ptr<struct ASTNode> opd, int l = -1, int c = -1) 
//         : op(o), operand(move(opd)), line(l), column(c) {}
// };
struct UnaryExpr {
    TokenType op;
    unique_ptr<struct ASTNode> operand;
    bool isPostfix;  // ADD THIS
    int line, column;
    
    // ADD isPostfix parameter with default false
    UnaryExpr(TokenType o, unique_ptr<struct ASTNode> opd, int l = -1, int c = -1, bool postfix = false) 
        : op(o), operand(move(opd)), isPostfix(postfix), line(l), column(c) {}
};


// ============= language syntax
struct IncludeStmt {
    string header;
    int line, column;
    IncludeStmt(string h, int l = -1, int c = -1) : header(move(h)), line(l), column(c) {}
};

// Enum
struct EnumValueList {
    vector<string> values;
    int line, column;
    EnumValueList(vector<string> v, int l = -1, int c = -1) : values(move(v)), line(l), column(c) {}
};

struct EnumDecl {
    string name;
    unique_ptr<struct ASTNode> values; // Will hold EnumValueList
    int line, column;
    EnumDecl(const string& n, unique_ptr<struct ASTNode> v, int l = -1, int c = -1) 
        : name(n), values(move(v)), line(l), column(c) {}
};

struct CallExpr {
    unique_ptr<struct ASTNode> callee;
    vector<unique_ptr<struct ASTNode>> args;
    int line, column;
    CallExpr(unique_ptr<struct ASTNode> c, vector<unique_ptr<struct ASTNode>> a, int l = -1, int col = -1) 
        : callee(move(c)), args(move(a)), line(l), column(col) {}
};

// === Statement Types ===
struct VarDecl {
    TokenType type;
    string name;
    unique_ptr<struct ASTNode> initializer;
    int line, column;
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
    int line, column;
    BlockStmt(vector<unique_ptr<struct ASTNode>> b, int l = -1, int c = -1) : body(move(b)), line(l), column(c) {}
};

// Add this struct to your header file after FunctionDecl
struct FunctionProto {
    TokenType returnType;
    string name;
    vector<pair<TokenType, string>> params;
    int line, column;
    FunctionProto(TokenType rt, const string& n, vector<pair<TokenType, string>> p, int l = -1, int c = -1)
        : returnType(rt), name(n), params(move(p)), line(l), column(c) {}
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
    int line, column;
    FunctionDecl(TokenType rt, const string& n,
                 vector<pair<TokenType, string>> p,
                 vector<unique_ptr<struct ASTNode>> b, int l = -1, int c = -1)
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
    int line, column;
    MainDecl(vector<unique_ptr<struct ASTNode>> b, int l = -1, int c = -1) : body(move(b)), line(l), column(c) {}
};

struct IfStmt {
    unique_ptr<struct ASTNode> condition;
    vector<unique_ptr<struct ASTNode>> ifBody;
    vector<unique_ptr<struct ASTNode>> elseBody;
    int line, column;
    IfStmt(unique_ptr<struct ASTNode> cond, vector<unique_ptr<struct ASTNode>> ifb, vector<unique_ptr<struct ASTNode>> elseb, int l = -1, int c = -1)
        : condition(move(cond)), ifBody(move(ifb)), elseBody(move(elseb)), line(l), column(c) {}
};

struct WhileStmt {
    unique_ptr<struct ASTNode> condition;
    vector<unique_ptr<struct ASTNode>> body;
    int line, column;
    WhileStmt(unique_ptr<struct ASTNode> cond, vector<unique_ptr<struct ASTNode>> b, int l = -1, int c = -1)
        : condition(move(cond)), body(move(b)), line(l), column(c) {}
};

struct DoWhileStmt {
    unique_ptr<struct ASTNode> body;
    unique_ptr<struct ASTNode> condition;
    int line, column;
    DoWhileStmt(unique_ptr<struct ASTNode> b, unique_ptr<struct ASTNode> c, int l = -1, int col = -1) 
        : body(move(b)), condition(move(c)), line(l), column(col) {}
};

struct ForStmt {
    unique_ptr<struct ASTNode> init;
    unique_ptr<struct ASTNode> condition;
    unique_ptr<struct ASTNode> update;
    unique_ptr<struct ASTNode> body;
    int line, column;
    ForStmt(unique_ptr<struct ASTNode> i, unique_ptr<struct ASTNode> c, unique_ptr<struct ASTNode> u, unique_ptr<struct ASTNode> b, int l = -1, int col = -1)
        : init(move(i)), condition(move(c)), update(move(u)), body(move(b)), line(l), column(col) {}
};

struct CaseBlock {
    unique_ptr<struct ASTNode> value;
    vector<unique_ptr<struct ASTNode>> body;
    int line, column;
    CaseBlock(unique_ptr<struct ASTNode> v, vector<unique_ptr<struct ASTNode>> b, int l = -1, int c = -1) 
        : value(move(v)), body(move(b)), line(l), column(c) {}
};

struct SwitchStmt {
    unique_ptr<struct ASTNode> expression;
    vector<unique_ptr<struct ASTNode>> cases;
    vector<unique_ptr<struct ASTNode>> defaultBody;
    int line, column;
    SwitchStmt(unique_ptr<struct ASTNode> e, vector<unique_ptr<struct ASTNode>> c, vector<unique_ptr<struct ASTNode>> d, int l = -1, int col = -1)
        : expression(move(e)), cases(move(c)), defaultBody(move(d)), line(l), column(col) {}
};

struct ReturnStmt {
    unique_ptr<struct ASTNode> value;
    int line, column;
    ReturnStmt(unique_ptr<struct ASTNode> val = nullptr, int l = -1, int c = -1) : value(move(val)), line(l), column(c) {}
};

// struct BreakStmt {
//     BreakStmt() = default;
// };

struct BreakStmt {
    int line, column;
    BreakStmt(int l = -1, int c = -1) : line(l), column(c) {}
};

struct PrintStmt {
    vector<unique_ptr<struct ASTNode>> args;
    int line, column;
    PrintStmt(vector<unique_ptr<struct ASTNode>> a, int l = -1, int c = -1) : args(move(a)), line(l), column(c) {}
};

struct ExpressionStmt {
    unique_ptr<struct ASTNode> expr;
    int line, column;
    ExpressionStmt(unique_ptr<struct ASTNode> e, int l = -1, int c = -1) : expr(move(e)), line(l), column(c) {}
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
    BreakStmt, // valid in statements
    ExpressionStmt
>;

struct ASTNode {
    ASTNodeVariant node;
    
    template<typename T>
    ASTNode(T&& t) : node(std::forward<T>(t)) {}
};

using ASTPtr = unique_ptr<ASTNode>;

// === Scope Analysis Types ===
struct SymbolInfo {
    TokenType type;
    string name;
    int line;
    int column;
    bool isFunction;
    bool isEnum;
    bool isEnumValue;
    bool isPrototype; // Distinguishes prototypes from definitions
    vector<pair<TokenType, string>> params;
    
    SymbolInfo(TokenType t, const string& n, int l, int c, bool isFunc = false, bool isEnumSym = false, bool isEnumVal = false, bool isProto = false, vector<pair<TokenType, string>> p = {})
        : type(t), name(n), line(l), column(c), isFunction(isFunc), isEnum(isEnumSym), isEnumValue(isEnumVal), isPrototype(isProto), params(p) {}
};

struct ScopeFrame {
    unordered_map<string, SymbolInfo> symbols;
    vector<unique_ptr<ScopeFrame>> children;
    ScopeFrame* parent;
    int level; // For shadowing detection
    
    ScopeFrame(ScopeFrame* p = nullptr, int l = 0) : parent(p), level(l) {}
    
    bool hasSymbol(const string& name) const {
        return symbols.find(name) != symbols.end();
    }
    
    SymbolInfo* findSymbol(const string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return &(it->second);
        }
        return nullptr;
    }
    
    const SymbolInfo* findSymbol(const string& name) const {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return &(it->second);
        }
        return nullptr;
    }
    
    void addSymbol(const SymbolInfo& sym) {
        symbols.insert({sym.name, sym});
    }
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
        : type(t), name(n), line(l), column(c) 
    {
        static const unordered_map<ScopeErrorType, string> texts = {
            { ScopeErrorType::UndeclaredVariableAccessed, "Undeclared variable accessed" },
            { ScopeErrorType::UndefinedFunctionCalled, "Undefined function called" },
            { ScopeErrorType::VariableRedefinition, "Variable redefinition" },
            { ScopeErrorType::FunctionPrototypeRedefinition, "Function prototype redefinition" },
            { ScopeErrorType::ConflictingFunctionDefinition, "Conflicting function definition" },
            { ScopeErrorType::ConflictingDeclaration, "Conflicting declaration" },
            { ScopeErrorType::ParameterRedefinition, "Parameter redefinition" },
            { ScopeErrorType::InvalidForwardReference, "Invalid forward reference" },
            { ScopeErrorType::InvalidStorageClassUsage, "Invalid storage class usage for" },
            { ScopeErrorType::EnumRedefinition, "Enum redefinition" },
            { ScopeErrorType::EnumVariantRedefinition, "Enum variant redefinition" }
        };

        auto it = texts.find(t);
        if (it != texts.end()) {
            message = it->second + ": '" + n + "' at - line: " + to_string(l)
                      + " column: " + to_string(c);
        }
    }
};

// === Type Checking Errors ===
enum TypeChkError {
    ErroneousVarDecl,            // Variable declared incorrectly or with invalid type
    FnCallParamCount,            // Function called with wrong number of arguments
    FnCallParamType,             // Function called with argument of wrong type
    ErroneousReturnType,         // Function returns a value of wrong type
    ExpressionTypeMismatch,      // Expression uses incompatible types
    ExpectedBooleanExpression,   // Expression expected to be boolean but isn't
    ErroneousBreak,              // 'break' used outside loop or switch
    NonBooleanCondStmt,          // Condition in if/while is not boolean 
    AttemptedBoolOpOnNonBools,   // Boolean operation used on non-boolean values
    AttemptedBitOpOnNonInt,      // Bitwise operation used on non-integer types
    AttemptedShiftOnNonInt,      // Shift operator used on non-integer type
    AttemptedAddOpOnNonNumeric,  // Addition/subtraction used on non-numeric types
    AttemptedExponentiationOfNonNumeric, // Exponentiation used on non-numeric types (Not in grammar, handled as Mismatch)
    ReturnStmtNotFound,          // Function missing a return statement where required (Not implemented, requires control flow analysis)
    // Bonus
    ReturnStmtInVoid,            // Return statement with value in void function (Not implemented, requires control flow analysis)
    InvalidSwitchConditionType,  // Switch expression does not have a integer or char
    InvalidCaseValueType,         // Case value type doesn't match switch expression
    IncrementDecrementOnNonInt,   // ++ or -- applied to a non-integer type
    NotOnNonBool               // ! applied to a non-boolean type
};

struct TypeCheckError {
    TypeChkError type;
    string name; // Optional, for errors related to specific identifiers
    int line;
    int column;
    string message;

    TypeCheckError(TypeChkError t, int l, int c, const string& n = "")
        : type(t), name(n), line(l), column(c) {
        static const unordered_map<TypeChkError, string> texts = {
            { ErroneousVarDecl, "Erroneous variable declaration" },
            { FnCallParamCount, "Function call parameter count mismatch" },
            { FnCallParamType, "Function call parameter type mismatch" },
            { ErroneousReturnType, "Function return type mismatch" },
            { ExpressionTypeMismatch, "Expression type mismatch" },
            { ExpectedBooleanExpression, "Expected boolean expression" },
            { ErroneousBreak, "Break statement outside loop or switch" },
            { NonBooleanCondStmt, "Non-boolean condition in statement" },
            { AttemptedBoolOpOnNonBools, "Attempted boolean operation on non-boolean types" },
            { AttemptedBitOpOnNonInt, "Attempted bitwise operation on non-integer type" },
            { AttemptedShiftOnNonInt, "Attempted shift operation on non-integer type" },
            { AttemptedAddOpOnNonNumeric, "Attempted addition/subtraction on non-numeric type" },
            { ReturnStmtNotFound, "Return statement not found where required" },
            // new ones
            { ReturnStmtInVoid, "Return statement with value in void function" },
            { InvalidSwitchConditionType, "Switch expression does not have a integer or char" },
            { InvalidCaseValueType, "Case value type doesn't match switch expression" },
            { IncrementDecrementOnNonInt, "Increment or decrement on non-integer type" },
            { NotOnNonBool, "Logical not used on non-boolean type" }
        };

        auto it = texts.find(t);
        string baseMsg = (it != texts.end()) ? it->second : "Unknown type error";
        message = baseMsg;
        if (!name.empty()) {
            message += ": '" + name + "'";
        }
        message += " (line: " + to_string(l) + ", col: " + to_string(c) + ")";
    }
};

// ********************************* FUNCTION DECLARATIONS ******************************************

vector<Token> lexAndDumpToFile(const string& inputFilename, const string& outputFilename);
vector<unique_ptr<ASTNode>> parseFromFile(const vector<Token>& tokens);

// Updated scope analysis function that returns the symbol table
void performScopeAnalysis(const vector<ASTPtr>& ast, const vector<Token>& tokens); 
void performTypeChecking(const vector<ASTPtr>& ast, const vector<Token>& tokens);

#endif