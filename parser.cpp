#include"compiler.h"

// === Parse Errors ===
enum class ParseErrorType {
    UnexpectedEOF,
    FailedToFindToken,
    ExpectedTypeToken,
    ExpectedIdentifier,
    UnexpectedToken,
    ExpectedFloatLit,
    ExpectedIntLit,
    ExpectedStringLit,
    ExpectedBoolLit,
    ExpectedExpr,
    InvalidCallTarget,
    MissingSemicolon,
    UnclosedBlock
};

struct ParseError {
    ParseErrorType type;
    Token token;
    string message;

    ParseError(ParseErrorType t, const Token& tok) : type(t), token(tok) {
        switch (t) {
            case ParseErrorType::UnexpectedEOF:
                message = "Unexpected end of file";
                break;
            case ParseErrorType::FailedToFindToken:
                message = "Failed to find expected token: " + getTokenName(tok.type);
                break;
            case ParseErrorType::ExpectedTypeToken:
                message = "Expected type token (int, float, etc.)";
                break;
            case ParseErrorType::ExpectedIdentifier:
                message = "Expected identifier";
                break;
            case ParseErrorType::UnexpectedToken:
                message = "Unexpected token: " + getTokenName(tok.type) + " (" + tok.value + ")";
                break;
            case ParseErrorType::ExpectedFloatLit:
                message = "Expected float literal";
                break;
            case ParseErrorType::ExpectedIntLit:
                message = "Expected int literal";
                break;
            case ParseErrorType::ExpectedStringLit:
                message = "Expected string literal";
                break;
            case ParseErrorType::ExpectedBoolLit:
                message = "Expected bool literal";
                break;
            case ParseErrorType::InvalidCallTarget:
                message = "Invalid function call target: only identifiers can be called";
                break;
            case ParseErrorType::MissingSemicolon:
                message = "Missing semicolon at end of statement";
                break;
            case ParseErrorType::UnclosedBlock:
                message = "Unclosed block, expected '}' before end of file";
                break;
            case ParseErrorType::ExpectedExpr:
                message = "Expected expression";
                break;
        }
    }

    string getTokenName(TokenType t) const {
        switch (t) {
            case T_INT: return "T_INT";
            case T_FLOAT: return "T_FLOAT";
            case T_DOUBLE: return "T_DOUBLE";
            case T_CHAR: return "T_CHAR";
            case T_VOID: return "T_VOID";
            case T_BOOL: return "T_BOOL";
            case T_STRING: return "T_STRING";
            case T_IDENTIFIER: return "T_IDENTIFIER";
            case T_INTLIT: return "T_INTLIT";
            case T_FLOATLIT: return "T_FLOATLIT";
            case T_STRINGLIT: return "T_STRINGLIT";
            case T_CHARLIT: return "T_CHARLIT";
            case T_BOOLLIT: return "T_BOOLLIT";
            case T_LPAREN: return "T_LPAREN";
            case T_RPAREN: return "T_RPAREN";
            case T_LBRACE: return "T_LBRACE";
            case T_RBRACE: return "T_RBRACE";
            case T_SEMICOLON: return "T_SEMICOLON";
            case T_COMMA: return "T_COMMA";
            case T_DOT: return "T_DOT";
            case T_ASSIGNOP: return "T_ASSIGNOP";
            case T_PLUS: return "T_PLUS";
            case T_MINUS: return "T_MINUS";
            case T_MULTIPLY: return "T_MULTIPLY";
            case T_DIVIDE: return "T_DIVIDE";
            case T_IF: return "T_IF";
            case T_ELSE: return "T_ELSE";
            case T_WHILE: return "T_WHILE";
            case T_RETURN: return "T_RETURN";
            case T_PRINT: return "T_PRINT";
            // new tokens
            case T_DO: return "T_DO";
            case T_FOR: return "T_FOR";
            case T_SWITCH: return "T_SWITCH";
            case T_CASE: return "T_CASE";
            case T_DEFAULT: return "T_DEFAULT";
            case T_BREAK: return "T_BREAK";
            case T_COLON: return "T_COLON";
            case T_INCLUDE: return "T_INCLUDE";
            case T_ENUM: return "T_ENUM";
            // Bitwise operators - Add these lines
            case T_BITAND: return "T_BITAND";
            case T_BITOR: return "T_BITOR";
            case T_BITXOR: return "T_BITXOR";
            case T_BITLSHIFT: return "T_BITLSHIFT";
            case T_BITRSHIFT: return "T_BITRSHIFT";
            case T_MAIN: return "T_MAIN";
            case T_EOF: return "T_EOF";
            default: return "T_UNKNOWN";
        }
    }
};

// Color codes
const string RESET = "\033[0m";

// Forward declarations for print functions
void printASTNode(const ASTNodeVariant& node, int indent = 0);

void printIntLiteral(const IntLiteral& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[95m" << "IntLiteral(" << node.value << ")" << RESET << "\n";
}

void printFloatLiteral(const FloatLiteral& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[95m" << "FloatLiteral(" << node.value << ")" << RESET << "\n";
}

void printStringLiteral(const StringLiteral& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[33m" << "StringLiteral(\"" << node.value << "\")" << RESET << "\n";
}

void printCharLiteral(const CharLiteral& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[33m" << "CharLiteral('" << node.value << "')" << RESET << "\n";
}

void printBoolLiteral(const BoolLiteral& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[35m" << "BoolLiteral(" << (node.value ? "true" : "false") << ")" << RESET << "\n";
}

void printIdentifier(const Identifier& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[32m" << "Identifier(\"" << node.name << "\")" << RESET << "\n";
}

void printBinaryExpr(const BinaryExpr& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[93m" << "BinaryExpr(";
    node.printOp(node.op);
    cout << ")" << RESET << "\n";
    if (node.left) printASTNode(node.left->node, indent + 2);
    if (node.right) printASTNode(node.right->node, indent + 2);
}

void printUnaryExpr(const UnaryExpr& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[31m" << "UnaryExpr(";
    if (node.op == T_MINUS) cout << "-";
    else if (node.op == T_NOT) cout << "!";
    else cout << "unary_op";
    cout << ")" << RESET << "\n";
    if (node.operand) printASTNode(node.operand->node, indent + 2);
}

void printIncludeStmt(const IncludeStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[34m" << "IncludeStmt(\"" << node.header << "\")" << RESET << "\n";
}

// enums
// Add print function for EnumValueList
void printEnumValueList(const EnumValueList& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[37m" << "EnumValueList(";
    for (size_t i = 0; i < node.values.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << node.values[i];
    }
    cout << ")" << RESET << endl;
}

// Add print function for EnumDecl
void printEnumDecl(const EnumDecl& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[37m" << "EnumDecl(\"" << node.name << "\")" << RESET << endl;
    if (node.values) {
        cout << string(indent + 2, ' ') << "Values:" << endl;
        printASTNode(node.values->node, indent + 4);
    }
}

void printCallExpr(const CallExpr& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[34m" << "CallExpr" << RESET << "\n";
    cout << string(indent + 2, ' ') << "Callee:\n";
    if (node.callee) printASTNode(node.callee->node, indent + 4);
    if (!node.args.empty()) {
        cout << string(indent + 2, ' ') << "Args:\n";
        for (const auto& arg : node.args) {
            if (arg) printASTNode(arg->node, indent + 4);
        }
    }
}

void printVarDecl(const VarDecl& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[36m" << "VarDecl(";
    node.printType(node.type);
    cout << ", \"" << node.name << "\")" << RESET << "\n";
    if (node.initializer) printASTNode(node.initializer->node, indent + 2);
}

void printBlockStmt(const BlockStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[37m" << "BlockStmt" << RESET << "\n";
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 2);
    }
}

void printFunctionDecl(const FunctionDecl& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[33m" << "FunctionDecl(";
    node.printType(node.returnType);
    cout << ", \"" << node.name << "\")" << RESET << "\n";
    for (const auto& param : node.params) {
        cout << string(indent + 2, ' ') << "Param: ";
        switch (param.first) {
            case T_INT: cout << "int"; break;
            case T_FLOAT: cout << "float"; break;
            case T_DOUBLE: cout << "double"; break;
            case T_CHAR: cout << "char"; break;
            case T_BOOL: cout << "bool"; break;
            case T_VOID: cout << "void"; break;
            default: cout << "unknown"; break;
        }
        cout << " " << param.second << "\n";
    }
    cout << string(indent + 2, ' ') << "Body:\n";
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 4);
    }
}

// Add print function for FunctionProto
void printFunctionProto(const FunctionProto& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[33m" << "FunctionProto(";
    node.printType(node.returnType);
    cout << ", \"" << node.name << "\")" << RESET << "\n";
    for (const auto& param : node.params) {
        cout << string(indent + 2, ' ') << "Param: ";
        switch (param.first) {
            case T_INT: cout << "int"; break;
            case T_FLOAT: cout << "float"; break;
            case T_DOUBLE: cout << "double"; break;
            case T_CHAR: cout << "char"; break;
            case T_BOOL: cout << "bool"; break;
            case T_VOID: cout << "void"; break;
            default: cout << "unknown"; break;
        }
        cout << " " << param.second << "\n";
    }
}

void printMainDecl(const MainDecl& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[33m" << "MainDecl" << RESET << "\n";
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 2);
    }
}

void printIfStmt(const IfStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[35m" << "IfStmt" << RESET << "\n";
    cout << string(indent + 2, ' ') << "Condition:\n";
    if (node.condition) printASTNode(node.condition->node, indent + 4);
    cout << string(indent + 2, ' ') << "IfBody:\n";
    for (const auto& stmt : node.ifBody) {
        if (stmt) printASTNode(stmt->node, indent + 4);
    }
    if (!node.elseBody.empty()) {
        cout << string(indent + 2, ' ') << "ElseBody:\n";
        for (const auto& stmt : node.elseBody) {
            if (stmt) printASTNode(stmt->node, indent + 4);
        }
    }
}

void printWhileStmt(const WhileStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[31m" << "WhileStmt" << RESET << "\n";
    cout << string(indent + 2, ' ') << "Condition:\n";
    if (node.condition) printASTNode(node.condition->node, indent + 4);
    cout << string(indent + 2, ' ') << "Body:\n";
    for (const auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent + 4);
    }
}

void printDoWhileStmt(const DoWhileStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[31m" << "DoWhileStmt" << RESET << "\n";
    cout << string(indent+2, ' ') << "Body:\n";
    if (node.body) printASTNode(node.body->node, indent+4);
    cout << string(indent+2, ' ') << "Condition:\n";
    if (node.condition) printASTNode(node.condition->node, indent+4);
}

void printForStmt(const ForStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[31m" << "ForStmt" << RESET << "\n";
    if (node.init) { cout << string(indent+2, ' ') << "Init:\n"; printASTNode(node.init->node, indent+4); }
    if (node.condition) { cout << string(indent+2, ' ') << "Condition:\n"; printASTNode(node.condition->node, indent+4); }
    if (node.update) { cout << string(indent+2, ' ') << "Update:\n"; printASTNode(node.update->node, indent+4); }
    cout << string(indent+2, ' ') << "Body:\n";
    if (node.body) printASTNode(node.body->node, indent+4);
}

void printCaseBlock(const CaseBlock& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[37m" << "CaseBlock" << RESET << "\n";
    cout << string(indent+2, ' ') << "Value:\n";
    if (node.value) printASTNode(node.value->node, indent+4);
    cout << string(indent+2, ' ') << "Body:\n";
    for (auto& stmt : node.body) {
        if (stmt) printASTNode(stmt->node, indent+4);
    }
}

void printSwitchStmt(const SwitchStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[37m" << "SwitchStmt" << RESET << "\n";
    cout << string(indent+2, ' ') << "Expression:\n";
    if (node.expression) printASTNode(node.expression->node, indent+4);
    cout << string(indent+2, ' ') << "Cases:\n";
    for (auto& c : node.cases) {
        if (c) printASTNode(c->node, indent+4);
    }
    if (!node.defaultBody.empty()) {
        cout << string(indent+2, ' ') << "Default:\n";
        for (auto& stmt : node.defaultBody) {
            if (stmt) printASTNode(stmt->node, indent+4);
        }
    }
}

void printReturnStmt(const ReturnStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[34m" << "ReturnStmt" << RESET << "\n";
    if (node.value) printASTNode(node.value->node, indent + 2);
}

void printPrintStmt(const PrintStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[32m" << "PrintStmt" << RESET << "\n";
    for (const auto& arg : node.args) {
        if (arg) printASTNode(arg->node, indent + 2);
    }
}
void printBreakStmt(const BreakStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[31m" << "BreakStmt" << RESET << "\n";
}

void printExpressionStmt(const ExpressionStmt& node, int indent) {
    cout << string(indent, ' ') << "\033[1m\033[37m" << "ExpressionStmt" << RESET << "\n";
    if (node.expr) printASTNode(node.expr->node, indent + 2);
}

void printASTNode(const ASTNodeVariant& node, int indent) {
    visit([indent](const auto& n) { 
        using T = decay_t<decltype(n)>;
        if constexpr (is_same_v<T, IntLiteral>) printIntLiteral(n, indent);
        else if constexpr (is_same_v<T, FloatLiteral>) printFloatLiteral(n, indent);
        else if constexpr (is_same_v<T, StringLiteral>) printStringLiteral(n, indent);
        else if constexpr (is_same_v<T, CharLiteral>) printCharLiteral(n, indent);
        else if constexpr (is_same_v<T, BoolLiteral>) printBoolLiteral(n, indent);
        else if constexpr (is_same_v<T, Identifier>) printIdentifier(n, indent);
        else if constexpr (is_same_v<T, BinaryExpr>) printBinaryExpr(n, indent);
        else if constexpr (is_same_v<T, UnaryExpr>) printUnaryExpr(n, indent);
        else if constexpr (is_same_v<T, IncludeStmt>) printIncludeStmt(n, indent);
        else if constexpr (is_same_v<T, EnumValueList>) printEnumValueList(n, indent); 
        else if constexpr (is_same_v<T, EnumDecl>) printEnumDecl(n, indent);// <-- enums case
        else if constexpr (is_same_v<T, CallExpr>) printCallExpr(n, indent);
        else if constexpr (is_same_v<T, VarDecl>) printVarDecl(n, indent);
        else if constexpr (is_same_v<T, BlockStmt>) printBlockStmt(n, indent);
        else if constexpr (is_same_v<T, FunctionDecl>) printFunctionDecl(n, indent);
        else if constexpr (is_same_v<T, FunctionProto>) printFunctionProto(n, indent); // <-- Add this
        else if constexpr (is_same_v<T, MainDecl>) printMainDecl(n, indent);
        else if constexpr (is_same_v<T, IfStmt>) printIfStmt(n, indent);
        else if constexpr (is_same_v<T, WhileStmt>) printWhileStmt(n, indent);
        else if constexpr (is_same_v<T, DoWhileStmt>) printDoWhileStmt(n, indent);
        else if constexpr (is_same_v<T, ForStmt>) printForStmt(n, indent);
        else if constexpr (is_same_v<T, CaseBlock>) printCaseBlock(n, indent);
        else if constexpr (is_same_v<T, SwitchStmt>) printSwitchStmt(n, indent);
        else if constexpr (is_same_v<T, ReturnStmt>) printReturnStmt(n, indent);
        else if constexpr (is_same_v<T, PrintStmt>) printPrintStmt(n, indent);
        else if constexpr (is_same_v<T, BreakStmt>) printBreakStmt(n, indent); // break statment
        else if constexpr (is_same_v<T, ExpressionStmt>) printExpressionStmt(n, indent);
    }, node);
}

// === Parser (refactored helpers) ===
class Parser {
private:
    vector<Token> tokens;
    size_t current;
    Token currentToken;

    enum Precedence {
        LOWEST = 0,
        ASSIGNMENT = 1,
        LOGICAL_OR = 2,
        LOGICAL_AND = 3,
        EQUALITY = 4,
        COMPARISON = 5,
        TERM = 6,
        FACTOR = 7,
        UNARY = 8,
        CALL = 9,
    };

    // Convenience helpers
    const Token& peek(size_t offset = 0) const {
        size_t idx = current + offset;
        if (idx < tokens.size()) return tokens[idx];
        static Token eofTok = {T_EOF, "EOF", -1, -1};
        return eofTok;
    }

    bool isAtEnd() const { return currentToken.type == T_EOF; }

    void advance() {
        if (current < tokens.size()) current++;
        if (current < tokens.size()) currentToken = tokens[current];
        else currentToken = {T_EOF, "EOF", -1, -1};
    }

    bool check(TokenType type) const { return currentToken.type == type; }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    Token expect(TokenType type, ParseErrorType errType = ParseErrorType::FailedToFindToken) {
        if (currentToken.type == type) {
            Token t = currentToken;
            advance();
            return t;
        }
        throw ParseError(errType, currentToken);
    }

    void consumeSemicolon() {
        if (!match(T_SEMICOLON)) throw ParseError(ParseErrorType::MissingSemicolon, currentToken);
    }

    // Precedence mapping
    int getPrecedence(TokenType type) const {
        switch (type) {
            case T_ASSIGNOP: return ASSIGNMENT;
            case T_OR: return LOGICAL_OR;
            case T_AND: return LOGICAL_AND;
            case T_EQUALOP: case T_NE: return EQUALITY;
            case T_LT: case T_GT: case T_LE: case T_GE: return COMPARISON;
            case T_PLUS: case T_MINUS: return TERM;
            case T_MULTIPLY: case T_DIVIDE: case T_MODULO: return FACTOR;
            case T_LPAREN: return CALL;
            // Add bitwise operators with appropriate precedence
            case T_BITAND: case T_BITOR: case T_BITXOR: return TERM;  // Bitwise ops have same precedence as +/-
            case T_BITLSHIFT: case T_BITRSHIFT: return TERM;  // Shift ops have same precedence as +/-
            default: return LOWEST;
        }
    }

    bool isTypeToken(TokenType type) const {
        return type == T_INT || type == T_FLOAT || type == T_DOUBLE ||
               type == T_CHAR || type == T_BOOL || type == T_VOID  || type == T_STRING;
    }

    bool isDeclarationStart(TokenType type) const {
        return isTypeToken(type) || type == T_MAIN || type == T_PRINT;
    }

    // Small utility to check if ASTPtr is Identifier
    bool isIdentifierNode(const ASTPtr& node) const {
        if (!node) return false;
        return holds_alternative<Identifier>(node->node);
    }

    // Check if current position is a function declaration vs function prototype
    bool isFunctionDefinition() const {
        if (current + 1 >= tokens.size()) return false;
        size_t i = current + 1;
        
        // Look for identifier followed by LPAREN, then LBRACE (function definition)
        if (i < tokens.size() && tokens[i].type == T_IDENTIFIER) {
            i++;
            if (i < tokens.size() && tokens[i].type == T_LPAREN) {
                i++;
                // Skip parameters
                int parenCount = 1;
                while (i < tokens.size() && parenCount > 0) {
                    if (tokens[i].type == T_LPAREN) parenCount++;
                    else if (tokens[i].type == T_RPAREN) parenCount--;
                    i++;
                }
                
                if (i < tokens.size() && tokens[i].type == T_LBRACE) {
                    return true;
                }
            }
        }
        return false;
    }

    // ---- Pratt parser core ----
    ASTPtr parseExpression(int precedence = LOWEST) {
        ASTPtr left = parsePrefix();

        while (!isAtEnd() && getPrecedence(currentToken.type) > precedence) {
            left = parseInfix(move(left), getPrecedence(currentToken.type));
        }

        return left;
    }

    ASTPtr parsePrefix() {
        switch (currentToken.type) {
            case T_INTLIT:    return parseIntLiteral();
            case T_FLOATLIT:  return parseFloatLiteral();
            case T_STRINGLIT: return parseStringLiteral();
            case T_CHARLIT:   return parseCharLiteral();
            case T_BOOLLIT:   return parseBoolLiteral();
            case T_IDENTIFIER: return parseIdentifier();
            case T_LPAREN:    return parseGroupedExpression();
            case T_MINUS:
            case T_NOT:
                return parseUnaryExpression();
            default:
                throw ParseError(ParseErrorType::ExpectedExpr, currentToken);
        }
    }

    ASTPtr parseInfix(ASTPtr left, int precedence) {
        switch (currentToken.type) {
            case T_ASSIGNOP: case T_PLUS: case T_MINUS: case T_MULTIPLY:
            case T_DIVIDE: case T_MODULO: case T_EQUALOP: case T_NE:
            case T_LT: case T_GT: case T_LE: case T_GE: case T_AND: case T_OR:
                return parseBinaryExpression(move(left), precedence);
                
            // Add bitwise operators here
            case T_BITAND: case T_BITOR: case T_BITXOR: case T_BITLSHIFT: case T_BITRSHIFT:
            return parseBinaryExpression(move(left), precedence);

            case T_LPAREN:
                if (isIdentifierNode(left)) return parseCallExpression(move(left));
                throw ParseError(ParseErrorType::InvalidCallTarget, currentToken);

            default:
                throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
        }
    }

    // ---- Literal parsers with position tracking ----
    ASTPtr parseIntLiteral() {
        Token t = currentToken;
        advance();
        return make_unique<ASTNode>(IntLiteral(stoi(t.value), t.line, t.column));
    }

    ASTPtr parseFloatLiteral() {
        Token t = currentToken;
        advance();
        return make_unique<ASTNode>(FloatLiteral(stod(t.value), t.line, t.column));
    }

    ASTPtr parseStringLiteral() {
        Token t = currentToken;
        advance();
        string s = t.value;
        if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
            s = s.substr(1, s.length() - 2);
        }
        return make_unique<ASTNode>(StringLiteral(s, t.line, t.column));
    }

    ASTPtr parseCharLiteral() {
        Token t = currentToken;
        advance();
        char c = '\0';
        if (t.value.size() >= 3 && t.value.front() == '\'' && t.value.back() == '\'') {
            c = t.value[1];
        } else if (!t.value.empty()) {
            c = t.value[0];
        }
        return make_unique<ASTNode>(CharLiteral(c, t.line, t.column));
    }

    ASTPtr parseBoolLiteral() {
        Token t = currentToken;
        advance();
        return make_unique<ASTNode>(BoolLiteral(t.value == "true", t.line, t.column));
    }

    ASTPtr parseIdentifier() {
        Token t = currentToken;
        advance();
        return make_unique<ASTNode>(Identifier(t.value, t.line, t.column));
    }

    ASTPtr parseGroupedExpression() {
        Token startToken = currentToken;  // Capture start position
        expect(T_LPAREN);
        ASTPtr expr = parseExpression();
        expect(T_RPAREN);
        // For grouped expressions, we'll use the position of the opening parenthesis
        return make_unique<ASTNode>(BinaryExpr(T_LPAREN, move(expr), nullptr, startToken.line, startToken.column));
    }

    ASTPtr parseUnaryExpression() {
        Token op = currentToken;
        advance();
        ASTPtr right = parseExpression(UNARY);
        return make_unique<ASTNode>(UnaryExpr(op.type, move(right), op.line, op.column));
    }

    ASTPtr parseBinaryExpression(ASTPtr left, int precedence) {
        Token op = currentToken;
        int nextPrecedence = getPrecedence(op.type);
        advance();
        ASTPtr right = parseExpression(nextPrecedence);

        if (op.type == T_ASSIGNOP) {
            if (!isIdentifierNode(left)) {
                throw ParseError(ParseErrorType::UnexpectedToken, op);
            }
        }

        return make_unique<ASTNode>(BinaryExpr(op.type, move(left), move(right), op.line, op.column));
    }

    ASTPtr parseEnumDeclaration() {
        Token enumToken = currentToken;
        expect(T_ENUM);
        Token nameToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
        string name = nameToken.value;
        expect(T_LBRACE); // Consume '{'

        vector<string> values;
        if (!check(T_RBRACE)) {
            do {
                Token valueToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
                values.push_back(valueToken.value);
            } while (match(T_COMMA)); // Handle comma-separated values
        }
        expect(T_RBRACE); // Consume '}'
        consumeSemicolon(); // Enums end with semicolon

        auto valueList = make_unique<ASTNode>(EnumValueList(move(values), nameToken.line, nameToken.column));
        return make_unique<ASTNode>(EnumDecl(name, move(valueList), enumToken.line, enumToken.column));
    }

    // ---- Call expression with position tracking ----
    ASTPtr parseCallExpression(ASTPtr callee) {
        if (!isIdentifierNode(callee)) throw ParseError(ParseErrorType::InvalidCallTarget, currentToken);

        Token callToken = currentToken; 
        expect(T_LPAREN);
        vector<ASTPtr> args;
        if (!check(T_RPAREN)) {
            do {
                args.push_back(parseExpression());
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);
        return make_unique<ASTNode>(CallExpr(move(callee), move(args), callToken.line, callToken.column));
    }
    
    // Modify parseStatement to recognize enum declarations and track positions
    ASTPtr parseStatement() {
        Token stmtToken = currentToken; 

        if (check(T_ENUM)) {
            return parseEnumDeclaration();
        }

        if (isTypeToken(currentToken.type)) {
            const Token& next = peek(1);
            if (next.type == T_IDENTIFIER) {
                size_t i = current + 2; // After type and identifier

                if (i < tokens.size() && tokens[i].type == T_LPAREN) {
                    if (isFunctionDefinition()) {
                        return parseFunctionDeclaration();
                    } else {
                        return parseFunctionPrototype();
                    }
                } else {
                    // It's a variable declaration
                    return parseVariableDeclaration();
                }
            } else {
                return parseVariableDeclaration();
            }
        }
        if (check(T_PRINT)) return parsePrintStatement();
        if (check(T_IF)) return parseIfStatement();
        if (check(T_WHILE)) return parseWhileStatement();
        if (match(T_DO)) return parseDoWhileStatement();
        if (match(T_FOR)) return parseForStatement();
        if (match(T_SWITCH)) return parseSwitchStatement();
        if (check(T_RETURN)) return parseReturnStatement();
        if (check(T_LBRACE)) return parseBlockStatement();
        if (check(T_MAIN)) return parseMainDeclaration(); 
        if (check(T_BREAK)) return parseBreakStatement();
        
        ASTPtr expr = parseExpression();
        consumeSemicolon();
        return make_unique<ASTNode>(ExpressionStmt(move(expr), stmtToken.line, stmtToken.column));
    }

    ASTPtr parseVariableDeclaration() {
        Token typeToken = currentToken;  // Capture type position
        TokenType type = currentToken.type;
        advance();

        Token identToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
        string name = identToken.value;

        ASTPtr initializer = nullptr;
        if (match(T_ASSIGNOP)) initializer = parseExpression();

        consumeSemicolon();
        return make_unique<ASTNode>(VarDecl(type, name, move(initializer), typeToken.line, typeToken.column));
    }

    ASTPtr parseFunctionPrototype() {
        Token returnToken = currentToken;  // Capture return type position
        TokenType returnType = currentToken.type;
        advance();

        Token nameToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
        string name = nameToken.value;

        expect(T_LPAREN);
        vector<pair<TokenType, string>> params;
        if (!check(T_RPAREN)) {
            do {
                TokenType paramType = currentToken.type;
                advance();
                Token paramName = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
                params.push_back({paramType, paramName.value});
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);
        consumeSemicolon(); // Prototypes end with semicolon
        
        return make_unique<ASTNode>(FunctionProto(returnType, name, move(params), returnToken.line, returnToken.column));
    }

    ASTPtr parseFunctionDeclaration() {
        Token returnToken = currentToken;  // Capture return type position
        TokenType returnType = currentToken.type;
        advance();

        Token nameToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
        string name = nameToken.value;

        expect(T_LPAREN);
        vector<pair<TokenType, string>> params;
        if (!check(T_RPAREN)) {
            do {
                TokenType paramType = currentToken.type;
                advance();
                Token paramName = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
                params.push_back({paramType, paramName.value});
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);

        vector<ASTPtr> body = parseBlock();
        return make_unique<ASTNode>(FunctionDecl(returnType, name, move(params), move(body), returnToken.line, returnToken.column));
    }

    ASTPtr parseBreakStatement() {
        Token breakToken = currentToken;  // Capture 'print' position
        expect(T_BREAK);
        expect(T_SEMICOLON);
        return make_unique<ASTNode>(BreakStmt(breakToken.line, breakToken.column));
    }

    ASTPtr parsePrintStatement() {
        Token printToken = currentToken;  // Capture 'print' position
        expect(T_PRINT);
        expect(T_LPAREN);
        vector<ASTPtr> args;
        if (!check(T_RPAREN)) {
            do {
                args.push_back(parseExpression());
            } while (match(T_COMMA));
        }
        expect(T_RPAREN);
        consumeSemicolon();
        return make_unique<ASTNode>(PrintStmt(move(args), printToken.line, printToken.column));
    }

    ASTPtr parseIfStatement() {
        Token ifToken = currentToken;  // Capture 'if' position
        expect(T_IF);
        expect(T_LPAREN);
        ASTPtr condition = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> ifBody = parseBlock();

        vector<ASTPtr> elseBody;
        if (match(T_ELSE)) elseBody = parseBlock();

        return make_unique<ASTNode>(IfStmt(move(condition), move(ifBody), move(elseBody), ifToken.line, ifToken.column));
    }

    ASTPtr parseWhileStatement() {
        Token whileToken = currentToken;  // Capture 'while' position
        expect(T_WHILE);
        expect(T_LPAREN);
        ASTPtr condition = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> body = parseBlock();
        return make_unique<ASTNode>(WhileStmt(move(condition), move(body), whileToken.line, whileToken.column));
    }

    // Replace the do-while method with position tracking
    ASTPtr parseDoWhileStatement() {
        Token doToken = currentToken;  // Capture 'do' position
        vector<ASTPtr> bodyVec = parseBlock(); // Parse as vector first
        ASTPtr body = make_unique<ASTNode>(BlockStmt(move(bodyVec), doToken.line, doToken.column)); // Wrap in BlockStmt
        expect(T_WHILE);
        expect(T_LPAREN);
        ASTPtr cond = parseExpression();
        expect(T_RPAREN);
        expect(T_SEMICOLON);
        return make_unique<ASTNode>(DoWhileStmt(move(body), move(cond), doToken.line, doToken.column));
    }

    // Replace the for method with position tracking
    ASTPtr parseForStatement() {
        Token forToken = currentToken;  // Capture 'for' position
        expect(T_LPAREN);

        // init
        ASTPtr init = nullptr;
        if (!check(T_SEMICOLON)) {
            if (isTypeToken(currentToken.type)) {
                // Parse variable declaration directly here
                TokenType type = currentToken.type; 
                advance();
                Token identToken = expect(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier);
                string name = identToken.value;
                ASTPtr initializer = nullptr;
                if (match(T_ASSIGNOP)) initializer = parseExpression();
                consumeSemicolon();
                init = make_unique<ASTNode>(VarDecl(type, name, move(initializer), forToken.line, forToken.column));
            } else {
                init = parseExpression();
                expect(T_SEMICOLON);
            }
        } else advance(); // skip ;

        // condition
        ASTPtr cond = nullptr;
        if (!check(T_SEMICOLON)) cond = parseExpression();
        expect(T_SEMICOLON);

        // update
        ASTPtr update = nullptr;
        if (!check(T_RPAREN)) update = parseExpression();
        expect(T_RPAREN);

        vector<ASTPtr> bodyVec = parseBlock(); // Parse as vector first
        ASTPtr body = make_unique<ASTNode>(BlockStmt(move(bodyVec), forToken.line, forToken.column)); // Wrap in BlockStmt
        return make_unique<ASTNode>(ForStmt(move(init), move(cond), move(update), move(body), forToken.line, forToken.column));
    }

    ASTPtr parseSwitchStatement() {
        Token switchToken = currentToken;  // Capture 'switch' position
        expect(T_LPAREN);
        ASTPtr expr = parseExpression();
        expect(T_RPAREN);
        expect(T_LBRACE);

        vector<ASTPtr> cases;
        vector<ASTPtr> defaultBody;

        while (!check(T_RBRACE) && !check(T_EOF)) {
            if (match(T_CASE)) {
                ASTPtr val = parseExpression();
                // Expect block statement directly after case value (no colon)
                vector<ASTPtr> caseBody = parseBlock(); // This will parse the { ... } block
                cases.push_back(make_unique<ASTNode>(CaseBlock(move(val), move(caseBody), currentToken.line, currentToken.column)));
            } 
            else if (match(T_DEFAULT)) {
                // Expect block statement directly after default (no colon)
                defaultBody = parseBlock(); // This will parse the { ... } block
            } 
            else {
                advance(); // skip unknown tokens
            }
        }
        expect(T_RBRACE);

        return make_unique<ASTNode>(SwitchStmt(move(expr), move(cases), move(defaultBody), switchToken.line, switchToken.column));
    }

    ASTPtr parseReturnStatement() {
        Token returnToken = currentToken;  // Capture 'return' position
        expect(T_RETURN);
        ASTPtr value = nullptr;
        if (!check(T_SEMICOLON) && !check(T_RBRACE)) value = parseExpression();
        consumeSemicolon();
        return make_unique<ASTNode>(ReturnStmt(move(value), returnToken.line, returnToken.column));
    }

    ASTPtr parseBlockStatement() {
        Token blockToken = currentToken;  // Capture '{' position
        vector<ASTPtr> body = parseBlock();
        return make_unique<ASTNode>(BlockStmt(move(body), blockToken.line, blockToken.column));
    }

    vector<ASTPtr> parseBlock() {
        Token lbraceToken = currentToken;  // Capture '{' position
        expect(T_LBRACE);
        vector<ASTPtr> statements;
        while (!check(T_RBRACE) && !check(T_EOF)) {
            statements.push_back(parseStatement());
        }
        if (check(T_EOF)) throw ParseError(ParseErrorType::UnclosedBlock, currentToken);
        expect(T_RBRACE);
        return statements;
    }

    ASTPtr parseMainDeclaration() {
        Token mainToken = currentToken;  // Capture 'main' position
        expect(T_MAIN);
        vector<ASTPtr> body = parseBlock();
        return make_unique<ASTNode>(MainDecl(move(body), mainToken.line, mainToken.column));
    }

    ASTPtr parseIncludeStatement() {
        Token includeToken = currentToken; 
        expect(T_INCLUDE); 

        if (match(T_LT)) { 
            if (check(T_MAIN)) {
                advance(); 
                expect(T_GT); 
                return make_unique<ASTNode>(IncludeStmt("main", includeToken.line, includeToken.column));
            } 
            else {
                string header;
                if (check(T_IDENTIFIER)) {
                    header = currentToken.value;
                    advance();
                } 
                else {
                    throw ParseError(ParseErrorType::ExpectedIdentifier, currentToken); 
                }
                expect(T_GT); // Consume '>'
                return make_unique<ASTNode>(IncludeStmt(header, includeToken.line, includeToken.column));
            }
        } 
        else if (check(T_STRINGLIT)) {
            Token headerTok = currentToken;
            advance();
            string header = headerTok.value;
            if (header.size() >= 2 && header.front() == '"' && header.back() == '"') {
                header = header.substr(1, header.size() - 2); 
            }
            return make_unique<ASTNode>(IncludeStmt(header, includeToken.line, includeToken.column));
        } 
        else {
            throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
        }
    }


public:
    Parser(const vector<Token>& toks) : tokens(toks), current(0) {
        if (!tokens.empty()) currentToken = tokens[0];
        else currentToken = {T_EOF, "EOF", -1, -1};
    }

    
    vector<ASTPtr> parseProgram() {
        vector<ASTPtr> declarations;

        // Enforce first token must be include<main>
        if (!(check(T_INCLUDE))) {
            throw ParseError(ParseErrorType::UnexpectedToken, currentToken);
        }
        declarations.push_back(parseIncludeStatement());

        // Continue with the rest
        while (currentToken.type != T_EOF) {
            if (check(T_INCLUDE)) { // Also handle subsequent include statements
                declarations.push_back(parseIncludeStatement());
                continue;
            }
            if (isTypeToken(currentToken.type)) {
                const Token& next = peek(1);
                if (next.type == T_IDENTIFIER) {
                    // Check if it's a function definition or prototype
                    if (isFunctionDefinition()) {
                        declarations.push_back(parseFunctionDeclaration());
                    } else {
                        declarations.push_back(parseFunctionPrototype());
                    }
                } else {
                    declarations.push_back(parseStatement());
                }
                continue;
            }
            if (check(T_MAIN)) {
                declarations.push_back(parseMainDeclaration());
                continue;
            }
            declarations.push_back(parseStatement());
        }
        return declarations;
    }

};

vector<unique_ptr<ASTNode>> parseFromFile(const vector<Token>& tokens) {
    try {
        Parser parser(tokens);
        auto ast = parser.parseProgram();

        cout << "=== Parsed AST ===\n";
        for (const auto& node : ast) {
            if (node) printASTNode(node->node);
        }
        cout << "\n=== Parsing Successful ===\n";

        return ast;
    } 
    catch (const ParseError& e) {
        cerr << "[Parser Error] " << e.message
             << " (line " << e.token.line
             << ", col " << e.token.column << ")\n";
        exit(EXIT_FAILURE);
    } 
    catch (const exception& e) {
        cerr << "[Parser Exception] " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }
}