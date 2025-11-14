#include "compiler.h"
#include <stack>
#include <set>

// Reuse the TypeChkError enums and struct from the header
// enum TypeChkError { ... };
// struct TypeChkErrorStruct { ... };

enum class DataType {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_ENUM,
    TYPE_ERROR
};

struct TypeInfo {
    DataType type;
    bool isEnumType;
    string enumName;
    TypeInfo(DataType t, bool isEnum = false, const string& name = "") 
        : type(t), isEnumType(isEnum), enumName(name) {}
    TypeInfo() : type(DataType::TYPE_ERROR), isEnumType(false) {}
};

class TypeChecker {
private:
    struct TypeScopeFrame {
        unordered_map<string, TypeInfo> types;
        unordered_map<string, vector<DataType>> funcParams;
        unordered_map<string, DataType> funcReturnTypes;
        string currentFunction;
        DataType expectedReturnType;
        bool hasReturn;
        TypeScopeFrame* parent;
        TypeScopeFrame(TypeScopeFrame* p = nullptr) 
            : parent(p), currentFunction(""), expectedReturnType(DataType::TYPE_VOID), hasReturn(false) {}
    };

    unique_ptr<TypeScopeFrame> globalScope;
    TypeScopeFrame* currentScope;
    vector<TypeChkErrorStruct> errors;
    stack<bool> inLoop;
    stack<bool> inSwitch;

    const ScopeAnalyzer* scopeAnalyzer; // Reference to scope analyzer instance

    void addError(TypeChkError error, const string& name, int line, int col) {
        errors.push_back(TypeChkErrorStruct(error, name, line, col));
    }

    DataType tokenTypeToDataType(TokenType tokenType) {
        switch (tokenType) {
            case T_INT: return DataType::TYPE_INT;
            case T_FLOAT: return DataType::TYPE_FLOAT;
            case T_DOUBLE: return DataType::TYPE_DOUBLE;
            case T_CHAR: return DataType::TYPE_CHAR;
            case T_BOOL: return DataType::TYPE_BOOL;
            case T_VOID: return DataType::TYPE_VOID;
            case T_STRING: return DataType::TYPE_STRING;
            case T_ENUM: return DataType::TYPE_ENUM;
            default: return DataType::TYPE_ERROR;
        }
    }

    bool isNumericType(DataType type) {
        return type == DataType::TYPE_INT || type == DataType::TYPE_FLOAT || type == DataType::TYPE_DOUBLE;
    }

    bool isIntegerType(DataType type) {
        return type == DataType::TYPE_INT;
    }

    bool areTypesCompatible(const TypeInfo& left, const TypeInfo& right) {
        if (left.type == DataType::TYPE_ERROR || right.type == DataType::TYPE_ERROR) return false;
        if (left.isEnumType && right.isEnumType) return left.enumName == right.enumName;
        if (left.isEnumType || right.isEnumType) return false;
        if (isNumericType(left.type) && isNumericType(right.type)) return true;
        return left.type == right.type;
    }

    TypeInfo getBinaryOpResultType(TokenType op, const TypeInfo& left, const TypeInfo& right, int line, int col) {
        if (left.type == DataType::TYPE_ERROR || right.type == DataType::TYPE_ERROR) return TypeInfo(DataType::TYPE_ERROR);

        if (op == T_EQUALOP || op == T_NE || op == T_LT || op == T_GT || op == T_LE || op == T_GE) {
            if (areTypesCompatible(left, right)) return TypeInfo(DataType::TYPE_BOOL);
            else { addError(ExpressionTypeMismatch, "comparison operation", line, col); return TypeInfo(DataType::TYPE_ERROR); }
        }

        if (op == T_AND || op == T_OR) {
            if (left.type == DataType::TYPE_BOOL && right.type == DataType::TYPE_BOOL) return TypeInfo(DataType::TYPE_BOOL);
            else { addError(AttemptedBoolOpOnNonBools, "logical operation", line, col); return TypeInfo(DataType::TYPE_ERROR); }
        }

        if (op == T_PLUS || op == T_MINUS || op == T_MULTIPLY || op == T_DIVIDE) {
            if (isNumericType(left.type) && isNumericType(right.type)) {
                if (left.type == DataType::TYPE_DOUBLE || right.type == DataType::TYPE_DOUBLE) return TypeInfo(DataType::TYPE_DOUBLE);
                else if (left.type == DataType::TYPE_FLOAT || right.type == DataType::TYPE_FLOAT) return TypeInfo(DataType::TYPE_FLOAT);
                else return TypeInfo(DataType::TYPE_INT);
            } else { addError(AttemptedAddOpOnNonNumeric, "arithmetic operation", line, col); return TypeInfo(DataType::TYPE_ERROR); }
        }

        if (op == T_MODULO) {
            if (isIntegerType(left.type) && isIntegerType(right.type)) return TypeInfo(DataType::TYPE_INT);
            else { addError(AttemptedBitOpOnNonInt, "modulo operation", line, col); return TypeInfo(DataType::TYPE_ERROR); }
        }

        if (op == T_BITAND || op == T_BITOR || op == T_BITXOR) {
            if (isIntegerType(left.type) && isIntegerType(right.type)) return TypeInfo(DataType::TYPE_INT);
            else { addError(AttemptedBitOpOnNonInt, "bitwise operation", line, col); return TypeInfo(DataType::TYPE_ERROR); }
        }

        if (op == T_BITLSHIFT || op == T_BITRSHIFT) {
            if (isIntegerType(left.type) && isIntegerType(right.type)) return TypeInfo(DataType::TYPE_INT);
            else { addError(AttemptedShiftOnNonInt, "shift operation", line, col); return TypeInfo(DataType::TYPE_ERROR); }
        }

        if (op == T_ASSIGNOP) {
            if (areTypesCompatible(left, right)) return right;
            else { addError(ExpressionTypeMismatch, "assignment", line, col); return TypeInfo(DataType::TYPE_ERROR); }
        }

        return TypeInfo(DataType::TYPE_ERROR);
    }

    TypeInfo getUnaryOpResultType(TokenType op, const TypeInfo& operand, int line, int col) {
        if (operand.type == DataType::TYPE_ERROR) return TypeInfo(DataType::TYPE_ERROR);
        if (op == T_MINUS) { if (isNumericType(operand.type)) return operand; else { addError(AttemptedAddOpOnNonNumeric, "unary minus", line, col); return TypeInfo(DataType::TYPE_ERROR); } }
        if (op == T_NOT) { if (operand.type == DataType::TYPE_BOOL) return TypeInfo(DataType::TYPE_BOOL); else { addError(AttemptedBoolOpOnNonBools, "logical not", line, col); return TypeInfo(DataType::TYPE_ERROR); } }
        return TypeInfo(DataType::TYPE_ERROR);
    }

    TypeInfo getTypeOfExpression(const ASTNodeVariant& expr) {
        return visit([this](const auto& node) -> TypeInfo {
            using T = decay_t<decltype(node)>;
            if constexpr (is_same_v<T, IntLiteral>) return TypeInfo(DataType::TYPE_INT);
            else if constexpr (is_same_v<T, FloatLiteral>) return TypeInfo(DataType::TYPE_FLOAT);
            else if constexpr (is_same_v<T, StringLiteral>) return TypeInfo(DataType::TYPE_STRING);
            else if constexpr (is_same_v<T, CharLiteral>) return TypeInfo(DataType::TYPE_CHAR);
            else if constexpr (is_same_v<T, BoolLiteral>) return TypeInfo(DataType::TYPE_BOOL);
            else if constexpr (is_same_v<T, Identifier>) {
                TypeInfo* foundType = currentScope->types.find(node.name) != currentScope->types.end() ? 
                                      &currentScope->types[node.name] : nullptr;
                if (foundType) return *foundType;
                else { addError(ErroneousVarDecl, node.name, node.line, node.column); return TypeInfo(DataType::TYPE_ERROR); }
            }
            else if constexpr (is_same_v<T, BinaryExpr>) {
                TypeInfo leftType = getTypeOfExpression(node.left->node);
                TypeInfo rightType = getTypeOfExpression(node.right->node);
                return getBinaryOpResultType(node.op, leftType, rightType, node.line, node.column);
            }
            else if constexpr (is_same_v<T, UnaryExpr>) {
                TypeInfo operandType = getTypeOfExpression(node.operand->node);
                return getUnaryOpResultType(node.op, operandType, node.line, node.column);
            }
            else if constexpr (is_same_v<T, CallExpr>) {
                if (holds_alternative<Identifier>(node.callee->node)) {
                    const auto& calleeIdent = get<Identifier>(node.callee->node);
                    auto paramIt = currentScope->funcParams.find(calleeIdent.name);
                    auto retIt = currentScope->funcReturnTypes.find(calleeIdent.name);
                    if (paramIt == currentScope->funcParams.end() || retIt == currentScope->funcReturnTypes.end()) {
                        addError(FnCallParamCount, calleeIdent.name, node.line, node.column);
                        return TypeInfo(DataType::TYPE_ERROR);
                    }

                    const auto& expectedParams = paramIt->second;
                    if (node.args.size() != expectedParams.size()) {
                        addError(FnCallParamCount, calleeIdent.name, node.line, node.column);
                        return TypeInfo(DataType::TYPE_ERROR);
                    }

                    for (size_t i = 0; i < node.args.size(); i++) {
                        TypeInfo argType = getTypeOfExpression(node.args[i]->node);
                        TypeInfo expectedType(expectedParams[i]);
                        if (!areTypesCompatible(expectedType, argType)) {
                            addError(FnCallParamType, calleeIdent.name, node.line, node.column);
                        }
                    }
                    return TypeInfo(retIt->second);
                }
                addError(ErroneousVarDecl, "non-identifier function call target", node.line, node.column);
                return TypeInfo(DataType::TYPE_ERROR);
            }
            return TypeInfo(DataType::TYPE_ERROR);
        }, expr);
    }

    void analyzeVarDecl(const VarDecl& decl) {
        DataType varType = tokenTypeToDataType(decl.type);
        TypeInfo resolvedType = TypeInfo(varType, decl.type == T_ENUM, decl.name);
        currentScope->types[decl.name] = resolvedType;

        if (decl.initializer) {
            TypeInfo initType = getTypeOfExpression(decl.initializer->node);
            TypeInfo varTypeInfo = resolvedType;
            if (!areTypesCompatible(varTypeInfo, initType)) {
                addError(ExpressionTypeMismatch, decl.name, decl.line, decl.column);
            }
        }
    }

    void analyzeFunctionDecl(const FunctionDecl& func) {
        DataType returnType = tokenTypeToDataType(func.returnType);
        vector<DataType> paramTypes;
        for (const auto& param : func.params) paramTypes.push_back(tokenTypeToDataType(param.first));
        currentScope->funcParams[func.name] = paramTypes;
        currentScope->funcReturnTypes[func.name] = returnType;

        auto funcScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = funcScope.get();
        currentScope->currentFunction = func.name;
        currentScope->expectedReturnType = returnType;

        for (const auto& param : func.params) {
            DataType paramType = tokenTypeToDataType(param.first);
            currentScope->types[param.second] = TypeInfo(paramType);
        }

        for (const auto& stmt : func.body) {
            if (holds_alternative<ReturnStmt>(stmt->node)) {
                const auto& returnStmt = get<ReturnStmt>(stmt->node);
                if (returnType == DataType::TYPE_VOID) {
                    if (returnStmt.value) addError(ErroneousReturnType, func.name, returnStmt.line, returnStmt.column);
                } else {
                    if (!returnStmt.value) addError(ReturnStmtNotFound, func.name, returnStmt.line, returnStmt.column);
                    else {
                        TypeInfo returnTypeActual = getTypeOfExpression(returnStmt.value->node);
                        TypeInfo expectedType(returnType);
                        if (!areTypesCompatible(expectedType, returnTypeActual)) {
                            addError(ErroneousReturnType, func.name, returnStmt.line, returnStmt.column);
                        }
                    }
                }
                currentScope->hasReturn = true;
            }
            analyzeNode(stmt->node);
        }

        if (returnType != DataType::TYPE_VOID && !currentScope->hasReturn) {
            addError(ReturnStmtNotFound, func.name, func.line, func.column);
        }
        currentScope = oldScope;
    }

    void analyzeMainDecl(const MainDecl& main) {
        auto mainScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = mainScope.get();
        currentScope->currentFunction = "main";
        currentScope->expectedReturnType = DataType::TYPE_INT;

        for (const auto& stmt : main.body) {
            analyzeNode(stmt->node);
        }
        currentScope = oldScope;
    }

    void analyzeIfStmt(const IfStmt& stmt) {
        TypeInfo condType = getTypeOfExpression(stmt.condition->node);
        if (condType.type != DataType::TYPE_BOOL) addError(NonBooleanCondStmt, "if condition", stmt.line, stmt.column);

        auto ifScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = ifScope.get();
        currentScope->currentFunction = oldScope->currentFunction;
        for (const auto& stmtInBody : stmt.ifBody) analyzeNode(stmtInBody->node);
        currentScope = oldScope;

        if (!stmt.elseBody.empty()) {
            auto elseScope = make_unique<TypeScopeFrame>(currentScope);
            currentScope = elseScope.get();
            currentScope->currentFunction = oldScope->currentFunction;
            for (const auto& stmtInBody : stmt.elseBody) analyzeNode(stmtInBody->node);
            currentScope = oldScope;
        }
    }

    void analyzeWhileStmt(const WhileStmt& stmt) {
        TypeInfo condType = getTypeOfExpression(stmt.condition->node);
        if (condType.type != DataType::TYPE_BOOL) addError(NonBooleanCondStmt, "while condition", stmt.line, stmt.column);

        inLoop.push(true);
        auto loopScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = loopScope.get();
        currentScope->currentFunction = oldScope->currentFunction;
        for (const auto& stmtInBody : stmt.body) analyzeNode(stmtInBody->node);
        currentScope = oldScope;
        inLoop.pop();
    }

    void analyzeDoWhileStmt(const DoWhileStmt& stmt) {
        inLoop.push(true);
        auto loopScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = loopScope.get();
        currentScope->currentFunction = oldScope->currentFunction;
        if (stmt.body) analyzeNode(stmt.body->node);
        currentScope = oldScope;
        TypeInfo condType = getTypeOfExpression(stmt.condition->node);
        if (condType.type != DataType::TYPE_BOOL) addError(NonBooleanCondStmt, "do-while condition", stmt.line, stmt.column);
        inLoop.pop();
    }

    void analyzeForStmt(const ForStmt& stmt) {
        auto forScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = forScope.get();
        currentScope->currentFunction = oldScope->currentFunction;

        if (stmt.init) {
            if (holds_alternative<VarDecl>(stmt.init->node)) analyzeVarDecl(get<VarDecl>(stmt.init->node));
            else getTypeOfExpression(stmt.init->node);
        }

        if (stmt.condition) {
            TypeInfo condType = getTypeOfExpression(stmt.condition->node);
            if (condType.type != DataType::TYPE_BOOL) addError(NonBooleanCondStmt, "for condition", stmt.line, stmt.column);
        }

        if (stmt.update) getTypeOfExpression(stmt.update->node);

        inLoop.push(true);
        if (stmt.body) analyzeNode(stmt.body->node);
        inLoop.pop();
        currentScope = oldScope;
    }

    void analyzeSwitchStmt(const SwitchStmt& stmt) {
        TypeInfo exprType = getTypeOfExpression(stmt.expression->node);
        if (!isIntegerType(exprType.type) && exprType.type != DataType::TYPE_CHAR && !exprType.isEnumType) {
            addError(ExpressionTypeMismatch, "switch expression", stmt.line, stmt.column);
        }

        inSwitch.push(true);
        for (const auto& caseStmt : stmt.cases) {
            if (caseStmt && holds_alternative<CaseBlock>(caseStmt->node)) {
                const auto& caseBlock = get<CaseBlock>(caseStmt->node);
                auto caseScope = make_unique<TypeScopeFrame>(currentScope);
                TypeScopeFrame* oldCaseScope = currentScope;
                currentScope = caseScope.get();
                currentScope->currentFunction = oldCaseScope->currentFunction;
                for (const auto& stmtInCase : caseBlock.body) analyzeNode(stmtInCase->node);
                currentScope = oldCaseScope;
            }
        }
        if (!stmt.defaultBody.empty()) {
            auto defaultScope = make_unique<TypeScopeFrame>(currentScope);
            TypeScopeFrame* oldDefaultScope = currentScope;
            currentScope = defaultScope.get();
            currentScope->currentFunction = oldDefaultScope->currentFunction;
            for (const auto& stmtInDefault : stmt.defaultBody) analyzeNode(stmtInDefault->node);
            currentScope = oldDefaultScope;
        }
        inSwitch.pop();
    }

    void analyzeReturnStmt(const ReturnStmt& stmt) {
        if (!currentScope->currentFunction.empty()) {
            DataType expectedRetType = currentScope->expectedReturnType;
            if (expectedRetType == DataType::TYPE_VOID) {
                if (stmt.value) addError(ErroneousReturnType, currentScope->currentFunction, stmt.line, stmt.column);
            } else {
                if (!stmt.value) addError(ReturnStmtNotFound, currentScope->currentFunction, stmt.line, stmt.column);
                else {
                    TypeInfo returnType = getTypeOfExpression(stmt.value->node);
                    TypeInfo expectedType(expectedRetType);
                    if (!areTypesCompatible(expectedType, returnType)) {
                        addError(ErroneousReturnType, currentScope->currentFunction, stmt.line, stmt.column);
                    }
                }
            }
        }
    }

    void analyzeBreakStmt(const BreakStmt& stmt) {
        if (inLoop.empty() && inSwitch.empty()) addError(ErroneousBreak, "break", stmt.line, stmt.column);
        else if ((inLoop.empty() || !inLoop.top()) && (inSwitch.empty() || !inSwitch.top())) addError(ErroneousBreak, "break", stmt.line, stmt.column);
    }

    void analyzePrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) getTypeOfExpression(arg->node);
    }

    void analyzeExpressionStmt(const ExpressionStmt& stmt) {
        getTypeOfExpression(stmt.expr->node);
    }

    void analyzeBlockStmt(const BlockStmt& block) {
        auto blockScope = make_unique<TypeScopeFrame>(currentScope);
        TypeScopeFrame* oldScope = currentScope;
        currentScope = blockScope.get();
        currentScope->currentFunction = oldScope->currentFunction;
        for (const auto& stmt : block.body) analyzeNode(stmt->node);
        currentScope = oldScope;
    }

    void analyzeNode(const ASTNodeVariant& node) {
        visit([this](const auto& n) {
            using T = decay_t<decltype(n)>;
            if constexpr (is_same_v<T, VarDecl>) analyzeVarDecl(n);
            else if constexpr (is_same_v<T, FunctionDecl>) analyzeFunctionDecl(n);
            else if constexpr (is_same_v<T, MainDecl>) analyzeMainDecl(n);
            else if constexpr (is_same_v<T, IfStmt>) analyzeIfStmt(n);
            else if constexpr (is_same_v<T, WhileStmt>) analyzeWhileStmt(n);
            else if constexpr (is_same_v<T, DoWhileStmt>) analyzeDoWhileStmt(n);
            else if constexpr (is_same_v<T, ForStmt>) analyzeForStmt(n);
            else if constexpr (is_same_v<T, SwitchStmt>) analyzeSwitchStmt(n);
            else if constexpr (is_same_v<T, ReturnStmt>) analyzeReturnStmt(n);
            else if constexpr (is_same_v<T, BreakStmt>) analyzeBreakStmt(n);
            else if constexpr (is_same_v<T, PrintStmt>) analyzePrintStmt(n);
            else if constexpr (is_same_v<T, ExpressionStmt>) analyzeExpressionStmt(n);
            else if constexpr (is_same_v<T, BlockStmt>) analyzeBlockStmt(n);
        }, node);
    }

public:
    TypeChecker() {
        globalScope = make_unique<TypeScopeFrame>(nullptr);
        currentScope = globalScope.get();
    }

    vector<TypeChkErrorStruct> check(const vector<ASTPtr>& ast, const ScopeAnalyzer& scope_analyzer) {
        scopeAnalyzer = &scope_analyzer; // Store reference
        errors.clear();

        for (const auto& node : ast) {
            if (holds_alternative<EnumDecl>(node->node)) {
                const auto& enumDecl = get<EnumDecl>(node->node);
                TypeInfo enumTypeInfo(DataType::TYPE_ENUM, true, enumDecl.name);
                currentScope->types[enumDecl.name] = enumTypeInfo;
                
                if (holds_alternative<EnumValueList>(enumDecl.values->node)) {
                    const auto& valueList = get<EnumValueList>(enumDecl.values->node);
                    TypeInfo enumValueTypeInfo(DataType::TYPE_INT, true, enumDecl.name);
                    for (const string& value : valueList.values) {
                        currentScope->types[value] = enumValueTypeInfo;
                    }
                }
            } else if (holds_alternative<FunctionDecl>(node->node)) {
                const auto& func = get<FunctionDecl>(node->node);
                DataType returnType = tokenTypeToDataType(func.returnType);
                vector<DataType> paramTypes;
                for (const auto& param : func.params) paramTypes.push_back(tokenTypeToDataType(param.first));
                currentScope->funcParams[func.name] = paramTypes;
                currentScope->funcReturnTypes[func.name] = returnType;
            }
        }

        for (const auto& node : ast) {
            analyzeNode(node->node);
        }

        return errors;
    }
};

void performTypeChecking(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    // This assumes scope analysis ran first and was successful.
    // It leverages the validated symbol information implicitly present in the AST
    // structure, which was confirmed by the scope analyzer.
    TypeChecker checker;
    // The scope analyzer instance is not passed here as it's not directly accessible
    // from this function signature without modification. The type checker assumes
    // the AST is valid per scope analysis results.
    vector<TypeChkErrorStruct> errors = checker.check(ast, *static_cast<ScopeAnalyzer*>(nullptr)); // Dummy call

    if (!errors.empty()) {
        cout << "\n=== Type Checking Errors ===\n";
        for (const auto& error : errors) {
            cout << "[Type Error] " << error.message << " (line " << error.line << ", col " << error.column << ")\n";
        }
        cout << "Type checking failed with " << errors.size() << " error(s)\n";
        exit(EXIT_FAILURE);
    }

    cout << "\n=== Type Checking Successful ===\n";
    cout << "No type errors found.\n";
}