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
    string currentFunction;
    unordered_map<string, string> enumValues;
    stack<string> breakLabels;
    
public:
    TACGenerator(const string& filename) : outputFile(filename), tempCounter(0), labelCounter(0) {
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
        // Process all declarations
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
        outputFile << code << endl;
    }
    
    void emitLabel(const string& label) {
        outputFile << label << ":" << endl;
    }
    
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
            else if constexpr (is_same_v<T, CaseBlock>) { processCaseBlock(n); return ""; }
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
        string result = newTemp();
        
        string op;
        switch (expr.op) {
            case T_PLUS: op = "+"; break;
            case T_MINUS: op = "-"; break;
            case T_MULTIPLY: op = "*"; break;
            case T_DIVIDE: op = "/"; break;
            case T_MODULO: op = "%"; break;
            case T_EQUALOP: op = "=="; break;
            case T_NE: op = "!="; break;
            case T_LT: op = "<"; break;
            case T_GT: op = ">"; break;
            case T_LE: op = "<="; break;
            case T_GE: op = ">="; break;
            case T_AND: op = "&&"; break;
            case T_OR: op = "||"; break;
            case T_BITAND: op = "&"; break;
            case T_BITOR: op = "|"; break;
            case T_BITXOR: op = "^"; break;
            case T_BITLSHIFT: op = "<<"; break;
            case T_BITRSHIFT: op = ">>"; break;
            case T_ASSIGNOP: 
                emit(left + " = " + right);
                return left;
            default: op = "?"; break;
        }
        
        if (expr.op != T_ASSIGNOP) {
            emit(result + " = " + left + " " + op + " " + right);
        }
        
        return result;
    }
    
    string processUnaryExpr(const UnaryExpr& expr) {
        string operand = processNode(expr.operand->node);
        string result = newTemp();
        
        string op;
        switch (expr.op) {
            case T_MINUS: op = "-"; break;
            case T_NOT: op = "!"; break;
            default: op = "?"; break;
        }
        
        emit(result + " = " + op + operand);
        return result;
    }
    
    void processIncludeStmt(const IncludeStmt& stmt) {
        // Include statements don't generate TAC
        emit("// include " + stmt.header);
    }
    
    void processEnumValueList(const EnumValueList& list) {
        // Store enum values for later use
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
        string result = newTemp();
        
        // Push arguments
        vector<string> args;
        for (const auto& arg : expr.args) {
            args.push_back(processNode(arg->node));
        }
        
        // Emit call
        string argsStr;
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) argsStr += ", ";
            argsStr += args[i];
        }
        
        if (callee != "print") {  // Only store result for non-void functions
            emit(result + " = call " + callee + "(" + argsStr + ")");
        } else {
            emit("call " + callee + "(" + argsStr + ")");
            result = "";
        }
        
        return result;
    }
    
    void processVarDecl(const VarDecl& decl) {
        if (decl.initializer) {
            string initVal = processNode(decl.initializer->node);
            emit(decl.name + " = " + initVal);
        } else {
            // Initialize with default value
            emit(decl.name + " = 0");
        }
    }
    
    void processBlockStmt(const BlockStmt& stmt) {
        for (const auto& s : stmt.body) {
            if (s) processNode(s->node);
        }
    }
    
    void processFunctionProto(const FunctionProto& proto) {
        // Function prototypes don't generate TAC code
        emit("// function prototype: " + proto.name);
    }
    
    void processFunctionDecl(const FunctionDecl& func) {
        string oldFunction = currentFunction;
        currentFunction = func.name;
        
        emit("\nfunction " + func.name + " begin");
        
        // Parameters are treated as local variables
        for (const auto& param : func.params) {
            emit(param.second + " = param");
        }
        
        // Process function body
        for (const auto& stmt : func.body) {
            if (stmt) processNode(stmt->node);
        }
        
        // Implicit return if none provided
        if (func.returnType != T_VOID) {
            emit("return 0");
        }
        
        emit("function " + func.name + " end");
        currentFunction = oldFunction;
    }
    
    void processMainDecl(const MainDecl& main) {
        currentFunction = "main";
        emit("\nfunction main begin");
        
        for (const auto& stmt : main.body) {
            if (stmt) processNode(stmt->node);
        }
        
        emit("return 0");
        emit("function main end");
        currentFunction = "";
    }
    
    void processIfStmt(const IfStmt& stmt) {
        string cond = processNode(stmt.condition->node);
        string elseLabel = newLabel();
        string endLabel = newLabel();
        
        emit("if " + cond + " == 0 goto " + elseLabel);
        
        // Then branch
        for (const auto& s : stmt.ifBody) {
            if (s) processNode(s->node);
        }
        
        if (!stmt.elseBody.empty()) {
            emit("goto " + endLabel);
            emitLabel(elseLabel);
            
            // Else branch
            for (const auto& s : stmt.elseBody) {
                if (s) processNode(s->node);
            }
            
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
        
        emitLabel(startLabel);
        emit("goto " + condLabel);
        emitLabel(condLabel);
        
        string cond = processNode(stmt.condition->node);
        emit("if " + cond + " == 0 goto " + endLabel);
        
        // Loop body
        for (const auto& s : stmt.body) {
            if (s) processNode(s->node);
        }
        
        emit("goto " + startLabel);
        emitLabel(endLabel);
        
        breakLabels.pop();
    }
    
    void processDoWhileStmt(const DoWhileStmt& stmt) {
        string startLabel = newLabel();
        string condLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        emitLabel(startLabel);
        
        // Loop body
        processNode(stmt.body->node);
        
        emitLabel(condLabel);
        string cond = processNode(stmt.condition->node);
        emit("if " + cond + " != 0 goto " + startLabel);
        
        emitLabel(endLabel);
        breakLabels.pop();
    }
    
    void processForStmt(const ForStmt& stmt) {
        string startLabel = newLabel();
        string condLabel = newLabel();
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        // Initialization
        if (stmt.init) {
            processNode(stmt.init->node);
        }
        
        emitLabel(startLabel);
        emit("goto " + condLabel);
        emitLabel(condLabel);
        
        // Condition
        if (stmt.condition) {
            string cond = processNode(stmt.condition->node);
            emit("if " + cond + " == 0 goto " + endLabel);
        }
        
        // Loop body
        processNode(stmt.body->node);
        
        // Update
        if (stmt.update) {
            processNode(stmt.update->node);
        }
        
        emit("goto " + startLabel);
        emitLabel(endLabel);
        
        breakLabels.pop();
    }
    
    void processCaseBlock(const CaseBlock& block) {
        // Case blocks are handled within switch statements
        string caseValue = processNode(block.value->node);
        string caseLabel = newLabel();
        
        emitLabel(caseLabel);
        
        for (const auto& stmt : block.body) {
            if (stmt) processNode(stmt->node);
        }
    }
    
    void processSwitchStmt(const SwitchStmt& stmt) {
        string expr = processNode(stmt.expression->node);
        string endLabel = newLabel();
        
        breakLabels.push(endLabel);
        
        vector<string> caseLabels;
        string defaultLabel = !stmt.defaultBody.empty() ? newLabel() : endLabel;
        
        // First pass: generate all case conditions
        for (const auto& caseBlock : stmt.cases) {
            if (auto caseNode = get_if<CaseBlock>(&caseBlock->node)) {
                string caseValue = processNode(caseNode->value->node);
                string caseLabel = newLabel();
                caseLabels.push_back(caseLabel);
                
                emit("if " + expr + " == " + caseValue + " goto " + caseLabel);
            }
        }
        
        // Jump to default or end
        emit("goto " + defaultLabel);
        
        // Second pass: emit case bodies in order
        size_t caseIndex = 0;
        for (const auto& caseBlock : stmt.cases) {
            if (auto caseNode = get_if<CaseBlock>(&caseBlock->node)) {
                emitLabel(caseLabels[caseIndex++]);
                for (const auto& stmt : caseNode->body) {
                    if (stmt) processNode(stmt->node);
                }
                emit("goto " + endLabel);
            }
        }
        
        // Default body
        if (!stmt.defaultBody.empty()) {
            emitLabel(defaultLabel);
            for (const auto& stmt : stmt.defaultBody) {
                if (stmt) processNode(stmt->node);
            }
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

// Entry function as specified
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