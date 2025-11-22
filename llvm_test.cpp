#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace llvm;
using namespace std;

struct FunctionInfo {
    Function* func;
    vector<string> paramNames;
};

class TACExecutor {
private:
    LLVMContext ctx;
    unique_ptr<Module> module;
    IRBuilder<> builder;
    
    // Type definitions
    Type* voidTy;
    Type* int32Ty;
    Type* int64Ty;
    Type* doubleTy;
    Type* ptrTy;
    
    // External functions
    Function* printfFunc;
    Function* scanfFunc;
    
    // Current function being processed
    Function* currentFunc;
    BasicBlock* currentBlock;
    
    // Symbol tables
    map<string, AllocaInst*> namedValues;  // Variables
    map<string, Value*> tempValues;         // Temporary values (t0, t1, etc.)
    map<string, BasicBlock*> labels;        // Labels (L0, L1, etc.)
    map<string, FunctionInfo> functions;    // Functions with param info
    
    vector<string> tacLines;
    size_t currentLine;
    
public:
    TACExecutor() : builder(ctx), currentFunc(nullptr), currentBlock(nullptr), currentLine(0) {
        module = make_unique<Module>("TACModule", ctx);
        
        // Initialize types
        voidTy = Type::getVoidTy(ctx);
        int32Ty = Type::getInt32Ty(ctx);
        int64Ty = Type::getInt64Ty(ctx);
        doubleTy = Type::getDoubleTy(ctx);
        ptrTy = PointerType::getUnqual(ctx);
        
        // Declare external functions
        declarePrintf();
        declareScanf();
    }
    
    void declarePrintf() {
        FunctionType* printfTy = FunctionType::get(int32Ty, {ptrTy}, true);
        printfFunc = Function::Create(printfTy, Function::ExternalLinkage, "printf", module.get());
    }
    
    void declareScanf() {
        FunctionType* scanfTy = FunctionType::get(int32Ty, {ptrTy}, true);
        scanfFunc = Function::Create(scanfTy, Function::ExternalLinkage, "scanf", module.get());
    }
    
    bool readTACFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Cannot open TAC file: " << filename << endl;
            return false;
        }
        
        string line;
        while (getline(file, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (!line.empty()) {
                tacLines.push_back(line);
            }
        }
        
        file.close();
        return true;
    }
    
    void execute() {
        // First pass: collect all function definitions and their parameters
        firstPass();
        
        // Second pass: generate code
        secondPass();
        
        // Verify the module
        string errStr;
        raw_string_ostream errStream(errStr);
        if (verifyModule(*module, &errStream)) {
            errs() << "Module verification failed:\n" << errStr << "\n";
        }
    }
    
    void firstPass() {
        for (size_t i = 0; i < tacLines.size(); i++) {
            string line = tacLines[i];
            
            if (line.find("function ") == 0 && line.find(" begin") != string::npos) {
                string funcName = extractFunctionName(line);
                
                // Count parameters by looking ahead for "param" statements
                vector<string> params;
                size_t j = i + 1;
                while (j < tacLines.size()) {
                    string nextLine = tacLines[j];
                    if (nextLine.find(" = param") != string::npos) {
                        size_t eqPos = nextLine.find(" = param");
                        string paramName = nextLine.substr(0, eqPos);
                        paramName.erase(0, paramName.find_first_not_of(" \t"));
                        paramName.erase(paramName.find_last_not_of(" \t") + 1);
                        params.push_back(paramName);
                        j++;
                    } else {
                        break;
                    }
                }
                
                // Create function with correct number of parameters
                vector<Type*> paramTypes(params.size(), int32Ty);
                FunctionType* funcTy = FunctionType::get(int32Ty, paramTypes, false);
                Function* func = Function::Create(funcTy, Function::ExternalLinkage, funcName, module.get());
                
                // Store function info
                functions[funcName] = {func, params};
            }
        }
    }
    
    void secondPass() {
        currentLine = 0;
        
        while (currentLine < tacLines.size()) {
            string line = tacLines[currentLine];
            
            if (line.find("function ") == 0 && line.find(" begin") != string::npos) {
                processFunction();
            } else if (line.find("//") == 0) {
                // Skip comments
                currentLine++;
            } else {
                currentLine++;
            }
        }
    }
    
    void processFunction() {
        string line = tacLines[currentLine];
        string funcName = extractFunctionName(line);
        
        FunctionInfo& funcInfo = functions[funcName];
        currentFunc = funcInfo.func;
        currentBlock = BasicBlock::Create(ctx, "entry", currentFunc);
        builder.SetInsertPoint(currentBlock);
        
        // Clear symbol tables for new function
        namedValues.clear();
        tempValues.clear();
        labels.clear();
        
        currentLine++;
        
        // Process parameters first
        size_t paramIdx = 0;
        for (auto& arg : currentFunc->args()) {
            if (paramIdx < funcInfo.paramNames.size()) {
                string paramName = funcInfo.paramNames[paramIdx];
                
                // Skip the "paramName = param" line in TAC
                if (currentLine < tacLines.size()) {
                    string paramLine = tacLines[currentLine];
                    if (paramLine.find(" = param") != string::npos) {
                        currentLine++;
                    }
                }
                
                // Create alloca and store parameter value
                AllocaInst* alloca = builder.CreateAlloca(int32Ty, nullptr, paramName);
                builder.CreateStore(&arg, alloca);
                namedValues[paramName] = alloca;
                
                paramIdx++;
            }
        }
        
        // Pre-scan for labels in this function
        size_t startLine = currentLine;
        while (currentLine < tacLines.size()) {
            string l = tacLines[currentLine];
            if (l.find("function ") == 0 && l.find(" end") != string::npos) {
                break;
            }
            if (l.back() == ':') {
                string labelName = l.substr(0, l.size() - 1);
                labels[labelName] = BasicBlock::Create(ctx, labelName, currentFunc);
            }
            currentLine++;
        }
        
        // Reset to process function body
        currentLine = startLine;
        
        // Process function body
        while (currentLine < tacLines.size()) {
            string l = tacLines[currentLine];
            
            if (l.find("function ") == 0 && l.find(" end") != string::npos) {
                // Ensure function has a return
                if (!currentBlock->getTerminator()) {
                    builder.CreateRet(ConstantInt::get(int32Ty, 0));
                }
                currentLine++;
                break;
            }
            
            processStatement(l);
            currentLine++;
        }
    }
    
    void processStatement(const string& stmt) {
        if (stmt.empty() || stmt.find("//") == 0) {
            return;
        }
        
        // Skip if current block already has terminator
        if (currentBlock->getTerminator()) {
            return;
        }
        
        // Handle labels
        if (stmt.back() == ':') {
            string labelName = stmt.substr(0, stmt.size() - 1);
            BasicBlock* labelBlock = labels[labelName];
            
            // Branch to label block if current block is not terminated
            if (!currentBlock->getTerminator()) {
                builder.CreateBr(labelBlock);
            }
            
            currentBlock = labelBlock;
            builder.SetInsertPoint(currentBlock);
            return;
        }
        
        // Assignment: var = value or var = t0 op t1
        if (stmt.find(" = ") != string::npos) {
            processAssignment(stmt);
        }
        // Conditional jump: if condition goto label
        else if (stmt.find("if ") == 0 && stmt.find(" goto ") != string::npos) {
            processConditionalGoto(stmt);
        }
        // Unconditional jump: goto label
        else if (stmt.find("goto ") == 0) {
            processGoto(stmt);
        }
        // Return statement
        else if (stmt.find("return") == 0) {
            processReturn(stmt);
        }
        // Function call without assignment
        else if (stmt.find("call ") == 0) {
            processFunctionCall(stmt);
        }
    }
    
    void processAssignment(const string& stmt) {
        size_t eqPos = stmt.find(" = ");
        string lhs = stmt.substr(0, eqPos);
        string rhs = stmt.substr(eqPos + 3);
        
        // Trim whitespace
        lhs.erase(0, lhs.find_first_not_of(" \t"));
        lhs.erase(lhs.find_last_not_of(" \t") + 1);
        rhs.erase(0, rhs.find_first_not_of(" \t"));
        rhs.erase(rhs.find_last_not_of(" \t") + 1);
        
        // Skip parameter assignments
        if (rhs == "param") {
            return;
        }
        
        Value* result = nullptr;
        
        // Check if RHS is a function call
        if (rhs.find("call ") == 0) {
            result = processFunctionCall(rhs);
        }
        // Check if RHS is a binary operation (a op b)
        else if (rhs.find(" + ") != string::npos || rhs.find(" - ") != string::npos ||
                 rhs.find(" * ") != string::npos || rhs.find(" / ") != string::npos ||
                 rhs.find(" % ") != string::npos || rhs.find(" == ") != string::npos ||
                 rhs.find(" != ") != string::npos || rhs.find(" < ") != string::npos ||
                 rhs.find(" > ") != string::npos || rhs.find(" <= ") != string::npos ||
                 rhs.find(" >= ") != string::npos || rhs.find(" && ") != string::npos ||
                 rhs.find(" || ") != string::npos) {
            result = processBinaryOp(rhs);
        }
        // Check if RHS is unary operation (-x or !x)
        else if ((rhs[0] == '-' || rhs[0] == '!') && rhs.size() > 1) {
            result = processUnaryOp(rhs);
        }
        // Otherwise, it's a simple value
        else {
            result = getValue(rhs);
        }
        
        if (!result) return;
        
        // Store result
        if (lhs[0] == 't') {
            // Temporary variable
            tempValues[lhs] = result;
        } else {
            // Named variable
            if (namedValues.find(lhs) == namedValues.end()) {
                // Allocate new variable
                AllocaInst* alloca = builder.CreateAlloca(int32Ty, nullptr, lhs);
                namedValues[lhs] = alloca;
            }
            builder.CreateStore(result, namedValues[lhs]);
        }
    }
    
    Value* processBinaryOp(const string& expr) {
        // Find operator
        vector<pair<string, string>> ops = {
            {" == ", "eq"}, {" != ", "ne"}, {" <= ", "le"}, {" >= ", "ge"},
            {" < ", "lt"}, {" > ", "gt"}, {" && ", "and"}, {" || ", "or"},
            {" + ", "add"}, {" - ", "sub"}, {" * ", "mul"}, {" / ", "div"}, {" % ", "mod"}
        };
        
        for (const auto& op : ops) {
            size_t pos = expr.find(op.first);
            if (pos != string::npos) {
                string left = expr.substr(0, pos);
                string right = expr.substr(pos + op.first.size());
                
                // Trim
                left.erase(0, left.find_first_not_of(" \t"));
                left.erase(left.find_last_not_of(" \t") + 1);
                right.erase(0, right.find_first_not_of(" \t"));
                right.erase(right.find_last_not_of(" \t") + 1);
                
                Value* lVal = getValue(left);
                Value* rVal = getValue(right);
                
                // Ensure both operands are same type (convert i1 to i32 if needed)
                if (lVal->getType()->isIntegerTy(1)) {
                    lVal = builder.CreateZExt(lVal, int32Ty);
                }
                if (rVal->getType()->isIntegerTy(1)) {
                    rVal = builder.CreateZExt(rVal, int32Ty);
                }
                
                // Perform operation
                Value* result = nullptr;
                if (op.second == "add") result = builder.CreateAdd(lVal, rVal);
                else if (op.second == "sub") result = builder.CreateSub(lVal, rVal);
                else if (op.second == "mul") result = builder.CreateMul(lVal, rVal);
                else if (op.second == "div") result = builder.CreateSDiv(lVal, rVal);
                else if (op.second == "mod") result = builder.CreateSRem(lVal, rVal);
                else if (op.second == "eq") result = builder.CreateICmpEQ(lVal, rVal);
                else if (op.second == "ne") result = builder.CreateICmpNE(lVal, rVal);
                else if (op.second == "lt") result = builder.CreateICmpSLT(lVal, rVal);
                else if (op.second == "gt") result = builder.CreateICmpSGT(lVal, rVal);
                else if (op.second == "le") result = builder.CreateICmpSLE(lVal, rVal);
                else if (op.second == "ge") result = builder.CreateICmpSGE(lVal, rVal);
                else if (op.second == "and") result = builder.CreateAnd(lVal, rVal);
                else if (op.second == "or") result = builder.CreateOr(lVal, rVal);
                
                // Convert comparison results (i1) to i32 for storage
                if (result && result->getType()->isIntegerTy(1)) {
                    result = builder.CreateZExt(result, int32Ty);
                }
                
                return result;
            }
        }
        
        return ConstantInt::get(int32Ty, 0);
    }
    
    Value* processUnaryOp(const string& expr) {
        char op = expr[0];
        string operand = expr.substr(1);
        operand.erase(0, operand.find_first_not_of(" \t"));
        
        Value* val = getValue(operand);
        
        if (op == '-') {
            return builder.CreateNeg(val);
        } else if (op == '!') {
            Value* result = builder.CreateNot(val);
            if (result->getType()->isIntegerTy(1)) {
                result = builder.CreateZExt(result, int32Ty);
            }
            return result;
        }
        
        return val;
    }
    
    Value* getValue(const string& name) {
        // Check if it's a number
        if (isdigit(name[0]) || (name[0] == '-' && name.size() > 1 && isdigit(name[1]))) {
            return ConstantInt::get(int32Ty, stoi(name));
        }
        
        // Check if it's a temporary
        if (tempValues.find(name) != tempValues.end()) {
            return tempValues[name];
        }
        
        // Check if it's a variable
        if (namedValues.find(name) != namedValues.end()) {
            return builder.CreateLoad(int32Ty, namedValues[name], name);
        }
        
        // Boolean literals
        if (name == "true") return ConstantInt::get(int32Ty, 1);
        if (name == "false") return ConstantInt::get(int32Ty, 0);
        
        // Default
        return ConstantInt::get(int32Ty, 0);
    }
    
    void processConditionalGoto(const string& stmt) {
        // Parse: if condition goto label
        size_t ifPos = stmt.find("if ");
        size_t gotoPos = stmt.find(" goto ");
        
        string condition = stmt.substr(ifPos + 3, gotoPos - ifPos - 3);
        string labelName = stmt.substr(gotoPos + 6);
        
        // Trim
        condition.erase(0, condition.find_first_not_of(" \t"));
        condition.erase(condition.find_last_not_of(" \t") + 1);
        labelName.erase(0, labelName.find_first_not_of(" \t"));
        labelName.erase(labelName.find_last_not_of(" \t") + 1);
        
        // Parse condition (var == 0 or var != 0)
        Value* condValue = nullptr;
        
        if (condition.find(" == 0") != string::npos) {
            string var = condition.substr(0, condition.find(" == 0"));
            var.erase(0, var.find_first_not_of(" \t"));
            var.erase(var.find_last_not_of(" \t") + 1);
            Value* val = getValue(var);
            
            // Ensure val is i32 for comparison
            if (val->getType()->isIntegerTy(1)) {
                val = builder.CreateZExt(val, int32Ty);
            }
            condValue = builder.CreateICmpEQ(val, ConstantInt::get(int32Ty, 0));
        } else if (condition.find(" != 0") != string::npos) {
            string var = condition.substr(0, condition.find(" != 0"));
            var.erase(0, var.find_first_not_of(" \t"));
            var.erase(var.find_last_not_of(" \t") + 1);
            Value* val = getValue(var);
            
            // Ensure val is i32 for comparison
            if (val->getType()->isIntegerTy(1)) {
                val = builder.CreateZExt(val, int32Ty);
            }
            condValue = builder.CreateICmpNE(val, ConstantInt::get(int32Ty, 0));
        }
        
        // Create blocks
        BasicBlock* thenBlock = labels[labelName];
        BasicBlock* elseBlock = BasicBlock::Create(ctx, "cont", currentFunc);
        
        builder.CreateCondBr(condValue, thenBlock, elseBlock);
        
        currentBlock = elseBlock;
        builder.SetInsertPoint(currentBlock);
    }
    
    void processGoto(const string& stmt) {
        string labelName = stmt.substr(5);
        labelName.erase(0, labelName.find_first_not_of(" \t"));
        labelName.erase(labelName.find_last_not_of(" \t") + 1);
        
        BasicBlock* targetBlock = labels[labelName];
        builder.CreateBr(targetBlock);
        
        // Create unreachable block for continuing
        currentBlock = BasicBlock::Create(ctx, "aftergoto", currentFunc);
        builder.SetInsertPoint(currentBlock);
    }
    
    void processReturn(const string& stmt) {
        if (stmt == "return") {
            builder.CreateRet(ConstantInt::get(int32Ty, 0));
        } else {
            string retVal = stmt.substr(7);
            retVal.erase(0, retVal.find_first_not_of(" \t"));
            retVal.erase(retVal.find_last_not_of(" \t") + 1);
            
            Value* val = getValue(retVal);
            builder.CreateRet(val);
        }
    }
    
    Value* processFunctionCall(const string& stmt) {
        // Parse: call func(args) or just call func(args)
        size_t callPos = stmt.find("call ");
        string rest = stmt.substr(callPos + 5);
        
        size_t parenPos = rest.find('(');
        string funcName = rest.substr(0, parenPos);
        
        // Get arguments
        size_t endParen = rest.find(')');
        string argsStr = rest.substr(parenPos + 1, endParen - parenPos - 1);
        
        vector<Value*> args;
        
        // Handle print specially
        if (funcName == "print") {
            // Parse argument
            argsStr.erase(0, argsStr.find_first_not_of(" \t"));
            argsStr.erase(argsStr.find_last_not_of(" \t") + 1);
            
            if (argsStr[0] == '"') {
                // String literal - just print it
                string str = argsStr.substr(1, argsStr.size() - 2);
                Value* strVal = builder.CreateGlobalString(str + "\n");
                builder.CreateCall(printfFunc, {strVal});
            } else {
                // Variable or number - print as integer
                Value* val = getValue(argsStr);
                Value* fmtStr = builder.CreateGlobalString("%d\n");
                builder.CreateCall(printfFunc, {fmtStr, val});
            }
            return nullptr;
        }
        
        // For other functions
        if (functions.find(funcName) != functions.end()) {
            // Parse arguments if any
            if (!argsStr.empty()) {
                stringstream ss(argsStr);
                string arg;
                while (getline(ss, arg, ',')) {
                    arg.erase(0, arg.find_first_not_of(" \t"));
                    arg.erase(arg.find_last_not_of(" \t") + 1);
                    args.push_back(getValue(arg));
                }
            }
            
            return builder.CreateCall(functions[funcName].func, args);
        }
        
        return nullptr;
    }
    
    string extractFunctionName(const string& line) {
        size_t start = line.find("function ") + 9;
        size_t end = line.find(" ", start);
        return line.substr(start, end - start);
    }
    
    void printIR() {
        outs() << "\n========== GENERATED LLVM IR ==========\n";
        module->print(outs(), nullptr);
        outs() << "=======================================\n\n";
    }
    
    void run() {
        outs() << "========== EXECUTING PROGRAM ==========\n";
        
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();
        
        string err;
        ExecutionEngine* ee = EngineBuilder(std::move(module)).setErrorStr(&err).create();
        
        if (!ee) {
            errs() << "Engine Error: " << err << "\n";
            return;
        }
        
        ee->finalizeObject();
        
        // Run main function
        if (functions.find("main") != functions.end()) {
            Function* mainFunc = functions["main"].func;
            ee->runFunction(mainFunc, {});
        } else {
            errs() << "Error: No main function found\n";
        }
        
        outs() << "=======================================\n";
        
        delete ee;
    }
};

int main(int argc, char* argv[]) {
    string tacFile = "tac.txt";
    
    if (argc > 1) {
        tacFile = argv[1];
    }
    
    TACExecutor executor;
    
    if (!executor.readTACFile(tacFile)) {
        return 1;
    }
    
    cout << "Reading TAC from: " << tacFile << endl;
    
    executor.execute();
    executor.printIR();
    executor.run();
    
    return 0;
}