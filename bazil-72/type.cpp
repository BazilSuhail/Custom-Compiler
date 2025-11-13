#include "compiler.h"
#include <stack>
#include <unordered_map>
#include <unordered_set>

// Type information for tracking
struct TypeInfo {
    DataType type;
    bool isEnumType; // True if this is an enum type
    string enumName; // Name of enum if isEnumType is true
    
    TypeInfo(DataType t, bool isEnum = false, const string& name = "") 
        : type(t), isEnumType(isEnum), enumName(name) {}
    
    TypeInfo() : type(TYPE_ERROR), isEnumType(false) {}
};

// Function signature for type checking
struct FunctionSignature {
    DataType returnType;
    vector<DataType> paramTypes;
    bool hasReturnStmt; // Track if function has return statement
    
    FunctionSignature(DataType ret, const vector<DataType>& params) 
        : returnType(ret), paramTypes(params), hasReturnStmt(false) {}
    
    FunctionSignature() : returnType(TYPE_VOID), hasReturnStmt(false) {}
};

struct TypeScopeFrame {
    unordered_map<string, TypeInfo> variables;
    unordered_map<string, FunctionSignature> functions;
    unordered_map<string, DataType> enumValues; // Map enum values to their types
    unordered_map<string, DataType> enumTypes;  // Map enum names to their types
    TypeScopeFrame* parent;
    
    TypeScopeFrame(TypeScopeFrame* p = nullptr) : parent(p) {}
    
    TypeInfo* findVariable(const string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return &(it->second);
        }
        if (parent) {
            return parent->findVariable(name);
        }
        return nullptr;
    }
    
    FunctionSignature* findFunction(const string& name) {
        auto it = functions.find(name);
        if (it != functions.end()) {
            return &(it->second);
        }
        if (parent) {
            return parent->findFunction(name);
        }
        return nullptr;
    }
    
    DataType* findEnumValue(const string& name) {
        auto it = enumValues.find(name);
        if (it != enumValues.end()) {
            return &(it->second);
        }
        if (parent) {
            return parent->findEnumValue(name);
        }
        return nullptr;
    }
    
    DataType* findEnumType(const string& name) {
        auto it = enumTypes.find(name);
        if (it != enumTypes.end()) {
            return &(it->second);
        }
        if (parent) {
            return parent->findEnumType(name);
        }
        return nullptr;
    }
    
    void addVariable(const string& name, const TypeInfo& type) {
        variables[name] = type;
    }
    
    void addFunction(const string& name, const FunctionSignature& sig) {
        functions[name] = sig;
    }
    
    void addEnumValue(const string& name, DataType type) {
        enumValues[name] = type;
    }
    
    void addEnumType(const string& name, DataType type) {
        enumTypes[name] = type;
    }
};

class TypeChecker {
private:
    unique_ptr<TypeScopeFrame> globalScope;
    TypeScopeFrame* currentScope;
    vector<TypeChkErrorStruct> errors;
    vector<Token> tokens;
    stack<bool> inLoop; // Track if we're inside a loop (for break statements)
    stack<bool> inSwitch; // Track if we're inside a switch (for break statements)
    
    // Map to store function return types for checking
    unordered_map<string, DataType> functionReturnTypes;
    
    void addError(TypeChkError error, const string& name, int line, int col) {
        errors.push_back(TypeChkErrorStruct(error, name, line, col));
    }
    
    // Convert TokenType to DataType
    DataType tokenTypeToDataType(TokenType tokenType) {
        switch (tokenType) {
            case T_INT: return TYPE_INT;
            case T_FLOAT: return TYPE_FLOAT;
            case T_DOUBLE: return TYPE_DOUBLE;
            case T_CHAR: return TYPE_CHAR;
            case T_BOOL: return TYPE_BOOL;
            case T_VOID: return TYPE_VOID;
            case T_STRING: return TYPE_STRING;
            case T_ENUM: return TYPE_ENUM;
            default: return TYPE_ERROR;
        }
    }
    
    // Check if a type is numeric
    bool isNumericType(DataType type) {
        return type == TYPE_INT || type == TYPE_FLOAT || type == TYPE_DOUBLE;
    }
    
    // Check if a type is integer
    bool isIntegerType(DataType type) {
        return type == TYPE_INT;
    }
    
    // Check if a type is boolean
    bool isBooleanType(DataType type) {
        return type == TYPE_BOOL;
    }
    
    // Check if types are compatible for assignment
    bool areTypesCompatible(DataType left, DataType right) {
        // Allow all numeric types to be compatible with each other for assignment
        if (isNumericType(left) && isNumericType(right)) {
            return true;
        }
        return left == right;
    }
    
    // Get the result type of a binary operation
    TypeInfo getBinaryOpResultType(TokenType op, const TypeInfo& left, const TypeInfo& right) {
        if (left.type == TYPE_ERROR || right.type == TYPE_ERROR) {
            return TypeInfo(TYPE_ERROR);
        }
        
        // Handle comparison operators (always return bool)
        if (op == T_EQUALOP || op == T_NE || op == T_LT || op == T_GT || op == T_LE || op == T_GE) {
            if (left.type == right.type || (isNumericType(left.type) && isNumericType(right.type))) {
                return TypeInfo(TYPE_BOOL);
            } else {
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        // Handle logical operators (always return bool)
        if (op == T_AND || op == T_OR) {
            if (left.type == TYPE_BOOL && right.type == TYPE_BOOL) {
                return TypeInfo(TYPE_BOOL);
            } else {
                addError(AttemptedBoolOpOnNonBools, "logical operation", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        // Handle arithmetic operators
        if (op == T_PLUS || op == T_MINUS || op == T_MULTIPLY || op == T_DIVIDE) {
            if (isNumericType(left.type) && isNumericType(right.type)) {
                // Return the "larger" numeric type
                if (left.type == TYPE_DOUBLE || right.type == TYPE_DOUBLE) {
                    return TypeInfo(TYPE_DOUBLE);
                } else if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
                    return TypeInfo(TYPE_FLOAT);
                } else {
                    return TypeInfo(TYPE_INT);
                }
            } else {
                addError(AttemptedAddOpOnNonNumeric, "arithmetic operation", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        // Handle modulo operator
        if (op == T_MODULO) {
            if (isIntegerType(left.type) && isIntegerType(right.type)) {
                return TypeInfo(TYPE_INT);
            } else {
                addError(AttemptedBitOpOnNonInt, "modulo operation", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        // Handle bitwise operators
        if (op == T_BITAND || op == T_BITOR || op == T_BITXOR) {
            if (isIntegerType(left.type) && isIntegerType(right.type)) {
                return TypeInfo(TYPE_INT);
            } else {
                addError(AttemptedBitOpOnNonInt, "bitwise operation", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        // Handle shift operators
        if (op == T_BITLSHIFT || op == T_BITRSHIFT) {
            if (isIntegerType(left.type) && isIntegerType(right.type)) {
                return TypeInfo(TYPE_INT);
            } else {
                addError(AttemptedShiftOnNonInt, "shift operation", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        // Handle assignment
        if (op == T_ASSIGNOP) {
            if (areTypesCompatible(left.type, right.type)) {
                return right; // Result type is the assigned value's type
            } else {
                addError(ExpressionTypeMismatch, "assignment", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        return TypeInfo(TYPE_ERROR);
    }
    
    // Get the type of a unary operation
    TypeInfo getUnaryOpResultType(TokenType op, const TypeInfo& operand) {
        if (operand.type == TYPE_ERROR) {
            return TypeInfo(TYPE_ERROR);
        }
        
        if (op == T_MINUS) {
            if (isNumericType(operand.type)) {
                return operand;
            } else {
                addError(AttemptedAddOpOnNonNumeric, "unary minus", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        if (op == T_NOT) {
            if (operand.type == TYPE_BOOL) {
                return TypeInfo(TYPE_BOOL);
            } else {
                addError(AttemptedBoolOpOnNonBools, "logical not", -1, -1);
                return TypeInfo(TYPE_ERROR);
            }
        }
        
        return TypeInfo(TYPE_ERROR);
    }
    
    // Get type of an expression
    TypeInfo getTypeOfExpression(const ASTNodeVariant& expr) {
        return visit([this](const auto& node) -> TypeInfo {
            using T = decay_t<decltype(node)>;
            
            if constexpr (is_same_v<T, IntLiteral>) {
                return TypeInfo(TYPE_INT);
            } else if constexpr (is_same_v<T, FloatLiteral>) {
                return TypeInfo(TYPE_FLOAT);
            } else if constexpr (is_same_v<T, StringLiteral>) {
                return TypeInfo(TYPE_STRING);
            } else if constexpr (is_same_v<T, CharLiteral>) {
                return TypeInfo(TYPE_CHAR);
            } else if constexpr (is_same_v<T, BoolLiteral>) {
                return TypeInfo(TYPE_BOOL);
            } else if constexpr (is_same_v<T, Identifier>) {
                // Check if it's an enum value first
                DataType* enumType = currentScope->findEnumValue(node.name);
                if (enumType) {
                    return TypeInfo(*enumType);
                }
                
                // Then check for variable
                TypeInfo* varType = currentScope->findVariable(node.name);
                if (varType) {
                    return *varType;
                }
                
                // Check if it's a function (for function pointers/calls)
                FunctionSignature* funcSig = currentScope->findFunction(node.name);
                if (funcSig) {
                    // For now, treat function as a special case
                    // In a real system, you'd have function types
                    return TypeInfo(TYPE_ERROR); // Functions should be called, not used as values
                }
                
                // If not found, it's an error
                addError(ErroneousVarDecl, node.name, -1, -1);
                return TypeInfo(TYPE_ERROR);
            } else if constexpr (is_same_v<T, BinaryExpr>) {
                TypeInfo leftType = getTypeOfExpression(node.left->node);
                TypeInfo rightType = getTypeOfExpression(node.right->node);
                return getBinaryOpResultType(node.op, leftType, rightType);
            } else if constexpr (is_same_v<T, UnaryExpr>) {
                TypeInfo operandType = getTypeOfExpression(node.operand->node);
                return getUnaryOpResultType(node.op, operandType);
            } else if constexpr (is_same_v<T, CallExpr>) {
                if (holds_alternative<Identifier>(node.callee->node)) {
                    const auto& calleeIdent = get<Identifier>(node.callee->node);
                    
                    FunctionSignature* funcSig = currentScope->findFunction(calleeIdent.name);
                    if (!funcSig) {
                        return TypeInfo(TYPE_ERROR);
                    }
                    
                    // Check parameter count
                    if (node.args.size() != funcSig->paramTypes.size()) {
                        addError(FnCallParamCount, calleeIdent.name, -1, -1);
                        return TypeInfo(TYPE_ERROR);
                    }
                    
                    // Check parameter types
                    for (size_t i = 0; i < node.args.size(); i++) {
                        TypeInfo argType = getTypeOfExpression(node.args[i]->node);
                        if (argType.type != funcSig->paramTypes[i] && 
                            !(isNumericType(argType.type) && isNumericType(funcSig->paramTypes[i]))) {
                            addError(FnCallParamType, calleeIdent.name, -1, -1);
                        }
                    }
                    
                    return TypeInfo(funcSig->returnType);
                }
                return TypeInfo(TYPE_ERROR);
            }
            
            return TypeInfo(TYPE_ERROR);
        }, expr);
    }
    
    void checkVariableDeclaration(const VarDecl& decl, int line, int col) {
        DataType varType = tokenTypeToDataType(decl.type);
        
        // Check if this is an enum type usage
        if (decl.type == T_ENUM) {
            DataType* enumType = currentScope->findEnumType(decl.name);
            if (enumType) {
                // This is using an enum type
                currentScope->addVariable(decl.name, TypeInfo(*enumType, true, decl.name));
            } else {
                addError(ErroneousVarDecl, decl.name, line, col);
            }
        } else {
            // Regular variable declaration
            currentScope->addVariable(decl.name, TypeInfo(varType));
            
            if (decl.initializer) {
                TypeInfo initType = getTypeOfExpression(decl.initializer->node);
                if (!areTypesCompatible(varType, initType.type)) {
                    addError(ExpressionTypeMismatch, decl.name, line, col);
                }
            }
        }
    }
    
    void checkFunctionDeclaration(const FunctionDecl& func) {
        DataType returnType = tokenTypeToDataType(func.returnType);
        vector<DataType> paramTypes;
        
        for (const auto& param : func.params) {
            paramTypes.push_back(tokenTypeToDataType(param.first));
        }
        
        FunctionSignature sig(returnType, paramTypes);
        currentScope->addFunction(func.name, sig);
        functionReturnTypes[func.name] = returnType;
        
        // Enter function scope
        auto funcScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = funcScope.get();
        
        // Add parameters to function scope
        for (const auto& param : func.params) {
            DataType paramType = tokenTypeToDataType(param.first);
            currentScope->addVariable(param.second, TypeInfo(paramType));
        }
        
        // Check function body
        bool hasReturn = false;
        for (const auto& stmt : func.body) {
            if (holds_alternative<ReturnStmt>(stmt->node)) {
                hasReturn = true;
            }
            checkNode(stmt->node);
        }
        
        // Check if function needs return statement
        if (returnType != TYPE_VOID && !hasReturn) {
            addError(ReturnStmtNotFound, func.name, -1, -1);
        }
        
        // Update function signature with return info
        FunctionSignature* updatedSig = currentScope->findFunction(func.name);
        if (updatedSig) {
            updatedSig->hasReturnStmt = hasReturn;
        }
        
        currentScope = oldScope;
    }
    
    void checkMainDeclaration(const MainDecl& main) {
        auto mainScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = mainScope.get();
        
        for (const auto& stmt : main.body) {
            checkNode(stmt->node);
        }
        
        currentScope = oldScope;
    }
    
    void checkIfStatement(const IfStmt& stmt) {
        // Check condition type
        TypeInfo condType = getTypeOfExpression(stmt.condition->node);
        if (condType.type != TYPE_BOOL) {
            addError(NonBooleanCondStmt, "if condition", -1, -1);
        }
        
        // Check if body
        auto ifScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = ifScope.get();
        
        for (const auto& ifStmt : stmt.ifBody) {
            checkNode(ifStmt->node);
        }
        
        currentScope = oldScope;
        
        // Check else body if exists
        if (!stmt.elseBody.empty()) {
            auto elseScope = make_unique<TypeScopeFrame>(currentScope);
            currentScope = elseScope.get();
            
            for (const auto& elseStmt : stmt.elseBody) {
                checkNode(elseStmt->node);
            }
            
            currentScope = oldScope;
        }
    }
    
    void checkWhileStatement(const WhileStmt& stmt) {
        // Check condition type
        TypeInfo condType = getTypeOfExpression(stmt.condition->node);
        if (condType.type != TYPE_BOOL) {
            addError(NonBooleanCondStmt, "while condition", -1, -1);
        }
        
        // Enter loop scope
        inLoop.push(true);
        
        auto loopScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = loopScope.get();
        
        for (const auto& bodyStmt : stmt.body) {
            checkNode(bodyStmt->node);
        }
        
        currentScope = oldScope;
        inLoop.pop();
    }
    
    void checkDoWhileStatement(const DoWhileStmt& stmt) {
        // Enter loop scope
        inLoop.push(true);
        
        auto loopScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = loopScope.get();
        
        if (stmt.body && holds_alternative<BlockStmt>(stmt.body->node)) {
            const auto& block = get<BlockStmt>(stmt.body->node);
            for (const auto& bodyStmt : block.body) {
                checkNode(bodyStmt->node);
            }
        }
        
        // Check condition type
        TypeInfo condType = getTypeOfExpression(stmt.condition->node);
        if (condType.type != TYPE_BOOL) {
            addError(NonBooleanCondStmt, "do-while condition", -1, -1);
        }
        
        currentScope = oldScope;
        inLoop.pop();
    }
    
    void checkForStatement(const ForStmt& stmt) {
        auto forScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = forScope.get();
        
        // Check initialization
        if (stmt.init) {
            if (holds_alternative<VarDecl>(stmt.init->node)) {
                const auto& varDecl = get<VarDecl>(stmt.init->node);
                checkVariableDeclaration(varDecl, -1, -1);
            } else {
                getTypeOfExpression(stmt.init->node);
            }
        }
        
        // Check condition
        if (stmt.condition) {
            TypeInfo condType = getTypeOfExpression(stmt.condition->node);
            if (condType.type != TYPE_BOOL) {
                addError(NonBooleanCondStmt, "for condition", -1, -1);
            }
        }
        
        // Check update
        if (stmt.update) {
            getTypeOfExpression(stmt.update->node);
        }
        
        // Enter loop scope
        inLoop.push(true);
        
        if (stmt.body && holds_alternative<BlockStmt>(stmt.body->node)) {
            const auto& block = get<BlockStmt>(stmt.body->node);
            for (const auto& bodyStmt : block.body) {
                checkNode(bodyStmt->node);
            }
        }
        
        inLoop.pop();
        currentScope = oldScope;
    }
    
    void checkSwitchStatement(const SwitchStmt& stmt) {
        TypeInfo exprType = getTypeOfExpression(stmt.expression->node);
        if (!isNumericType(exprType.type) && exprType.type != TYPE_CHAR) {
            addError(ExpressionTypeMismatch, "switch expression", -1, -1);
        }
        
        inSwitch.push(true);
        
        // Check cases
        for (const auto& caseStmt : stmt.cases) {
            if (caseStmt && holds_alternative<CaseBlock>(caseStmt->node)) {
                const auto& caseBlock = get<CaseBlock>(caseStmt->node);
                
                auto caseScope = make_unique<TypeScopeFrame>(currentScope);
                TypeScopeFrame* oldScope = currentScope;
                currentScope = caseScope.get();
                
                for (const auto& stmtInCase : caseBlock.body) {
                    checkNode(stmtInCase->node);
                }
                
                currentScope = oldScope;
            }
        }
        
        // Check default case
        if (!stmt.defaultBody.empty()) {
            auto defaultScope = make_unique<TypeScopeFrame>(currentScope);
            TypeScopeFrame* oldScope = currentScope;
            currentScope = defaultScope.get();
            
            for (const auto& defaultStmt : stmt.defaultBody) {
                checkNode(defaultStmt->node);
            }
            
            currentScope = oldScope;
        }
        
        inSwitch.pop();
    }
    
    void checkReturnStatement(const ReturnStmt& stmt, const string& currentFunctionName) {
        DataType expectedReturnType = functionReturnTypes[currentFunctionName];
        
        if (expectedReturnType == TYPE_VOID) {
            if (stmt.value) {
                addError(ErroneousReturnType, currentFunctionName, -1, -1);
            }
        } else {
            if (!stmt.value) {
                addError(ReturnStmtNotFound, currentFunctionName, -1, -1);
            } else {
                TypeInfo returnType = getTypeOfExpression(stmt.value->node);
                if (!areTypesCompatible(expectedReturnType, returnType.type)) {
                    addError(ErroneousReturnType, currentFunctionName, -1, -1);
                }
            }
        }
    }
    
    void checkBreakStatement() {
        if (inLoop.empty() && inSwitch.empty()) {
            addError(ErroneousBreak, "break", -1, -1);
        } else if (!inLoop.empty() && !inLoop.top() && !inSwitch.empty() && !inSwitch.top()) {
            addError(ErroneousBreak, "break", -1, -1);
        }
    }
    
    void checkNode(const ASTNodeVariant& node) {
        visit([this, &node](const auto& n) {  // Capture node by reference
            using T = decay_t<decltype(n)>;
            
            if constexpr (is_same_v<T, VarDecl>) {
                checkVariableDeclaration(n, -1, -1);
            } else if constexpr (is_same_v<T, FunctionDecl>) {
                checkFunctionDeclaration(n);
            } else if constexpr (is_same_v<T, MainDecl>) {
                checkMainDeclaration(n);
            } else if constexpr (is_same_v<T, IfStmt>) {
                checkIfStatement(n);
            } else if constexpr (is_same_v<T, WhileStmt>) {
                checkWhileStatement(n);
            } else if constexpr (is_same_v<T, DoWhileStmt>) {
                checkDoWhileStatement(n);
            } else if constexpr (is_same_v<T, ForStmt>) {
                checkForStatement(n);
            } else if constexpr (is_same_v<T, SwitchStmt>) {
                checkSwitchStatement(n);
            } else if constexpr (is_same_v<T, ReturnStmt>) {
                // We need to know the current function name for return checking
                // For now, we'll assume we're in a function and check the return type
                checkReturnStatement(n, "current_function"); // This would need context in a real implementation
            } else if constexpr (is_same_v<T, BreakStmt>) {
                checkBreakStatement();
            } else if constexpr (is_same_v<T, CallExpr>) {
                getTypeOfExpression(node);
            } else if constexpr (is_same_v<T, ExpressionStmt>) {
                getTypeOfExpression(n.expr->node);
            } else if constexpr (is_same_v<T, BlockStmt>) {
                auto blockScope = make_unique<TypeScopeFrame>(currentScope);
                TypeScopeFrame* oldScope = currentScope;
                currentScope = blockScope.get();
                
                for (const auto& stmt : n.body) {
                    checkNode(stmt->node);
                }
                
                currentScope = oldScope;
            } else if constexpr (is_same_v<T, PrintStmt>) {
                for (const auto& arg : n.args) {
                    getTypeOfExpression(arg->node);
                }
            }
        }, node);
    }

public:
    TypeChecker() {
        globalScope = make_unique<TypeScopeFrame>(nullptr);
        currentScope = globalScope.get();
    }

    vector<TypeChkErrorStruct> check(const vector<ASTPtr>& ast, const vector<Token>& tokenList) {
        tokens = tokenList;
        errors.clear();
        
        // Add built-in types and functions if needed
        // For now, process the AST
        for (const auto& node : ast) {
            if (holds_alternative<EnumDecl>(node->node)) {
                const auto& enumDecl = get<EnumDecl>(node->node);
                currentScope->addEnumType(enumDecl.name, TYPE_INT);
                
                if (holds_alternative<EnumValueList>(enumDecl.values->node)) {
                    const auto& valueList = get<EnumValueList>(enumDecl.values->node);
                    for (const string& value : valueList.values) {
                        currentScope->addEnumValue(value, TYPE_INT);
                    }
                }
            } else if (holds_alternative<FunctionDecl>(node->node)) {
                const auto& func = get<FunctionDecl>(node->node);
                DataType returnType = tokenTypeToDataType(func.returnType);
                vector<DataType> paramTypes;
                for (const auto& param : func.params) {
                    paramTypes.push_back(tokenTypeToDataType(param.first));
                }
                FunctionSignature sig(returnType, paramTypes);
                currentScope->addFunction(func.name, sig);
                functionReturnTypes[func.name] = returnType;
            }
        }
        
        // Process all nodes
        for (const auto& node : ast) {
            checkNode(node->node);
        }
        
        return errors;
    }
};

void performTypeChecking(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    TypeChecker checker;
    vector<TypeChkErrorStruct> errors = checker.check(ast, tokens);

    if (!errors.empty()) {
        cout << "\n=== Type Checking Errors ===\n";
        for (const auto& error : errors) {
            cout << "[Type Error] " << error.message << ")\n";
        }
        cout << "Type checking failed with " << errors.size() << " error(s)\n";
        exit(EXIT_FAILURE);
    }

    cout << "\n=== Type Checking Successful ===\n";
    cout << "No type errors found.\n";
}