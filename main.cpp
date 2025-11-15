#include "compiler.h"
#include <string>

int main(int argc, char* argv[]) {
    string fileName = (argc > 1) ? argv[1] : "sample"; // user-provided or default
    string inputFile = "tester/" + fileName + ".txt";
    string tokenFile = "tester/tokens.txt";

    // Run lexer and save tokens
    vector<Token> tokens = lexAndDumpToFile(inputFile, tokenFile);

    // Parse tokens into AST
    auto ast = parseFromFile(tokens); 
    // Scope Analysis
    performScopeAnalysis(ast, tokens);
    // Type Checking
    performTypeChecking(ast, tokens);
    
    return 0;
}
