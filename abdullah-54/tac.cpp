#include "compiler.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stack>
#include <memory>
#include <stdexcept>

using namespace std;

class TACGenerator {
private:
    ofstream outputFile;
    int tempCounter;
    int labelCounter;
    string currentFunction;
    unordered_map<string, string> enumValues;
    stack<string> breakLabels;
    stack<string> continueLabels;
    
    // Indentation level for readability
    int indentLevel;
    
    // Track emitted lines for optimization/validation
    vector<string> emittedLines;
    
public:
    explicit TACGenerator(const string& filename) 
        : tempCounter(0), labelCounter(0), indentLevel(0) {
        outputFile.open(filename, ios::binary);
        if (!outputFile.is_open()) {
            throw runtime_error("Cannot open output file for TAC: " + filename);
        }
    }
    
    ~TACGenerator() {
        if (outputFile.is_open()) {
            outputFile.flush();
            outputFile.close();
        }
    }
    
    void generate(const vector<ASTPtr>& ast) {
        try {
            for (const auto& node : ast) {
                if (node) {
                    processNode(node->node);
                }
            }
        } catch (const exception& e) {
            throw runtime_error("Error during TAC generation: " + string(e.what()));
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
        string indented = string(indentLevel * 2, ' ') + code;
        outputFile << indented << '\n';
        emittedLines.push_back(indented);
    }
    
    void emitLabel(const string& label) {
        // Labels should not be indented
        outputFile << label << ":\n";
        emittedLines.push_back(label + ":");
    }
    
    // Helper to validate temp/label variables
    void validateVariable(const string& var) {
        if (var.empty() || (var[0] != 't' && var[0] != 'L')) {
            // Not a generated temp or label, assume it's a valid identifier
        }
    }
    
    string processNode(const ASTNodeVariant& node) {
        return visit([this](const auto& n) -> string {
            using T = decay_t<decltype(n)>;
            
            if constexpr (is_same_v<T, IntLiteral>) {
                return to_string(n.value);
            }
            else if constexpr (is_same_v<T, FloatLiteral>) {
                return to_string(n.value);
            }
            else if constexpr (is_same_v<T, StringLiteral>) {
                return "\"" + escapeString(n.value) + "\"";
            }
            else if constexpr (is_same_v<T, CharLiteral>) {
                return "'" + string(1, n.value) + "'";
            }
            else if constexpr (is_same_v<T, BoolLiteral>) {
                return n.value ? "true" : "false";
            }
            else if constexpr (is_same_v<T, Identifier>) {
                return n.name;
            }
            else if constexpr (is_same_v<T, BinaryExpr>) {
                return processBinaryExpr(n);
            }
            else if constexpr (is_same_v<T, UnaryExpr>) {
                return processUnaryExpr(n);
            }
            else if constexpr (is_same_v<T, IncludeStmt>) {
                processIncludeStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, EnumValueList>) {
                processEnumValueList(n);
                return "";
            }
            else if constexpr (is_same_v<T, EnumDecl>) {
                processEnumDecl(n);
                return "";
            }
            else if constexpr (is_same_v<T, CallExpr>) {
                return processCallExpr(n);
            }
            else if constexpr (is_same_v<T, VarDecl>) {
                processVarDecl(n);
                return "";
            }
            else if constexpr (is_same_v<T, BlockStmt>) {
                processBlockStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, FunctionProto>) {
                processFunctionProto(n);
                return "";
            }
            else if constexpr (is_same_v<T, FunctionDecl>) {
                processFunctionDecl(n);
                return "";
            }
            else if constexpr (is_same_v<T, MainDecl>) {
                processMainDecl(n);
                return "";
            }
            else if constexpr (is_same_v<T, IfStmt>) {
                processIfStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, WhileStmt>) {
                processWhileStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, DoWhileStmt>) {
                processDoWhileStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, ForStmt>) {
                processForStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, SwitchStmt>) {
                processSwitchStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, ReturnStmt>) {
                processReturnStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, PrintStmt>) {
                processPrintStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, BreakStmt>) {
                processBreakStmt(n);
                return "";
            }
            else if constexpr (is_same_v<T, ExpressionStmt>) {
                processExpressionStmt(n);
                return "";
            }
            else {
                return "";
            }
        }, node);
    }
    
    string processBinaryExpr(const BinaryExpr& expr) {
        string left = processNode(expr.left->node);
        string right = processNode(expr.right->node);
        string result = newTemp();
        
        string op = getBinaryOpString(expr.op);
        
        if (expr.op == T_ASSIGNOP) {
            emit(left + " = " + right);
            return left;
        }
        
        emit(result + " = " + left + " " + op + " " + right);
        return result;
    }
    
    string getBinaryOpString(TokenType op) const {
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
        
        string op = (expr.op == T_MINUS) ? "-" : "!";
        emit(result + " = " + op + operand);
        return result;
    }
    
    string escapeString(const string& str) {
        string escaped;
        for (char c : str) {
            switch (c) {
                case '\n': escaped += "\\n"; break;
                case '\t': escaped += "\\t"; break;
                case '\r': escaped += "\\r"; break;
                case '"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                default: escaped += c;
            }
        }
        return escaped;
    }
    
    void processIncludeStmt(const IncludeStmt& stmt) {
        emit("; include " + stmt.header);
    }
    
    void processEnumValueList(const EnumValueList& list) {
        int value = 0;
        for (const auto& val : list.values) {
            enumValues[val] = to_string(value++);
        }
    }
    
    void processEnumDecl(const EnumDecl& decl) {
        emit("; enum " + decl.name + " {");
        indentLevel++;
        processNode(decl.values->node);
        indentLevel--;
        emit("; }");
    }
    
    string processCallExpr(const CallExpr& expr) {
        string callee = processNode(expr.callee->node);
        
        vector<string> args;
        for (const auto& arg : expr.args) {
            args.push_back(processNode(arg->node));
        }
        
        string argsStr = joinArgs(args);
        
        if (callee == "print") {
            emit("call print(" + argsStr + ")");
            return "";
        } else {
            string result = newTemp();
            emit(result + " = call " + callee + "(" + argsStr + ")");
            return result;
        }
    }
    
    string joinArgs(const vector<string>& args) const {
        string result;
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i];
        }
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
        indentLevel++;
        for (const auto& s : stmt.body) {
            if (s) processNode(s->node);
        }
        indentLevel--;
    }
    
    void processFunctionProto(const FunctionProto& proto) {
        emit("; function prototype: " + proto.name);
    }
    
    void processFunctionDecl(const FunctionDecl& func) {
        string oldFunction = currentFunction;
        currentFunction = func.name;
        
        emit("\nfunction " + func.name + " begin");
        indentLevel++;
        
        for (const auto& param : func.params) {
            emit(param.second + " = param");
        }
        
        for (const auto& stmt : func.body) {
            if (stmt) processNode(stmt->node);
        }
        
        if (func.returnType != T_VOID) {
            emit("return 0");
        }
        
        indentLevel--;
        emit("function " + func.name + " end");
        currentFunction = oldFunction;
    }
    
    void processMainDecl(const MainDecl& main) {
        currentFunction = "main";
        emit("\nfunction main begin");
        indentLevel++;
        
        for (const auto& stmt : main.body) {
            if (stmt) processNode(stmt->node);
        }
        
        emit("return 0");
        indentLevel--;
        emit("function main end");
        currentFunction = "";
    }
    
    void processIfStmt(const IfStmt& stmt) {
        string cond = processNode(stmt.condition->node);
        string elseLabel = newLabel();
        string endLabel = newLabel();
        
        emit("if " + cond + " == 0 goto " + elseLabel);
        indentLevel++;
        
        for (const auto& s : stmt.ifBody) {
            if (s) processNode(s->node);
        }
        
        indentLevel--;
        
        if (!stmt.elseBody.empty()) {
            emit("goto " + endLabel);
            emitLabel(elseLabel);
            indentLevel++;
            
            for (const auto& s : stmt.elseBody) {
                if (s) processNode(s->node);
            }
            
            indentLevel--;
            emitLabel(endLabel);
        } else {
            emitLabel(elseLabel);
        }
    }
    
    void processWhileStmt(const WhileStmt& stmt) {
        string startLabel = newLabel();
        string condLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        continueLabels.push(condLabel);
        
        emit("goto " + condLabel);
        emitLabel(startLabel);
        indentLevel++;
        
        for (const auto& s : stmt.body) {
            if (s) processNode(s->node);
        }
        
        indentLevel--;
        emitLabel(condLabel);
        string cond = processNode(stmt.condition->node);
        emit("if " + cond + " != 0 goto " + startLabel);
        emitLabel(endLabel);
        
        breakLabels.pop();
        continueLabels.pop();
    }
    
    void processDoWhileStmt(const DoWhileStmt& stmt) {
        string startLabel = newLabel();
        string condLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        continueLabels.push(condLabel);
        
        emitLabel(startLabel);
        indentLevel++;
        processNode(stmt.body->node);
        indentLevel--;
        
        emitLabel(condLabel);
        string cond = processNode(stmt.condition->node);
        emit("if " + cond + " != 0 goto " + startLabel);
        emitLabel(endLabel);
        
        breakLabels.pop();
        continueLabels.pop();
    }
    
    void processForStmt(const ForStmt& stmt) {
        string startLabel = newLabel();
        string condLabel = newLabel();
        string updateLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        continueLabels.push(updateLabel);
        
        if (stmt.init) {
            processNode(stmt.init->node);
        }
        
        emit("goto " + condLabel);
        emitLabel(startLabel);
        indentLevel++;
        
        processNode(stmt.body->node);
        
        indentLevel--;
        emitLabel(updateLabel);
        
        if (stmt.update) {
            processNode(stmt.update->node);
        }
        
        emitLabel(condLabel);
        
        if (stmt.condition) {
            string cond = processNode(stmt.condition->node);
            emit("if " + cond + " != 0 goto " + startLabel);
        } else {
            emit("goto " + startLabel);
        }
        
        emitLabel(endLabel);
        
        breakLabels.pop();
        continueLabels.pop();
    }
    
    void processSwitchStmt(const SwitchStmt& stmt) {
        string expr = processNode(stmt.expression->node);
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        vector<pair<string, string>> casePairs;
        string defaultLabel = !stmt.defaultBody.empty() ? newLabel() : endLabel;
        
        // First pass: generate case conditions
        for (const auto& caseBlock : stmt.cases) {
            if (auto caseNode = get_if<CaseBlock>(&caseBlock->node)) {
                string caseValue = processNode(caseNode->value->node);
                string caseLabel = newLabel();
                casePairs.push_back({caseValue, caseLabel});
                emit("if " + expr + " == " + caseValue + " goto " + caseLabel);
            }
        }
        
        emit("goto " + defaultLabel);
        
        // Second pass: emit case bodies
        size_t caseIndex = 0;
        for (const auto& caseBlock : stmt.cases) {
            if (auto caseNode = get_if<CaseBlock>(&caseBlock->node)) {
                emitLabel(casePairs[caseIndex++].second);
                indentLevel++;
                
                for (const auto& s : caseNode->body) {
                    if (s) processNode(s->node);
                }
                
                indentLevel--;
                emit("goto " + endLabel);
            }
        }
        
        if (!stmt.defaultBody.empty()) {
            emitLabel(defaultLabel);
            indentLevel++;
            
            for (const auto& s : stmt.defaultBody) {
                if (s) processNode(s->node);
            }
            
            indentLevel--;
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
        if (breakLabels.empty()) {
            throw runtime_error("break statement outside of loop/switch");
        }
        emit("goto " + breakLabels.top());
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