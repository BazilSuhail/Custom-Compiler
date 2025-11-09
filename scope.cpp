#include "compiler.h"
#include <stack>
#include <set>

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

class ScopeAnalyzer {
private:
    unique_ptr<ScopeFrame> globalScope;
    ScopeFrame* currentScope;
    vector<ScopeError> errors;
    vector<Token> tokens;
    size_t currentTokenIndex;

    // Walks up scope chain to find symbol
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

    void enterScope() {
        auto newScope = make_unique<ScopeFrame>(currentScope, currentScope->level + 1);
        ScopeFrame* newScopePtr = newScope.get();
        currentScope->children.push_back(move(newScope));
        currentScope = newScopePtr;
    }

    void exitScope() {
        if (currentScope->parent != nullptr) {
            currentScope = currentScope->parent;
        }
    }

    void addError(ScopeErrorType type, const string& name, int line, int col) {
        errors.push_back(ScopeError(type, name, line, col));
    }

    // C-style: compare parameter types only, not names
    bool areFunctionSignaturesEqual(const vector<pair<TokenType, string>>& params1, 
                                   const vector<pair<TokenType, string>>& params2) {
        if (params1.size() != params2.size()) return false;
        for (size_t i = 0; i < params1.size(); i++) {
            if (params1[i].first != params2[i].first) return false;
        }
        return true;
    }

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

    void analyzeFunctionDecl(const FunctionDecl& func, int line, int col) {
    SymbolInfo* localSym = currentScope->findSymbol(func.name);
    
    if (localSym) {
        if (localSym->isFunction) {
            if (areFunctionSignaturesEqual(localSym->params, func.params)) {
                if (localSym->isPrototype) {
                    // Replace prototype with definition
                    localSym->isPrototype = false;
                } else {
                    addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col);
                }
            } else {
                addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
            }
        } else {
            addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
        }
    } else {
        SymbolInfo* existing = lookupSymbol(func.name);
        
        if (existing) {
            if (existing->isFunction) {
                if (areFunctionSignaturesEqual(existing->params, func.params)) {
                    if (!existing->isPrototype) {
                        // In C, can't redefine function even from parent scope
                        addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col);
                    }
                } else {
                    addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
                }
            } else {
                addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
            }
        }
        
        currentScope->addSymbol(SymbolInfo(func.returnType, func.name, line, col, true, false, false, false, func.params));
    }
    
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
    
    exitScope();
}

    void analyzeFunctionProto(const FunctionProto& proto, int line, int col) {
        SymbolInfo* existing = lookupSymbol(proto.name);
        
        if (existing && existing->isFunction) {
            if (areFunctionSignaturesEqual(existing->params, proto.params)) {
                if (existing->isPrototype) {
                    addError(ScopeErrorType::FunctionPrototypeRedefinition, proto.name, line, col);
                }
            } else {
                addError(ScopeErrorType::ConflictingDeclaration, proto.name, line, col);
            }
        } else if (existing) {
            addError(ScopeErrorType::ConflictingDeclaration, proto.name, line, col);
        } else {
            currentScope->addSymbol(SymbolInfo(proto.returnType, proto.name, line, col, true, false, false, true, proto.params));
        }
    }

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

    void analyzeMainDecl(const MainDecl& main) {
        enterScope();
        
        for (const auto& stmt : main.body) {
            analyzeNode(stmt->node);
        }
        
        exitScope();
    }

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

    void analyzeIdentifier(const Identifier& ident, int line, int col) {
        SymbolInfo* sym = lookupSymbol(ident.name);
        if (!sym) {
            addError(ScopeErrorType::UndeclaredVariableAccessed, ident.name, line, col);
        }
    }

    void analyzeBinaryExpr(const BinaryExpr& expr) {
        if (expr.left) analyzeExpression(expr.left->node);
        if (expr.right) analyzeExpression(expr.right->node);
    }

    void analyzeUnaryExpr(const UnaryExpr& expr) {
        if (expr.operand) analyzeExpression(expr.operand->node);
    }

    void analyzeExpression(const ASTNodeVariant& expr) {
        visit([this](const auto& node) {
            using T = decay_t<decltype(node)>;
            if constexpr (is_same_v<T, IntLiteral> || is_same_v<T, FloatLiteral> || 
                         is_same_v<T, StringLiteral> || is_same_v<T, CharLiteral> || 
                         is_same_v<T, BoolLiteral>) {
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

    void analyzeIfStmt(const IfStmt& stmt) {
        if (stmt.condition) analyzeExpression(stmt.condition->node);
        
        enterScope();
        for (const auto& ifStmt : stmt.ifBody) {
            analyzeNode(ifStmt->node);
        }
        exitScope();
        
        enterScope();
        for (const auto& elseStmt : stmt.elseBody) {
            analyzeNode(elseStmt->node);
        }
        exitScope();
    }

    void analyzeWhileStmt(const WhileStmt& stmt) {
        if (stmt.condition) analyzeExpression(stmt.condition->node);
        
        enterScope();
        for (const auto& bodyStmt : stmt.body) {
            analyzeNode(bodyStmt->node);
        }
        exitScope();
    }

    void analyzeDoWhileStmt(const DoWhileStmt& stmt) {
        enterScope();
        if (stmt.body) {
            analyzeNode(stmt.body->node);
        }
        exitScope();
        
        if (stmt.condition) analyzeExpression(stmt.condition->node);
    }

    void analyzeForStmt(const ForStmt& stmt) {
        enterScope();
        
        if (stmt.init) analyzeNode(stmt.init->node);
        if (stmt.condition) analyzeExpression(stmt.condition->node);
        if (stmt.update) analyzeExpression(stmt.update->node);
        
        if (stmt.body) {
            analyzeNode(stmt.body->node);
        }
        
        exitScope();
    }

    void analyzeSwitchStmt(const SwitchStmt& stmt) {
        if (stmt.expression) analyzeExpression(stmt.expression->node);
        
        for (const auto& caseStmt : stmt.cases) {
            if (caseStmt) {
                enterScope();
                analyzeNode(caseStmt->node);
                exitScope();
            }
        }
        
        enterScope();
        for (const auto& defaultStmt : stmt.defaultBody) {
            analyzeNode(defaultStmt->node);
        }
        exitScope();
    }

    void analyzeReturnStmt(const ReturnStmt& stmt) {
        if (stmt.value) {
            analyzeExpression(stmt.value->node);
        }
    }

    void analyzePrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) {
            analyzeExpression(arg->node);
        }
    }

    void analyzeExpressionStmt(const ExpressionStmt& stmt) {
        analyzeExpression(stmt.expr->node);
    }

    void analyzeBlockStmt(const BlockStmt& block) {
        enterScope();
        for (const auto& stmt : block.body) {
            analyzeNode(stmt->node);
        }
        exitScope();
    }

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

void performScopeAnalysis(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    ScopeAnalyzer analyzer;
    vector<ScopeError> errors = analyzer.analyze(ast, tokens);

    if (!errors.empty()) {
        cout << "\n=== Scope Analysis Errors ===\n";
        for (const auto& error : errors) {
            cout << "[Scope Error] " << error.message << ")\n";
            //cout << "[Scope Error] " << error.message << " (line " << error.line << ", col " << error.column << ")\n";
        }
        cout << "Scope analysis failed with " << errors.size() << " error(s)\n";
        exit(EXIT_FAILURE);
    }

    cout << "\n=== Scope Analysis Successful ===\n";
    cout << "No scope errors found.\n";
}