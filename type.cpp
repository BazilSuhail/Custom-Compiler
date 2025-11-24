#include "compiler.h"

class TypeChecker {
private:
    unique_ptr<ScopeFrame> globalScope;
    ScopeFrame* currentScope;
    vector<TypeCheckError> errors;
    
    // Context tracking for validation
    TokenType currentFunctionReturnType;
    string currentFunctionName;
    int loopDepth;
    int switchDepth;
    bool foundReturnStmt;
    
    // ===== SCOPE MANAGEMENT =====
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
    
    // ===== ERROR REPORTING =====
    
    void addError(TypeChkError type, int line, int col, const string& name = "") {
        errors.push_back(TypeCheckError(type, line, col, name));
    }
    
    // ===== TYPE CLASSIFICATION =====
    bool isNumericType(TokenType type) const {
        return type == T_INT || type == T_FLOAT || type == T_DOUBLE;
    }
    
    bool isBooleanOperator(TokenType op) const {
        return op == T_AND || op == T_OR || op == T_NOT;
    }
    
    bool isBitwiseOperator(TokenType op) const {
        return op == T_BITAND || op == T_BITOR || op == T_BITXOR;
    }
    
    bool isShiftOperator(TokenType op) const {
        return op == T_BITLSHIFT || op == T_BITRSHIFT;
    }
    
    bool isArithmeticOperator(TokenType op) const {
        return op == T_PLUS || op == T_MINUS || op == T_MULTIPLY || 
               op == T_DIVIDE || op == T_MODULO;
    }
    
    bool isComparisonOperator(TokenType op) const {
        return op == T_EQUALOP || op == T_NE || op == T_LT || 
               op == T_GT || op == T_LE || op == T_GE;
    }
    
    // ===== TYPE INFERENCE =====
    
    TokenType inferType(const ASTNodeVariant& node) {
        // Literals
        if (holds_alternative<IntLiteral>(node)) {
            return T_INT;
        }
        if (holds_alternative<FloatLiteral>(node)) {
            return T_FLOAT;
        }
        if (holds_alternative<StringLiteral>(node)) {
            return T_STRING;
        }
        if (holds_alternative<CharLiteral>(node)) {
            return T_CHAR;
        }
        if (holds_alternative<BoolLiteral>(node)) {
            return T_BOOL;
        }
        
        // Identifier - lookup in symbol table
        if (holds_alternative<Identifier>(node)) {
            const Identifier& ident = get<Identifier>(node);
            SymbolInfo* sym = lookupSymbol(ident.name);
            if (sym != nullptr) {
                return sym->type;
            }
            return T_ERROR;
        }
        
        // Binary expression
        if (holds_alternative<BinaryExpr>(node)) {
            const BinaryExpr& expr = get<BinaryExpr>(node);
            
            // Comparison operators always return bool
            if (isComparisonOperator(expr.op)) {
                return T_BOOL;
            }
            
            // Boolean operators return bool
            if (isBooleanOperator(expr.op)) {
                return T_BOOL;
            }
            
            // For arithmetic/bitwise, return type of left operand (they must match anyway)
            if (expr.left) {
                return inferType(expr.left->node);
            }
            
            return T_ERROR;
        }
        
        // Unary expression
        // if (holds_alternative<UnaryExpr>(node)) {
        //     const UnaryExpr& expr = get<UnaryExpr>(node);
            
        //     // NOT operator returns bool
        //     if (expr.op == T_NOT) {
        //         return T_BOOL;
        //     }
            
        //     // Unary minus returns same type as operand
        //     if (expr.op == T_MINUS && expr.operand) {
        //         return inferType(expr.operand->node);
        //     }
            
        //     return T_ERROR;
        // }
        // Unary expression
        if (holds_alternative<UnaryExpr>(node)) {
            const UnaryExpr& expr = get<UnaryExpr>(node);
            
            // NOT operator returns bool
            if (expr.op == T_NOT) {
                return T_BOOL;
            }
            
            if ((expr.op == T_INCREMENT || expr.op == T_DECREMENT) && expr.operand) {
                return inferType(expr.operand->node);
            }
            
            if (expr.op == T_MINUS && expr.operand) {
                return inferType(expr.operand->node);
            }
            
            return T_ERROR;
        }
        
        // Function call
        if (holds_alternative<CallExpr>(node)) {
            const CallExpr& call = get<CallExpr>(node);
            if (holds_alternative<Identifier>(call.callee->node)) {
                const Identifier& func = get<Identifier>(call.callee->node);
                SymbolInfo* sym = lookupSymbol(func.name);
                if (sym != nullptr && sym->isFunction) {
                    return sym->type;
                }
            }
            return T_ERROR;
        }
        
        return T_ERROR;
    }
    
    // ===== TYPE COMPATIBILITY (Strict - no conversions) =====
    
    bool areTypesStrictlyEqual(TokenType t1, TokenType t2) const {
        if (t1 == T_ERROR || t2 == T_ERROR) {
            return false;
        }
        
        return t1 == t2;
    }
    
    // ===== DECLARATION CHECKING =====
    
    void checkVarDecl(const VarDecl& decl) { 
        currentScope->addSymbol(SymbolInfo(decl.type, decl.name, decl.line, decl.column, false));
        
        if (decl.initializer) {
            checkExpressionNode(decl.initializer->node);
            
            TokenType initType = inferType(decl.initializer->node);
            
            if (!areTypesStrictlyEqual(decl.type, initType)) {
                addError(ErroneousVarDecl, decl.line, decl.column, decl.name);
            }
        }
    }
    
    void checkFunctionDecl(const FunctionDecl& func) {

        currentScope->addSymbol(SymbolInfo(func.returnType, func.name, func.line, func.column, true, false, false, false, func.params));
        
        currentFunctionReturnType = func.returnType;
        currentFunctionName = func.name;
        foundReturnStmt = false; 
        
        enterScope();
        
        for (const auto& param : func.params) {
            currentScope->addSymbol(SymbolInfo(param.first, param.second, func.line, func.column, false));
        }
        
        // Check function body
        for (const auto& stmt : func.body) {
            checkASTNode(stmt->node);
        }
        
        if (func.returnType != T_VOID && !foundReturnStmt) {
            addError(ReturnStmtNotFound, func.line, func.column, func.name);
        }
        
        exitScope();
        
        currentFunctionReturnType = T_VOID;
        currentFunctionName = "";
        foundReturnStmt = false; 
    }
    
    void checkFunctionProto(const FunctionProto& proto) {
        currentScope->addSymbol(
            SymbolInfo(proto.returnType, proto.name, proto.line, proto.column, true, false, false, true, proto.params)
        );
    }
    
    // ===== EXPRESSION CHECKING =====
    
    void checkBinaryExpr(const BinaryExpr& expr) {
        if (!expr.left || !expr.right) {
            return;
        }
        
        checkExpressionNode(expr.left->node);
        checkExpressionNode(expr.right->node);
        
        TokenType leftType = inferType(expr.left->node);
        TokenType rightType = inferType(expr.right->node);
        
        if (isBooleanOperator(expr.op)) {
            if (leftType != T_BOOL || rightType != T_BOOL) {
                addError(AttemptedBoolOpOnNonBools, expr.line, expr.column);
            }
            return;
        }
        
        if (isBitwiseOperator(expr.op)) {
            if (leftType != T_INT || rightType != T_INT) {
                addError(AttemptedBitOpOnNonInt, expr.line, expr.column);
            }
            return;
        }
        
        if (isShiftOperator(expr.op)) {
            if (leftType != T_INT || rightType != T_INT) {
                addError(AttemptedShiftOnNonInt, expr.line, expr.column);
            }
            return;
        }
        
        if (isArithmeticOperator(expr.op)) {
            if (!isNumericType(leftType) || !isNumericType(rightType)) {
                addError(AttemptedAddOpOnNonNumeric, expr.line, expr.column);
            } 
            else if (!areTypesStrictlyEqual(leftType, rightType)) {
                addError(ExpressionTypeMismatch, expr.line, expr.column);
            }
            return;
        }
        
        // Comparison operators - operands must be same type
        if (isComparisonOperator(expr.op)) {
            if (!areTypesStrictlyEqual(leftType, rightType)) {
                addError(ExpressionTypeMismatch, expr.line, expr.column);
            }
            return;
        }
        
        // Assignment operator
        if (expr.op == T_ASSIGNOP) {
            if (!holds_alternative<Identifier>(expr.left->node)) {
                addError(ExpressionTypeMismatch, expr.line, expr.column);
                return;
            }
            
            if (!areTypesStrictlyEqual(leftType, rightType)) {
                addError(ExpressionTypeMismatch, expr.line, expr.column);
            }
            return;
        }
    }
    
    void checkUnaryExpr(const UnaryExpr& expr) {
        if (!expr.operand) {
            return;
        }
        
        checkExpressionNode(expr.operand->node);
        TokenType operandType = inferType(expr.operand->node);
        
        if (expr.op == T_INCREMENT || expr.op == T_DECREMENT) {
            if (operandType != T_INT) {
                addError(IncrementDecrementOnNonInt, expr.line, expr.column);
            }
            return;
        }
        
        if (expr.op == T_NOT) {
            if (operandType != T_BOOL) {
                addError(NotOnNonBool, expr.line, expr.column);
            }
            return;
        }
        
        if (expr.op == T_MINUS) {
            if (!isNumericType(operandType)) {
                addError(AttemptedAddOpOnNonNumeric, expr.line, expr.column);
            }
            return;
        }
    }
    
    void checkCallExpr(const CallExpr& call) {
        if (!holds_alternative<Identifier>(call.callee->node)) {
            return;
        }
        
        const Identifier& funcIdent = get<Identifier>(call.callee->node);
        SymbolInfo* funcSym = lookupSymbol(funcIdent.name);
        
        if (!funcSym || !funcSym->isFunction) {
            return;
        }
        
        if (call.args.size() != funcSym->params.size()) {
            addError(FnCallParamCount, call.line, call.column, funcIdent.name);
            return;
        }
        
        for (size_t i = 0; i < call.args.size(); i++) {
            checkExpressionNode(call.args[i]->node);
            TokenType argType = inferType(call.args[i]->node);
            TokenType paramType = funcSym->params[i].first;
            
            if (!areTypesStrictlyEqual(argType, paramType)) {
                addError(FnCallParamType, call.line, call.column, funcIdent.name);
            }
        }
    }
    
    void checkExpressionNode(const ASTNodeVariant& expr) {
        if (holds_alternative<BinaryExpr>(expr)) {
            checkBinaryExpr(get<BinaryExpr>(expr));
        } else if (holds_alternative<UnaryExpr>(expr)) {
            checkUnaryExpr(get<UnaryExpr>(expr));
        } else if (holds_alternative<CallExpr>(expr)) {
            checkCallExpr(get<CallExpr>(expr));
        }
        // Literals and identifiers don't need checking
    }
    
    // ===== STATEMENT CHECKING =====
    
    void checkReturnStmt(const ReturnStmt& stmt) {
        foundReturnStmt = true; 
        
        if (stmt.value) {
            checkExpressionNode(stmt.value->node);
            TokenType returnType = inferType(stmt.value->node);
            
            // Check if returning a value in a void function
            if (currentFunctionReturnType == T_VOID) {
                addError(ReturnStmtInVoid, stmt.line, stmt.column, currentFunctionName);
                return;
            }
            
            if (!areTypesStrictlyEqual(currentFunctionReturnType, returnType)) {
                addError(ErroneousReturnType, stmt.line, stmt.column, currentFunctionName);
            }
        } else {
            // Return with no value - must be void function
            if (currentFunctionReturnType != T_VOID) {
                addError(ErroneousReturnType, stmt.line, stmt.column, currentFunctionName);
            }
        }
    }
    
    void checkIfStmt(const IfStmt& stmt) {
        // Check condition is boolean
        if (stmt.condition) {
            checkExpressionNode(stmt.condition->node);
            TokenType condType = inferType(stmt.condition->node);
            
            if (condType != T_BOOL) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
        
        enterScope();
        for (const auto& s : stmt.ifBody) {
            checkASTNode(s->node);
        }
        exitScope();
        
        if (!stmt.elseBody.empty()) {
            enterScope();
            for (const auto& s : stmt.elseBody) {
                checkASTNode(s->node);
            }
            exitScope();
        }
    }
    
    void checkWhileStmt(const WhileStmt& stmt) {
        if (stmt.condition) {
            checkExpressionNode(stmt.condition->node);
            TokenType condType = inferType(stmt.condition->node);
            
            if (condType != T_BOOL) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
        
        loopDepth++;
        enterScope();
        for (const auto& s : stmt.body) {
            checkASTNode(s->node);
        }
        exitScope();
        loopDepth--;
    }
    
    void checkDoWhileStmt(const DoWhileStmt& stmt) {
        
        loopDepth++;
        enterScope();
        if (stmt.body) {
            checkASTNode(stmt.body->node);
        }
        exitScope();
        loopDepth--;
        
        // 
        if (stmt.condition) {
            checkExpressionNode(stmt.condition->node);
            TokenType condType = inferType(stmt.condition->node);
            
            if (condType != T_BOOL) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
    }
    
    void checkForStmt(const ForStmt& stmt) {
        enterScope();
        
        // Check init
        if (stmt.init) {
            checkASTNode(stmt.init->node);
        }
        
        // Check condition is boolean
        if (stmt.condition) {
            checkExpressionNode(stmt.condition->node);
            TokenType condType = inferType(stmt.condition->node);
            
            if (condType != T_BOOL) {
                addError(NonBooleanCondStmt, stmt.line, stmt.column);
            }
        }
        
        // Check update
        if (stmt.update) {
            checkExpressionNode(stmt.update->node);
        }
        
        // Check body
        loopDepth++;
        if (stmt.body) {
            checkASTNode(stmt.body->node);
        }
        loopDepth--;
        
        exitScope();
    }
    
    void checkSwitchStmt(const SwitchStmt& stmt) {
        // Check switch expression
        TokenType switchExprType = T_ERROR;
        if (stmt.expression) {
            checkExpressionNode(stmt.expression->node);
            switchExprType = inferType(stmt.expression->node);
            
            // Switch expression must be int or char
            if (switchExprType != T_INT && switchExprType != T_CHAR) {
                addError(InvalidSwitchConditionType, stmt.line, stmt.column);
            }
        }
        
        // Enter switch context
        switchDepth++;
        
        // Check each case
        for (const auto& c : stmt.cases) {
            if (c && holds_alternative<CaseBlock>(c->node)) {
                const CaseBlock& caseBlock = get<CaseBlock>(c->node);
                
                // Check case value
                if (caseBlock.value) {
                    checkExpressionNode(caseBlock.value->node);
                    TokenType caseValueType = inferType(caseBlock.value->node);
                    
                    // Case value type must match switch expression type
                    if (switchExprType != T_ERROR && !areTypesStrictlyEqual(switchExprType, caseValueType)) {
                        addError(InvalidCaseValueType, caseBlock.line, caseBlock.column);
                    }
                }
                
                // Check case body
                enterScope();
                for (const auto& s : caseBlock.body) {
                    checkASTNode(s->node);
                }
                exitScope();
            }
        }
        
        // Check default body
        if (!stmt.defaultBody.empty()) {
            enterScope();
            for (const auto& s : stmt.defaultBody) {
                checkASTNode(s->node);
            }
            exitScope();
        }
        
        switchDepth--;
    }
    
    void checkBreakStmt(const BreakStmt& stmt) {
        // Break is only valid inside loop or switch
        if (loopDepth == 0 && switchDepth == 0) {
            addError(ErroneousBreak, stmt.line, stmt.column);
        }
    }
    
    void checkPrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) {
            checkExpressionNode(arg->node);
        }
    }
    
    void checkExpressionStmt(const ExpressionStmt& stmt) {
        if (stmt.expr) {
            checkExpressionNode(stmt.expr->node);
        }
    }
    
    void checkBlockStmt(const BlockStmt& block) {
        enterScope();
        for (const auto& stmt : block.body) {
            checkASTNode(stmt->node);
        }
        exitScope();
    }
    
    void checkMainDecl(const MainDecl& main) {
        currentFunctionReturnType = T_VOID;
        currentFunctionName = "main";
        
        enterScope();
        for (const auto& stmt : main.body) {
            checkASTNode(stmt->node);
        }
        exitScope();
        
        currentFunctionReturnType = T_VOID;
        currentFunctionName = "";
    }
    
    void checkEnumDecl(const EnumDecl& enm) {
        currentScope->addSymbol(SymbolInfo(T_ENUM, enm.name, enm.line, enm.column, false, true, false, false, {}));
        
        if (holds_alternative<EnumValueList>(enm.values->node)) {
            const EnumValueList& valueList = get<EnumValueList>(enm.values->node);
            for (const string& value : valueList.values) {
                currentScope->addSymbol(SymbolInfo(T_INT, value, enm.line, enm.column, false, false, true, false, {}));
            }
        }
    }
    
    // ===== MAIN NODE DISPATCHER =====
    
    void checkASTNode(const ASTNodeVariant& node) {
        if (holds_alternative<VarDecl>(node)) {
            checkVarDecl(get<VarDecl>(node));
        } 
        else if (holds_alternative<FunctionDecl>(node)) {
            checkFunctionDecl(get<FunctionDecl>(node));
        } 
        else if (holds_alternative<FunctionProto>(node)) {
            checkFunctionProto(get<FunctionProto>(node));
        } 
        else if (holds_alternative<EnumDecl>(node)) {
            checkEnumDecl(get<EnumDecl>(node));
        } 
        else if (holds_alternative<MainDecl>(node)) {
            checkMainDecl(get<MainDecl>(node));
        } 
        else if (holds_alternative<ReturnStmt>(node)) {
            checkReturnStmt(get<ReturnStmt>(node));
        } 
        else if (holds_alternative<IfStmt>(node)) {
            checkIfStmt(get<IfStmt>(node));
        } 
        else if (holds_alternative<WhileStmt>(node)) {
            checkWhileStmt(get<WhileStmt>(node));
        } 
        else if (holds_alternative<DoWhileStmt>(node)) {
            checkDoWhileStmt(get<DoWhileStmt>(node));
        } 
        else if (holds_alternative<ForStmt>(node)) {
            checkForStmt(get<ForStmt>(node));
        } 
        else if (holds_alternative<SwitchStmt>(node)) {
            checkSwitchStmt(get<SwitchStmt>(node));
        } 
        else if (holds_alternative<BreakStmt>(node)) {
            checkBreakStmt(get<BreakStmt>(node));
        } 
        else if (holds_alternative<PrintStmt>(node)) {
            checkPrintStmt(get<PrintStmt>(node));
        } 
        else if (holds_alternative<ExpressionStmt>(node)) {
            checkExpressionStmt(get<ExpressionStmt>(node));
        } 
        else if (holds_alternative<BlockStmt>(node)) {
            checkBlockStmt(get<BlockStmt>(node));
        }
    }
    
public:
    TypeChecker() {
        globalScope = make_unique<ScopeFrame>(nullptr, 0);
        currentScope = globalScope.get();
        currentFunctionReturnType = T_VOID;
        currentFunctionName = "";
        loopDepth = 0;
        switchDepth = 0;
        foundReturnStmt = false;  
    }
    
    vector<TypeCheckError> check(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
        errors.clear();
        
        // Perform type checking on all top-level declarations
        for (const auto& node : ast) {
            checkASTNode(node->node);
        }
        
        return errors;
    }
};

void performTypeChecking(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    try {
        TypeChecker checker;
        vector<TypeCheckError> errors = checker.check(ast, tokens);

        if (!errors.empty()) {
            cerr << "\n=== Type Checking Errors ===\n";
            
            for (const auto& error : errors) {
                cerr << "[Type Error] " << error.message << "\n";
            }
            
            cerr << "Type checking failed with " << errors.size() << " error(s)\n";
            exit(EXIT_FAILURE);
        }

        cout << "\n=== Type Checking Successful ===\n";
        cout << "No type errors found.\n";

    }
    catch (const exception& e) {
        cerr << "[Type Checking Exception] " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }
}
