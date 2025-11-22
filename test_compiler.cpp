#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>

using namespace llvm;

int main() {
    std::cout << "=== Custom Compiler LLVM Test ===\n";
    
    // Create core LLVM structures
    LLVMContext Context;
    Module* MyModule = new Module("my_tac_module", Context);
    IRBuilder<> Builder(Context);

    // Create main function
    FunctionType* MainType = FunctionType::get(Builder.getInt32Ty(), false);
    Function* MainFunc = Function::Create(MainType, Function::ExternalLinkage, "main", MyModule);

    // Create entry block
    BasicBlock* EntryBlock = BasicBlock::Create(Context, "entry", MainFunc);
    Builder.SetInsertPoint(EntryBlock);

    // Create some simple TAC-like operations
    Value* Counter = Builder.getInt32(25);  // Like your num2 = 25
    Value* Three = Builder.getInt32(3);
    
    // Create comparison: t0 = counter < 3
    Value* Comparison = Builder.CreateICmpSLT(Counter, Three, "t0");
    
    // Create return value
    Builder.CreateRet(Builder.getInt32(42));

    // Print the generated LLVM IR
    std::cout << "\nGenerated LLVM IR:\n";
    MyModule->print(outs(), nullptr);
    
    std::cout << "\n✅ LLVM integration working!\n";
    return 0;
}