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

// === Scope Analysis Types ===
enum class SymbolType {
    VARIABLE,
    FUNCTION,
    PARAMETER
};

enum class ScopeError {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition
};

struct ErrorInfo {
    ScopeError errorType;
    string symbolName;
    int line;
    int column;
    
    ErrorInfo(ScopeError type, const string& name, int l, int c)
        : errorType(type), symbolName(name), line(l), column(c) {}
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
    
    bool operator<(const FunctionSignature& other) const {
        if (returnType != other.returnType) return returnType < other.returnType;
        if (paramTypes.size() != other.paramTypes.size()) 
            return paramTypes.size() < other.paramTypes.size();
        for (size_t i = 0; i < paramTypes.size(); i++) {
            if (paramTypes[i] != other.paramTypes[i]) 
                return paramTypes[i] < other.paramTypes[i];
        }
        return false;
    }
};

struct Symbol {
    string name;
    SymbolType type;
    string dataType;
    int line;
    int column;
    bool isDefined;
    FunctionSignature funcSignature; // For functions
    
    Symbol(const string& n, SymbolType t, const string& dt, int l, int c)
        : name(n), type(t), dataType(dt), line(l), column(c), isDefined(false) {}
    
    Symbol(const string& n, SymbolType t, const string& dt, int l, int c, 
           const FunctionSignature& sig)
        : name(n), type(t), dataType(dt), line(l), column(c), isDefined(false), funcSignature(sig) {}
};

// === Spaghetti Stack (Scope) Implementation ===
class Scope {
private:
    map<string, vector<Symbol>> symbols; // Multiple symbols with same name (function overloading)
    Scope* parent;
    vector<unique_ptr<Scope>> children;
    int depth;
    string scopeType;

public:
    Scope(Scope* p = nullptr, int d = 0, const string& type = "block") 
        : parent(p), depth(d), scopeType(type) {}
    
    bool declareSymbol(const string& name, SymbolType type, const string& dataType, int line, int column) {
        // Check for redefinition in current scope
        if (symbols.find(name) != symbols.end()) {
            for (const auto& sym : symbols[name]) {
                if (sym.type == type) {
                    return false; // Redefinition
                }
            }
        }
        symbols[name].emplace_back(name, type, dataType, line, column);
        return true;
    }
    
    bool declareFunction(const string& name, const FunctionSignature& signature, int line, int column, bool isDefinition) {
        // Check for function redefinition with same signature
        if (symbols.find(name) != symbols.end()) {
            for (const auto& sym : symbols[name]) {
                if (sym.type == SymbolType::FUNCTION && sym.funcSignature == signature) {
                    return false; // Function with same signature already exists
                }
            }
        }
        
        symbols[name].emplace_back(name, SymbolType::FUNCTION, signature.returnType, line, column, signature);
        symbols[name].back().isDefined = isDefinition;
        return true;
    }
    
    Symbol* findSymbol(const string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end() && !it->second.empty()) {
            return &it->second.back();
        }
        return nullptr;
    }
    
    Symbol* findSymbolInScope(const string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end() && !it->second.empty()) {
            return &it->second.back();
        }
        return nullptr;
    }
    
    vector<Symbol*> findFunctionBySignature(const string& name, const FunctionSignature& signature) {
        vector<Symbol*> result;
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            for (auto& sym : it->second) {
                if (sym.type == SymbolType::FUNCTION && sym.funcSignature == signature) {
                    result.push_back(&sym);
                }
            }
        }
        return result;
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
    
    void print(ostream& out, int indent = 0) const {
        string indentStr(indent, ' ');
        out << indentStr << "Scope [" << scopeType << "] (depth: " << depth << "):\n";
        for (const auto& pair : symbols) {
            for (const auto& sym : pair.second) {
                string typeStr;
                switch (sym.type) {
                    case SymbolType::VARIABLE: typeStr = "VARIABLE"; break;
                    case SymbolType::FUNCTION: typeStr = "FUNCTION"; break;
                    case SymbolType::PARAMETER: typeStr = "PARAMETER"; break;
                }
                out << indentStr << "  " << sym.name << " : " << typeStr 
                    << " (" << sym.dataType << ") at " << sym.line << ":" << sym.column;
                if (sym.type == SymbolType::FUNCTION) {
                    out << " [defined: " << (sym.isDefined ? "yes" : "no") << "]";
                    out << " signature: " << sym.funcSignature.returnType << "(";
                    for (size_t i = 0; i < sym.funcSignature.paramTypes.size(); i++) {
                        if (i > 0) out << ", ";
                        out << sym.funcSignature.paramTypes[i];
                    }
                    out << ")";
                }
                out << "\n";
            }
        }
        for (const auto& child : children) {
            child->print(out, indent + 2);
        }
    }
};

// === Scope Analyzer (manages the spaghetti stack) ===
class ScopeAnalyzer {
private:
    Scope* currentScope;
    Scope globalScope;
    vector<ErrorInfo> errors;
    set<string> declaredButNotDefinedFunctions;
    
public:
    ScopeAnalyzer() : globalScope(nullptr, 0, "global"), currentScope(&globalScope) {}
    
    void enterScope(const string& type = "block") {
        currentScope = currentScope->createChildScope(type);
    }
    
    void exitScope() {
        if (currentScope->getParent() != nullptr) {
            currentScope = currentScope->getParent();
        }
    }
    
    bool declareVariable(const string& name, const string& dataType, int line, int column) {
        if (currentScope->findSymbolInScope(name) != nullptr) {
            errors.emplace_back(ScopeError::VariableRedefinition, name, line, column);
            return false;
        }
        return currentScope->declareSymbol(name, SymbolType::VARIABLE, dataType, line, column);
    }
    
    bool declareFunction(const string& name, const FunctionSignature& signature, int line, int column, bool isDefinition = false) {
        Scope* searchScope = &globalScope;
        
        // Check for function redefinition with same signature
        vector<Symbol*> existingFuncs = searchScope->findFunctionBySignature(name, signature);
        if (!existingFuncs.empty()) {
            Symbol* existing = existingFuncs[0];
            if (isDefinition && existing->isDefined) {
                errors.emplace_back(ScopeError::FunctionPrototypeRedefinition, name, line, column);
                return false;
            }
            if (isDefinition) {
                existing->isDefined = true;
                declaredButNotDefinedFunctions.erase(name);
            }
            return true;
        }
        
        // Check for variable with same name
        Symbol* existingVar = searchScope->findSymbolInScope(name);
        if (existingVar != nullptr && existingVar->type != SymbolType::FUNCTION) {
            errors.emplace_back(ScopeError::FunctionPrototypeRedefinition, name, line, column);
            return false;
        }
        
        bool result = searchScope->declareFunction(name, signature, line, column, isDefinition);
        if (result && !isDefinition) {
            declaredButNotDefinedFunctions.insert(name);
        }
        return result;
    }
    
    bool declareParameter(const string& name, const string& dataType, int line, int column) {
        if (currentScope->findSymbolInScope(name) != nullptr) {
            errors.emplace_back(ScopeError::VariableRedefinition, name, line, column);
            return false;
        }
        return currentScope->declareSymbol(name, SymbolType::PARAMETER, dataType, line, column);
    }
    
    Symbol* findVariable(const string& name, int line = 0, int column = 0) {
        Scope* scope = currentScope;
        while (scope != nullptr) {
            Symbol* symbol = scope->findSymbolInScope(name);
            if (symbol != nullptr && (symbol->type == SymbolType::VARIABLE || symbol->type == SymbolType::PARAMETER)) {
                return symbol;
            }
            scope = scope->getParent();
        }
        errors.emplace_back(ScopeError::UndeclaredVariableAccessed, name, line, column);
        return nullptr;
    }
    
    Symbol* findFunction(const string& name, const FunctionSignature& signature, int line = 0, int column = 0) {
        Symbol* symbol = globalScope.findSymbolInScope(name);
        if (symbol != nullptr && symbol->type == SymbolType::FUNCTION) {
            vector<Symbol*> matchingFuncs = globalScope.findFunctionBySignature(name, signature);
            if (!matchingFuncs.empty()) {
                Symbol* func = matchingFuncs[0];
                if (!func->isDefined) {
                    errors.emplace_back(ScopeError::UndefinedFunctionCalled, name, line, column);
                }
                return func;
            }
        }
        errors.emplace_back(ScopeError::UndefinedFunctionCalled, name, line, column);
        return nullptr;
    }
    
    // New method to check for declared but not defined functions
    void checkForUndefinedFunctions() {
        for (const auto& funcName : declaredButNotDefinedFunctions) {
            vector<Symbol*> funcs = globalScope.getAllSymbols(funcName);
            for (const auto& func : funcs) {
                if (func->type == SymbolType::FUNCTION && !func->isDefined) {
                    errors.emplace_back(ScopeError::UndefinedFunctionCalled, funcName, func->line, func->column);
                }
            }
        }
    }
    
    const vector<ErrorInfo>& getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    void printScopeTree(ostream& out) const {
        out << "=== SCOPE TREE (Spaghetti Stack) ===\n";
        globalScope.print(out);
        out << "====================================\n";
    }
    
    const Scope& getGlobalScope() const { return globalScope; }
    Scope* getCurrentScope() const { return currentScope; }
};

// === AST Visitor for Scope Analysis ===
class ScopeAnalysisVisitor {
private:
    ScopeAnalyzer& analyzer;
    bool inFunctionBody;

public:
    ScopeAnalysisVisitor(ScopeAnalyzer& analyzer) : analyzer(analyzer), inFunctionBody(false) {}

    void visitProgram(Program* node) {
        // First pass: declare all functions (including prototypes)
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
                analyzer.declareFunction(func->name, sig, func->line, func->column, true);
            }
        }
        
        // Second pass: process everything
        for (auto& decl : node->declarations) {
            if (decl) {
                visitNode(decl.get());
            }
        }
        
        // Check for undefined functions
        analyzer.checkForUndefinedFunctions();
    }

    void visitFunctionDecl(FunctionDecl* node) {
        // Function already declared in first pass, now process body
        FunctionSignature sig;
        sig.returnType = node->returnType;
        for (auto& param : node->parameters) {
            if (param) {
                sig.paramTypes.push_back(param->type);
            }
        }
        
        // Enter function scope for parameters
        analyzer.enterScope("function_" + node->name);
        
        // Declare parameters in function scope
        for (auto& param : node->parameters) {
            if (param) {
                analyzer.declareParameter(param->name, param->type, param->line, param->column);
            }
        }
        
        // Mark that we're entering function body
        inFunctionBody = true;
        
        // Visit function body
        if (node->body) {
            visitNode(node->body.get());
        }
        
        inFunctionBody = false;
        
        // Exit function scope
        analyzer.exitScope();
    }

    void visitBlock(Block* node) {
        bool createNewScope = !inFunctionBody;
        
        if (createNewScope) {
            analyzer.enterScope("block");
        } else {
            inFunctionBody = false;
        }
        
        for (auto& stmt : node->statements) {
            if (stmt) {
                visitNode(stmt.get());
            }
        }
        
        if (createNewScope) {
            analyzer.exitScope();
        }
    }

    void visitVarDecl(VarDecl* node) {
        analyzer.declareVariable(node->name, node->type, node->line, node->column);
        
        if (node->initializer) {
            visitNode(node->initializer.get());
        }
    }

    void visitAssignment(Assignment* node) {
        analyzer.findVariable(node->variable, node->line, node->column);
        
        if (node->expression) {
            visitNode(node->expression.get());
        }
    }

    void visitBinaryOp(BinaryOp* node) {
        if (node->left) visitNode(node->left.get());
        if (node->right) visitNode(node->right.get());
    }

    void visitUnaryOp(UnaryOp* node) {
        if (node->operand) visitNode(node->operand.get());
    }

    void visitFunctionCall(FunctionCall* node) {
        // Create function signature from arguments (simplified)
        FunctionSignature sig;
        sig.returnType = "int"; // Default assumption
        
        for (auto& arg : node->arguments) {
            if (arg) {
                // In a real implementation, you'd infer types from arguments
                sig.paramTypes.push_back("int"); // Simplified
            }
        }
        
        analyzer.findFunction(node->functionName, sig, node->line, node->column);
        
        for (auto& arg : node->arguments) {
            if (arg) {
                visitNode(arg.get());
            }
        }
    }

    void visitIdentifier(Identifier* node) {
        analyzer.findVariable(node->name, node->line, node->column);
    }

    void visitIntLiteral(IntLiteral* node) {}
    void visitFloatLiteral(FloatLiteral* node) {}
    void visitStringLiteral(StringLiteral* node) {}

    void visitReturnStmt(ReturnStmt* node) {
        if (node->expression) {
            visitNode(node->expression.get());
        }
    }

    void visitIfStmt(IfStmt* node) {
        if (node->condition) {
            visitNode(node->condition.get());
        }
        
        if (node->thenBranch) {
            visitNode(node->thenBranch.get());
        }
        
        if (node->elseBranch) {
            visitNode(node->elseBranch.get());
        }
    }

    void visitWhileStmt(WhileStmt* node) {
        if (node->condition) {
            visitNode(node->condition.get());
        }
        
        if (node->body) {
            visitNode(node->body.get());
        }
    }

    void visitPrintStmt(PrintStmt* node) {
        for (auto& arg : node->arguments) {
            if (arg) {
                visitNode(arg.get());
            }
        }
    }

    void visitNode(ASTNode* node) {
        switch (node->nodeType) {
            case NodeType::Program:
                visitProgram(static_cast<Program*>(node));
                break;
            case NodeType::FunctionDecl:
                visitFunctionDecl(static_cast<FunctionDecl*>(node));
                break;
            case NodeType::Block:
                visitBlock(static_cast<Block*>(node));
                break;
            case NodeType::VarDecl:
                visitVarDecl(static_cast<VarDecl*>(node));
                break;
            case NodeType::Assignment:
                visitAssignment(static_cast<Assignment*>(node));
                break;
            case NodeType::BinaryOp:
                visitBinaryOp(static_cast<BinaryOp*>(node));
                break;
            case NodeType::UnaryOp:
                visitUnaryOp(static_cast<UnaryOp*>(node));
                break;
            case NodeType::FunctionCall:
                visitFunctionCall(static_cast<FunctionCall*>(node));
                break;
            case NodeType::Identifier:
                visitIdentifier(static_cast<Identifier*>(node));
                break;
            case NodeType::IntLiteral:
                visitIntLiteral(static_cast<IntLiteral*>(node));
                break;
            case NodeType::FloatLiteral:
                visitFloatLiteral(static_cast<FloatLiteral*>(node));
                break;
            case NodeType::StringLiteral:
                visitStringLiteral(static_cast<StringLiteral*>(node));
                break;
            case NodeType::ReturnStmt:
                visitReturnStmt(static_cast<ReturnStmt*>(node));
                break;
            case NodeType::IfStmt:
                visitIfStmt(static_cast<IfStmt*>(node));
                break;
            case NodeType::WhileStmt:
                visitWhileStmt(static_cast<WhileStmt*>(node));
                break;
            case NodeType::PrintStmt:
                visitPrintStmt(static_cast<PrintStmt*>(node));
                break;
            default:
                break;
        }
    }
};



// === Main function ===
int main() {
    try {
        cout << "=== COMPILER: PARSER + SCOPE ANALYZER ===\n" << endl;
        
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
        
        // Debug: Print how many declarations were found
        cout << "[DEBUG] Found " << ast->declarations.size() << " top-level declarations" << endl;
        for (size_t i = 0; i < ast->declarations.size(); i++) {
            auto& decl = ast->declarations[i];
            if (decl->nodeType == NodeType::FunctionDecl) {
                FunctionDecl* func = static_cast<FunctionDecl*>(decl.get());
                cout << "[DEBUG] Declaration " << i << ": Function '" << func->name << "'" << endl;
            } else if (decl->nodeType == NodeType::VarDecl) {
                VarDecl* var = static_cast<VarDecl*>(decl.get());
                cout << "[DEBUG] Declaration " << i << ": Variable '" << var->name << "'" << endl;
            }
        }
        
        // Perform scope analysis
        cout << "\n=== PERFORMING SCOPE ANALYSIS ===\n" << endl;
        
        ScopeAnalyzer analyzer;
        ScopeAnalysisVisitor visitor(analyzer);
        
        visitor.visitProgram(ast.get());
        
        // Create output string
        stringstream output;
        
        // Print scope tree
        analyzer.printScopeTree(output);
        
        // Print errors if any
        const auto& errors = analyzer.getErrors();
        if (!errors.empty()) {
            output << "\n=== SCOPE ERRORS DETECTED ===" << endl;
            output << "Total errors: " << errors.size() << "\n" << endl;
            
            for (const auto& error : errors) {
                output << "Error at line " << error.line << ", column " << error.column << ": ";
                switch (error.errorType) {
                    case ScopeError::UndeclaredVariableAccessed:
                        output << "Undeclared variable accessed: '" << error.symbolName << "'";
                        break;
                    case ScopeError::UndefinedFunctionCalled:
                        output << "Undefined function called: '" << error.symbolName << "'";
                        break;
                    case ScopeError::VariableRedefinition:
                        output << "Variable redefinition: '" << error.symbolName << "'";
                        break;
                    case ScopeError::FunctionPrototypeRedefinition:
                        output << "Function redefinition: '" << error.symbolName << "'";
                        break;
                }
                output << endl;
            }
        } else {
            output << "\n✓ No scope errors found!" << endl;
        }
        
        output << "\n=== SCOPE ANALYSIS COMPLETE ===\n" << endl;
        
        // Print to console
        string outputStr = output.str();
        cout << outputStr;
        
        // Write to file
        ofstream outFile("tester/scope_analysis_output.txt");
        if (outFile.is_open()) {
            outFile << outputStr;
            outFile.close();
            cout << "\n✓ Scope analysis output written to: tester/scope_analysis_output.txt" << endl;
        } else {
            cerr << "\n✗ Error: Could not open output file for writing." << endl;
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}