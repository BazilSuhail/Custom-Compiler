#include "compiler.h"

int main(int argc, char* argv[]) {
    // Determine input file - // user-provided or default
    string fileName = (argc > 1) ? argv[1] : "sample"; 
    string inputFile = "tester/" + fileName + ".txt";
    string tokenFile = "tester/tokens.txt";

    // Run lexer and save tokens
    vector<Token> tokens = lexAndDumpToFile(inputFile, tokenFile);

    // Parse tokens into AST
    auto ast = parseFromFile(tokens); 
    performScopeAnalysis(ast, tokens);
 
    return 0;
}

