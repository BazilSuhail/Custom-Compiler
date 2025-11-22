#include "compiler.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stack>

using namespace std;

class TACGenerator {
private:
    ofstream outputFile;
    int tempCounter;
    int labelCounter;
    int indentLevel;  // Track indentation level
    string currentFunction;
    unordered_map<string, string> enumValues;
    stack<string> breakLabels;
    
public:
    TACGenerator(const string& filename) 
        : outputFile(filename), tempCounter(0), labelCounter(0), indentLevel(0) {
        if (!outputFile.is_open()) {
            throw runtime_error("Cannot open output file for TAC");
        }
    }
    
    ~TACGenerator() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
    
    void generate(const vector<ASTPtr>& ast) {
        for (const auto& node : ast) {
            if (node) {
                processNode(node->node);
            }
        }
    }

private:
    string newTemp() {
        return "t" + to_string(tempCounter++);
    }
    
    string newLabel() {
        return "L" + to_string(labelCounter++);
    }
    
    void emit(const string& code) {
        // Add indentation before emitting code
        outputFile << string(indentLevel * 2, ' ') << code << endl;
    }
    
    void emitLabel(const string& label) {
        // Labels are not indented (or minimally indented)
        outputFile << label << ":" << endl;
    }
    
    void increaseIndent() { indentLevel++; }
    void decreaseIndent() { if (indentLevel > 0) indentLevel--; }
    
    string processNode(const ASTNodeVariant& node) {
        return visit([this](const auto& n) -> string {
            using T = decay_t<decltype(n)>;
            
            if constexpr (is_same_v<T, IntLiteral>) return to_string(n.value);
            else if constexpr (is_same_v<T, FloatLiteral>) return to_string(n.value);
            else if constexpr (is_same_v<T, StringLiteral>) return "\"" + n.value + "\"";
            else if constexpr (is_same_v<T, CharLiteral>) return "'" + string(1, n.value) + "'";
            else if constexpr (is_same_v<T, BoolLiteral>) return n.value ? "true" : "false";
            else if constexpr (is_same_v<T, Identifier>) return n.name;
            else if constexpr (is_same_v<T, BinaryExpr>) return processBinaryExpr(n);
            else if constexpr (is_same_v<T, UnaryExpr>) return processUnaryExpr(n);
            else if constexpr (is_same_v<T, IncludeStmt>) { processIncludeStmt(n); return ""; }
            else if constexpr (is_same_v<T, EnumValueList>) { processEnumValueList(n); return ""; }
            else if constexpr (is_same_v<T, EnumDecl>) { processEnumDecl(n); return ""; }
            else if constexpr (is_same_v<T, CallExpr>) return processCallExpr(n);
            else if constexpr (is_same_v<T, VarDecl>) { processVarDecl(n); return ""; }
            else if constexpr (is_same_v<T, BlockStmt>) { processBlockStmt(n); return ""; }
            else if constexpr (is_same_v<T, FunctionProto>) { processFunctionProto(n); return ""; }
            else if constexpr (is_same_v<T, FunctionDecl>) { processFunctionDecl(n); return ""; }
            else if constexpr (is_same_v<T, MainDecl>) { processMainDecl(n); return ""; }
            else if constexpr (is_same_v<T, IfStmt>) { processIfStmt(n); return ""; }
            else if constexpr (is_same_v<T, WhileStmt>) { processWhileStmt(n); return ""; }
            else if constexpr (is_same_v<T, DoWhileStmt>) { processDoWhileStmt(n); return ""; }
            else if constexpr (is_same_v<T, ForStmt>) { processForStmt(n); return ""; }
            else if constexpr (is_same_v<T, SwitchStmt>) { processSwitchStmt(n); return ""; }
            else if constexpr (is_same_v<T, ReturnStmt>) { processReturnStmt(n); return ""; }
            else if constexpr (is_same_v<T, PrintStmt>) { processPrintStmt(n); return ""; }
            else if constexpr (is_same_v<T, BreakStmt>) { processBreakStmt(n); return ""; }
            else if constexpr (is_same_v<T, ExpressionStmt>) { processExpressionStmt(n); return ""; }
            else return "";
        }, node);
    }
    
    string processBinaryExpr(const BinaryExpr& expr) {
        string left = processNode(expr.left->node);
        string right = processNode(expr.right->node);
        
        // Handle assignment separately (no temp variable needed)
        if (expr.op == T_ASSIGNOP) {
            emit(left + " = " + right);
            return left;
        }
        
        string result = newTemp();
        string op = getOperatorString(expr.op);
        emit(result + " = " + left + " " + op + " " + right);
        
        return result;
    }
    
    string getOperatorString(TokenType op) {
        switch (op) {
            case T_PLUS: return "+";
            case T_MINUS: return "-";
            case T_MULTIPLY: return "*";
            case T_DIVIDE: return "/";
            case T_MODULO: return "%";
            case T_EQUALOP: return "==";
            case T_NE: return "!=";
            case T_LT: return "<";
            case T_GT: return ">";
            case T_LE: return "<=";
            case T_GE: return ">=";
            case T_AND: return "&&";
            case T_OR: return "||";
            case T_BITAND: return "&";
            case T_BITOR: return "|";
            case T_BITXOR: return "^";
            case T_BITLSHIFT: return "<<";
            case T_BITRSHIFT: return ">>";
            default: return "?";
        }
    }
    
    string processUnaryExpr(const UnaryExpr& expr) {
        string operand = processNode(expr.operand->node);
        string result = newTemp();
        
        string op = (expr.op == T_MINUS) ? "-" : (expr.op == T_NOT) ? "!" : "?";
        emit(result + " = " + op + operand);
        
        return result;
    }
    
    void processIncludeStmt(const IncludeStmt& stmt) {
        emit("// include " + stmt.header);
    }
    
    void processEnumValueList(const EnumValueList& list) {
        int value = 0;
        for (const auto& val : list.values) {
            enumValues[val] = to_string(value++);
        }
    }
    
    void processEnumDecl(const EnumDecl& decl) {
        emit("// enum " + decl.name);
        processNode(decl.values->node);
    }
    
    string processCallExpr(const CallExpr& expr) {
        string callee = processNode(expr.callee->node);
        
        // Build arguments string
        string argsStr;
        for (size_t i = 0; i < expr.args.size(); ++i) {
            if (i > 0) argsStr += ", ";
            argsStr += processNode(expr.args[i]->node);
        }
        
        // For void functions like print, don't create temp variable
        if (callee == "print") {
            emit("call " + callee + "(" + argsStr + ")");
            return "";
        }
        
        string result = newTemp();
        emit(result + " = call " + callee + "(" + argsStr + ")");
        return result;
    }
    
    void processVarDecl(const VarDecl& decl) {
        if (decl.initializer) {
            string initVal = processNode(decl.initializer->node);
            emit(decl.name + " = " + initVal);
        } else {
            emit(decl.name + " = 0");
        }
    }
    
    void processBlockStmt(const BlockStmt& stmt) {
        for (const auto& s : stmt.body) {
            if (s) processNode(s->node);
        }
    }
    
    void processFunctionProto(const FunctionProto& proto) {
        emit("// function prototype: " + proto.name);
    }
    
    void processFunctionDecl(const FunctionDecl& func) {
        string oldFunction = currentFunction;
        currentFunction = func.name;
        
        emit("\nfunction " + func.name + " begin");
        increaseIndent();
        
        // Parameters
        for (const auto& param : func.params) {
            emit(param.second + " = param");
        }
        
        // Function body
        for (const auto& stmt : func.body) {
            if (stmt) processNode(stmt->node);
        }
        
        // Implicit return for non-void functions
        /*if (func.returnType != T_VOID) {
            emit("return 0");
        }*/
        
        decreaseIndent();
        emit("function " + func.name + " end");
        currentFunction = oldFunction;
    }
    
    void processMainDecl(const MainDecl& main) {
        currentFunction = "main";
        emit("\nfunction main begin");
        increaseIndent();
        
        for (const auto& stmt : main.body) {
            if (stmt) processNode(stmt->node);
        }
        
        // emit("return 0");

        decreaseIndent();
        emit("function main end");
        currentFunction = "";
    }
    
    void processIfStmt(const IfStmt& stmt) {
        string cond = processNode(stmt.condition->node);
        string elseLabel = newLabel();
        string endLabel = newLabel();
        
        emit("if " + cond + " == 0 goto " + elseLabel);
        
        increaseIndent();
        for (const auto& s : stmt.ifBody) {
            if (s) processNode(s->node);
        }
        decreaseIndent();
        
        if (!stmt.elseBody.empty()) {
            emit("goto " + endLabel);
            emitLabel(elseLabel);
            
            increaseIndent();
            for (const auto& s : stmt.elseBody) {
                if (s) processNode(s->node);
            }
            decreaseIndent();
            
            emitLabel(endLabel);
        } else {
            emitLabel(elseLabel);
        }
    }
    
    void processWhileStmt(const WhileStmt& stmt) {
        string startLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        emitLabel(startLabel);
        string cond = processNode(stmt.condition->node);
        emit("if " + cond + " == 0 goto " + endLabel);
        
        increaseIndent();
        for (const auto& s : stmt.body) {
            if (s) processNode(s->node);
        }
        decreaseIndent();
        
        emit("goto " + startLabel);
        emitLabel(endLabel);
        
        breakLabels.pop();
    }
    
    void processDoWhileStmt(const DoWhileStmt& stmt) {
        string startLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        emitLabel(startLabel);
        increaseIndent();
        processNode(stmt.body->node);
        decreaseIndent();
        
        string cond = processNode(stmt.condition->node);
        emit("if " + cond + " != 0 goto " + startLabel);
        
        emitLabel(endLabel);
        breakLabels.pop();
    }
    
    void processForStmt(const ForStmt& stmt) {
        string startLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        // Initialization
        if (stmt.init) {
            processNode(stmt.init->node);
        }
        
        emitLabel(startLabel);
        
        // Condition
        if (stmt.condition) {
            string cond = processNode(stmt.condition->node);
            emit("if " + cond + " == 0 goto " + endLabel);
        }
        
        // Loop body
        increaseIndent();
        processNode(stmt.body->node);
        decreaseIndent();
        
        // Update
        if (stmt.update) {
            processNode(stmt.update->node);
        }
        
        emit("goto " + startLabel);
        emitLabel(endLabel);
        
        breakLabels.pop();
    }
    
    void processSwitchStmt(const SwitchStmt& stmt) {
        string expr = processNode(stmt.expression->node);
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        // Generate case labels and conditions
        vector<pair<string, const CaseBlock*>> caseLabelPairs;
        string defaultLabel = !stmt.defaultBody.empty() ? newLabel() : endLabel;
        
        // Generate all case tests
        for (const auto& caseBlock : stmt.cases) {
            if (auto caseNode = get_if<CaseBlock>(&caseBlock->node)) {
                string caseValue = processNode(caseNode->value->node);
                string caseLabel = newLabel();
                caseLabelPairs.push_back({caseLabel, caseNode});
                
                emit("if " + expr + " == " + caseValue + " goto " + caseLabel);
            }
        }
        
        // Jump to default or end if no cases match
        emit("goto " + defaultLabel);
        
        for (const auto& [caseLabel, caseNode] : caseLabelPairs) {
            emitLabel(caseLabel);
            increaseIndent();
            
            for (const auto& stmt : caseNode->body) {
                if (stmt) processNode(stmt->node);
            }
            
            decreaseIndent();
        }
        
        // Default case
        if (!stmt.defaultBody.empty()) {
            emitLabel(defaultLabel);
            increaseIndent();
            for (const auto& stmt : stmt.defaultBody) {
                if (stmt) processNode(stmt->node);
            }
            decreaseIndent();
        }
        
        emitLabel(endLabel);
        breakLabels.pop();
    }

    void processReturnStmt(const ReturnStmt& stmt) {
        if (stmt.value) {
            string retVal = processNode(stmt.value->node);
            emit("return " + retVal);
        } else {
            emit("return");
        }
    }
    
    void processPrintStmt(const PrintStmt& stmt) {
        for (const auto& arg : stmt.args) {
            string argVal = processNode(arg->node);
            emit("call print(" + argVal + ")");
        }
    }
    
    void processBreakStmt(const BreakStmt& stmt) {
        if (!breakLabels.empty()) {
            emit("goto " + breakLabels.top());
        }
    }
    
    void processExpressionStmt(const ExpressionStmt& stmt) {
        if (stmt.expr) {
            processNode(stmt.expr->node);
        }
    }
};

void generateTAC(const vector<ASTPtr>& ast, const string& outputFilename) {
    try {
        TACGenerator generator(outputFilename);
        generator.generate(ast);
        cout << "TAC generation successful. Output written to: " << outputFilename << endl;
    } catch (const exception& e) {
        cerr << "TAC Generation Error: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}