#include "compiler.h"
#include <string>

int main(int argc, char* argv[]) {
    // Determine input file
    string fileName = (argc > 1) ? argv[1] : "sample"; // user-provided or default
    string inputFile = "tester/" + fileName + ".txt";               // prepend tester/
    string tokenFile = "tester/tokens.txt";

    // Run lexer and save tokens
    vector<Token> tokens = lexAndDumpToFile(inputFile, tokenFile);

    // Parse tokens into AST
    auto ast = parseFromFile(tokens);
    //performScopeAnalysis(ast, tokens);
// Get both errors and symbol table
    auto [errors, symbolTable] = performScopeAnalysis(ast, tokens);

    // Handle errors if any
    if (!errors.empty()) {
        cout << "\n=== Scope Analysis Errors ===\n";
        for (const auto& error : errors) {
            cout << "[Scope Error] " << error.message << ")\n";
        }
        cout << "Scope analysis failed with " << errors.size() << " error(s)\n";
        exit(EXIT_FAILURE);
    }

    cout << "\n=== Scope Analysis Successful ===\n";
    cout << "No scope errors found.\n";
    
    // Now you can use the symbol table (symbolTable) for further processing
    // Example: print the global scope symbols
    cout << "\n=== Global Scope Symbols ===\n";
    for (const auto& [name, sym] : symbolTable->symbols) {
        cout << "Symbol: " << name << ", Type: " << getTokenTypeName(sym.type) 
             << ", IsFunction: " << sym.isFunction << ", Level: " << sym.scopeLevel << "\n";
    }



    return 0;
}

