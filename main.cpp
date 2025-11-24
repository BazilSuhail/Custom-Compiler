#include "compiler.h"
#include <chrono>

int main(int argc, char* argv[]) {
    auto start = chrono::high_resolution_clock::now();

    string fileName = (argc > 1) ? argv[1] : "sample";
    string inputFile = "tester/" + fileName + ".txt";
    string tokenFile = "tester/tokens.txt";

    vector<Token> tokens = lexAndDumpToFile(inputFile, tokenFile);

    auto ast = parseFromFile(tokens);

    performScopeAnalysis(ast, tokens);

    performTypeChecking(ast, tokens);

    generateTAC(ast, "tester/tac.txt");

    auto end = chrono::high_resolution_clock::now();

    long long durationMs = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "\nTotal time: " << durationMs << " ms\n";

    return 0;
}

// #include "compiler.h"
// #include <string>

// int main(int argc, char* argv[]) {
//     string fileName = (argc > 1) ? argv[1] : "sample"; // user-provided or default
//     string inputFile = "tester/" + fileName + ".txt";
//     string tokenFile = "tester/tokens.txt";

//     // Run lexer and save tokens
//     vector<Token> tokens = lexAndDumpToFile(inputFile, tokenFile);

//     // Parse tokens into AST
//     auto ast = parseFromFile(tokens); 
//     // Scope Analysis
//     performScopeAnalysis(ast, tokens);
//     // Type Checking
//     performTypeChecking(ast, tokens);
//     generateTAC(ast,"tester/tac.txt");
//     return 0;
// }
