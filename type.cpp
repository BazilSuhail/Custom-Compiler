#include "compiler.h"
#include <stack>
#include <unordered_map>

class TypeChecker {
private:
    vector<TypeChkErrorStruct> errors;
    vector<Token> tokens;
    
    // Symbol table: variable name -> type
    unordered_map<string, DataType> symbolTypes;
    
    // Function signatures: name -> (return_type, param_types)
    unordered_map<string, pair<DataType, vector<DataType>>> functionSignatures;
    
    // Enum values: name -> type
    unordered_map<string, DataType> enumValueTypes;
    
    // Current function context
    stack<string> functionContext;
    
    // Track if inside loop/switch for break checking
    stack<bool> inLoopOrSwitch;
    
    // Track if function has return statement
    unordered_map<string, bool> functionRequiresReturn;

public:
    void addError(TypeChkError type, const string& name, int line, int col) {
        errors.push_back(TypeChkErrorStruct(type, name, line, col));
    }

    DataType tokenTypeToDataType(TokenType tokenType) {
        switch (tokenType) {
            case T_INT: return TYPE_INT;
            case T_FLOAT: return TYPE_FLOAT;
            case T_DOUBLE: return TYPE_DOUBLE;
            case T_CHAR: return TYPE_CHAR;
            case T_BOOL: return TYPE_BOOL;
            case T_STRING: return TYPE_STRING;
            case T_VOID: return TYPE_VOID;
            case T_ENUM: return TYPE_ENUM;
            default: return TYPE_ERROR;
        }
    }

    bool isNumericType(DataType type) {
        return type == TYPE_INT || type == TYPE_FLOAT || type == TYPE_DOUBLE;
    }

    bool isIntegerType(DataType type) {
        return type == TYPE_INT;
    }

    bool isBooleanType(DataType type) {
        return type == TYPE_BOOL;
    }

    bool isStringType(DataType type) {
        return type == TYPE_STRING;
    }

    bool isCharType(DataType type) {
        return type == TYPE_CHAR;
    }

    DataType getBinaryOpResultType(TokenType op, DataType left, DataType right) {
        switch (op) {
            case T_PLUS:
            case T_MINUS:
            case T_MULTIPLY:
            case T_DIVIDE:
                if (isNumericType(left) && isNumericType(right)) {
                    if (left == TYPE_DOUBLE || right == TYPE_DOUBLE) return TYPE_DOUBLE;
                    if (left == TYPE_FLOAT || right == TYPE_FLOAT) return TYPE_FLOAT;
                    return TYPE_INT;
                } else if (isStringType(left) && isStringType(right)) {
                    return TYPE_STRING; // String concatenation
                }
                return TYPE_ERROR;
                
            case T_MODULO:
                if (isIntegerType(left) && isIntegerType(right)) return TYPE_INT;
                return TYPE_ERROR;
                
            case T_EQUALOP:
            case T_NE:
            case T_LT:
            case T_GT:
            case T_LE:
            case T_GE:
                if ((isNumericType(left) && isNumericType(right)) ||
                    (isStringType(left) && isStringType(right)) ||
                    (isCharType(left) && isCharType(right)) ||
                    (left == right)) {
                    return TYPE_BOOL;
                }
                return TYPE_ERROR;
                
            case T_AND:
            case T_OR:
                if (isBooleanType(left) && isBooleanType(right)) return TYPE_BOOL;
                return TYPE_ERROR;
                
            case T_BITAND:
            case T_BITOR:
            case T_BITXOR:
                if (isIntegerType(left) && isIntegerType(right)) return TYPE_INT;
                return TYPE_ERROR;
                
            case T_BITLSHIFT:
            case T_BITRSHIFT:
                if (isIntegerType(left) && isIntegerType(right)) return TYPE_INT;
                return TYPE_ERROR;
                
            default:
                return TYPE_ERROR;
        }
    }

    DataType getTypeOfExpression(const ASTNodeVariant& expr) {
        return visit([this](const auto& node) -> DataType {
            using T = decay_t<decltype(node)>;
            
            if constexpr (is_same_v<T, IntLiteral>) return TYPE_INT;
            else if constexpr (is_same_v<T, FloatLiteral>) return TYPE_FLOAT;
            else if constexpr (is_same_v<T, StringLiteral>) return TYPE_STRING;
            else if constexpr (is_same_v<T, CharLiteral>) return TYPE_CHAR;
            else if constexpr (is_same_v<T, BoolLiteral>) return TYPE_BOOL;
            else if constexpr (is_same_v<T, Identifier>) {
                auto it = symbolTypes.find(node.name);
                if (it != symbolTypes.end()) return it->second;
                auto enumIt = enumValueTypes.find(node.name);
                if (enumIt != enumValueTypes.end()) return enumIt->second;
                return TYPE_ERROR;
            }
            else if constexpr (is_same_v<T, BinaryExpr>) {
                DataType leftType = getTypeOfExpression(node.left->node);
                DataType rightType = getTypeOfExpression(node.right->node);
                return getBinaryOpResultType(node.op, leftType, rightType);
            }
            else if constexpr (is_same_v<T, UnaryExpr>) {
                DataType operandType = getTypeOfExpression(node.operand->node);
                if (node.op == T_MINUS) {
                    return isNumericType(operandType) ? operandType : TYPE_ERROR;
                } else if (node.op == T_NOT) {
                    return isBooleanType(operandType) ? TYPE_BOOL : TYPE_ERROR;
                }
                return TYPE_ERROR;
            }
            else if constexpr (is_same_v<T, CallExpr>) {
                if (holds_alternative<Identifier>(node.callee->node)) {
                    const auto& callee = get<Identifier>(node.callee->node);
                    auto funcIt = functionSignatures.find(callee.name);
                    if (funcIt != functionSignatures.end()) {
                        const auto& expectedParams = funcIt->second.second;
                        
                        if (node.args.size() != expectedParams.size()) {
                            addError(FnCallParamCount, callee.name, -1, -1);
                            return TYPE_ERROR;
                        }
                        
                        for (size_t i = 0; i < node.args.size(); i++) {
                            DataType actualType = getTypeOfExpression(node.args[i]->node);
                            if (actualType != expectedParams[i]) {
                                addError(FnCallParamType, callee.name, -1, -1);
                                return TYPE_ERROR;
                            }
                        }
                        
                        return funcIt->second.first;
                    }
                }
                return TYPE_ERROR;
            }
            return TYPE_ERROR;
        }, expr);
    }

    bool isBooleanExpression(const ASTNodeVariant& expr) {
        return getTypeOfExpression(expr) == TYPE_BOOL;
    }

    void analyzeVarDecl(const VarDecl& decl, int line, int col) {
        DataType declaredType = tokenTypeToDataType(decl.type);
        
        if (decl.initializer) {
            DataType initializerType = getTypeOfExpression(decl.initializer->node);
            if (initializerType == TYPE_ERROR) {
                addError(ErroneousVarDecl, decl.name, line, col);
                return;
            }
            
            if (declaredType != initializerType && 
                !(isNumericType(declaredType) && isNumericType(initializerType))) {
                addError(ExpressionTypeMismatch, decl.name, line, col);
                return;
            }
        }
        
        symbolTypes[decl.name] = declaredType;
    }

    void analyzeFunctionDecl(const FunctionDecl& func, int line, int col) {
        DataType returnType = tokenTypeToDataType(func.returnType);
        vector<DataType> paramTypes;
        
        for (const auto& param : func.params) {
            paramTypes.push_back(tokenTypeToDataType(param.first));
        }
        functionSignatures[func.name] = make_pair(returnType, paramTypes);
        
        functionContext.push(func.name);
        functionRequiresReturn[func.name] = (returnType != TYPE_VOID);
        
        unordered_map<string, DataType> oldSymbolTypes = symbolTypes;
        
        for (const auto& param : func.params) {
            symbolTypes[param.second] = tokenTypeToDataType(param.first);
        }
        
        bool hasReturnStmt = false;
        for (const auto& stmt : func.body) {
            analyzeNode(stmt->node, true, hasReturnStmt);
        }
        
        if (functionRequiresReturn[func.name] && !hasReturnStmt) {
            addError(ReturnStmtNotFound, func.name, line, col);
        }
        
        symbolTypes = oldSymbolTypes;
        functionContext.pop();
    }

    void analyzeFunctionProto(const FunctionProto& proto) {
        DataType returnType = tokenTypeToDataType(proto.returnType);
        vector<DataType> paramTypes;
        
        for (const auto& param : proto.params) {
            paramTypes.push_back(tokenTypeToDataType(param.first));
        }
        functionSignatures[proto.name] = make_pair(returnType, paramTypes);
    }

    void analyzeEnumDecl(const EnumDecl& enm) {
        if (holds_alternative<EnumValueList>(enm.values->node)) {
            const auto& valueList = get<EnumValueList>(enm.values->node);
            for (const string& value : valueList.values) {
                enumValueTypes[value] = TYPE_INT;
            }
        }
        symbolTypes[enm.name] = TYPE_ENUM;
    }

    void analyzeIfStmt(const IfStmt& stmt) {
        if (stmt.condition && !isBooleanExpression(stmt.condition->node)) {
            addError(NonBooleanCondStmt, "if condition", -1, -1);
        }
        
        for (const auto& ifStmt : stmt.ifBody) {
            analyzeNode(ifStmt->node, false);
        }
        for (const auto& elseStmt : stmt.elseBody) {
            analyzeNode(elseStmt->node, false);
        }
    }

    void analyzeWhileStmt(const WhileStmt& stmt) {
        if (stmt.condition && !isBooleanExpression(stmt.condition->node)) {
            addError(NonBooleanCondStmt, "while condition", -1, -1);
        }
        
        inLoopOrSwitch.push(true);
        for (const auto& bodyStmt : stmt.body) {
            analyzeNode(bodyStmt->node, false);
        }
        inLoopOrSwitch.pop();
    }

    void analyzeDoWhileStmt(const DoWhileStmt& stmt) {
        inLoopOrSwitch.push(true);
        if (stmt.body) analyzeNode(stmt.body->node, false);
        if (stmt.condition && !isBooleanExpression(stmt.condition->node)) {
            addError(NonBooleanCondStmt, "do-while condition", -1, -1);
        }
        inLoopOrSwitch.pop();
    }

    void analyzeForStmt(const ForStmt& stmt) {
        inLoopOrSwitch.push(true);
        if (stmt.init) analyzeNode(stmt.init->node, false);
        if (stmt.condition && !isBooleanExpression(stmt.condition->node)) {
            addError(NonBooleanCondStmt, "for condition", -1, -1);
        }
        if (stmt.update) analyzeExpression(stmt.update->node);
        if (stmt.body) analyzeNode(stmt.body->node, false);
        inLoopOrSwitch.pop();
    }

    void analyzeSwitchStmt(const SwitchStmt& stmt) {
        inLoopOrSwitch.push(true);
        if (stmt.expression) {
            DataType exprType = getTypeOfExpression(stmt.expression->node);
            if (!isIntegerType(exprType)) {
                addError(ExpressionTypeMismatch, "switch expression", -1, -1);
            }
        }
        
        for (const auto& caseStmt : stmt.cases) {
            if (caseStmt && holds_alternative<CaseBlock>(caseStmt->node)) {
                const auto& caseBlock = get<CaseBlock>(caseStmt->node);
                for (const auto& caseBodyStmt : caseBlock.body) {
                    analyzeNode(caseBodyStmt->node, false);
                }
            }
        }
        
        for (const auto& defaultStmt : stmt.defaultBody) {
            analyzeNode(defaultStmt->node, false);
        }
        inLoopOrSwitch.pop();
    }

    void analyzeReturnStmt(const ReturnStmt& stmt, bool& hasReturnStmt) {
        hasReturnStmt = true;
        
        if (functionContext.empty()) return;
        string currentFunc = functionContext.top();
        auto funcIt = functionSignatures.find(currentFunc);
        if (funcIt == functionSignatures.end()) return;
        
        DataType expectedReturnType = funcIt->second.first;
        
        if (stmt.value) {
            DataType actualReturnType = getTypeOfExpression(stmt.value->node);
            if (expectedReturnType == TYPE_VOID) {
                addError(ErroneousReturnType, currentFunc, -1, -1);
            } else if (actualReturnType != expectedReturnType && 
                      !(isNumericType(expectedReturnType) && isNumericType(actualReturnType))) {
                addError(ErroneousReturnType, currentFunc, -1, -1);
            }
        } else if (expectedReturnType != TYPE_VOID) {
            addError(ErroneousReturnType, currentFunc, -1, -1);
        }
    }

    void analyzeBreakStmt() {
        if (inLoopOrSwitch.empty() || !inLoopOrSwitch.top()) {
            addError(ErroneousBreak, "break", -1, -1);
        }
    }

    void analyzeExpressionStmt(const ExpressionStmt& stmt) {
        analyzeExpression(stmt.expr->node);
    }

    void analyzeBinaryExpr(const BinaryExpr& expr) {
        DataType leftType = getTypeOfExpression(expr.left->node);
        DataType rightType = getTypeOfExpression(expr.right->node);
        
        if (leftType == TYPE_ERROR || rightType == TYPE_ERROR) {
            addError(ExpressionTypeMismatch, "binary expression", -1, -1);
            return;
        }
        
        DataType resultType = getBinaryOpResultType(expr.op, leftType, rightType);
        if (resultType == TYPE_ERROR) {
            if ((expr.op == T_AND || expr.op == T_OR) && 
                (!isBooleanType(leftType) || !isBooleanType(rightType))) {
                addError(AttemptedBoolOpOnNonBools, "boolean operation", -1, -1);
            } else if ((expr.op == T_BITAND || expr.op == T_BITOR || expr.op == T_BITXOR) &&
                      (!isIntegerType(leftType) || !isIntegerType(rightType))) {
                addError(AttemptedBitOpOnNonInt, "bitwise operation", -1, -1);
            } else if ((expr.op == T_BITLSHIFT || expr.op == T_BITRSHIFT) &&
                      (!isIntegerType(leftType) || !isIntegerType(rightType))) {
                addError(AttemptedShiftOnNonInt, "shift operation", -1, -1);
            } else if ((expr.op == T_PLUS || expr.op == T_MINUS) &&
                      (!isNumericType(leftType) || !isNumericType(rightType))) {
                addError(AttemptedAddOpOnNonNumeric, "arithmetic operation", -1, -1);
            } else {
                addError(ExpressionTypeMismatch, "binary expression", -1, -1);
            }
        }
    }

    void analyzeCallExpr(const CallExpr& call) {
        if (holds_alternative<Identifier>(call.callee->node)) {
            const auto& callee = get<Identifier>(call.callee->node);
            auto funcIt = functionSignatures.find(callee.name);
            if (funcIt != functionSignatures.end()) {
                if (call.args.size() != funcIt->second.second.size()) {
                    addError(FnCallParamCount, callee.name, -1, -1);
                }
            }
        }
    }

    void analyzeExpression(const ASTNodeVariant& expr) {
        visit([this](const auto& node) {
            using T = decay_t<decltype(node)>;
            if constexpr (is_same_v<T, BinaryExpr>) analyzeBinaryExpr(node);
            else if constexpr (is_same_v<T, CallExpr>) analyzeCallExpr(node);
        }, expr);
    }

    void analyzeNode(const ASTNodeVariant& node, bool inFunction = false, bool& hasReturnStmt = *(new bool(false))) {
        visit([this, inFunction, &hasReturnStmt](const auto& n) {
            using T = decay_t<decltype(n)>;
            
            if constexpr (is_same_v<T, VarDecl>) analyzeVarDecl(n, -1, -1);
            else if constexpr (is_same_v<T, FunctionDecl>) analyzeFunctionDecl(n, -1, -1);
            else if constexpr (is_same_v<T, FunctionProto>) analyzeFunctionProto(n);
            else if constexpr (is_same_v<T, EnumDecl>) analyzeEnumDecl(n);
            else if constexpr (is_same_v<T, IfStmt>) analyzeIfStmt(n);
            else if constexpr (is_same_v<T, WhileStmt>) analyzeWhileStmt(n);
            else if constexpr (is_same_v<T, DoWhileStmt>) analyzeDoWhileStmt(n);
            else if constexpr (is_same_v<T, ForStmt>) analyzeForStmt(n);
            else if constexpr (is_same_v<T, SwitchStmt>) analyzeSwitchStmt(n);
            else if constexpr (is_same_v<T, ReturnStmt>) analyzeReturnStmt(n, hasReturnStmt);
            else if constexpr (is_same_v<T, ExpressionStmt>) analyzeExpressionStmt(n);
            else if constexpr (is_same_v<T, BreakStmt>) analyzeBreakStmt();
        }, node);
    }

    vector<TypeChkErrorStruct> analyze(const vector<ASTPtr>& ast, const vector<Token>& tokenList) {
        tokens = tokenList;
        errors.clear();
        symbolTypes.clear();
        functionSignatures.clear();
        enumValueTypes.clear();
        functionContext = stack<string>();
        inLoopOrSwitch = stack<bool>();
        functionRequiresReturn.clear();
        
        for (const auto& node : ast) {
            bool dummyReturn = false;
            analyzeNode(node->node, false, dummyReturn);
        }
        
        return errors;
    }
};

void performTypeChecking(const vector<ASTPtr>& ast, const vector<Token>& tokens) {
    TypeChecker checker;
    vector<TypeChkErrorStruct> errors = checker.analyze(ast, tokens);

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