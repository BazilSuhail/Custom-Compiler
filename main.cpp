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

    return 0;
}


// #include "compiler.h"
// using namespace std;

// int main() {
//     vector<Token> tokens = lexAndDumpToFile("tester/sample.txt", "tester/tokens.txt");
//     auto ast = parseFromFile(tokens);
//     return 0;
// }
