#include "parser.h"

// === Keywords Map ===
static const map<string, TokenType> keywords = {
    {"int", T_INT}, {"float", T_FLOAT}, {"double", T_DOUBLE},
    {"char", T_CHAR}, {"void", T_VOID}, {"bool", T_BOOL},
    {"true", T_BOOLLIT}, {"false", T_BOOLLIT},
    {"if", T_IF}, {"else", T_ELSE}, {"while", T_WHILE}, {"return", T_RETURN},
    {"print", T_PRINT}, {"main", T_MAIN}
};

// === ASTNode Implementation ===
ASTNode::ASTNode(NodeType type, int l, int c) : nodeType(type), line(l), column(c) {}

void ASTNode::printIndent(int indent) const {
    for (int i = 0; i < indent; i++) cout << "  ";
}

// === Program Implementation ===
Program::Program() : ASTNode(NodeType::Program) {}

void Program::print(int indent) const {
    printIndent(indent);
    cout << "Program [\n";
    for (const auto& decl : declarations) {
        decl->print(indent + 1);
    }
    printIndent(indent);
    cout << "]\n";
}

// === Parameter Implementation ===
Parameter::Parameter(const string& t, const string& n) 
    : ASTNode(NodeType::Parameter), type(t), name(n) {}

void Parameter::print(int indent) const {
    printIndent(indent);
    cout << "Param { type: " << type << ", name: \"" << name << "\" }\n";
}

// === FunctionDecl Implementation ===
FunctionDecl::FunctionDecl(const string& retType, const string& funcName) 
    : ASTNode(NodeType::FunctionDecl), returnType(retType), name(funcName) {}

void FunctionDecl::print(int indent) const {
    printIndent(indent);
    cout << "Fn(\n";
    printIndent(indent + 1);
    cout << "FnDecl {\n";
    printIndent(indent + 2);
    cout << "type_tok: " << returnType << ",\n";
    printIndent(indent + 2);
    cout << "ident: \"" << name << "\",\n";
    printIndent(indent + 2);
    cout << "params: [\n";
    for (const auto& param : parameters) {
        param->print(indent + 3);
    }
    printIndent(indent + 2);
    cout << "],\n";
    printIndent(indent + 2);
    cout << "block: [\n";
    if (body) body->print(indent + 3);
    printIndent(indent + 2);
    cout << "]\n";
    printIndent(indent + 1);
    cout << "}\n";
    printIndent(indent);
    cout << ")\n";
}

// === Block Implementation ===
Block::Block() : ASTNode(NodeType::Block) {}

void Block::print(int indent) const {
    for (const auto& stmt : statements) {
        stmt->print(indent);
    }
}

// === VarDecl Implementation ===
VarDecl::VarDecl(const string& t, const string& n) 
    : ASTNode(NodeType::VarDecl), type(t), name(n) {}

void VarDecl::print(int indent) const {
    printIndent(indent);
    cout << "Var(\n";
    printIndent(indent + 1);
    cout << "VarDecl {\n";
    printIndent(indent + 2);
    cout << "type_tok: " << type << ",\n";
    printIndent(indent + 2);
    cout << "ident: \"" << name << "\",\n";
    printIndent(indent + 2);
    cout << "expr: ";
    if (initializer) {
        cout << "Some(\n";
        initializer->print(indent + 3);
        printIndent(indent + 2);
        cout << ")\n";
    } else {
        cout << "None\n";
    }
    printIndent(indent + 1);
    cout << "}\n";
    printIndent(indent);
    cout << ")\n";
}

// === Assignment Implementation ===
Assignment::Assignment(const string& var) 
    : ASTNode(NodeType::Assignment), variable(var) {}

void Assignment::print(int indent) const {
    printIndent(indent);
    cout << "Assign(\"" << variable << "\", ";
    if (expression) expression->print(0);
    cout << ")\n";
}

// === BinaryOp Implementation ===
BinaryOp::BinaryOp(const string& op) 
    : ASTNode(NodeType::BinaryOp), operator_(op) {}

string BinaryOp::getOpName() const {
    if (operator_ == "+") return "Add";
    if (operator_ == "-") return "Sub";
    if (operator_ == "*") return "Mul";
    if (operator_ == "/") return "Div";
    if (operator_ == "==") return "Comp";
    if (operator_ == "!=") return "Comp";
    if (operator_ == "<") return "Comp";
    if (operator_ == ">") return "Comp";
    if (operator_ == "<=") return "Comp";
    if (operator_ == ">=") return "Comp";
    return "BinOp";
}

void BinaryOp::print(int indent) const {
    printIndent(indent);
    cout << "::" << getOpName() << "(" << operator_ << ")\n";
    if (left) left->print(indent + 1);
    if (right) right->print(indent + 1);
}

// === UnaryOp Implementation ===
UnaryOp::UnaryOp(const string& op) 
    : ASTNode(NodeType::UnaryOp), operator_(op) {}

void UnaryOp::print(int indent) const {
    printIndent(indent);
    cout << "Unary(" << operator_ << ")\n";
    if (operand) operand->print(indent + 1);
}

// === FunctionCall Implementation ===
FunctionCall::FunctionCall(const string& name) 
    : ASTNode(NodeType::FunctionCall), functionName(name) {}

void FunctionCall::print(int indent) const {
    printIndent(indent);
    cout << "Call(FnCall {\n";
    printIndent(indent + 1);
    cout << "ident: \"" << functionName << "\",\n";
    printIndent(indent + 1);
    cout << "args: [\n";
    for (const auto& arg : arguments) {
        printIndent(indent + 2);
        cout << "Some(\n";
        arg->print(indent + 3);
        printIndent(indent + 2);
        cout << "),\n";
    }
    printIndent(indent + 1);
    cout << "]\n";
    printIndent(indent);
    cout << "})\n";
}

// === Identifier Implementation ===
Identifier::Identifier(const string& n) 
    : ASTNode(NodeType::Identifier), name(n) {}

void Identifier::print(int indent) const {
    printIndent(indent);
    cout << "\"" << name << "\"\n";
}

// === IntLiteral Implementation ===
IntLiteral::IntLiteral(int v) 
    : ASTNode(NodeType::IntLiteral), value(v) {}

void IntLiteral::print(int indent) const {
    printIndent(indent);
    cout << value << "\n";
}

// === FloatLiteral Implementation ===
FloatLiteral::FloatLiteral(float v) 
    : ASTNode(NodeType::FloatLiteral), value(v) {}

void FloatLiteral::print(int indent) const {
    printIndent(indent);
    cout << value << "\n";
}

// === StringLiteral Implementation ===
StringLiteral::StringLiteral(const string& v) 
    : ASTNode(NodeType::StringLiteral), value(v) {}

void StringLiteral::print(int indent) const {
    printIndent(indent);
    cout << "\"" << value << "\"\n";
}

// === ReturnStmt Implementation ===
ReturnStmt::ReturnStmt() : ASTNode(NodeType::ReturnStmt) {}

void ReturnStmt::print(int indent) const {
    printIndent(indent);
    cout << "Ret(\n";
    printIndent(indent + 1);
    cout << "ExprStmt {\n";
    printIndent(indent + 2);
    cout << "expr: ";
    if (expression) {
        cout << "Some(\n";
        expression->print(indent + 3);
        printIndent(indent + 2);
        cout << ")\n";
    } else {
        cout << "None\n";
    }
    printIndent(indent + 1);
    cout << "}\n";
    printIndent(indent);
    cout << ")\n";
}

// === IfStmt Implementation ===
IfStmt::IfStmt() : ASTNode(NodeType::IfStmt) {}

void IfStmt::print(int indent) const {
    printIndent(indent);
    cout << "If(\n";
    printIndent(indent + 1);
    cout << "IfStmt {\n";
    printIndent(indent + 2);
    cout << "cond: ";
    if (condition) {
        cout << "Some(\n";
        condition->print(indent + 3);
        printIndent(indent + 2);
        cout << "),\n";
    } else {
        cout << "None,\n";
    }
    printIndent(indent + 2);
    cout << "if_block: [\n";
    if (thenBranch) thenBranch->print(indent + 3);
    printIndent(indent + 2);
    cout << "],\n";
    printIndent(indent + 2);
    cout << "else_block: [\n";
    if (elseBranch) elseBranch->print(indent + 3);
    printIndent(indent + 2);
    cout << "]\n";
    printIndent(indent + 1);
    cout << "}\n";
    printIndent(indent);
    cout << ")\n";
}

// === WhileStmt Implementation ===
WhileStmt::WhileStmt() : ASTNode(NodeType::WhileStmt) {}

void WhileStmt::print(int indent) const {
    printIndent(indent);
    cout << "While(\n";
    printIndent(indent + 1);
    cout << "WhileStmt {\n";
    printIndent(indent + 2);
    cout << "cond: ";
    if (condition) {
        cout << "Some(\n";
        condition->print(indent + 3);
        printIndent(indent + 2);
        cout << "),\n";
    }
    printIndent(indent + 2);
    cout << "body: [\n";
    if (body) body->print(indent + 3);
    printIndent(indent + 2);
    cout << "]\n";
    printIndent(indent + 1);
    cout << "}\n";
    printIndent(indent);
    cout << ")\n";
}

// === PrintStmt Implementation ===
PrintStmt::PrintStmt() : ASTNode(NodeType::PrintStmt) {}

void PrintStmt::print(int indent) const {
    printIndent(indent);
    cout << "Print([\n";
    for (const auto& arg : arguments) {
        arg->print(indent + 1);
    }
    printIndent(indent);
    cout << "])\n";
}

// === Parser Implementation ===
Parser::Parser(const vector<Token>& tokenList) : tokens(tokenList), current(0) {}

unique_ptr<Program> Parser::parse() {
    auto program = make_unique<Program>();
    
    while (!isAtEnd()) {
        if (peek().type == T_EOF) break;
        
        try {
            auto decl = parseDeclaration();
            if (decl) {
                program->declarations.push_back(move(decl));
            }
        } catch (const exception& e) {
            cerr << "Parse error: " << e.what() << endl;
            synchronize();
        }
    }
    
    return program;
}

// === Utility Methods Implementation ===
bool Parser::isAtEnd() const {
    return current >= tokens.size() || peek().type == T_EOF;
}

Token Parser::peek() const {
    if (current < tokens.size()) {
        return tokens[current];
    }
    return Token{T_EOF, "", 0, 0};
}

Token Parser::previous() const {
    if (current > 0 && current <= tokens.size()) {
        return tokens[current - 1];
    }
    return Token{T_EOF, "", 0, 0};
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const string& message) {
    if (check(type)) return advance();
    throw runtime_error(message + " at line " + to_string(peek().line));
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == T_SEMICOLON) return;
        
        switch (peek().type) {
            case T_IF:
            case T_WHILE:
            case T_RETURN:
            case T_INT:
            case T_FLOAT:
            case T_DOUBLE:
            case T_CHAR:
            case T_VOID:
            case T_BOOL:
                return;
            default:
                break;
        }
        advance();
    }
}

bool Parser::isTypeToken(TokenType type) const {
    return type == T_INT || type == T_FLOAT || type == T_DOUBLE || 
           type == T_CHAR || type == T_VOID || type == T_BOOL;
}

// === Parsing Methods Implementation ===
unique_ptr<ASTNode> Parser::parseDeclaration() {
    if (isTypeToken(peek().type)) {
        Token typeToken = advance();
        
        if (!check(T_IDENTIFIER)) {
            throw runtime_error("Expected identifier after type");
        }
        
        Token identifier = advance();
        
        if (match(T_LPAREN)) {
            return parseFunctionDeclaration(typeToken.value, identifier.value);
        } else {
            return parseVariableDeclaration(typeToken.value, identifier.value);
        }
    }
    
    return parseStatement();
}

unique_ptr<FunctionDecl> Parser::parseFunctionDeclaration(const string& returnType, const string& name) {
    Token identToken = previous(); // Get the identifier token for line/column
    auto func = make_unique<FunctionDecl>(returnType, name);
    func->line = identToken.line;
    func->column = identToken.column;
    
    // Parse parameters
    if (!check(T_RPAREN)) {
        do {
            if (!isTypeToken(peek().type)) {
                throw runtime_error("Expected parameter type");
            }
            Token paramType = advance();
            
            if (!check(T_IDENTIFIER)) {
                throw runtime_error("Expected parameter name");
            }
            Token paramName = advance();
            
            func->parameters.push_back(make_unique<Parameter>(paramType.value, paramName.value));
        } while (match(T_COMMA));
    }
    
    consume(T_RPAREN, "Expected ')' after parameters");
    consume(T_LBRACE, "Expected '{' before function body");
    
    func->body = parseBlock();
    
    consume(T_RBRACE, "Expected '}' after function body");
    
    return func;
}

unique_ptr<VarDecl> Parser::parseVariableDeclaration(const string& type, const string& name) {
    Token identToken = previous(); // Get the identifier token for line/column
    auto varDecl = make_unique<VarDecl>(type, name);
    varDecl->line = identToken.line;
    varDecl->column = identToken.column;
    
    if (match(T_ASSIGNOP)) {
        varDecl->initializer = parseExpression();
    }
    
    consume(T_SEMICOLON, "Expected ';' after variable declaration");
    return varDecl;
}

unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(T_IF)) return parseIfStatement();
    if (match(T_WHILE)) return parseWhileStatement();
    if (match(T_RETURN)) return parseReturnStatement();
    if (match(T_PRINT)) return parsePrintStatement();
    if (match(T_LBRACE)) return parseBlock();
    
    return parseExpressionStatement();
}

unique_ptr<Block> Parser::parseBlock() {
    auto block = make_unique<Block>();
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        auto stmt = parseDeclaration();
        if (stmt) {
            block->statements.push_back(move(stmt));
        }
    }
    
    return block;
}

unique_ptr<IfStmt> Parser::parseIfStatement() {
    Token ifToken = previous(); // 'if' token
    auto ifStmt = make_unique<IfStmt>();
    ifStmt->line = ifToken.line;
    ifStmt->column = ifToken.column;
    
    consume(T_LPAREN, "Expected '(' after 'if'");
    ifStmt->condition = parseExpression();
    consume(T_RPAREN, "Expected ')' after if condition");
    
    ifStmt->thenBranch = parseStatement();
    
    if (match(T_ELSE)) {
        ifStmt->elseBranch = parseStatement();
    }
    
    return ifStmt;
}

unique_ptr<WhileStmt> Parser::parseWhileStatement() {
    Token whileToken = previous(); // 'while' token
    auto whileStmt = make_unique<WhileStmt>();
    whileStmt->line = whileToken.line;
    whileStmt->column = whileToken.column;
    
    consume(T_LPAREN, "Expected '(' after 'while'");
    whileStmt->condition = parseExpression();
    consume(T_RPAREN, "Expected ')' after while condition");
    
    whileStmt->body = parseStatement();
    
    return whileStmt;
}

unique_ptr<ReturnStmt> Parser::parseReturnStatement() {
    Token returnToken = previous(); // 'return' token
    auto returnStmt = make_unique<ReturnStmt>();
    returnStmt->line = returnToken.line;
    returnStmt->column = returnToken.column;
    
    if (!check(T_SEMICOLON)) {
        returnStmt->expression = parseExpression();
    }
    
    consume(T_SEMICOLON, "Expected ';' after return statement");
    return returnStmt;
}

unique_ptr<PrintStmt> Parser::parsePrintStatement() {
    Token printToken = previous(); // 'print' token
    auto printStmt = make_unique<PrintStmt>();
    printStmt->line = printToken.line;
    printStmt->column = printToken.column;
    
    consume(T_LPAREN, "Expected '(' after 'print'");
    
    if (!check(T_RPAREN)) {
        do {
            printStmt->arguments.push_back(parseExpression());
        } while (match(T_COMMA));
    }
    
    consume(T_RPAREN, "Expected ')' after print arguments");
    consume(T_SEMICOLON, "Expected ';' after print statement");
    
    return printStmt;
}

unique_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    consume(T_SEMICOLON, "Expected ';' after expression");
    return expr;
}

unique_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignment();
}

unique_ptr<ASTNode> Parser::parseAssignment() {
    auto expr = parseLogicalOr();
    
    if (match(T_ASSIGNOP)) {
        Token assignToken = previous();
        if (expr->nodeType == NodeType::Identifier) {
            auto identifier = static_cast<Identifier*>(expr.get());
            auto assignment = make_unique<Assignment>(identifier->name);
            assignment->line = identifier->line;
            assignment->column = identifier->column;
            assignment->expression = parseAssignment();
            expr.release(); // Release the identifier since we're replacing it
            return move(assignment);
        } else {
            throw runtime_error("Invalid assignment target");
        }
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(T_OR)) {
        Token opToken = previous();
        string op = opToken.value;
        auto right = parseLogicalAnd();
        auto binary = make_unique<BinaryOp>(op);
        binary->line = opToken.line;
        binary->column = opToken.column;
        binary->left = move(expr);
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (match(T_AND)) {
        Token opToken = previous();
        string op = opToken.value;
        auto right = parseEquality();
        auto binary = make_unique<BinaryOp>(op);
        binary->line = opToken.line;
        binary->column = opToken.column;
        binary->left = move(expr);
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match({T_EQUALOP, T_NE})) {
        Token opToken = previous();
        string op = opToken.value;
        auto right = parseComparison();
        auto binary = make_unique<BinaryOp>(op);
        binary->line = opToken.line;
        binary->column = opToken.column;
        binary->left = move(expr);
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (match({T_GT, T_GE, T_LT, T_LE})) {
        Token opToken = previous();
        string op = opToken.value;
        auto right = parseTerm();
        auto binary = make_unique<BinaryOp>(op);
        binary->line = opToken.line;
        binary->column = opToken.column;
        binary->left = move(expr);
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (match({T_MINUS, T_PLUS})) {
        Token opToken = previous();
        string op = opToken.value;
        auto right = parseFactor();
        auto binary = make_unique<BinaryOp>(op);
        binary->line = opToken.line;
        binary->column = opToken.column;
        binary->left = move(expr);
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (match({T_DIVIDE, T_MULTIPLY, T_MODULO})) {
        Token opToken = previous();
        string op = opToken.value;
        auto right = parseUnary();
        auto binary = make_unique<BinaryOp>(op);
        binary->line = opToken.line;
        binary->column = opToken.column;
        binary->left = move(expr);
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parseUnary() {
    if (match({T_NOT, T_MINUS})) {
        Token opToken = previous();
        string op = opToken.value;
        auto right = parseUnary();
        auto unary = make_unique<UnaryOp>(op);
        unary->line = opToken.line;
        unary->column = opToken.column;
        unary->operand = move(right);
        return move(unary);
    }
    
    return parseCall();
}

unique_ptr<ASTNode> Parser::parseCall() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(T_LPAREN)) {
            if (expr->nodeType == NodeType::Identifier) {
                auto identifier = static_cast<Identifier*>(expr.get());
                auto call = make_unique<FunctionCall>(identifier->name);
                call->line = identifier->line;
                call->column = identifier->column;
                
                if (!check(T_RPAREN)) {
                    do {
                        call->arguments.push_back(parseExpression());
                    } while (match(T_COMMA));
                }
                
                consume(T_RPAREN, "Expected ')' after arguments");
                expr.release(); // Release the identifier
                expr = move(call);
            } else {
                throw runtime_error("Only identifiers can be called");
            }
        } else {
            break;
        }
    }
    
    return expr;
}

unique_ptr<ASTNode> Parser::parsePrimary() {
    if (match(T_BOOLLIT)) {
        Token boolToken = previous();
        auto literal = make_unique<IntLiteral>(boolToken.value == "true" ? 1 : 0);
        literal->line = boolToken.line;
        literal->column = boolToken.column;
        return literal;
    }
    
    if (match(T_INTLIT)) {
        Token intToken = previous();
        auto literal = make_unique<IntLiteral>(stoi(intToken.value));
        literal->line = intToken.line;
        literal->column = intToken.column;
        return literal;
    }
    
    if (match(T_FLOATLIT)) {
        Token floatToken = previous();
        auto literal = make_unique<FloatLiteral>(stof(floatToken.value));
        literal->line = floatToken.line;
        literal->column = floatToken.column;
        return literal;
    }
    
    if (match(T_STRINGLIT)) {
        Token strToken = previous();
        auto literal = make_unique<StringLiteral>(strToken.value);
        literal->line = strToken.line;
        literal->column = strToken.column;
        return literal;
    }
    
    if (match(T_IDENTIFIER)) {
        Token identToken = previous();
        auto identifier = make_unique<Identifier>(identToken.value);
        identifier->line = identToken.line;
        identifier->column = identToken.column;
        return identifier;
    }
    
    if (match(T_LPAREN)) {
        auto expr = parseExpression();
        consume(T_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw runtime_error("Expected expression at line " + to_string(peek().line));
}

// === Lexer Integration Implementation ===
const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case T_INT: return "T_INT";
        case T_FLOAT: return "T_FLOAT";
        case T_IDENTIFIER: return "T_IDENTIFIER";
        case T_INTLIT: return "T_INTLIT";
        case T_FLOATLIT: return "T_FLOATLIT";
        case T_STRINGLIT: return "T_STRINGLIT";
        case T_LPAREN: return "T_LPAREN";
        case T_RPAREN: return "T_RPAREN";
        case T_LBRACE: return "T_LBRACE";
        case T_RBRACE: return "T_RBRACE";
        case T_SEMICOLON: return "T_SEMICOLON";
        case T_COMMA: return "T_COMMA";
        case T_ASSIGNOP: return "T_ASSIGNOP";
        case T_EQUALOP: return "T_EQUALOP";
        case T_LT: return "T_LT";
        case T_GT: return "T_GT";
        case T_PLUS: return "T_PLUS";
        case T_MINUS: return "T_MINUS";
        case T_MULTIPLY: return "T_MULTIPLY";
        case T_DIVIDE: return "T_DIVIDE";
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_RETURN: return "T_RETURN";
        case T_PRINT: return "T_PRINT";
        case T_EOF: return "T_EOF";
        default: return "T_UNKNOWN";
    }
}

vector<Token> loadTokensFromFile(const string& filename) {
    vector<Token> tokens;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open " << filename << endl;
        return tokens;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // Parse format: TOKEN_TYPE(value),line,column
        size_t parenPos = line.find('(');
        size_t closeParenPos = line.find(')');
        size_t firstCommaPos = line.find(',', closeParenPos);
        size_t secondCommaPos = line.find(',', firstCommaPos + 1);
        
        if (parenPos == string::npos || closeParenPos == string::npos || 
            firstCommaPos == string::npos || secondCommaPos == string::npos) {
            continue;
        }
        
        string tokenTypeStr = line.substr(0, parenPos);
        string value = line.substr(parenPos + 1, closeParenPos - parenPos - 1);
        int lineNum = stoi(line.substr(firstCommaPos + 1, secondCommaPos - firstCommaPos - 1));
        int column = stoi(line.substr(secondCommaPos + 1));
        
        // Convert string to TokenType
        TokenType type = T_EOF;
        if (tokenTypeStr == "T_INT") type = T_INT;
        else if (tokenTypeStr == "T_FLOAT") type = T_FLOAT;
        else if (tokenTypeStr == "T_IDENTIFIER") type = T_IDENTIFIER;
        else if (tokenTypeStr == "T_INTLIT") type = T_INTLIT;
        else if (tokenTypeStr == "T_FLOATLIT") type = T_FLOATLIT;
        else if (tokenTypeStr == "T_STRINGLIT") type = T_STRINGLIT;
        else if (tokenTypeStr == "T_LPAREN") type = T_LPAREN;
        else if (tokenTypeStr == "T_RPAREN") type = T_RPAREN;
        else if (tokenTypeStr == "T_LBRACE") type = T_LBRACE;
        else if (tokenTypeStr == "T_RBRACE") type = T_RBRACE;
        else if (tokenTypeStr == "T_SEMICOLON") type = T_SEMICOLON;
        else if (tokenTypeStr == "T_COMMA") type = T_COMMA;
        else if (tokenTypeStr == "T_ASSIGNOP") type = T_ASSIGNOP;
        else if (tokenTypeStr == "T_EQUALOP") type = T_EQUALOP;
        else if (tokenTypeStr == "T_LT") type = T_LT;
        else if (tokenTypeStr == "T_GT") type = T_GT;
        else if (tokenTypeStr == "T_LE") type = T_LE;
        else if (tokenTypeStr == "T_GE") type = T_GE;
        else if (tokenTypeStr == "T_NE") type = T_NE;
        else if (tokenTypeStr == "T_PLUS") type = T_PLUS;
        else if (tokenTypeStr == "T_MINUS") type = T_MINUS;
        else if (tokenTypeStr == "T_MULTIPLY") type = T_MULTIPLY;
        else if (tokenTypeStr == "T_DIVIDE") type = T_DIVIDE;
        else if (tokenTypeStr == "T_MODULO") type = T_MODULO;
        else if (tokenTypeStr == "T_AND") type = T_AND;
        else if (tokenTypeStr == "T_OR") type = T_OR;
        else if (tokenTypeStr == "T_NOT") type = T_NOT;
        else if (tokenTypeStr == "T_IF") type = T_IF;
        else if (tokenTypeStr == "T_ELSE") type = T_ELSE;
        else if (tokenTypeStr == "T_WHILE") type = T_WHILE;
        else if (tokenTypeStr == "T_RETURN") type = T_RETURN;
        else if (tokenTypeStr == "T_PRINT") type = T_PRINT;
        else if (tokenTypeStr == "T_BOOLLIT") type = T_BOOLLIT;
        else if (tokenTypeStr == "T_CHARLIT") type = T_CHARLIT;
        else continue; // Skip unknown tokens
        
        tokens.push_back({type, value, lineNum, column});
    }
    
    // Add EOF token
    tokens.push_back({T_EOF, "", 0, 0});
    return tokens;
}