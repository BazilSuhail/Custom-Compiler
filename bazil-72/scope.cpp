#include "compiler.h"
 
class ScopeAnalyzer {
private:
    unique_ptr<ScopeFrame> globalScope;
    ScopeFrame* currentScope;
    vector<ScopeError> errors;
    vector<Token> tokens;
    size_t currentTokenIndex;
    
    // Store all declared symbols for forward reference checking
    unordered_map<string, vector<SymbolInfo>> allDeclaredSymbols;

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

    // Check if a symbol exists anywhere in the program (for forward reference)
    bool isSymbolDeclaredAnywhere(const string& name) {
        return allDeclaredSymbols.find(name) != allDeclaredSymbols.end();
    }

    // Check if function exists with matching signature anywhere
    bool isFunctionDeclaredAnywhere(const string& name, const vector<pair<TokenType, string>>& params) {
        auto it = allDeclaredSymbols.find(name);
        if (it != allDeclaredSymbols.end()) {
            for (const auto& sym : it->second) {
                if (sym.isFunction && areFunctionSignaturesEqual(sym.params, params)) {
                    return true;
                }
            }
        }
        return false;
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

    void analyzeVarDecl(const VarDecl& decl) {
        int line = decl.line;
        int col = decl.column;
        
        // Check if enum is being used as variable type
        if (decl.type == T_ENUM) {
            SymbolInfo* enumType = lookupSymbol(decl.name);
            if (enumType && enumType->isEnum) {
                // This is actually an enum declaration, not a variable with enum type
            } else {
                // If trying to declare variable with enum type, check if enum exists
                SymbolInfo* enumSym = lookupSymbol(decl.name);
                if (!enumSym || !enumSym->isEnum) {
                    addError(ScopeErrorType::UndeclaredVariableAccessed, decl.name, line, col);
                }
            }
        }
        
        if (currentScope->hasSymbol(decl.name)) {
            addError(ScopeErrorType::VariableRedefinition, decl.name, line, col);
        } else {
            SymbolInfo* existing = lookupSymbol(decl.name);
            if (existing && (existing->isFunction || existing->isEnum || existing->isEnumValue)) {
                addError(ScopeErrorType::ConflictingDeclaration, decl.name, line, col);
            } else {
                currentScope->addSymbol(SymbolInfo(decl.type, decl.name, line, col, false));
                allDeclaredSymbols[decl.name].push_back(SymbolInfo(decl.type, decl.name, line, col, false));
            }
        }
        
        if (decl.initializer) {
            analyzeExpression(decl.initializer->node);
        }
    }

    void analyzeFunctionDecl(const FunctionDecl& func) {
        int line = func.line;
        int col = func.column;
        
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
                    addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col); // Changed from ConflictingDeclaration
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
                        addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col); // Changed from ConflictingDeclaration
                    }
                } else {
                    addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
                }
            }
            
            currentScope->addSymbol(SymbolInfo(func.returnType, func.name, line, col, true, false, false, false, func.params));
            allDeclaredSymbols[func.name].push_back(SymbolInfo(func.returnType, func.name, line, col, true, false, false, false, func.params));
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

    void analyzeFunctionProto(const FunctionProto& proto) {
        int line = proto.line;
        int col = proto.column;
        
        SymbolInfo* existing = lookupSymbol(proto.name);
        
        if (existing && existing->isFunction) {
            if (areFunctionSignaturesEqual(existing->params, proto.params)) {
                if (existing->isPrototype) {
                    addError(ScopeErrorType::FunctionPrototypeRedefinition, proto.name, line, col);
                }
            } else {
                addError(ScopeErrorType::ConflictingFunctionDefinition, proto.name, line, col); // Changed from ConflictingDeclaration
            }
        } else if (existing) {
            addError(ScopeErrorType::ConflictingDeclaration, proto.name, line, col); // This handles var/fun conflicts
        } else {
            currentScope->addSymbol(SymbolInfo(proto.returnType, proto.name, line, col, true, false, false, true, proto.params));
            allDeclaredSymbols[proto.name].push_back(SymbolInfo(proto.returnType, proto.name, line, col, true, false, false, true, proto.params));
        }
    }

    void analyzeEnumDecl(const EnumDecl& enm) {
        int line = enm.line;
        int col = enm.column;
        
        // Check if enum is declared in global scope (not inside function)
        if (currentScope->level > 0) { // Not global scope
            addError(ScopeErrorType::InvalidStorageClassUsage, enm.name, line, col);
        }
        
        if (currentScope->hasSymbol(enm.name)) {
            addError(ScopeErrorType::EnumRedefinition, enm.name, line, col);
        } else {
            SymbolInfo* existing = lookupSymbol(enm.name);
            if (existing) {
                addError(ScopeErrorType::ConflictingDeclaration, enm.name, line, col);
            } else {
                currentScope->addSymbol(SymbolInfo(T_ENUM, enm.name, line, col, false, true, false, false, {}));
                allDeclaredSymbols[enm.name].push_back(SymbolInfo(T_ENUM, enm.name, line, col, false, true, false, false, {}));
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
                        allDeclaredSymbols[value].push_back(SymbolInfo(T_INT, value, line, col, false, false, true, false, {}));
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

    void analyzeCallExpr(const CallExpr& call) {
        int line = call.line;
        int col = call.column;
        
        if (holds_alternative<Identifier>(call.callee->node)) {
            const auto& ident = get<Identifier>(call.callee->node);
            SymbolInfo* funcSym = lookupSymbol(ident.name);
            
            if (!funcSym || !funcSym->isFunction) {
                // Check if function exists anywhere in the program (forward reference)
                if (isSymbolDeclaredAnywhere(ident.name)) {
                    addError(ScopeErrorType::InvalidForwardReference, ident.name, line, col);
                } else {
                    addError(ScopeErrorType::UndefinedFunctionCalled, ident.name, line, col);
                }
            }
        } else {
            analyzeExpression(call.callee->node);
        }
        
        for (const auto& arg : call.args) {
            analyzeExpression(arg->node);
        }
    }

    void analyzeIdentifier(const Identifier& ident) {
        int line = ident.line;
        int col = ident.column;
        
        SymbolInfo* sym = lookupSymbol(ident.name);
        if (!sym) {
            // Check if this symbol exists anywhere in the program (forward reference)
            if (isSymbolDeclaredAnywhere(ident.name)) {
                addError(ScopeErrorType::InvalidForwardReference, ident.name, line, col);
            } else {
                addError(ScopeErrorType::UndeclaredVariableAccessed, ident.name, line, col);
            }
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
            if constexpr (is_same_v<T, IntLiteral>) {
                // Literals don't need scope analysis - they are values, not identifiers
            } else if constexpr (is_same_v<T, FloatLiteral>) {
                // Literals don't need scope analysis - they are values, not identifiers
            } else if constexpr (is_same_v<T, StringLiteral>) {
                // Literals don't need scope analysis - they are values, not identifiers
            } else if constexpr (is_same_v<T, CharLiteral>) {
                // Literals don't need scope analysis - they are values, not identifiers
            } else if constexpr (is_same_v<T, BoolLiteral>) {
                // Literals don't need scope analysis - they are values, not identifiers
            } else if constexpr (is_same_v<T, Identifier>) {
                analyzeIdentifier(node);
            } else if constexpr (is_same_v<T, BinaryExpr>) {
                analyzeBinaryExpr(node);
            } else if constexpr (is_same_v<T, UnaryExpr>) {
                analyzeUnaryExpr(node);
            } else if constexpr (is_same_v<T, CallExpr>) {
                analyzeCallExpr(node);
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
                analyzeVarDecl(n);
            } else if constexpr (is_same_v<T, FunctionDecl>) {
                analyzeFunctionDecl(n);
            } else if constexpr (is_same_v<T, FunctionProto>) {
                analyzeFunctionProto(n);
            } else if constexpr (is_same_v<T, EnumDecl>) {
                analyzeEnumDecl(n);
            } else if constexpr (is_same_v<T, MainDecl>) {
                analyzeMainDecl(n);
            } else if constexpr (is_same_v<T, CallExpr>) {
                analyzeCallExpr(n);
            } else if constexpr (is_same_v<T, Identifier>) {
                analyzeIdentifier(n);
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

    // Add this method to transfer ownership of the symbol table
    unique_ptr<ScopeFrame> transferSymbolTable() {
        return move(globalScope);
    }

    vector<ScopeError> analyze(const vector<ASTPtr>& ast, const vector<Token>& tokenList) {
        tokens = tokenList;
        errors.clear();
        allDeclaredSymbols.clear(); // Clear for fresh analysis
        
        // First pass: collect all declared symbols
        for (const auto& node : ast) {
            collectDeclarations(node->node);
        }
        
        // Second pass: perform actual analysis
        for (const auto& node : ast) {
            analyzeNode(node->node);
        }
        
        return errors;
    }
    
    const char* tokenTypeToString(TokenType type)  const {
        switch (type) {
            case T_INCLUDE: return "T_INCLUDE";
            case T_INT: return "T_INT";
            case T_FLOAT: return "T_FLOAT";
            case T_DOUBLE: return "T_DOUBLE";
            case T_CHAR: return "T_CHAR";
            case T_VOID: return "T_VOID";
            case T_BOOL: return "T_BOOL";
            case T_ENUM: return "T_ENUM";
            case T_IDENTIFIER: return "T_IDENTIFIER";
            case T_INTLIT: return "T_INTLIT";
            case T_FLOATLIT: return "T_FLOATLIT";
            case T_STRINGLIT: return "T_STRINGLIT";
            case T_CHARLIT: return "T_CHARLIT";
            case T_BOOLLIT: return "T_BOOLLIT";
            case T_STRING: return "T_STRING";
            case T_DO: return "T_DO";
            case T_SWITCH: return "T_SWITCH";
            case T_BREAK: return "T_BREAK";
            case T_FOR: return "T_FOR";
            case T_DEFAULT: return "T_DEFAULT";
            case T_CASE: return "T_CASE";
            case T_COLON: return "T_COLON";
            case T_LPAREN: return "T_LPAREN";
            case T_RPAREN: return "T_RPAREN";
            case T_LBRACE: return "T_LBRACE";
            case T_RBRACE: return "T_RBRACE";
            case T_LBRACKET: return "T_LBRACKET";
            case T_RBRACKET: return "T_RBRACKET";
            case T_SEMICOLON: return "T_SEMICOLON";
            case T_COMMA: return "T_COMMA";
            case T_DOT: return "T_DOT";
            case T_ASSIGNOP: return "T_ASSIGNOP";
            case T_EQUALOP: return "T_EQUALOP";
            case T_NE: return "T_NE";
            case T_LT: return "T_LT";
            case T_GT: return "T_GT";
            case T_LE: return "T_LE";
            case T_GE: return "T_GE";
            case T_BITAND: return "T_BITAND";      // Added
            case T_BITOR: return "T_BITOR";        // Added
            case T_BITXOR: return "T_BITXOR";      // Added
            case T_BITLSHIFT: return "T_BITLSHIFT"; // Added
            case T_BITRSHIFT: return "T_BITRSHIFT"; // Added
            case T_PLUS: return "T_PLUS";
            case T_MINUS: return "T_MINUS";
            case T_MULTIPLY: return "T_MULTIPLY";
            case T_DIVIDE: return "T_DIVIDE";
            case T_MODULO: return "T_MODULO";
            case T_INCREMENT: return "T_INCREMENT";
            case T_DECREMENT: return "T_DECREMENT";
            case T_AND: return "T_AND";
            case T_OR: return "T_OR";
            case T_NOT: return "T_NOT";
            case T_IF: return "T_IF";
            case T_ELSE: return "T_ELSE";
            case T_WHILE: return "T_WHILE";
            case T_RETURN: return "T_RETURN";
            case T_PRINT: return "T_PRINT";
            case T_MAIN: return "T_MAIN";
            case T_SINGLE_COMMENT: return "T_SINGLE_COMMENT";
            case T_MULTI_COMMENT: return "T_MULTI_COMMENT";
            case T_ERROR: return "T_ERROR";
            case T_EOF: return "T_EOF";
            default: return "T_UNKNOWN";
        }
    }

        // Add the print method here
    void printSymbolTable(const ScopeFrame* frame, int indent = 0) const {
        if (!frame) return; // Safety check

        string indentStr(indent, ' ');

        cout << indentStr << "Scope Level " << frame->level << ":\n";
        
        if (frame->symbols.empty()) {
            cout << indentStr << "  (No symbols)\n";
        } else {
            cout << indentStr << "  Symbols:\n";
            for (const auto& [name, sym] : frame->symbols) {
                cout << indentStr << "    Name: '" << name << "', Type: " << this->tokenTypeToString(sym.type) // Use your switch function
                    << ", IsFunction: " << (sym.isFunction ? "Yes" : "No")
                    << ", IsEnum: " << (sym.isEnum ? "Yes" : "No")
                    << ", IsEnumValue: " << (sym.isEnumValue ? "Yes" : "No")
                    << ", IsPrototype: " << (sym.isPrototype ? "Yes" : "No");
                
                if (!sym.params.empty()) {
                    cout << ", Params: (";
                    for (size_t i = 0; i < sym.params.size(); ++i) {
                        cout << this->tokenTypeToString(sym.params[i].first) << " " << sym.params[i].second; // Use your switch function
                        if (i < sym.params.size() - 1) cout << ", ";
                    }
                    cout << ")";
                }
                cout << "\n";
            }
        }

        cout << indentStr << "  Children:\n";
        for (const auto& child : frame->children) {
            printSymbolTable(child.get(), indent + 4); // Increase indent for children
        }
    }
    

    private:
    // Helper function to collect all declarations for forward reference checking
    void collectDeclarations(const ASTNodeVariant& node) {
        visit([this](const auto& n) {
            using T = decay_t<decltype(n)>;
            
            if constexpr (is_same_v<T, VarDecl>) {
                allDeclaredSymbols[n.name].push_back(SymbolInfo(n.type, n.name, n.line, n.column, false));
            } else if constexpr (is_same_v<T, FunctionDecl>) {
                allDeclaredSymbols[n.name].push_back(SymbolInfo(n.returnType, n.name, n.line, n.column, true, false, false, false, n.params));
            } else if constexpr (is_same_v<T, FunctionProto>) {
                allDeclaredSymbols[n.name].push_back(SymbolInfo(n.returnType, n.name, n.line, n.column, true, false, false, true, n.params));
            } else if constexpr (is_same_v<T, EnumDecl>) {
                allDeclaredSymbols[n.name].push_back(SymbolInfo(T_ENUM, n.name, n.line, n.column, false, true, false, false, {}));
            }
        }, node);
    }


    // Add a getter to access the global scope from outside without transferring ownership
    const ScopeFrame* getGlobalScope() const { return globalScope.get(); }
};

void performScopeAnalysis(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    try {
        ScopeAnalyzer analyzer;
        vector<ScopeError> errors = analyzer.analyze(ast, tokens);

        if (!errors.empty()) {
            cerr << "\n=== Scope Analysis Errors ===\n";
            for (const auto& error : errors) {
                cerr << "[Scope Error] " << error.message << ")\n";
            }
            cerr << "Scope analysis failed with " << errors.size() << " error(s)\n";
            exit(EXIT_FAILURE);
        }

        cout << "\n=== Scope Analysis Successful ===\n";
        cout << "No scope errors found.\n";

        //analyzer.printSymbolTable(analyzer.transferSymbolTable());
    }
    catch (const exception& e) {
        cerr << "[Scope Analysis Exception] " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }
}