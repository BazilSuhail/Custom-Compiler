#include "compiler.h"

class TypeChecker {
private:
    unique_ptr<ScopeFrame> globalScope;
    ScopeFrame* currentScope;
    vector<TypeCheckError> errors;
    vector<Token> tokens;
    
    // Track current function's return type for return statement validation
    TokenType currentFunctionReturnType;
    bool insideFunction;
    
    // Track if we're inside a loop or switch for break statement validation
    int loopDepth;
    int switchDepth;
    
    // Store all declared symbols
    unordered_map<string, vector<SymbolInfo>> allDeclaredSymbols;

    // ===== SCOPE MANAGEMENT =====
    
    // Search for a symbol by walking up the scope chain
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

    // Create a new nested scope
    void enterScope() {
        auto newScope = make_unique<ScopeFrame>(currentScope, currentScope->level + 1);
        ScopeFrame* newScopePtr = newScope.get();
        currentScope->children.push_back(move(newScope));
        currentScope = newScopePtr;
    }

    // Exit current scope and return to parent
    void exitScope() {
        if (currentScope->parent != nullptr) {
            currentScope = currentScope->parent;
        }
    }

    // Record a type checking error
    void addError(TypeChkError type, int line, int col, const string& name = "") {
        errors.push_back(TypeCheckError(type, line, col, name));
    }

    // ===== TYPE CHECKING UTILITIES =====
    
    // Check if a type is numeric (int, float, double)
    bool isNumericType(TokenType type) {
        return type == T_INT || type == T_FLOAT || type == T_DOUBLE;
    }
    
    // Check if a type is boolean
    bool isBooleanType(TokenType type) {
        return type == T_BOOL;
    }
    
    // Check if a type is integer
    bool isIntegerType(TokenType type) {
        return type == T_INT;
    }
    
    // Check if two types are compatible for assignment
    bool areTypesCompatible(TokenType left, TokenType right) {
        // Exact match
        if (left == right) {
            return true;
        }
        
        // Numeric type promotions allowed
        if (isNumericType(left) && isNumericType(right)) {
            return true;
        }
        
        return false;
    }
    
    // Get the result type of a binary operation
    TokenType getResultType(TokenType left, TokenType right, TokenType op) {
        // Boolean operations always return bool
        if (op == T_AND || op == T_OR) {
            return T_BOOL;
        }
        
        // Comparison operations return bool
        if (op == T_EQUALOP || op == T_NE || op == T_LT || 
            op == T_GT || op == T_LE || op == T_GE) {
            return T_BOOL;
        }
        
        // For arithmetic operations, use the wider type
        if (left == T_DOUBLE || right == T_DOUBLE) {
            return T_DOUBLE;
        }
        if (left == T_FLOAT || right == T_FLOAT) {
            return T_FLOAT;
        }
        
        return T_INT;
    }

    // ===== SYMBOL TABLE BUILDING =====
    
    // Collect all declarations to build symbol table
    void collectDeclarations(const ASTNodeVariant& node) {
        // Variable declaration
        if (holds_alternative<VarDecl>(node)) {
            const VarDecl& varDecl = get<VarDecl>(node);
            currentScope->addSymbol(SymbolInfo(varDecl.type, varDecl.name, 
                                              varDecl.line, varDecl.column, false));
            allDeclaredSymbols[varDecl.name].push_back(
                SymbolInfo(varDecl.type, varDecl.name, varDecl.line, varDecl.column, false)
            );
            return;
        }
        
        // Function definition
        if (holds_alternative<FunctionDecl>(node)) {
            const FunctionDecl& funcDecl = get<FunctionDecl>(node);
            currentScope->addSymbol(SymbolInfo(funcDecl.returnType, funcDecl.name, 
                                              funcDecl.line, funcDecl.column, 
                                              true, false, false, false, funcDecl.params));
            allDeclaredSymbols[funcDecl.name].push_back(
                SymbolInfo(funcDecl.returnType, funcDecl.name, funcDecl.line, 
                          funcDecl.column, true, false, false, false, funcDecl.params)
            );
            
            // Enter function scope and add parameters
            enterScope();
            for (const auto& param : funcDecl.params) {
                currentScope->addSymbol(SymbolInfo(param.first, param.second, 
                                                  funcDecl.line, funcDecl.column, false));
            }
            
            // Collect declarations in function body
            for (const auto& stmt : funcDecl.body) {
                collectDeclarations(stmt->node);
            }
            exitScope();
            return;
        }
        
        // Function prototype
        if (holds_alternative<FunctionProto>(node)) {
            const FunctionProto& protoDecl = get<FunctionProto>(node);
            currentScope->addSymbol(SymbolInfo(protoDecl.returnType, protoDecl.name, 
                                              protoDecl.line, protoDecl.column, 
                                              true, false, false, true, protoDecl.params));
            allDeclaredSymbols[protoDecl.name].push_back(
                SymbolInfo(protoDecl.returnType, protoDecl.name, protoDecl.line, 
                          protoDecl.column, true, false, false, true, protoDecl.params)
            );
            return;
        }
        
        // Enum declaration
        if (holds_alternative<EnumDecl>(node)) {
            const EnumDecl& enumDecl = get<EnumDecl>(node);
            currentScope->addSymbol(SymbolInfo(T_ENUM, enumDecl.name, 
                                              enumDecl.line, enumDecl.column, 
                                              false, true, false, false, {}));
            allDeclaredSymbols[enumDecl.name].push_back(
                SymbolInfo(T_ENUM, enumDecl.name, enumDecl.line, 
                          enumDecl.column, false, true, false, false, {})
            );
            
            // Add enum values
            if (holds_alternative<EnumValueList>(enumDecl.values->node)) {
                const EnumValueList& valueList = get<EnumValueList>(enumDecl.values->node);
                for (const string& value : valueList.values) {
                    currentScope->addSymbol(SymbolInfo(T_INT, value, 
                                                      enumDecl.line, enumDecl.column, 
                                                      false, false, true, false, {}));
                    allDeclaredSymbols[value].push_back(
                        SymbolInfo(T_INT, value, enumDecl.line, 
                                  enumDecl.column, false, false, true, false, {})
                    );
                }
            }
            return;
        }
        
        // Main function
        if (holds_alternative<MainDecl>(node)) {
            const MainDecl& mainDecl = get<MainDecl>(node);
            enterScope();
            for (const auto& stmt : mainDecl.body) {
                collectDeclarations(stmt->node);
            }
            exitScope();
            return;
        }
        
        // If statement - check both branches
        if (holds_alternative<IfStmt>(node)) {
            const IfStmt& ifStmt = get<IfStmt>(node);
            
            enterScope();
            for (const auto& stmt : ifStmt.ifBody) {
                collectDeclarations(stmt->node);
            }
            exitScope();
            
            enterScope();
            for (const auto& stmt : ifStmt.elseBody) {
                collectDeclarations(stmt->node);
            }
            exitScope();
            return;
        }
        
        // While loop
        if (holds_alternative<WhileStmt>(node)) {
            const WhileStmt& whileStmt = get<WhileStmt>(node);
            enterScope();
            for (const auto& stmt : whileStmt.body) {
                collectDeclarations(stmt->node);
            }
            exitScope();
            return;
        }
        
        // Do-while loop
        if (holds_alternative<DoWhileStmt>(node)) {
            const DoWhileStmt& doWhileStmt = get<DoWhileStmt>(node);
            enterScope();
            if (doWhileStmt.body) {
                collectDeclarations(doWhileStmt.body->node);
            }
            exitScope();
            return;
        }
        
        // For loop
        if (holds_alternative<ForStmt>(node)) {
            const ForStmt& forStmt = get<ForStmt>(node);
            enterScope();
            if (forStmt.init) {
                collectDeclarations(forStmt.init->node);
            }
            if (forStmt.body) {
                collectDeclarations(forStmt.body->node);
            }
            exitScope();
            return;
        }
        
        // Switch statement
        if (holds_alternative<SwitchStmt>(node)) {
            const SwitchStmt& switchStmt = get<SwitchStmt>(node);
            
            for (const auto& caseStmt : switchStmt.cases) {
                if (caseStmt) {
                    enterScope();
                    collectDeclarations(caseStmt->node);
                    exitScope();
                }
            }
            
            enterScope();
            for (const auto& stmt : switchStmt.defaultBody) {
                collectDeclarations(stmt->node);
            }
            exitScope();
            return;
        }
        
        // Block statement
        if (holds_alternative<BlockStmt>(node)) {
            const BlockStmt& block = get<BlockStmt>(node);
            enterScope();
            for (const auto& stmt : block.body) {
                collectDeclarations(stmt->node);
            }
            exitScope();
            return;
        }
    }

    // ===== EXPRESSION TYPE CHECKING =====
    
    // Get the type of an expression
    TokenType getExpressionType(const ASTNodeVariant& expr) {
        // Integer literal
        if (holds_alternative<IntLiteral>(expr)) {
            return T_INT;
        }
        
        // Float literal
        if (holds_alternative<FloatLiteral>(expr)) {
            return T_FLOAT;
        }
        
        // String literal
        if (holds_alternative<StringLiteral>(expr)) {
            return T_STRING;
        }
        
        // Char literal
        if (holds_alternative<CharLiteral>(expr)) {
            return T_CHAR;
        }
        
        // Bool literal
        if (holds_alternative<BoolLiteral>(expr)) {
            return T_BOOL;
        }
        
        // Identifier - look up its type
        if (holds_alternative<Identifier>(expr)) {
            const Identifier& ident = get<Identifier>(expr);
            SymbolInfo* sym = lookupSymbol(ident.name);
            if (sym != nullptr) {
                return sym->type;
            }
            return T_ERROR;
        }
        
        // Binary expression
        if (holds_alternative<BinaryExpr>(expr)) {
            return checkBinaryExpr(get<BinaryExpr>(expr));
        }
        
        // Unary expression
        if (holds_alternative<UnaryExpr>(expr)) {
            return checkUnaryExpr(get<UnaryExpr>(expr));
        }
        
        // Function call
        if (holds_alternative<CallExpr>(expr)) {
            return checkCallExpr(get<CallExpr>(expr));
        }
        
        return T_ERROR;
    }
    
    // Check binary expression and return its type
    TokenType checkBinaryExpr(const BinaryExpr& expr) {
        if (!expr.left || !expr.right) {
            return T_ERROR;
        }
        
        TokenType leftType = getExpressionType(expr.left->node);
        TokenType rightType = getExpressionType(expr.right->node);
        
        // Boolean operators (&&, ||)
        if (expr.op == T_AND || expr.op == T_OR) {
            if (!isBooleanType(leftType) || !isBooleanType(rightType)) {
                addError(AttemptedBoolOpOnNonBools, expr.line, expr.column);
                return T_ERROR;
            }
            return T_BOOL;
        }
        
        // Bitwise operators (&, |, ^)
        if (expr.op == T_BITAND || expr.op == T_BITOR || expr.op == T_BITXOR) {
            if (!isIntegerType(leftType) || !isIntegerType(rightType)) {
                addError(AttemptedBitOpOnNonInt, expr.line, expr.column);
                return T_ERROR;
            }
            return T_INT;
        }
        
        // Shift operators (<<, >>)
        if (expr.op == T_BITLSHIFT || expr.op == T_BITRSHIFT) {
            if (!isIntegerType(leftType) || !isIntegerType(rightType)) {
                addError(AttemptedShiftOnNonInt, expr.line, expr.column);
                return T_ERROR;
            }
            return T_INT;
        }
        
        // Comparison operators (==, !=, <, >, <=, >=)
        if (expr.op == T_EQUALOP || expr.op == T_NE || 
            expr.op == T_LT || expr.op == T_GT || 
            expr.op == T_LE || expr.op == T_GE) {
            
            if (!areTypesCompatible(leftType, rightType)) {
                addError(ExpressionTypeMismatch, expr.line, expr.column);
                return T_ERROR;
            }
            return T_BOOL;
        }
        
        // Arithmetic operators (+, -, *, /, %)
        if (expr.op == T_PLUS || expr.op == T_MINUS || 
            expr.op == T_MULTIPLY || expr.op == T_DIVIDE || expr.op == T_MODULO) {
            
            if (!isNumericType(leftType) || !isNumericType(rightType)) {
                addError(AttemptedAddOpOnNonNumeric, expr.line, expr.column);
                return T_ERROR;
            }
            return getResultType(leftType, rightType, expr.op);
        }
        
        // Assignment operator (=)
        if (expr.op == T_ASSIGNOP) {
            if (!areTypesCompatible(leftType, rightType)) {
                addError(ExpressionTypeMismatch, expr.line, expr.column);
                return T_ERROR;
            }
            return leftType;
        }
        
        return T_ERROR;
    }
    
    // Check unary expression and return its type
    TokenType checkUnaryExpr(const UnaryExpr& expr) {
        if (!expr.operand) {
            return T_ERROR;
        }
        
        TokenType operandType = getExpressionType(expr.operand->node);
        
        // Logical NOT (!)
        if (expr.op == T_NOT) {
            if (!isBooleanType(operandType)) {
                addError(AttemptedBoolOpOnNonBools, expr.line, expr.column);
                return T_ERROR;
            }
            return T_BOOL;
        }
        
        // Unary minus/plus (-, +)
        if (expr.op == T_MINUS || expr.op == T_PLUS) {
            if (!isNumericType(operandType)) {
                addError(AttemptedAddOpOnNonNumeric, expr.line, expr.column);
                return T_ERROR;
            }
            return operandType;
        }
        
        // Increment/Decrement (++, --)
        if (expr.op == T_INCREMENT || expr.op == T_DECREMENT) {
            if (!isNumericType(operandType)) {
                addError(AttemptedAddOpOnNonNumeric, expr.line, expr.column);
                return T_ERROR;
            }
            return operandType;
        }
        
        return operandType;
    }
    
    // Check function call and return its type
    TokenType checkCallExpr(const CallExpr& call) {
        // Get function name
        if (!holds_alternative<Identifier>(call.callee->node)) {
            return T_ERROR;
        }
        
        const Identifier& funcIdent = get<Identifier>(call.callee->node);
        SymbolInfo* funcSym = lookupSymbol(funcIdent.name);
        
        if (funcSym == nullptr || !funcSym->isFunction) {
            return T_ERROR;
        }
        
        // Check parameter count
        if (call.args.size() != funcSym->params.size()) {
            addError(FnCallParamCount, call.line, call.column, funcIdent.name);
            return funcSym->type;
        }
        
        // Check parameter types
        for (size_t i = 0; i < call.args.size(); i++) {
            TokenType argType = getExpressionType(call.args[i]->node);
            TokenType paramType = funcSym->params[i].first;
            
            if (!areTypesCompatible(paramType, argType)) {
                addError(FnCallParamType, call.line, call.column, funcIdent.name);
            }
        }
        
        return funcSym->type;
    }

    // ===== STATEMENT TYPE CHECKING =====
    
    // Check variable declaration
    void checkVarDecl(const VarDecl& decl) {
        // Check if type is valid
        if (decl.type == T_VOID) {
            addError(ErroneousVarDecl, decl.line, decl.column, decl.name);
            return;
        }
        
        // Check initializer if present
        if (decl.initializer) {
            TokenType initType = getExpressionType(decl.initializer->node);
            
            if (!areTypesCompatible(decl.type, initType)) {
                addError(ExpressionTypeMismatch, decl.line, decl.column, decl.name);
            }
        }
    }
    
    // Check function declaration
    void checkFunctionDecl(const FunctionDecl& func) {
        // Set current function context
        currentFunctionReturnType = func.returnType;
        insideFunction = true;
        
        enterScope();
        
        // Add parameters to scope
        for (const auto& param : func.params) {
            currentScope->addSymbol(SymbolInfo(param.first, param.second, 
                                              func.line, func.column, false));
        }
        
        // Check function body
        for (const auto& stmt : func.body) {
            checkNode(stmt->node);
        }
        
        exitScope();
        
        insideFunction = false;
    }
    
    // Check main function
    void checkMainDecl(const MainDecl& main) {
        enterScope();
        
        for (const auto& stmt : main.body) {
            checkNode(stmt->node);
        }
        
        exitScope();
    }
    
    // Check if statement
    void checkIfStmt(const IfStmt& stmt) {
        // Check condition is boolean
        if (stmt.condition) {
            TokenType condType = getExpressionType(stmt.condition->node);
            if (!isBooleanType(condType)) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
        
        // Check if body
        enterScope();
        for (const auto& ifStmt : stmt.ifBody) {
            checkNode(ifStmt->node);
        }
        exitScope();
        
        // Check else body
        enterScope();
        for (const auto& elseStmt : stmt.elseBody) {
            checkNode(elseStmt->node);
        }
        exitScope();
    }
    
    // Check while loop
    void checkWhileStmt(const WhileStmt& stmt) {
        // Check condition is boolean
        if (stmt.condition) {
            TokenType condType = getExpressionType(stmt.condition->node);
            if (!isBooleanType(condType)) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
        
        // Enter loop context
        loopDepth++;
        
        enterScope();
        for (const auto& bodyStmt : stmt.body) {
            checkNode(bodyStmt->node);
        }
        exitScope();
        
        loopDepth--;
    }
    
    // Check do-while loop
    void checkDoWhileStmt(const DoWhileStmt& stmt) {
        // Enter loop context
        loopDepth++;
        
        enterScope();
        if (stmt.body) {
            checkNode(stmt.body->node);
        }
        exitScope();
        
        loopDepth--;
        
        // Check condition is boolean
        if (stmt.condition) {
            TokenType condType = getExpressionType(stmt.condition->node);
            if (!isBooleanType(condType)) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
    }
    
    // Check for loop
    void checkForStmt(const ForStmt& stmt) {
        enterScope();
        
        // Check initialization
        if (stmt.init) {
            checkNode(stmt.init->node);
        }
        
        // Check condition is boolean
        if (stmt.condition) {
            TokenType condType = getExpressionType(stmt.condition->node);
            if (!isBooleanType(condType)) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
        
        // Enter loop context
        loopDepth++;
        
        // Check body
        if (stmt.body) {
            checkNode(stmt.body->node);
        }
        
        loopDepth--;
        
        exitScope();
    }
    
    // Check switch statement
    void checkSwitchStmt(const SwitchStmt& stmt) {
        // Check switch expression
        if (stmt.expression) {
            getExpressionType(stmt.expression->node);
        }
        
        // Enter switch context
        switchDepth++;
        
        // Check each case
        for (const auto& caseStmt : stmt.cases) {
            if (caseStmt) {
                enterScope();
                checkNode(caseStmt->node);
                exitScope();
            }
        }
        
        // Check default case
        enterScope();
        for (const auto& defaultStmt : stmt.defaultBody) {
            checkNode(defaultStmt->node);
        }
        exitScope();
        
        switchDepth--;
    }
    
    // Check return statement
    void checkReturnStmt(const ReturnStmt& stmt) {
        if (!insideFunction) {
            return;
        }
        
        // Check if function expects a return value
        if (currentFunctionReturnType == T_VOID) {
            if (stmt.value != nullptr) {
                addError(ErroneousReturnType, stmt.line, stmt.column);
            }
        } else {
            if (stmt.value == nullptr) {
                addError(ErroneousReturnType, stmt.line, stmt.column);
            } else {
                TokenType returnType = getExpressionType(stmt.value->node);
                if (!areTypesCompatible(currentFunctionReturnType, returnType)) {
                    addError(ErroneousReturnType, stmt.line, stmt.column);
                }
            }
        }
    }
    
    // Check break statement
    void checkBreakStmt(const BreakStmt& stmt) {
        if (loopDepth == 0 && switchDepth == 0) {
            addError(ErroneousBreak, stmt.line, stmt.column);
        }
    }
    
    // Check print statement
    void checkPrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) {
            getExpressionType(arg->node);
        }
    }
    
    // Check expression statement
    void checkExpressionStmt(const ExpressionStmt& stmt) {
        getExpressionType(stmt.expr->node);
    }
    
    // Check block statement
    void checkBlockStmt(const BlockStmt& block) {
        enterScope();
        for (const auto& stmt : block.body) {
            checkNode(stmt->node);
        }
        exitScope();
    }
    
    // Check case block
    void checkCaseBlock(const CaseBlock& caseBlock) {
        if (caseBlock.value) {
            getExpressionType(caseBlock.value->node);
        }
        
        for (const auto& stmt : caseBlock.body) {
            checkNode(stmt->node);
        }
    }

    // ===== MAIN NODE DISPATCHER =====
    
    // Main dispatcher for type checking
    void checkNode(const ASTNodeVariant& node) {
        // Variable declaration
        if (holds_alternative<VarDecl>(node)) {
            checkVarDecl(get<VarDecl>(node));
            return;
        }
        
        // Function declaration
        if (holds_alternative<FunctionDecl>(node)) {
            checkFunctionDecl(get<FunctionDecl>(node));
            return;
        }
        
        // Function prototype - no type checking needed
        if (holds_alternative<FunctionProto>(node)) {
            return;
        }
        
        // Enum declaration - no type checking needed
        if (holds_alternative<EnumDecl>(node)) {
            return;
        }
        
        // Main function
        if (holds_alternative<MainDecl>(node)) {
            checkMainDecl(get<MainDecl>(node));
            return;
        }
        
        // If statement
        if (holds_alternative<IfStmt>(node)) {
            checkIfStmt(get<IfStmt>(node));
            return;
        }
        
        // While loop
        if (holds_alternative<WhileStmt>(node)) {
            checkWhileStmt(get<WhileStmt>(node));
            return;
        }
        
        // Do-while loop
        if (holds_alternative<DoWhileStmt>(node)) {
            checkDoWhileStmt(get<DoWhileStmt>(node));
            return;
        }
        
        // For loop
        if (holds_alternative<ForStmt>(node)) {
            checkForStmt(get<ForStmt>(node));
            return;
        }
        
        // Switch statement
        if (holds_alternative<SwitchStmt>(node)) {
            checkSwitchStmt(get<SwitchStmt>(node));
            return;
        }
        
        // Return statement
        if (holds_alternative<ReturnStmt>(node)) {
            checkReturnStmt(get<ReturnStmt>(node));
            return;
        }
        
        // Break statement
        if (holds_alternative<BreakStmt>(node)) {
            checkBreakStmt(get<BreakStmt>(node));
            return;
        }
        
        // Print statement
        if (holds_alternative<PrintStmt>(node)) {
            checkPrintStmt(get<PrintStmt>(node));
            return;
        }
        
        // Expression statement
        if (holds_alternative<ExpressionStmt>(node)) {
            checkExpressionStmt(get<ExpressionStmt>(node));
            return;
        }
        
        // Block statement
        if (holds_alternative<BlockStmt>(node)) {
            checkBlockStmt(get<BlockStmt>(node));
            return;
        }
        
        // Case block
        if (holds_alternative<CaseBlock>(node)) {
            checkCaseBlock(get<CaseBlock>(node));
            return;
        }
    }

public:
    // ===== PUBLIC INTERFACE =====
    
    // Constructor
    TypeChecker() {
        globalScope = make_unique<ScopeFrame>(nullptr, 0);
        currentScope = globalScope.get();
        currentFunctionReturnType = T_VOID;
        insideFunction = false;
        loopDepth = 0;
        switchDepth = 0;
    }

    // Main type checking function
    vector<TypeCheckError> check(const vector<ASTPtr>& ast, const vector<Token>& tokenList) {
        tokens = tokenList;
        errors.clear();
        allDeclaredSymbols.clear();
        
        // First pass: build symbol table
        for (const auto& node : ast) {
            collectDeclarations(node->node);
        }
        
        // Reset to global scope
        currentScope = globalScope.get();
        
        // Second pass: perform type checking
        for (const auto& node : ast) {
            checkNode(node->node);
        }
        
        return errors;
    }
};

// ===== MAIN ENTRY POINT =====

// Perform type checking on the entire program
void performTypeChecking(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    try {
        // Create type checker instance
        TypeChecker checker;
        
        // Run type checking and get errors
        vector<TypeCheckError> errors = checker.check(ast, tokens);

        // Check if there were any errors
        if (!errors.empty()) {
            cerr << "\n=== Type Checking Errors ===\n";
            
            // Print each error
            for (const auto& error : errors) {
                cerr << "[Type Error] " << error.message << "\n";
            }
            
            cerr << "Type checking failed with " << errors.size() << " error(s)\n";
            exit(EXIT_FAILURE);
        }

        // Success message
        cout << "\n=== Type Checking Successful ===\n";
        cout << "No type errors found.\n";
    }
    catch (const exception& e) {
        cerr << "[Type Checking Exception] " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }
}