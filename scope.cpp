#include "compiler.h"
#include <stack>
#include <set>

// === Symbol Information ===
struct SymbolInfo {
    TokenType type;
    string name;
    int line;
    int column;
    bool isFunction;
    bool isEnum;
    bool isEnumValue;
    bool isPrototype; // New field to distinguish prototypes from definitions
    vector<pair<TokenType, string>> params; // For functions
    
    SymbolInfo(TokenType t, const string& n, int l, int c, bool isFunc = false, bool isEnumSym = false, bool isEnumVal = false, bool isProto = false, vector<pair<TokenType, string>> p = {})
        : type(t), name(n), line(l), column(c), isFunction(isFunc), isEnum(isEnumSym), isEnumValue(isEnumVal), isPrototype(isProto), params(p) {}
};

// === Scope Frame (Spaghetti Stack Node) ===
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

// === Scope Analyzer ===
class ScopeAnalyzer {
private:
    unique_ptr<ScopeFrame> globalScope;
    ScopeFrame* currentScope;
    vector<ScopeError> errors;
    vector<Token> tokens; // For line/column info
    size_t currentTokenIndex;

    // Find a symbol in the current scope and all parent scopes
    SymbolInfo* lookupSymbol(const string& name) {
        ScopeFrame* frame = currentScope;
        while (frame != nullptr) {
            SymbolInfo* sym = frame->findSymbol(name);
            if (sym != nullptr) {
                return sym;
            }
            frame = frame->parent;
        }
        return nullptr;
    }

    // Create a new child scope
    void enterScope() {
        auto newScope = make_unique<ScopeFrame>(currentScope, currentScope->level + 1);
        ScopeFrame* newScopePtr = newScope.get();
        currentScope->children.push_back(move(newScope));
        currentScope = newScopePtr;
    }

    // Exit the current scope
    void exitScope() {
        if (currentScope->parent != nullptr) {
            currentScope = currentScope->parent;
        }
    }

    // Add an error
    void addError(ScopeErrorType type, const string& name, int line, int col) {
        errors.push_back(ScopeError(type, name, line, col));
    }

    // Check if two function signatures are the same (C-style: types only, not names)
    bool areFunctionSignaturesEqual(const vector<pair<TokenType, string>>& params1, 
                                   const vector<pair<TokenType, string>>& params2) {
        if (params1.size() != params2.size()) return false;
        for (size_t i = 0; i < params1.size(); i++) {
            if (params1[i].first != params2[i].first) return false;  // Compare types only
        }
        return true;
    }

    // Analyze variable declaration
    void analyzeVarDecl(const VarDecl& decl, int line, int col) {
        if (currentScope->hasSymbol(decl.name)) {
            addError(ScopeErrorType::VariableRedefinition, decl.name, line, col);
        } else {
            SymbolInfo* existing = lookupSymbol(decl.name);
            if (existing && (existing->isFunction || existing->isEnum || existing->isEnumValue)) {
                addError(ScopeErrorType::ConflictingDeclaration, decl.name, line, col);
            } else {
                currentScope->addSymbol(SymbolInfo(decl.type, decl.name, line, col, false));
            }
        }
        
        if (decl.initializer) {
            analyzeExpression(decl.initializer->node);
        }
    }

    // Analyze function declaration
    void analyzeFunctionDecl(const FunctionDecl& func, int line, int col) {
        // Check globally for existing function with same name
        SymbolInfo* existing = lookupSymbol(func.name);
        
        if (existing && existing->isFunction) {
            if (areFunctionSignaturesEqual(existing->params, func.params)) {
                if (existing->isPrototype) {
                    // Replace prototype with definition in current scope
                    currentScope->addSymbol(SymbolInfo(func.returnType, func.name, line, col, true, false, false, false, func.params));
                } else {
                    addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col);
                }
            } else {
                addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
            }
        } else if (existing) {
            addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
        } else {
            // Add function to current scope (which should be global scope at this point)
            currentScope->addSymbol(SymbolInfo(func.returnType, func.name, line, col, true, false, false, false, func.params));
        }
        
        // Enter function scope for parameters
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        
        set<string> paramNames;
        for (const auto& param : func.params) {
            if (paramNames.count(param.second) > 0) {
                addError(ScopeErrorType::ParameterRedefinition, param.second, line, col);
            } else {
                paramNames.insert(param.second);
                currentScope->addSymbol(SymbolInfo(param.first, param.second, line, col, false));
            }
        }
        
        for (const auto& stmt : func.body) {
            analyzeNode(stmt->node);
        }
        
        // Exit function scope and return to original scope
        currentScope = originalScope;
    }

    // Analyze function prototype
    void analyzeFunctionProto(const FunctionProto& proto, int line, int col) {
        SymbolInfo* existing = lookupSymbol(proto.name); // Check globally
        
        if (existing && existing->isFunction) {
            if (areFunctionSignaturesEqual(existing->params, proto.params)) {
                if (existing->isPrototype) {
                    addError(ScopeErrorType::FunctionPrototypeRedefinition, proto.name, line, col);
                }
                // If existing is a definition, adding prototype is OK
            } else {
                addError(ScopeErrorType::ConflictingDeclaration, proto.name, line, col);
            }
        } else if (existing) {
            addError(ScopeErrorType::ConflictingDeclaration, proto.name, line, col);
        } else {
            currentScope->addSymbol(SymbolInfo(proto.returnType, proto.name, line, col, true, false, false, true, proto.params));
        }
    }

    // Analyze enum declaration
    void analyzeEnumDecl(const EnumDecl& enm, int line, int col) {
        if (currentScope->hasSymbol(enm.name)) {
            addError(ScopeErrorType::EnumRedefinition, enm.name, line, col);
        } else {
            SymbolInfo* existing = lookupSymbol(enm.name);
            if (existing) {
                addError(ScopeErrorType::ConflictingDeclaration, enm.name, line, col);
            } else {
                currentScope->addSymbol(SymbolInfo(T_ENUM, enm.name, line, col, false, true, false, false, {}));
            }
        }
        
        if (holds_alternative<EnumValueList>(enm.values->node)) {
            const auto& valueList = get<EnumValueList>(enm.values->node);
            set<string> enumValues;
            
            for (const string& value : valueList.values) {
                if (enumValues.count(value) > 0) {
                    addError(ScopeErrorType::EnumVariantRedefinition, value, line, col);
                } else {
                    enumValues.insert(value);
                    SymbolInfo* existing = lookupSymbol(value);
                    if (existing) {
                        addError(ScopeErrorType::ConflictingDeclaration, value, line, col);
                    } else {
                        currentScope->addSymbol(SymbolInfo(T_INT, value, line, col, false, false, true, false, {}));
                    }
                }
            }
        }
    }

    // Analyze main declaration
    void analyzeMainDecl(const MainDecl& main) {
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        
        for (const auto& stmt : main.body) {
            analyzeNode(stmt->node);
        }
        
        // Exit main scope and return to original scope
        currentScope = originalScope;
    }

    // Analyze function call
    void analyzeCallExpr(const CallExpr& call, int line, int col) {
        if (holds_alternative<Identifier>(call.callee->node)) {
            const auto& ident = get<Identifier>(call.callee->node);
            SymbolInfo* funcSym = lookupSymbol(ident.name);
            if (!funcSym || !funcSym->isFunction) {
                addError(ScopeErrorType::UndefinedFunctionCalled, ident.name, line, col);
            }
        } else {
            analyzeExpression(call.callee->node);
        }
        
        for (const auto& arg : call.args) {
            analyzeExpression(arg->node);
        }
    }

    // Analyze identifier (variable access)
    void analyzeIdentifier(const Identifier& ident, int line, int col) {
        SymbolInfo* sym = lookupSymbol(ident.name);
        if (!sym) {
            addError(ScopeErrorType::UndeclaredVariableAccessed, ident.name, line, col);
        }
    }

    // Analyze binary expression
    void analyzeBinaryExpr(const BinaryExpr& expr) {
        if (expr.left) analyzeExpression(expr.left->node);
        if (expr.right) analyzeExpression(expr.right->node);
    }

    // Analyze unary expression
    void analyzeUnaryExpr(const UnaryExpr& expr) {
        if (expr.operand) analyzeExpression(expr.operand->node);
    }

    // Analyze expression
    void analyzeExpression(const ASTNodeVariant& expr) {
        visit([this](const auto& node) {
            using T = decay_t<decltype(node)>;
            if constexpr (is_same_v<T, IntLiteral> || is_same_v<T, FloatLiteral> || 
                         is_same_v<T, StringLiteral> || is_same_v<T, CharLiteral> || 
                         is_same_v<T, BoolLiteral>) {
                // Literals don't need scope analysis
            } else if constexpr (is_same_v<T, Identifier>) {
                analyzeIdentifier(node, -1, -1);
            } else if constexpr (is_same_v<T, BinaryExpr>) {
                analyzeBinaryExpr(node);
            } else if constexpr (is_same_v<T, UnaryExpr>) {
                analyzeUnaryExpr(node);
            } else if constexpr (is_same_v<T, CallExpr>) {
                analyzeCallExpr(node, -1, -1);
            }
        }, expr);
    }

    // Analyze if statement
    void analyzeIfStmt(const IfStmt& stmt) {
        if (stmt.condition) analyzeExpression(stmt.condition->node);
        
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        
        for (const auto& ifStmt : stmt.ifBody) {
            analyzeNode(ifStmt->node);
        }
        
        currentScope = originalScope; // Return to original scope
        
        if (!stmt.elseBody.empty()) {
            enterScope();
            for (const auto& elseStmt : stmt.elseBody) {
                analyzeNode(elseStmt->node);
            }
            currentScope = originalScope; // Return to original scope again
        }
    }

    // Analyze while statement
    void analyzeWhileStmt(const WhileStmt& stmt) {
        if (stmt.condition) analyzeExpression(stmt.condition->node);
        
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        
        for (const auto& bodyStmt : stmt.body) {
            analyzeNode(bodyStmt->node);
        }
        
        currentScope = originalScope; // Return to original scope
    }

    // Analyze do-while statement
    void analyzeDoWhileStmt(const DoWhileStmt& stmt) {
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        
        if (stmt.body) {
            analyzeNode(stmt.body->node);
        }
        
        currentScope = originalScope; // Return to original scope
        
        if (stmt.condition) analyzeExpression(stmt.condition->node);
    }

    // Analyze for statement
    void analyzeForStmt(const ForStmt& stmt) {
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        
        if (stmt.init) analyzeNode(stmt.init->node);
        if (stmt.condition) analyzeExpression(stmt.condition->node);
        if (stmt.update) analyzeExpression(stmt.update->node);
        
        if (stmt.body) {
            analyzeNode(stmt.body->node);
        }
        
        currentScope = originalScope; // Return to original scope
    }

    // Analyze switch statement
    void analyzeSwitchStmt(const SwitchStmt& stmt) {
        if (stmt.expression) analyzeExpression(stmt.expression->node);
        
        for (const auto& caseStmt : stmt.cases) {
            if (caseStmt) {
                ScopeFrame* originalScope = currentScope; // Save current scope
                enterScope();
                analyzeNode(caseStmt->node);
                currentScope = originalScope; // Return to original scope
            }
        }
        
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        for (const auto& defaultStmt : stmt.defaultBody) {
            analyzeNode(defaultStmt->node);
        }
        currentScope = originalScope; // Return to original scope
    }

    // Analyze return statement
    void analyzeReturnStmt(const ReturnStmt& stmt) {
        if (stmt.value) {
            analyzeExpression(stmt.value->node);
        }
    }

    // Analyze print statement
    void analyzePrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) {
            analyzeExpression(arg->node);
        }
    }

    // Analyze expression statement
    void analyzeExpressionStmt(const ExpressionStmt& stmt) {
        analyzeExpression(stmt.expr->node);
    }

    // Analyze block statement
    void analyzeBlockStmt(const BlockStmt& block) {
        ScopeFrame* originalScope = currentScope; // Save current scope
        enterScope();
        
        for (const auto& stmt : block.body) {
            analyzeNode(stmt->node);
        }
        
        currentScope = originalScope; // Return to original scope
    }

    // Main analysis function for AST nodes
    void analyzeNode(const ASTNodeVariant& node) {
        visit([this](const auto& n) {
            using T = decay_t<decltype(n)>;
            
            if constexpr (is_same_v<T, VarDecl>) {
                analyzeVarDecl(n, -1, -1);
            } else if constexpr (is_same_v<T, FunctionDecl>) {
                analyzeFunctionDecl(n, -1, -1);
            } else if constexpr (is_same_v<T, FunctionProto>) {
                analyzeFunctionProto(n, -1, -1);
            } else if constexpr (is_same_v<T, EnumDecl>) {
                analyzeEnumDecl(n, -1, -1);
            } else if constexpr (is_same_v<T, MainDecl>) {
                analyzeMainDecl(n);
            } else if constexpr (is_same_v<T, CallExpr>) {
                analyzeCallExpr(n, -1, -1);
            } else if constexpr (is_same_v<T, Identifier>) {
                analyzeIdentifier(n, -1, -1);
            } else if constexpr (is_same_v<T, BinaryExpr>) {
                analyzeBinaryExpr(n);
            } else if constexpr (is_same_v<T, UnaryExpr>) {
                analyzeUnaryExpr(n);
            } else if constexpr (is_same_v<T, IfStmt>) {
                analyzeIfStmt(n);
            } else if constexpr (is_same_v<T, WhileStmt>) {
                analyzeWhileStmt(n);
            } else if constexpr (is_same_v<T, DoWhileStmt>) {
                analyzeDoWhileStmt(n);
            } else if constexpr (is_same_v<T, ForStmt>) {
                analyzeForStmt(n);
            } else if constexpr (is_same_v<T, SwitchStmt>) {
                analyzeSwitchStmt(n);
            } else if constexpr (is_same_v<T, ReturnStmt>) {
                analyzeReturnStmt(n);
            } else if constexpr (is_same_v<T, PrintStmt>) {
                analyzePrintStmt(n);
            } else if constexpr (is_same_v<T, ExpressionStmt>) {
                analyzeExpressionStmt(n);
            } else if constexpr (is_same_v<T, BlockStmt>) {
                analyzeBlockStmt(n);
            }
        }, node);
    }

public:
    ScopeAnalyzer() {
        globalScope = make_unique<ScopeFrame>(nullptr, 0);
        currentScope = globalScope.get();
    }

    vector<ScopeError> analyze(const vector<ASTPtr>& ast, const vector<Token>& tokenList) {
        tokens = tokenList;
        errors.clear();
        
        for (const auto& node : ast) {
            analyzeNode(node->node);
        }
        
        return errors;
    }
};

// Function to perform scope analysis
void performScopeAnalysis(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    ScopeAnalyzer analyzer;
    vector<ScopeError> errors = analyzer.analyze(ast, tokens);

    if (!errors.empty()) {
        cout << "\n=== Scope Analysis Errors ===\n";
        for (const auto& error : errors) {
            cout << "[Scope Error] " << error.message
                 << " (line " << error.line << ", col " << error.column << ")\n";
        }
        cout << "Scope analysis failed with " << errors.size() << " error(s)\n";
        exit(EXIT_FAILURE);
    }

    cout << "\n=== Scope Analysis Successful ===\n";
    cout << "No scope errors found.\n";
}