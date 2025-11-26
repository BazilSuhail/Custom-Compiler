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

    // ===== SCOPE MANAGEMENT FUNCTIONS =====
    
    // Search for a symbol by walking up the scope chain from current to global
    SymbolInfo* lookupSymbol(const string& name) {
        ScopeFrame* frame = currentScope;
        
        // Keep searching parent scopes until we find the symbol or reach the end
        while (frame != nullptr) {
            SymbolInfo* sym = frame->findSymbol(name);
            if (sym != nullptr) {
                return sym;
            }
            frame = frame->parent;
        }
        
        return nullptr;  // Symbol not found
    }

    // Create a new nested scope and enter it
    void enterScope() {
        auto newScope = make_unique<ScopeFrame>(currentScope, currentScope->level + 1);
        ScopeFrame* newScopePtr = newScope.get();
        currentScope->children.push_back(move(newScope));
        currentScope = newScopePtr;
    }

    // Exit current scope and return to parent scope
    void exitScope() {
        if (currentScope->parent != nullptr) {
            currentScope = currentScope->parent;
        }
    }

    // Record a scope error with location information
    void addError(ScopeErrorType type, const string& name, int line, int col) {
        errors.push_back(ScopeError(type, name, line, col));
    }

    // ===== FORWARD REFERENCE CHECKING =====
    
    // Check if a symbol exists anywhere in the entire program
    bool isSymbolDeclaredAnywhere(const string& name) {
        return allDeclaredSymbols.find(name) != allDeclaredSymbols.end();
    }

    // Check if function with matching signature exists anywhere in program
    bool isFunctionDeclaredAnywhere(const string& name, const vector<pair<TokenType, string>>& params) {
        auto it = allDeclaredSymbols.find(name);
        if (it == allDeclaredSymbols.end()) {
            return false;
        }
        
        // Check each symbol with this name
        for (const auto& sym : it->second) {
            if (sym.isFunction && areFunctionSignaturesEqual(sym.params, params)) {
                return true;
            }
        }
        
        return false;
    }

    // ===== SIGNATURE COMPARISON =====
    
    // Compare function signatures (C-style: only parameter types matter, not names)
    bool areFunctionSignaturesEqual(const vector<pair<TokenType, string>>& params1, 
                                   const vector<pair<TokenType, string>>& params2) {
        // Different number of parameters means different signatures
        if (params1.size() != params2.size()) {
            return false;
        }
        
        // Check if each parameter type matches
        for (size_t i = 0; i < params1.size(); i++) {
            if (params1[i].first != params2[i].first) {
                return false;
            }
        }
        
        return true;
    }

    // ===== VARIABLE DECLARATION ANALYSIS =====
    
    // Analyze a variable declaration
    void analyzeVarDecl(const VarDecl& decl) {
        int line = decl.line;
        int col = decl.column;
        
        // Check for redefinition in current scope
        if (currentScope->hasSymbol(decl.name)) {
            addError(ScopeErrorType::VariableRedefinition, decl.name, line, col);
            return;
        }
        
        // Check for conflicts with functions, enums, or enum values
        SymbolInfo* existing = lookupSymbol(decl.name);
        if (existing != nullptr) {
            if (existing->isFunction || existing->isEnum || existing->isEnumValue) {
                addError(ScopeErrorType::ConflictingDeclaration, decl.name, line, col);
                return;
            }
        }
        
        // Add variable to current scope
        currentScope->addSymbol(SymbolInfo(decl.type, decl.name, line, col, false));
        allDeclaredSymbols[decl.name].push_back(SymbolInfo(decl.type, decl.name, line, col, false));
        
        // Analyze initializer if present
        if (decl.initializer) {
            analyzeExpressionNode(decl.initializer->node);
        }
    }

    // ===== FUNCTION DECLARATION ANALYSIS =====
    
    // Analyze a function definition
    void analyzeFunctionDecl(const FunctionDecl& func) {
        int line = func.line;
        int col = func.column;
        
        // Check for conflicts in current scope
        SymbolInfo* localSym = currentScope->findSymbol(func.name);
        
        if (localSym != nullptr) {
            handleLocalFunctionConflict(localSym, func, line, col);
        } else {
            handleParentScopeFunctionConflict(func, line, col);
        }
        
        // Add function to symbol table
        currentScope->addSymbol(SymbolInfo(func.returnType, func.name, line, col, true, false, false, false, func.params));
        allDeclaredSymbols[func.name].push_back(SymbolInfo(func.returnType, func.name, line, col, true, false, false, false, func.params));
        
        // Enter function scope and analyze parameters and body
        analyzeFunctionBody(func);
    }
    
    // Handle conflicts when function exists in current scope
    void handleLocalFunctionConflict(SymbolInfo* localSym, const FunctionDecl& func, int line, int col) {
        if (localSym->isFunction) {
            if (areFunctionSignaturesEqual(localSym->params, func.params)) {
                // Check if this is a prototype being replaced by definition
                if (localSym->isPrototype) {
                    localSym->isPrototype = false;  // Replace prototype
                } else {
                    addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col);
                }
            } else {
                addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col);
            }
        } else {
            addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
        }
    }
    
    // Handle conflicts when function exists in parent scope
    void handleParentScopeFunctionConflict(const FunctionDecl& func, int line, int col) {
        SymbolInfo* existing = lookupSymbol(func.name);
        
        if (existing == nullptr) {
            return;  // No conflict
        }
        
        if (existing->isFunction) {
            if (areFunctionSignaturesEqual(existing->params, func.params)) {
                if (!existing->isPrototype) {
                    // Cannot redefine function from parent scope
                    addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col);
                }
            } else {
                addError(ScopeErrorType::ConflictingFunctionDefinition, func.name, line, col);
            }
        } else {
            addError(ScopeErrorType::ConflictingDeclaration, func.name, line, col);
        }
    }
    
    // Analyze function parameters and body
    void analyzeFunctionBody(const FunctionDecl& func) {
        enterScope();
        
        // Check for duplicate parameter names
        set<string> paramNames;
        for (const auto& param : func.params) {
            if (paramNames.count(param.second) > 0) {
                addError(ScopeErrorType::ParameterRedefinition, param.second, func.line, func.column);
            } else {
                paramNames.insert(param.second);
                currentScope->addSymbol(SymbolInfo(param.first, param.second, func.line, func.column, false));
            }
        }
        
        // Analyze each statement in function body
        for (const auto& stmt : func.body) {
            analyzeASTNode(stmt->node);
        }
        
        exitScope();
    }

    // ===== FUNCTION PROTOTYPE ANALYSIS =====
    
    // Analyze a function prototype (forward declaration)
    void analyzeFunctionProto(const FunctionProto& proto) {
        int line = proto.line;
        int col = proto.column;
        
        SymbolInfo* existing = lookupSymbol(proto.name);
        
        if (existing != nullptr && existing->isFunction) {
            handlePrototypeConflict(existing, proto, line, col);
        } else if (existing != nullptr) {
            addError(ScopeErrorType::ConflictingDeclaration, proto.name, line, col);
        } else {
            // Add prototype to symbol table
            currentScope->addSymbol(SymbolInfo(proto.returnType, proto.name, line, col, true, false, false, true, proto.params));
            allDeclaredSymbols[proto.name].push_back(SymbolInfo(proto.returnType, proto.name, line, col, true, false, false, true, proto.params));
        }
    }
    
    // Handle conflicts when prototype already exists
    void handlePrototypeConflict(SymbolInfo* existing, const FunctionProto& proto, int line, int col) {
        if (areFunctionSignaturesEqual(existing->params, proto.params)) {
            if (existing->isPrototype) {
                addError(ScopeErrorType::FunctionPrototypeRedefinition, proto.name, line, col);
            }
        } else {
            addError(ScopeErrorType::ConflictingFunctionDefinition, proto.name, line, col);
        }
    }

    // ===== ENUM DECLARATION ANALYSIS =====
    
    // Analyze an enum declaration
    void analyzeEnumDecl(const EnumDecl& enm) {
        int line = enm.line;
        int col = enm.column;
        
        // Enums must be declared in global scope only
        if (currentScope->level > 0) {
            addError(ScopeErrorType::InvalidStorageClassUsage, enm.name, line, col);
            return;
        }
        
        // Check for enum redefinition
        if (currentScope->hasSymbol(enm.name)) {
            addError(ScopeErrorType::EnumRedefinition, enm.name, line, col);
            return;
        }
        
        // Check for conflicts with other symbols
        SymbolInfo* existing = lookupSymbol(enm.name);
        if (existing != nullptr) {
            addError(ScopeErrorType::ConflictingDeclaration, enm.name, line, col);
            return;
        }
        
        // Add enum to symbol table
        currentScope->addSymbol(SymbolInfo(T_ENUM, enm.name, line, col, false, true, false, false, {}));
        allDeclaredSymbols[enm.name].push_back(SymbolInfo(T_ENUM, enm.name, line, col, false, true, false, false, {}));
        
        // Analyze enum values
        analyzeEnumValues(enm, line, col);
    }
    
    // Analyze enum value list
    void analyzeEnumValues(const EnumDecl& enm, int line, int col) {
        if (!holds_alternative<EnumValueList>(enm.values->node)) {
            return;
        }
        
        const EnumValueList& valueList = get<EnumValueList>(enm.values->node);
        set<string> enumValues;
        
        for (const string& value : valueList.values) {
            // Check for duplicate enum values
            if (enumValues.count(value) > 0) {
                addError(ScopeErrorType::EnumVariantRedefinition, value, line, col);
                continue;
            }
            
            enumValues.insert(value);
            
            // Check for conflicts with other symbols
            SymbolInfo* existing = lookupSymbol(value);
            if (existing != nullptr) {
                addError(ScopeErrorType::ConflictingDeclaration, value, line, col);
            } else {
                currentScope->addSymbol(SymbolInfo(T_INT, value, line, col, false, false, true, false, {}));
                allDeclaredSymbols[value].push_back(SymbolInfo(T_INT, value, line, col, false, false, true, false, {}));
            }
        }
    }

    // ===== MAIN FUNCTION ANALYSIS =====
    
    // Analyze the main function
    void analyzeMainDecl(const MainDecl& main) {
        enterScope();
        
        for (const auto& stmt : main.body) {
            analyzeASTNode(stmt->node);
        }
        
        exitScope();
    }

    // ===== EXPRESSION ANALYSIS =====
    
    // Analyze a function call expression
    void analyzeCallExpr(const CallExpr& call) {
        int line = call.line;
        int col = call.column;
        
        // Check if callee is an identifier (function name)
        if (holds_alternative<Identifier>(call.callee->node)) {
            const Identifier& ident = get<Identifier>(call.callee->node);
            checkFunctionCall(ident.name, line, col);
        } else {
            analyzeExpressionNode(call.callee->node);
        }
        
        // Analyze all arguments
        for (const auto& arg : call.args) {
            analyzeExpressionNode(arg->node);
        }
    }
    
    // Check if function being called exists
    void checkFunctionCall(const string& funcName, int line, int col) {
        SymbolInfo* funcSym = lookupSymbol(funcName);
        
        if (funcSym == nullptr || !funcSym->isFunction) {
            // Check for forward reference
            if (isSymbolDeclaredAnywhere(funcName)) {
                addError(ScopeErrorType::InvalidForwardReference, funcName, line, col);
            } else {
                addError(ScopeErrorType::UndefinedFunctionCalled, funcName, line, col);
            }
        }
    }

    // Analyze an identifier (variable reference)
    void analyzeIdentifier(const Identifier& ident) {
        int line = ident.line;
        int col = ident.column;
        
        SymbolInfo* sym = lookupSymbol(ident.name);
        
        if (sym == nullptr) {
            // Check for forward reference
            if (isSymbolDeclaredAnywhere(ident.name)) {
                addError(ScopeErrorType::InvalidForwardReference, ident.name, line, col);
            } else {
                addError(ScopeErrorType::UndeclaredVariableAccessed, ident.name, line, col);
            }
        }
    }

    // Analyze a binary expression (e.g., a + b)
    void analyzeBinaryExpr(const BinaryExpr& expr) {
        if (expr.left) {
            analyzeExpressionNode(expr.left->node);
        }
        if (expr.right) {
            analyzeExpressionNode(expr.right->node);
        }
    }

    // Analyze a unary expression (e.g., -x, !flag)
    void analyzeUnaryExpr(const UnaryExpr& expr) {
        if (expr.operand) {
            analyzeExpressionNode(expr.operand->node);
        }
    }

    // Main expression analyzer - dispatches to appropriate handler
    void analyzeExpressionNode(const ASTNodeVariant& expr) {
        // Handle integer literals
        if (holds_alternative<IntLiteral>(expr)) {
            return;  // Literals don't need scope analysis
        }
        
        // Handle float literals
        if (holds_alternative<FloatLiteral>(expr)) {
            return;
        }
        
        // Handle string literals
        if (holds_alternative<StringLiteral>(expr)) {
            return;
        }
        
        // Handle char literals
        if (holds_alternative<CharLiteral>(expr)) {
            return;
        }
        
        // Handle bool literals
        if (holds_alternative<BoolLiteral>(expr)) {
            return;
        }
        
        // Handle identifiers
        if (holds_alternative<Identifier>(expr)) {
            analyzeIdentifier(get<Identifier>(expr));
            return;
        }
        
        // Handle binary expressions
        if (holds_alternative<BinaryExpr>(expr)) {
            analyzeBinaryExpr(get<BinaryExpr>(expr));
            return;
        }
        
        // Handle unary expressions
        if (holds_alternative<UnaryExpr>(expr)) {
            analyzeUnaryExpr(get<UnaryExpr>(expr));
            return;
        }
        
        // Handle function calls
        if (holds_alternative<CallExpr>(expr)) {
            analyzeCallExpr(get<CallExpr>(expr));
            return;
        }
    }

    // ===== STATEMENT ANALYSIS =====
    
    // Analyze an if statement
    void analyzeIfStmt(const IfStmt& stmt) {
        // Check condition
        if (stmt.condition) {
            analyzeExpressionNode(stmt.condition->node);
        }
        
        // Analyze if body in new scope
        enterScope();
        for (const auto& ifStmt : stmt.ifBody) {
            analyzeASTNode(ifStmt->node);
        }
        exitScope();
        
        // Analyze else body in new scope
        enterScope();
        for (const auto& elseStmt : stmt.elseBody) {
            analyzeASTNode(elseStmt->node);
        }
        exitScope();
    }

    // Analyze a while loop
    void analyzeWhileStmt(const WhileStmt& stmt) {
        // Check condition
        if (stmt.condition) {
            analyzeExpressionNode(stmt.condition->node);
        }
        
        // Analyze body in new scope
        enterScope();
        for (const auto& bodyStmt : stmt.body) {
            analyzeASTNode(bodyStmt->node);
        }
        exitScope();
    }

    // Analyze a do-while loop
    void analyzeDoWhileStmt(const DoWhileStmt& stmt) {
        // Analyze body in new scope
        enterScope();
        if (stmt.body) {
            analyzeASTNode(stmt.body->node);
        }
        exitScope();
        
        // Check condition
        if (stmt.condition) {
            analyzeExpressionNode(stmt.condition->node);
        }
    }

    // Analyze a for loop
    void analyzeForStmt(const ForStmt& stmt) {
        enterScope();
        
        // Analyze initialization
        if (stmt.init) {
            analyzeASTNode(stmt.init->node);
        }
        
        // Check condition
        if (stmt.condition) {
            analyzeExpressionNode(stmt.condition->node);
        }
        
        // Analyze update expression
        if (stmt.update) {
            analyzeExpressionNode(stmt.update->node);
        }
        
        // Analyze body
        if (stmt.body) {
            analyzeASTNode(stmt.body->node);
        }
        
        exitScope();
    }

    // Analyze a switch statement
    void analyzeSwitchStmt(const SwitchStmt& stmt) {
        // Check switch expression
        if (stmt.expression) {
            analyzeExpressionNode(stmt.expression->node);
        }
        
        // Analyze each case in its own scope
        for (const auto& caseStmt : stmt.cases) {
            if (caseStmt) {
                enterScope();
                analyzeASTNode(caseStmt->node);
                exitScope();
            }
        }
        
        // Analyze default case in its own scope
        enterScope();
        for (const auto& defaultStmt : stmt.defaultBody) {
            analyzeASTNode(defaultStmt->node);
        }
        exitScope();
    }

    // Analyze a return statement
    void analyzeReturnStmt(const ReturnStmt& stmt) {
        if (stmt.value) {
            analyzeExpressionNode(stmt.value->node);
        }
    }

    // Analyze a print statement
    void analyzePrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) {
            analyzeExpressionNode(arg->node);
        }
    }

    // Analyze an expression statement
    void analyzeExpressionStmt(const ExpressionStmt& stmt) {
        analyzeExpressionNode(stmt.expr->node);
    }

    // Analyze a block statement
    void analyzeBlockStmt(const BlockStmt& block) {
        enterScope();
        for (const auto& stmt : block.body) {
            analyzeASTNode(stmt->node);
        }
        exitScope();
    }

    // ===== MAIN NODE DISPATCHER =====
    
    // Main dispatcher - routes AST nodes to appropriate analyzer
    void analyzeASTNode(const ASTNodeVariant& node) {
        // Variable declaration
        if (holds_alternative<VarDecl>(node)) {
            analyzeVarDecl(get<VarDecl>(node));
            return;
        }
        
        // Function definition
        if (holds_alternative<FunctionDecl>(node)) {
            analyzeFunctionDecl(get<FunctionDecl>(node));
            return;
        }
        
        // Function prototype
        if (holds_alternative<FunctionProto>(node)) {
            analyzeFunctionProto(get<FunctionProto>(node));
            return;
        }
        
        // Enum declaration
        if (holds_alternative<EnumDecl>(node)) {
            analyzeEnumDecl(get<EnumDecl>(node));
            return;
        }
        
        // Main function
        if (holds_alternative<MainDecl>(node)) {
            analyzeMainDecl(get<MainDecl>(node));
            return;
        }
        
        // Function call
        if (holds_alternative<CallExpr>(node)) {
            analyzeCallExpr(get<CallExpr>(node));
            return;
        }
        
        // Identifier
        if (holds_alternative<Identifier>(node)) {
            analyzeIdentifier(get<Identifier>(node));
            return;
        }
        
        // Binary expression
        if (holds_alternative<BinaryExpr>(node)) {
            analyzeBinaryExpr(get<BinaryExpr>(node));
            return;
        }
        
        // Unary expression
        if (holds_alternative<UnaryExpr>(node)) {
            analyzeUnaryExpr(get<UnaryExpr>(node));
            return;
        }
        
        // If statement
        if (holds_alternative<IfStmt>(node)) {
            analyzeIfStmt(get<IfStmt>(node));
            return;
        }
        
        // While loop
        if (holds_alternative<WhileStmt>(node)) {
            analyzeWhileStmt(get<WhileStmt>(node));
            return;
        }
        
        // Do-while loop
        if (holds_alternative<DoWhileStmt>(node)) {
            analyzeDoWhileStmt(get<DoWhileStmt>(node));
            return;
        }
        
        // For loop
        if (holds_alternative<ForStmt>(node)) {
            analyzeForStmt(get<ForStmt>(node));
            return;
        }
        
        // Switch statement
        if (holds_alternative<SwitchStmt>(node)) {
            analyzeSwitchStmt(get<SwitchStmt>(node));
            return;
        }
        
        // Return statement
        if (holds_alternative<ReturnStmt>(node)) {
            analyzeReturnStmt(get<ReturnStmt>(node));
            return;
        }
        
        // Print statement
        if (holds_alternative<PrintStmt>(node)) {
            analyzePrintStmt(get<PrintStmt>(node));
            return;
        }
        
        // Expression statement
        if (holds_alternative<ExpressionStmt>(node)) {
            analyzeExpressionStmt(get<ExpressionStmt>(node));
            return;
        }
        
        // Block statement
        if (holds_alternative<BlockStmt>(node)) {
            analyzeBlockStmt(get<BlockStmt>(node));
            return;
        }
    }

    // ===== DECLARATION COLLECTOR =====
    
    // First pass: collect all declarations for forward reference checking
    void collectDeclarations(const ASTNodeVariant& node) {
        // Variable declaration
        if (holds_alternative<VarDecl>(node)) {
            const VarDecl& varDecl = get<VarDecl>(node);
            allDeclaredSymbols[varDecl.name].push_back(
                SymbolInfo(varDecl.type, varDecl.name, varDecl.line, varDecl.column, false)
            );
            return;
        }
        
        // Function definition
        if (holds_alternative<FunctionDecl>(node)) {
            const FunctionDecl& funcDecl = get<FunctionDecl>(node);
            allDeclaredSymbols[funcDecl.name].push_back(
                SymbolInfo(funcDecl.returnType, funcDecl.name, funcDecl.line, funcDecl.column, 
                          true, false, false, false, funcDecl.params)
            );
            return;
        }
        
        // Function prototype
        if (holds_alternative<FunctionProto>(node)) {
            const FunctionProto& protoDecl = get<FunctionProto>(node);
            allDeclaredSymbols[protoDecl.name].push_back(
                SymbolInfo(protoDecl.returnType, protoDecl.name, protoDecl.line, protoDecl.column, 
                          true, false, false, true, protoDecl.params)
            );
            return;
        }
        
        // Enum declaration
        if (holds_alternative<EnumDecl>(node)) {
            const EnumDecl& enumDecl = get<EnumDecl>(node);
            allDeclaredSymbols[enumDecl.name].push_back(
                SymbolInfo(T_ENUM, enumDecl.name, enumDecl.line, enumDecl.column, 
                          false, true, false, false, {})
            );
            return;
        }
    }

public:
    // Constructor - initializes global scope
    ScopeAnalyzer() {
        globalScope = make_unique<ScopeFrame>(nullptr, 0);
        currentScope = globalScope.get();
    }

    // Transfer ownership of the symbol table to caller
    unique_ptr<ScopeFrame> transferSymbolTable() {
        return move(globalScope);
    }
    
    // Get read-only access to global scope
    const ScopeFrame* getGlobalScope() const { 
        return globalScope.get(); 
    }

    // Main analysis function - performs two-pass analysis
    vector<ScopeError> analyze(const vector<ASTPtr>& ast, const vector<Token>& tokenList) {
        tokens = tokenList;
        errors.clear();
        allDeclaredSymbols.clear();
        
        // First pass: collect all declarations
        for (const auto& node : ast) {
            collectDeclarations(node->node);
        }
        
        // Second pass: perform actual scope analysis
        for (const auto& node : ast) {
            analyzeASTNode(node->node);
        }
        
        return errors;
    }
    
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

    }
    catch (const exception& e) {
        cerr << "[Scope Analysis Exception] " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }
}