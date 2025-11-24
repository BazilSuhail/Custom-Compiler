#include "compiler.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>

#include <algorithm>

using namespace llvm;
using namespace std;

enum class VarType {
    INT,
    FLOAT,
    DOUBLE,
    CHAR,
    STRING,
    BOOL,
    VOID,
    UNKNOWN
};

struct VariableInfo {
    AllocaInst* alloca;
    VarType type;
};

struct FunctionInfo {
    Function* func;
    vector<string> paramNames;
    vector<VarType> paramTypes;
    VarType returnType;
};

class TACExecutor {
private:
    LLVMContext ctx;
    unique_ptr<Module> module;
    IRBuilder<> builder;
    
    Type* voidTy;
    Type* int32Ty;
    Type* int8Ty;
    Type* floatTy;
    Type* doubleTy;
    Type* ptrTy;
    
    Function* printfFunc;
    Function* putsFunc;
    Function* putcharFunc;
    
    Function* currentFunc;
    BasicBlock* currentBlock;
    
    map<string, VariableInfo> namedValues;
    map<string, pair<Value*, VarType>> tempValues;
    map<string, BasicBlock*> labels;
    map<string, FunctionInfo> functions;
    
    vector<string> tacLines;
    size_t currentLine;
    
public:
    TACExecutor() : builder(ctx), currentFunc(nullptr), currentBlock(nullptr), currentLine(0) {
        module = make_unique<Module>("TACModule", ctx);
        
        voidTy = Type::getVoidTy(ctx);
        int32Ty = Type::getInt32Ty(ctx);
        int8Ty = Type::getInt8Ty(ctx);
        floatTy = Type::getFloatTy(ctx);
        doubleTy = Type::getDoubleTy(ctx);
        ptrTy = PointerType::getUnqual(ctx);
        
        declarePrintf();
        declarePuts();
        declarePutchar();
    }
    
    void declarePrintf() {
        FunctionType* printfTy = FunctionType::get(int32Ty, {ptrTy}, true);
        printfFunc = Function::Create(printfTy, Function::ExternalLinkage, "printf", module.get());
    }
    
    void declarePuts() {
        FunctionType* putsTy = FunctionType::get(int32Ty, {ptrTy}, false);
        putsFunc = Function::Create(putsTy, Function::ExternalLinkage, "puts", module.get());
    }
    
    void declarePutchar() {
        FunctionType* putcharTy = FunctionType::get(int32Ty, {int32Ty}, false);
        putcharFunc = Function::Create(putcharTy, Function::ExternalLinkage, "putchar", module.get());
    }
    
    bool readTACFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Cannot open TAC file: " << filename << endl;
            return false;
        }
        
        string line;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (!line.empty()) {
                tacLines.push_back(line);
            }
        }
        
        file.close();
        return true;
    }
    
    VarType parseType(const string& typeStr) {
        if (typeStr == "int") return VarType::INT;
        if (typeStr == "float") return VarType::FLOAT;
        if (typeStr == "double") return VarType::DOUBLE;
        if (typeStr == "char") return VarType::CHAR;
        if (typeStr == "string") return VarType::STRING;
        if (typeStr == "bool") return VarType::BOOL;
        if (typeStr == "void") return VarType::VOID;
        return VarType::UNKNOWN;
    }
    
    Type* getLLVMType(VarType type) {
        switch (type) {
            case VarType::INT: return int32Ty;
            case VarType::BOOL: return int32Ty;
            case VarType::FLOAT: return floatTy;
            case VarType::DOUBLE: return doubleTy;
            case VarType::CHAR: return int8Ty;
            case VarType::STRING: return ptrTy;
            case VarType::VOID: return voidTy;
            default: return int32Ty;
        }
    }
    
    void execute() {
        firstPass();
        secondPass();
        
        string errStr;
        raw_string_ostream errStream(errStr);
        if (verifyModule(*module, &errStream)) {
            errs() << "Module verification failed:\n" << errStr << "\n";
        }
    }
    
    void firstPass() {
        for (size_t i = 0; i < tacLines.size(); i++) {
            string line = tacLines[i];
            
            // Parse function declaration: [type] function name begin
            if (line.find("function ") != string::npos && line.find(" begin") != string::npos) {
                VarType returnType = VarType::INT;  // Default
                string funcName;
                
                // Check if return type is specified
                size_t funcPos = line.find("function ");
                string beforeFunc = line.substr(0, funcPos);
                beforeFunc.erase(0, beforeFunc.find_first_not_of(" \t"));
                beforeFunc.erase(beforeFunc.find_last_not_of(" \t") + 1);
                
                if (!beforeFunc.empty()) {
                    returnType = parseType(beforeFunc);
                }
                
                funcName = extractFunctionName(line);
                
                // Parse parameters
                vector<string> params;
                vector<VarType> paramTypes;
                size_t j = i + 1;
                
                while (j < tacLines.size()) {
                    string nextLine = tacLines[j];
                    if (nextLine.find(" = param") != string::npos) {
                        size_t eqPos = nextLine.find(" = param");
                        string paramDecl = nextLine.substr(0, eqPos);
                        paramDecl.erase(0, paramDecl.find_first_not_of(" \t"));
                        paramDecl.erase(paramDecl.find_last_not_of(" \t") + 1);
                        
                        // Parse: type name = param
                        istringstream iss(paramDecl);
                        string typeStr, paramName;
                        iss >> typeStr >> paramName;
                        
                        VarType paramType = parseType(typeStr);
                        params.push_back(paramName);
                        paramTypes.push_back(paramType);
                        j++;
                    } else {
                        break;
                    }
                }
                
                // Create function signature
                vector<Type*> llvmParamTypes;
                for (auto type : paramTypes) {
                    llvmParamTypes.push_back(getLLVMType(type));
                }
                
                Type* llvmReturnType = getLLVMType(returnType);
                FunctionType* funcTy = FunctionType::get(llvmReturnType, llvmParamTypes, false);
                Function* func = Function::Create(funcTy, Function::ExternalLinkage, funcName, module.get());
                
                functions[funcName] = {func, params, paramTypes, returnType};
            }
        }
    }
    
    void secondPass() {
        currentLine = 0;
        
        while (currentLine < tacLines.size()) {
            string line = tacLines[currentLine];
            
            if (line.find("function ") != string::npos && line.find(" begin") != string::npos) {
                processFunction();
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
        
        namedValues.clear();
        tempValues.clear();
        labels.clear();
        
        currentLine++;
        
        // Process parameters
        size_t paramIdx = 0;
        for (auto& arg : currentFunc->args()) {
            if (paramIdx < funcInfo.paramNames.size()) {
                string paramName = funcInfo.paramNames[paramIdx];
                VarType paramType = funcInfo.paramTypes[paramIdx];
                
                if (currentLine < tacLines.size() && tacLines[currentLine].find(" = param") != string::npos) {
                    currentLine++;
                }
                
                Type* llvmType = getLLVMType(paramType);
                AllocaInst* alloca = builder.CreateAlloca(llvmType, nullptr, paramName);
                builder.CreateStore(&arg, alloca);
                namedValues[paramName] = {alloca, paramType};
                
                paramIdx++;
            }
        }
        
        // Pre-scan for labels
        size_t startLine = currentLine;
        while (currentLine < tacLines.size()) {
            string l = tacLines[currentLine];
            if (l.find("function ") != string::npos && l.find(" end") != string::npos) {
                break;
            }
            if (!l.empty() && l.back() == ':') {
                string labelName = l.substr(0, l.size() - 1);
                labels[labelName] = BasicBlock::Create(ctx, labelName, currentFunc);
            }
            currentLine++;
        }
        
        currentLine = startLine;
        
        // Process function body
        while (currentLine < tacLines.size()) {
            string l = tacLines[currentLine];
            
            if (l.find("function ") != string::npos && l.find(" end") != string::npos) {
                if (!currentBlock->getTerminator()) {
                    if (funcInfo.returnType == VarType::VOID) {
                        builder.CreateRetVoid();
                    } else if (funcInfo.returnType == VarType::FLOAT) {
                        builder.CreateRet(ConstantFP::get(floatTy, 0.0));
                    } else if (funcInfo.returnType == VarType::DOUBLE) {
                        builder.CreateRet(ConstantFP::get(doubleTy, 0.0));
                    } else {
                        builder.CreateRet(ConstantInt::get(int32Ty, 0));
                    }
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
        
        if (currentBlock->getTerminator()) {
            return;
        }
        
        // Handle labels
        if (!stmt.empty() && stmt.back() == ':') {
            string labelName = stmt.substr(0, stmt.size() - 1);
            BasicBlock* labelBlock = labels[labelName];
            
            if (!currentBlock->getTerminator()) {
                builder.CreateBr(labelBlock);
            }
            
            currentBlock = labelBlock;
            builder.SetInsertPoint(currentBlock);
            return;
        }
        
        if (stmt.find(" = ") != string::npos) {
            processAssignment(stmt);
        }
        else if (stmt.find("if ") == 0 && stmt.find(" goto ") != string::npos) {
            processConditionalGoto(stmt);
        }
        else if (stmt.find("goto ") == 0) {
            processGoto(stmt);
        }
        else if (stmt.find("return") == 0) {
            processReturn(stmt);
        }
        else if (stmt.find("call ") == 0) {
            processFunctionCall(stmt);
        }
    }
    
    VarType extractTypeFromDeclaration(const string& lhs, string& varName) {
        istringstream iss(lhs);
        string first, second;
        iss >> first >> second;
        
        if (second.empty()) {
            // No type declaration, just variable name
            varName = first;
            return VarType::UNKNOWN;
        }
        
        // Type declaration present
        varName = second;
        return parseType(first);
    }
    
    void processAssignment(const string& stmt) {
        size_t eqPos = stmt.find(" = ");
        string lhs = stmt.substr(0, eqPos);
        string rhs = stmt.substr(eqPos + 3);
        
        lhs.erase(0, lhs.find_first_not_of(" \t"));
        lhs.erase(lhs.find_last_not_of(" \t") + 1);
        rhs.erase(0, rhs.find_first_not_of(" \t"));
        rhs.erase(rhs.find_last_not_of(" \t") + 1);
        
        if (rhs == "param") {
            return;
        }
        
        // Extract variable name and type
        string varName;
        VarType declaredType = extractTypeFromDeclaration(lhs, varName);
        
        Value* result = nullptr;
        VarType resultType = VarType::INT;
        
        // Evaluate RHS
        if (rhs.find("call ") == 0) {
            auto [val, type] = processFunctionCallWithType(rhs);
            result = val;
            resultType = type;
        }
        else if (rhs.find(" + ") != string::npos || rhs.find(" - ") != string::npos ||
                 rhs.find(" * ") != string::npos || rhs.find(" / ") != string::npos ||
                 rhs.find(" % ") != string::npos || rhs.find(" == ") != string::npos ||
                 rhs.find(" != ") != string::npos || rhs.find(" < ") != string::npos ||
                 rhs.find(" > ") != string::npos || rhs.find(" <= ") != string::npos ||
                 rhs.find(" >= ") != string::npos || rhs.find(" && ") != string::npos ||
                 rhs.find(" || ") != string::npos || rhs.find(" & ") != string::npos ||
                 rhs.find(" | ") != string::npos || rhs.find(" ^ ") != string::npos ||
                 rhs.find(" << ") != string::npos || rhs.find(" >> ") != string::npos) {
            auto [val, type] = processBinaryOp(rhs);
            result = val;
            resultType = type;
        }
        else if ((rhs[0] == '-' || rhs[0] == '!') && rhs.size() > 1 && !isdigit(rhs[1])) {
            auto [val, type] = processUnaryOp(rhs);
            result = val;
            resultType = type;
        }
        else {
            auto [val, type] = getValueWithType(rhs);
            result = val;
            resultType = type;
        }
        
        if (!result) return;
        
        // Use declared type if available
        if (declaredType != VarType::UNKNOWN) {
            resultType = declaredType;
            result = convertToType(result, resultType);
        }
        
        // Store result
        if (varName[0] == 't' && declaredType == VarType::UNKNOWN) {
            tempValues[varName] = {result, resultType};
        } else {
            if (namedValues.find(varName) == namedValues.end()) {
                Type* llvmType = getLLVMType(resultType);
                AllocaInst* alloca = builder.CreateAlloca(llvmType, nullptr, varName);
                namedValues[varName] = {alloca, resultType};
            }
            builder.CreateStore(result, namedValues[varName].alloca);
        }
    }
    
    pair<Value*, VarType> processBinaryOp(const string& expr) {
        vector<pair<string, string>> ops = {
            {" << ", "shl"}, {" >> ", "shr"},
            {" == ", "eq"}, {" != ", "ne"}, {" <= ", "le"}, {" >= ", "ge"},
            {" < ", "lt"}, {" > ", "gt"}, 
            {" && ", "logand"}, {" || ", "logor"},
            {" & ", "bitand"}, {" | ", "bitor"}, {" ^ ", "bitxor"},
            {" + ", "add"}, {" - ", "sub"}, {" * ", "mul"}, {" / ", "div"}, {" % ", "mod"}
        };
        
        for (const auto& op : ops) {
            size_t pos = expr.find(op.first);
            if (pos != string::npos) {
                string left = expr.substr(0, pos);
                string right = expr.substr(pos + op.first.size());
                
                left.erase(0, left.find_first_not_of(" \t"));
                left.erase(left.find_last_not_of(" \t") + 1);
                right.erase(0, right.find_first_not_of(" \t"));
                right.erase(right.find_last_not_of(" \t") + 1);
                
                auto [lVal, lType] = getValueWithType(left);
                auto [rVal, rType] = getValueWithType(right);
                
                // Determine result type based on operand types
                VarType resultType = VarType::INT;
                Type* lLLVMType = lVal->getType();
                Type* rLLVMType = rVal->getType();
                
                // Check actual LLVM types for float detection
                if (lLLVMType->isDoubleTy() || rLLVMType->isDoubleTy()) {
                    resultType = VarType::DOUBLE;
                } else if (lLLVMType->isFloatTy() || rLLVMType->isFloatTy()) {
                    resultType = VarType::FLOAT;
                }
                
                // Bitwise/shift/mod ops require INT - force conversion
                if (op.second == "bitand" || op.second == "bitor" || op.second == "bitxor" ||
                    op.second == "shl" || op.second == "shr" || op.second == "mod") {
                    lVal = convertToType(lVal, VarType::INT);
                    rVal = convertToType(rVal, VarType::INT);
                    resultType = VarType::INT;
                } else if (resultType != VarType::INT) {
                    // Only convert for float operations if types don't match
                    if (lLLVMType != rLLVMType) {
                        Type* targetType = getLLVMType(resultType);
                        if (lLLVMType != targetType) {
                            lVal = convertToType(lVal, resultType);
                        }
                        if (rLLVMType != targetType) {
                            rVal = convertToType(rVal, resultType);
                        }
                    }
                }
                
                Value* result = nullptr;
                bool isFloat = (resultType == VarType::FLOAT || resultType == VarType::DOUBLE);
                
                if (op.second == "add") result = isFloat ? builder.CreateFAdd(lVal, rVal) : builder.CreateAdd(lVal, rVal);
                else if (op.second == "sub") result = isFloat ? builder.CreateFSub(lVal, rVal) : builder.CreateSub(lVal, rVal);
                else if (op.second == "mul") result = isFloat ? builder.CreateFMul(lVal, rVal) : builder.CreateMul(lVal, rVal);
                else if (op.second == "div") result = isFloat ? builder.CreateFDiv(lVal, rVal) : builder.CreateSDiv(lVal, rVal);
                else if (op.second == "mod") result = builder.CreateSRem(lVal, rVal);
                else if (op.second == "eq") {
                    result = isFloat ? builder.CreateFCmpOEQ(lVal, rVal) : builder.CreateICmpEQ(lVal, rVal);
                    resultType = VarType::INT;
                }
                else if (op.second == "ne") {
                    result = isFloat ? builder.CreateFCmpONE(lVal, rVal) : builder.CreateICmpNE(lVal, rVal);
                    resultType = VarType::INT;
                }
                else if (op.second == "lt") {
                    result = isFloat ? builder.CreateFCmpOLT(lVal, rVal) : builder.CreateICmpSLT(lVal, rVal);
                    resultType = VarType::INT;
                }
                else if (op.second == "gt") {
                    result = isFloat ? builder.CreateFCmpOGT(lVal, rVal) : builder.CreateICmpSGT(lVal, rVal);
                    resultType = VarType::INT;
                }
                else if (op.second == "le") {
                    result = isFloat ? builder.CreateFCmpOLE(lVal, rVal) : builder.CreateICmpSLE(lVal, rVal);
                    resultType = VarType::INT;
                }
                else if (op.second == "ge") {
                    result = isFloat ? builder.CreateFCmpOGE(lVal, rVal) : builder.CreateICmpSGE(lVal, rVal);
                    resultType = VarType::INT;
                }
                else if (op.second == "logand" || op.second == "bitand") result = builder.CreateAnd(lVal, rVal);
                else if (op.second == "logor" || op.second == "bitor") result = builder.CreateOr(lVal, rVal);
                else if (op.second == "bitxor") result = builder.CreateXor(lVal, rVal);
                else if (op.second == "shl") result = builder.CreateShl(lVal, rVal);
                else if (op.second == "shr") result = builder.CreateAShr(lVal, rVal);
                
                if (result && result->getType()->isIntegerTy(1)) {
                    result = builder.CreateZExt(result, int32Ty);
                }
                
                return {result, resultType};
            }
        }
        
        return {ConstantInt::get(int32Ty, 0), VarType::INT};
    }
    
    pair<Value*, VarType> processUnaryOp(const string& expr) {
        char op = expr[0];
        string operand = expr.substr(1);
        operand.erase(0, operand.find_first_not_of(" \t"));
        
        auto [val, type] = getValueWithType(operand);
        
        if (op == '-') {
            Value* result = (type == VarType::FLOAT || type == VarType::DOUBLE) ?
                builder.CreateFNeg(val) : builder.CreateNeg(val);
            return {result, type};
        } else if (op == '!') {
            Value* result = builder.CreateNot(val);
            if (result->getType()->isIntegerTy(1)) {
                result = builder.CreateZExt(result, int32Ty);
            }
            return {result, VarType::INT};
        }
        
        return {val, type};
    }
    
    Value* convertToType(Value* val, VarType toType) {
        Type* valType = val->getType();
        
        // Already correct type
        Type* targetType = getLLVMType(toType);
        if (valType == targetType) return val;
        
        // Handle i1 (boolean)
        if (valType->isIntegerTy(1)) {
            val = builder.CreateZExt(val, int32Ty);
            valType = int32Ty;
        }
        
        // INT -> FLOAT/DOUBLE
        if (valType->isIntegerTy()) {
            if (toType == VarType::FLOAT) return builder.CreateSIToFP(val, floatTy);
            if (toType == VarType::DOUBLE) return builder.CreateSIToFP(val, doubleTy);
        }
        
        // FLOAT <-> DOUBLE
        if (valType->isFloatTy() && toType == VarType::DOUBLE) {
            return builder.CreateFPExt(val, doubleTy);
        }
        if (valType->isDoubleTy() && toType == VarType::FLOAT) {
            return builder.CreateFPTrunc(val, floatTy);
        }
        
        // FLOAT/DOUBLE -> INT
        if (valType->isFloatingPointTy() && toType == VarType::INT) {
            return builder.CreateFPToSI(val, int32Ty);
        }
        
        return val;
    }
    
    pair<Value*, VarType> getValueWithType(const string& name) {
        // String literal
        if (name.size() >= 2 && name[0] == '"' && name.back() == '"') {
            string str = name.substr(1, name.size() - 2);
            return {builder.CreateGlobalString(str), VarType::STRING};
        }
        
        // Char literal
        if (name.size() >= 2 && name[0] == '\'' && name.back() == '\'') {
            char ch = (name.size() == 3) ? name[1] : '\0';
            return {ConstantInt::get(int8Ty, ch), VarType::CHAR};
        }
        
        // Boolean literals
        if (name == "true") return {ConstantInt::get(int32Ty, 1), VarType::BOOL};
        if (name == "false") return {ConstantInt::get(int32Ty, 0), VarType::BOOL};
        
        // Temporary variable
        if (tempValues.find(name) != tempValues.end()) {
            return tempValues[name];
        }
        
        // Named variable
        if (namedValues.find(name) != namedValues.end()) {
            VariableInfo& varInfo = namedValues[name];
            Type* llvmType = getLLVMType(varInfo.type);
            Value* loaded = builder.CreateLoad(llvmType, varInfo.alloca, name);
            return {loaded, varInfo.type};
        }
        
        // Numeric literal
        if (name.find('.') != string::npos || name.find('e') != string::npos || name.find('E') != string::npos) {
            if (name.back() == 'f' || name.back() == 'F') {
                string numStr = name.substr(0, name.size() - 1);
                return {ConstantFP::get(floatTy, stof(numStr)), VarType::FLOAT};
            }
            return {ConstantFP::get(doubleTy, stod(name)), VarType::DOUBLE};
        }
        
        if ((name[0] == '-' || isdigit(name[0])) && name.find_first_not_of("-0123456789") == string::npos) {
            return {ConstantInt::get(int32Ty, stoi(name)), VarType::INT};
        }
        
        return {ConstantInt::get(int32Ty, 0), VarType::INT};
    }
    
    void processConditionalGoto(const string& stmt) {
        size_t ifPos = stmt.find("if ");
        size_t gotoPos = stmt.find(" goto ");
        
        string condition = stmt.substr(ifPos + 3, gotoPos - ifPos - 3);
        string labelName = stmt.substr(gotoPos + 6);
        
        condition.erase(0, condition.find_first_not_of(" \t"));
        condition.erase(condition.find_last_not_of(" \t") + 1);
        labelName.erase(0, labelName.find_first_not_of(" \t"));
        labelName.erase(labelName.find_last_not_of(" \t") + 1);
        
        Value* condValue = nullptr;
        
        if (condition.find(" == ") != string::npos) {
            size_t eqPos = condition.find(" == ");
            string leftOp = condition.substr(0, eqPos);
            string rightOp = condition.substr(eqPos + 4);
            
            leftOp.erase(0, leftOp.find_first_not_of(" \t"));
            leftOp.erase(leftOp.find_last_not_of(" \t") + 1);
            rightOp.erase(0, rightOp.find_first_not_of(" \t"));
            rightOp.erase(rightOp.find_last_not_of(" \t") + 1);
            
            auto [lVal, lType] = getValueWithType(leftOp);
            auto [rVal, rType] = getValueWithType(rightOp);
            
            // Check if comparing chars
            if (lVal->getType()->isIntegerTy(8) || rVal->getType()->isIntegerTy(8)) {
                // Extend char to i32 for comparison
                if (lVal->getType()->isIntegerTy(8)) {
                    lVal = builder.CreateZExt(lVal, int32Ty);
                }
                if (rVal->getType()->isIntegerTy(8)) {
                    rVal = builder.CreateZExt(rVal, int32Ty);
                }
                condValue = builder.CreateICmpEQ(lVal, rVal);
            } else {
                // Regular comparison
                lVal = convertToType(lVal, VarType::INT);
                rVal = convertToType(rVal, VarType::INT);
                condValue = builder.CreateICmpEQ(lVal, rVal);
            }
        } else if (condition.find(" != 0") != string::npos) {
            string var = condition.substr(0, condition.find(" != 0"));
            var.erase(0, var.find_first_not_of(" \t"));
            var.erase(var.find_last_not_of(" \t") + 1);
            auto [val, type] = getValueWithType(var);
            val = convertToType(val, VarType::INT);
            condValue = builder.CreateICmpNE(val, ConstantInt::get(int32Ty, 0));
        } else if (condition.find(" == 0") != string::npos) {
            string var = condition.substr(0, condition.find(" == 0"));
            var.erase(0, var.find_first_not_of(" \t"));
            var.erase(var.find_last_not_of(" \t") + 1);
            auto [val, type] = getValueWithType(var);
            val = convertToType(val, VarType::INT);
            condValue = builder.CreateICmpEQ(val, ConstantInt::get(int32Ty, 0));
        }
        
        if (labels.find(labelName) != labels.end()) {
            BasicBlock* thenBlock = labels[labelName];
            BasicBlock* elseBlock = BasicBlock::Create(ctx, "cont", currentFunc);
            
            builder.CreateCondBr(condValue, thenBlock, elseBlock);
            
            currentBlock = elseBlock;
            builder.SetInsertPoint(currentBlock);
        }
    }
    
    void processGoto(const string& stmt) {
        string labelName = stmt.substr(5);
        labelName.erase(0, labelName.find_first_not_of(" \t"));
        labelName.erase(labelName.find_last_not_of(" \t") + 1);
        
        if (labels.find(labelName) != labels.end()) {
            builder.CreateBr(labels[labelName]);
            
            // Create unreachable block for any code after goto
            currentBlock = BasicBlock::Create(ctx, "aftergoto", currentFunc);
            builder.SetInsertPoint(currentBlock);
        }
    }
    
    void processReturn(const string& stmt) {
        FunctionInfo& funcInfo = functions[currentFunc->getName().str()];
        
        if (stmt == "return") {
            if (funcInfo.returnType == VarType::VOID) {
                builder.CreateRetVoid();
            } else if (funcInfo.returnType == VarType::FLOAT) {
                builder.CreateRet(ConstantFP::get(floatTy, 0.0));
            } else if (funcInfo.returnType == VarType::DOUBLE) {
                builder.CreateRet(ConstantFP::get(doubleTy, 0.0));
            } else {
                builder.CreateRet(ConstantInt::get(int32Ty, 0));
            }
        } else {
            string retVal = stmt.substr(7);
            retVal.erase(0, retVal.find_first_not_of(" \t"));
            retVal.erase(retVal.find_last_not_of(" \t") + 1);
            
            auto [val, type] = getValueWithType(retVal);
            val = convertToType(val, funcInfo.returnType);
            builder.CreateRet(val);
        }
    }
    
    pair<Value*, VarType> processFunctionCallWithType(const string& stmt) {
        size_t callPos = stmt.find("call ");
        string rest = stmt.substr(callPos + 5);
        
        size_t parenPos = rest.find('(');
        string funcName = rest.substr(0, parenPos);
        
        if (funcName == "print") {
            processFunctionCall(stmt);
            return {ConstantInt::get(int32Ty, 0), VarType::INT};
        }
        
        if (functions.find(funcName) != functions.end()) {
            Value* result = processFunctionCall(stmt);
            VarType returnType = functions[funcName].returnType;
            return {result, returnType};
        }
        
        return {ConstantInt::get(int32Ty, 0), VarType::INT};
    }
    
    Value* processFunctionCall(const string& stmt) {
        size_t callPos = stmt.find("call ");
        string rest = stmt.substr(callPos + 5);
        
        size_t parenPos = rest.find('(');
        string funcName = rest.substr(0, parenPos);
        
        size_t endParen = rest.find(')');
        string argsStr = rest.substr(parenPos + 1, endParen - parenPos - 1);
        
        // Handle print specially
        if (funcName == "print") {
            argsStr.erase(0, argsStr.find_first_not_of(" \t"));
            argsStr.erase(argsStr.find_last_not_of(" \t") + 1);
            
            if (argsStr[0] == '"') {
                // String literal
                string str = argsStr.substr(1, argsStr.size() - 2);
                Value* strVal = builder.CreateGlobalString(str);
                builder.CreateCall(putsFunc, {strVal});
            } else if (argsStr[0] == '\'' && argsStr.size() >= 3) {
                // Char literal
                char ch = argsStr[1];
                builder.CreateCall(putcharFunc, {ConstantInt::get(int32Ty, ch)});
                builder.CreateCall(putcharFunc, {ConstantInt::get(int32Ty, '\n')});
            } else {
                // Variable or expression
                auto [val, type] = getValueWithType(argsStr);
                Type* valType = val->getType();
                
                // Check if it's a string variable (pointer type and VarType::STRING)
                if (type == VarType::STRING && valType->isPointerTy()) {
                    // String variable - use puts
                    builder.CreateCall(putsFunc, {val});
                } else if (valType->isFloatingPointTy()) {
                    // Float or double
                    Value* fmtStr = builder.CreateGlobalString("%f\n");
                    if (valType->isFloatTy()) {
                        val = builder.CreateFPExt(val, doubleTy);
                    }
                    builder.CreateCall(printfFunc, {fmtStr, val});
                } else if (valType->isIntegerTy(8)) {
                    // Char
                    Value* charVal = builder.CreateZExt(val, int32Ty);
                    builder.CreateCall(putcharFunc, {charVal});
                    builder.CreateCall(putcharFunc, {ConstantInt::get(int32Ty, '\n')});
                } else {
                    // Integer
                    if (valType->isIntegerTy(1)) {
                        val = builder.CreateZExt(val, int32Ty);
                    }
                    Value* fmtStr = builder.CreateGlobalString("%d\n");
                    builder.CreateCall(printfFunc, {fmtStr, val});
                }
            }
            return nullptr;
        }
        
        // Regular function call
        if (functions.find(funcName) != functions.end()) {
            vector<Value*> args;
            
            if (!argsStr.empty()) {
                stringstream ss(argsStr);
                string arg;
                size_t paramIdx = 0;
                
                while (getline(ss, arg, ',')) {
                    arg.erase(0, arg.find_first_not_of(" \t"));
                    arg.erase(arg.find_last_not_of(" \t") + 1);
                    
                    auto [val, type] = getValueWithType(arg);
                    
                    // Convert to expected parameter type
                    if (paramIdx < functions[funcName].paramTypes.size()) {
                        VarType expectedType = functions[funcName].paramTypes[paramIdx];
                        val = convertToType(val, expectedType);
                    }
                    
                    args.push_back(val);
                    paramIdx++;
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
        
        if (functions.find("main") != functions.end()) {
            Function* mainFunc = functions["main"].func;
            ee->runFunction(mainFunc, {});
        } 
        else {
            errs() << "Error: No main function found\n";
        }
        
        outs() << "=======================================\n";
        
        delete ee;
    }
};

void executeTACProgram() {
    TACExecutor executor;
    const string tacFile = "tester/tac.txt";
    if (!executor.readTACFile(tacFile)) {
        std::cerr << "Error: Failed to read TAC file: " << tacFile << std::endl;
        exit(1);
    }
    
    cout << "Reading TAC from: " << tacFile << std::endl;
    
    executor.execute();
    executor.printIR();
    executor.run();
}