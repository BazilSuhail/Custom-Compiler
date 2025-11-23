#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <set>
#include "parser.h"

using namespace std;

// === Type Checking Error Types ===
enum class TypeChkError {
    ErroneousVarDecl,
    FnCallParamCount,
    FnCallParamType,
    ErroneousReturnType,
    ExpressionTypeMismatch,
    ExpectedBooleanExpression,
    ErroneousBreak,
    NonBooleanCondStmt,
    EmptyExpression,
    AttemptedBoolOpOnNonBools,
    AttemptedBitOpOnNonNumeric,
    AttemptedShiftOnNonInt,
    AttemptedAddOpOnNonNumeric,
    AttemptedExponentiationOfNonNumeric,
    ReturnStmtNotFound
};

struct TypeErrorInfo {
    TypeChkError errorType;
    string message;
    int line;
    int column;
    
    TypeErrorInfo(TypeChkError type, const string& msg, int l, int c)
        : errorType(type), message(msg), line(l), column(c) {}
};

// === Symbol and Scope Types (from scope-analysis.cpp) ===
enum class SymbolType {
    VARIABLE,
    FUNCTION,
    PARAMETER
};

struct FunctionSignature {
    string returnType;
    vector<string> paramTypes;
    
    bool operator==(const FunctionSignature& other) const {
        if (returnType != other.returnType) return false;
        if (paramTypes.size() != other.paramTypes.size()) return false;
        for (size_t i = 0; i < paramTypes.size(); i++) {
            if (paramTypes[i] != other.paramTypes[i]) return false;
        }
        return true;
    }
};

struct Symbol {
    string name;
    SymbolType type;
    string dataType;
    int line;
    int column;
    bool isDefined;
    FunctionSignature funcSignature;
    
    Symbol(const string& n, SymbolType t, const string& dt, int l, int c)
        : name(n), type(t), dataType(dt), line(l), column(c), isDefined(false) {}
    
    Symbol(const string& n, SymbolType t, const string& dt, int l, int c, 
           const FunctionSignature& sig)
        : name(n), type(t), dataType(dt), line(l), column(c), isDefined(false), funcSignature(sig) {}
};

class Scope {
private:
    map<string, vector<Symbol>> symbols;
    Scope* parent;
    vector<unique_ptr<Scope>> children;
    int depth;
    string scopeType;

public:
    Scope(Scope* p = nullptr, int d = 0, const string& type = "block") 
        : parent(p), depth(d), scopeType(type) {}
    
    bool declareSymbol(const string& name, SymbolType type, const string& dataType, int line, int column) {
        if (symbols.find(name) != symbols.end()) {
            for (const auto& sym : symbols[name]) {
                if (sym.type == type) {
                    return false;
                }
            }
        }
        symbols[name].emplace_back(name, type, dataType, line, column);
        return true;
    }
    
    bool declareFunction(const string& name, const FunctionSignature& signature, int line, int column, bool isDefinition) {
        if (symbols.find(name) != symbols.end()) {
            for (const auto& sym : symbols[name]) {
                if (sym.type == SymbolType::FUNCTION && sym.funcSignature == signature) {
                    return false;
                }
            }
        }
        
        symbols[name].emplace_back(name, SymbolType::FUNCTION, signature.returnType, line, column, signature);
        symbols[name].back().isDefined = isDefinition;
        return true;
    }
    
    Symbol* findSymbolInScope(const string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end() && !it->second.empty()) {
            return &it->second.back();
        }
        return nullptr;
    }
    
    vector<Symbol*> getAllSymbols(const string& name) {
        vector<Symbol*> result;
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            for (auto& sym : it->second) {
                result.push_back(&sym);
            }
        }
        return result;
    }
    
    Scope* getParent() const { return parent; }
    int getDepth() const { return depth; }
    string getScopeType() const { return scopeType; }
    
    Scope* createChildScope(const string& type = "block") {
        children.push_back(make_unique<Scope>(this, depth + 1, type));
        return children.back().get();
    }
};

// === Type Checker ===
class TypeChecker {
private:
    Scope* currentScope;
    Scope globalScope;
    vector<TypeErrorInfo> errors;
    string currentFunctionReturnType;
    bool hasReturnStatement;
    map<string, string> variableTypes;
    
    // Helper function to check if a type is numeric
    bool isNumericType(const string& type) const {
        return type == "int" || type == "float" || type == "double";
    }
    
    // Helper function to check if a type is integer
    bool isIntegerType(const string& type) const {
        return type == "int";
    }
    
    // Helper function to check if a type is boolean
    bool isBooleanType(const string& type) const {
        return type == "bool";
    }
    
    // Helper function to get result type of binary operation
    string getResultType(const string& leftType, const string& rightType, const string& op) const {
        if (leftType == "error" || rightType == "error") return "error";
        
        // For comparison operators
        if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
            return "bool";
        }
        
        // For logical operators
        if (op == "&&" || op == "||") {
            return "bool";
        }
        
        // For arithmetic operators
        if (leftType == "double" || rightType == "double") return "double";
        if (leftType == "float" || rightType == "float") return "float";
        return "int";
    }
    
    Symbol* findVariable(const string& name) {
        Scope* scope = currentScope;
        while (scope != nullptr) {
            Symbol* symbol = scope->findSymbolInScope(name);
            if (symbol != nullptr && (symbol->type == SymbolType::VARIABLE || symbol->type == SymbolType::PARAMETER)) {
                return symbol;
            }
            scope = scope->getParent();
        }
        return nullptr;
    }
    
    Symbol* findFunction(const string& name) {
        return globalScope.findSymbolInScope(name);
    }

public:
    TypeChecker() : globalScope(nullptr, 0, "global"), currentScope(&globalScope), hasReturnStatement(false) {}
    
    void enterScope(const string& type = "block") {
        currentScope = currentScope->createChildScope(type);
    }
    
    void exitScope() {
        if (currentScope->getParent() != nullptr) {
            currentScope = currentScope->getParent();
        }
    }
    
    void declareVariable(const string& name, const string& dataType, int line, int column) {
        currentScope->declareSymbol(name, SymbolType::VARIABLE, dataType, line, column);
        variableTypes[name] = dataType;
    }
    
    void declareFunction(const string& name, const FunctionSignature& signature, int line, int column, bool isDefinition = false) {
        globalScope.declareFunction(name, signature, line, column, isDefinition);
    }
    
    void declareParameter(const string& name, const string& dataType, int line, int column) {
        currentScope->declareSymbol(name, SymbolType::PARAMETER, dataType, line, column);
        variableTypes[name] = dataType;
    }
    
    string checkExpression(ASTNode* node);
    
    void checkProgram(Program* node) {
        // First pass: declare all functions
        for (auto& decl : node->declarations) {
            if (decl && decl->nodeType == NodeType::FunctionDecl) {
                FunctionDecl* func = static_cast<FunctionDecl*>(decl.get());
                FunctionSignature sig;
                sig.returnType = func->returnType;
                for (auto& param : func->parameters) {
                    if (param) {
                        sig.paramTypes.push_back(param->type);
                    }
                }
                declareFunction(func->name, sig, func->line, func->column, true);
            }
        }
        
        // Second pass: type check everything
        for (auto& decl : node->declarations) {
            if (decl) {
                checkNode(decl.get());
            }
        }
    }
    
    void checkFunctionDecl(FunctionDecl* node) {
        currentFunctionReturnType = node->returnType;
        hasReturnStatement = false;
        
        FunctionSignature sig;
        sig.returnType = node->returnType;
        for (auto& param : node->parameters) {
            if (param) {
                sig.paramTypes.push_back(param->type);
            }
        }
        
        enterScope("function_" + node->name);
        
        // Declare parameters
        for (auto& param : node->parameters) {
            if (param) {
                declareParameter(param->name, param->type, param->line, param->column);
            }
        }
        
        // Check function body
        if (node->body) {
            checkNode(node->body.get());
        }
        
        // Check if non-void function has return statement
        if (node->returnType != "void" && !hasReturnStatement) {
            errors.emplace_back(
                TypeChkError::ReturnStmtNotFound,
                "Function '" + node->name + "' with return type '" + node->returnType + "' must have a return statement",
                node->line,
                node->column
            );
        }
        
        exitScope();
    }
    
    void checkVarDecl(VarDecl* node) {
        declareVariable(node->name, node->type, node->line, node->column);
        
        if (node->initializer) {
            // Set line/column info in initializer if not set
            if (node->initializer->line == 0 && node->initializer->column == 0) {
                node->initializer->line = node->line;
                node->initializer->column = node->column;
            }
            
            string initType = checkExpression(node->initializer.get());
            
            if (initType != "error" && initType != node->type) {
                // Check if types are compatible (e.g., int to float is ok)
                if (!(isNumericType(node->type) && isNumericType(initType))) {
                    errors.emplace_back(
                        TypeChkError::ErroneousVarDecl,
                        "Variable '" + node->name + "' declared as '" + node->type + 
                        "' but initialized with expression of type '" + initType + "'",
                        node->line,
                        node->column
                    );
                }
            }
        }
    }
    
    void checkAssignment(Assignment* node) {
        Symbol* var = findVariable(node->variable);
        if (!var) return; // Error already reported by scope analyzer
        
        string varType = var->dataType;
        string exprType = checkExpression(node->expression.get());
        
        if (exprType != "error" && varType != exprType) {
            if (!(isNumericType(varType) && isNumericType(exprType))) {
                errors.emplace_back(
                    TypeChkError::ExpressionTypeMismatch,
                    "Cannot assign expression of type '" + exprType + 
                    "' to variable '" + node->variable + "' of type '" + varType + "'",
                    node->line,
                    node->column
                );
            }
        }
    }
    
    void checkIfStmt(IfStmt* node) {
        if (node->condition) {
            string condType = checkExpression(node->condition.get());
            if (condType != "error" && condType != "bool") {
                errors.emplace_back(
                    TypeChkError::NonBooleanCondStmt,
                    "If statement condition must be of type 'bool', got '" + condType + "'",
                    node->line,
                    node->column
                );
            }
        }
        
        if (node->thenBranch) {
            checkNode(node->thenBranch.get());
        }
        
        if (node->elseBranch) {
            checkNode(node->elseBranch.get());
        }
    }
    
    void checkWhileStmt(WhileStmt* node) {
        if (node->condition) {
            string condType = checkExpression(node->condition.get());
            if (condType != "error" && condType != "bool") {
                errors.emplace_back(
                    TypeChkError::NonBooleanCondStmt,
                    "While statement condition must be of type 'bool', got '" + condType + "'",
                    node->line,
                    node->column
                );
            }
        }
        
        if (node->body) {
            checkNode(node->body.get());
        }
    }
    
    void checkReturnStmt(ReturnStmt* node) {
        hasReturnStatement = true;
        
        if (node->expression) {
            string exprType = checkExpression(node->expression.get());
            
            if (currentFunctionReturnType == "void") {
                errors.emplace_back(
                    TypeChkError::ErroneousReturnType,
                    "Void function cannot return a value",
                    node->line,
                    node->column
                );
            } else if (exprType != "error" && exprType != currentFunctionReturnType) {
                if (!(isNumericType(currentFunctionReturnType) && isNumericType(exprType))) {
                    errors.emplace_back(
                        TypeChkError::ErroneousReturnType,
                        "Return type mismatch: expected '" + currentFunctionReturnType + 
                        "', got '" + exprType + "'",
                        node->line,
                        node->column
                    );
                }
            }
        } else {
            if (currentFunctionReturnType != "void") {
                errors.emplace_back(
                    TypeChkError::ErroneousReturnType,
                    "Non-void function must return a value of type '" + currentFunctionReturnType + "'",
                    node->line,
                    node->column
                );
            }
        }
    }
    
    void checkBlock(Block* node) {
        bool createNewScope = true;
        
        if (createNewScope) {
            enterScope("block");
        }
        
        for (auto& stmt : node->statements) {
            if (stmt) {
                checkNode(stmt.get());
            }
        }
        
        if (createNewScope) {
            exitScope();
        }
    }
    
    void checkPrintStmt(PrintStmt* node) {
        for (auto& arg : node->arguments) {
            if (arg) {
                checkExpression(arg.get());
            }
        }
    }
    
    void checkNode(ASTNode* node) {
        switch (node->nodeType) {
            case NodeType::Program:
                checkProgram(static_cast<Program*>(node));
                break;
            case NodeType::FunctionDecl:
                checkFunctionDecl(static_cast<FunctionDecl*>(node));
                break;
            case NodeType::Block:
                checkBlock(static_cast<Block*>(node));
                break;
            case NodeType::VarDecl:
                checkVarDecl(static_cast<VarDecl*>(node));
                break;
            case NodeType::Assignment:
                checkAssignment(static_cast<Assignment*>(node));
                break;
            case NodeType::IfStmt:
                checkIfStmt(static_cast<IfStmt*>(node));
                break;
            case NodeType::WhileStmt:
                checkWhileStmt(static_cast<WhileStmt*>(node));
                break;
            case NodeType::ReturnStmt:
                checkReturnStmt(static_cast<ReturnStmt*>(node));
                break;
            case NodeType::PrintStmt:
                checkPrintStmt(static_cast<PrintStmt*>(node));
                break;
            case NodeType::ExprStmt:
                if (node) checkExpression(node);
                break;
            default:
                break;
        }
    }
    
    const vector<TypeErrorInfo>& getErrors() const { return errors; }
};

string TypeChecker::checkExpression(ASTNode* node) {
    if (!node) {
        errors.emplace_back(
            TypeChkError::EmptyExpression,
            "Empty expression encountered",
            0, 0
        );
        return "error";
    }
    
    switch (node->nodeType) {
        case NodeType::IntLiteral:
            return "int";
            
        case NodeType::FloatLiteral:
            return "float";
            
        case NodeType::StringLiteral:
            return "string";
            
        case NodeType::CharLiteral:
            return "char";
            
        case NodeType::BoolLiteral:
            return "bool";
            
        case NodeType::Identifier: {
            Identifier* id = static_cast<Identifier*>(node);
            Symbol* var = findVariable(id->name);
            if (var) {
                return var->dataType;
            }
            // Don't report error here - scope analyzer already did
            return "error";
        }
        
        case NodeType::BinaryOp: {
            BinaryOp* binOp = static_cast<BinaryOp*>(node);
            string leftType = checkExpression(binOp->left.get());
            string rightType = checkExpression(binOp->right.get());
            
            // Logical operators (&&, ||)
            if (binOp->operator_ == "&&" || binOp->operator_ == "||") {
                if (!isBooleanType(leftType) || !isBooleanType(rightType)) {
                    errors.emplace_back(
                        TypeChkError::AttemptedBoolOpOnNonBools,
                        "Boolean operator '" + binOp->operator_ + 
                        "' requires boolean operands, got '" + leftType + "' and '" + rightType + "'",
                        binOp->line,
                        binOp->column
                    );
                    return "error";
                }
                return "bool";
            }
            
            // Arithmetic operators (+, -, *, /, %)
            if (binOp->operator_ == "+" || binOp->operator_ == "-" || 
                binOp->operator_ == "*" || binOp->operator_ == "/") {
                if (!isNumericType(leftType) || !isNumericType(rightType)) {
                    errors.emplace_back(
                        TypeChkError::AttemptedAddOpOnNonNumeric,
                        "Arithmetic operator '" + binOp->operator_ + 
                        "' requires numeric operands, got '" + leftType + "' and '" + rightType + "'",
                        binOp->line,
                        binOp->column
                    );
                    return "error";
                }
                return getResultType(leftType, rightType, binOp->operator_);
            }
            
            // Modulo operator (%)
            if (binOp->operator_ == "%") {
                if (!isIntegerType(leftType) || !isIntegerType(rightType)) {
                    errors.emplace_back(
                        TypeChkError::AttemptedBitOpOnNonNumeric,
                        "Modulo operator requires integer operands, got '" + 
                        leftType + "' and '" + rightType + "'",
                        binOp->line,
                        binOp->column
                    );
                    return "error";
                }
                return "int";
            }
            
            // Comparison operators
            if (binOp->operator_ == "==" || binOp->operator_ == "!=" ||
                binOp->operator_ == "<" || binOp->operator_ == ">" ||
                binOp->operator_ == "<=" || binOp->operator_ == ">=") {
                if (leftType != rightType && !(isNumericType(leftType) && isNumericType(rightType))) {
                    errors.emplace_back(
                        TypeChkError::ExpressionTypeMismatch,
                        "Comparison operator '" + binOp->operator_ + 
                        "' requires compatible types, got '" + leftType + "' and '" + rightType + "'",
                        binOp->line,
                        binOp->column
                    );
                    return "error";
                }
                return "bool";
            }
            
            return getResultType(leftType, rightType, binOp->operator_);
        }
        
        
        case NodeType::UnaryOp: {
            UnaryOp* unOp = static_cast<UnaryOp*>(node);
            string operandType = checkExpression(unOp->operand.get());
            
            if (unOp->operator_ == "!") {
                if (!isBooleanType(operandType)) {
                    errors.emplace_back(
                        TypeChkError::AttemptedBoolOpOnNonBools,
                        "Logical NOT operator requires boolean operand, got '" + operandType + "'",
                        unOp->line,
                        unOp->column
                    );
                    return "error";
                }
                return "bool";
            }
            
            if (unOp->operator_ == "-" || unOp->operator_ == "+") {
                if (!isNumericType(operandType)) {
                    errors.emplace_back(
                        TypeChkError::AttemptedAddOpOnNonNumeric,
                        "Unary operator '" + unOp->operator_ + 
                        "' requires numeric operand, got '" + operandType + "'",
                        unOp->line,
                        unOp->column
                    );
                    return "error";
                }
                return operandType;
            }
            
            return operandType;
        }
        
        case NodeType::FunctionCall: {
            FunctionCall* call = static_cast<FunctionCall*>(node);
            Symbol* func = findFunction(call->functionName);
            
            if (!func || func->type != SymbolType::FUNCTION) {
                return "error";
            }
            
            // Check parameter count
            if (call->arguments.size() != func->funcSignature.paramTypes.size()) {
                errors.emplace_back(
                    TypeChkError::FnCallParamCount,
                    "Function '" + call->functionName + "' expects " + 
                    to_string(func->funcSignature.paramTypes.size()) + " arguments, got " + 
                    to_string(call->arguments.size()),
                    call->line,
                    call->column
                );
                return func->funcSignature.returnType;
            }
            
            // Check parameter types
            for (size_t i = 0; i < call->arguments.size(); i++) {
                string argType = checkExpression(call->arguments[i].get());
                string paramType = func->funcSignature.paramTypes[i];
                
                if (argType != "error" && argType != paramType) {
                    if (!(isNumericType(paramType) && isNumericType(argType))) {
                        errors.emplace_back(
                            TypeChkError::FnCallParamType,
                            "Function '" + call->functionName + "' parameter " + 
                            to_string(i + 1) + " expects type '" + paramType + 
                            "', got '" + argType + "'",
                            call->line,
                            call->column
                        );
                    }
                }
            }
            
            return func->funcSignature.returnType;
        }
        
        default:
            return "error";
    }
}

// === Utility Functions ===
vector<Token> loadTokensFromFile(const string& filename);

const char* typeErrorToString(TypeChkError error) {
    switch (error) {
        case TypeChkError::ErroneousVarDecl: return "ErroneousVarDecl";
        case TypeChkError::FnCallParamCount: return "FnCallParamCount";
        case TypeChkError::FnCallParamType: return "FnCallParamType";
        case TypeChkError::ErroneousReturnType: return "ErroneousReturnType";
        case TypeChkError::ExpressionTypeMismatch: return "ExpressionTypeMismatch";
        case TypeChkError::ExpectedBooleanExpression: return "ExpectedBooleanExpression";
        case TypeChkError::ErroneousBreak: return "ErroneousBreak";
        case TypeChkError::NonBooleanCondStmt: return "NonBooleanCondStmt";
        case TypeChkError::EmptyExpression: return "EmptyExpression";
        case TypeChkError::AttemptedBoolOpOnNonBools: return "AttemptedBoolOpOnNonBools";
        case TypeChkError::AttemptedBitOpOnNonNumeric: return "AttemptedBitOpOnNonNumeric";
        case TypeChkError::AttemptedShiftOnNonInt: return "AttemptedShiftOnNonInt";
        case TypeChkError::AttemptedAddOpOnNonNumeric: return "AttemptedAddOpOnNonNumeric";
        case TypeChkError::AttemptedExponentiationOfNonNumeric: return "AttemptedExponentiationOfNonNumeric";
        case TypeChkError::ReturnStmtNotFound: return "ReturnStmtNotFound";
        default: return "Unknown";
    }
}

// === Main function ===
int main() {
    try {
        cout << "=== COMPILER: TYPE CHECKER ===\n" << endl;
        
        // Load tokens from lexer output
        vector<Token> tokens = loadTokensFromFile("tester/tokens.txt");
        
        if (tokens.empty()) {
            cerr << "Error: No tokens found or could not read tokens file." << endl;
            return 1;
        }
        
        cout << "✓ Loaded " << tokens.size() << " tokens from lexer output." << endl;
        
        // Parse tokens to create AST
        Parser parser(tokens);
        auto ast = parser.parse();
        
        cout << "✓ Parsing completed successfully!" << endl;
        
        // Perform type checking
        cout << "\n=== PERFORMING TYPE CHECKING ===\n" << endl;
        
        TypeChecker typeChecker;
        typeChecker.checkProgram(ast.get());
        
        // Create output string
        stringstream output;
        
        // Print errors if any
        const auto& errors = typeChecker.getErrors();
        if (!errors.empty()) {
            output << "=== TYPE ERRORS DETECTED ===" << endl;
            output << "Total errors: " << errors.size() << "\n" << endl;
            
            for (const auto& error : errors) {
                output << "Error at line " << error.line << ", column " << error.column << ": ";
                output << "[" << typeErrorToString(error.errorType) << "] ";
                output << error.message << endl;
            }
        } else {
            output << "✓ No type errors found!" << endl;
        }
        
        output << "\n=== TYPE CHECKING COMPLETE ===\n" << endl;
        
        // Print to console
        string outputStr = output.str();
        cout << outputStr;
        
        // Write to file
        ofstream outFile("tester/type_check_output.txt");
        if (outFile.is_open()) {
            outFile << outputStr;
            outFile.close();
            cout << "\n✓ Type checking output written to: tester/type_check_output.txt" << endl;
        } else {
            cerr << "\n✗ Error: Could not open output file for writing." << endl;
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}