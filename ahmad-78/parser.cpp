#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <initializer_list>
#include <stdexcept>
#include <map>
using namespace std;

#include "lexer.cpp"

// AST NODES
struct AstNode {
    virtual ~AstNode() = default;
    virtual string toString() const = 0;
};

struct FnDecl : AstNode {
    string returnType;
    string name;
    vector<pair<string, string>> params;
    vector<unique_ptr<AstNode>> body;
    
    string toString() const override {
        string result = "Fn(FnDecl { returnType: \"" + returnType + "\", name: \"" + name + "\", params: [";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) result += ", ";
            result += "(\"" + params[i].first + "\", \"" + params[i].second + "\")";
        }
        result += "], block: [";
        for (size_t i = 0; i < body.size(); ++i) {
            if (i > 0) result += ", ";
            result += body[i]->toString();
        }
        result += "] })";
        return result;
    }
};

struct VarDecl : AstNode {
    string type;
    string name;
    unique_ptr<AstNode> initializer;
    
    string toString() const override {
        string initStr = initializer ? initializer->toString() : "null";
        return "Var(VarDecl { type: \"" + type + "\", name: \"" + name + "\", expr: " + initStr + " })";
    }
};

struct AssignStmt : AstNode {
    string identifier;
    unique_ptr<AstNode> expr;
    
    string toString() const override {
        return "Assign(AssignStmt { identifier: \"" + identifier + "\", expr: " + 
               (expr ? expr->toString() : "null") + " })";
    }
};

struct ReturnStmt : AstNode {
    unique_ptr<AstNode> expr;
    
    string toString() const override {
        return "Ret(ExprStmt { expr: " + (expr ? expr->toString() : "null") + " })";
    }
};

struct IfStmt : AstNode {
    unique_ptr<AstNode> condition;
    vector<unique_ptr<AstNode>> thenBranch;
    vector<unique_ptr<AstNode>> elseBranch;
    
    string toString() const override {
        string thenStr = "[";
        for (size_t i = 0; i < thenBranch.size(); ++i) {
            if (i > 0) thenStr += ", ";
            thenStr += thenBranch[i]->toString();
        }
        thenStr += "]";
        
        string elseStr = "[";
        for (size_t i = 0; i < elseBranch.size(); ++i) {
            if (i > 0) elseStr += ", ";
            elseStr += elseBranch[i]->toString();
        }
        elseStr += "]";
        
        return "If(IfStmt { cond: " + (condition ? condition->toString() : "null") + 
               ", if_block: " + thenStr + ", else_block: " + elseStr + " })";
    }
};

struct ForStmt : AstNode {
    unique_ptr<AstNode> initializer;
    unique_ptr<AstNode> condition;
    unique_ptr<AstNode> increment;
    vector<unique_ptr<AstNode>> body;
    
    string toString() const override {
        string initStr = initializer ? initializer->toString() : "null";
        string condStr = condition ? condition->toString() : "null";
        string incrStr = increment ? increment->toString() : "null";
        
        string bodyStr = "[";
        for (size_t i = 0; i < body.size(); ++i) {
            if (i > 0) bodyStr += ", ";
            bodyStr += body[i]->toString();
        }
        bodyStr += "]";
        
        return "For(ForStmt { init: " + initStr + ", cond: " + condStr + 
               ", updt: " + incrStr + ", block: " + bodyStr + " })";
    }
};

struct BreakStmt : AstNode {
    string toString() const override {
        return "Break";
    }
};

struct ContinueStmt : AstNode {
    string toString() const override {
        return "Continue";
    }
};

struct WhileStmt : AstNode {
    unique_ptr<AstNode> condition;
    vector<unique_ptr<AstNode>> body;
    
    string toString() const override {
        string bodyStr = "[";
        for (size_t i = 0; i < body.size(); ++i) {
            if (i > 0) bodyStr += ", ";
            bodyStr += body[i]->toString();
        }
        bodyStr += "]";
        
        return "While(WhileStmt { cond: " + (condition ? condition->toString() : "null") + 
               ", block: " + bodyStr + " })";
    }
};

struct DoWhileStmt : AstNode {
    vector<unique_ptr<AstNode>> body;
    unique_ptr<AstNode> condition;
    
    string toString() const override {
        string bodyStr = "[";
        for (size_t i = 0; i < body.size(); ++i) {
            if (i > 0) bodyStr += ", ";
            bodyStr += body[i]->toString();
        }
        bodyStr += "]";
        
        return "DoWhile(DoWhileStmt { block: " + bodyStr + 
               ", cond: " + (condition ? condition->toString() : "null") + " })";
    }
};

struct SwitchStmt : AstNode {
    unique_ptr<AstNode> expr;
    vector<unique_ptr<AstNode>> cases;
    
    string toString() const override {
        string casesStr = "[";
        for (size_t i = 0; i < cases.size(); ++i) {
            if (i > 0) casesStr += ", ";
            casesStr += cases[i]->toString();
        }
        casesStr += "]";
        
        return "Switch(SwitchStmt { expr: " + (expr ? expr->toString() : "null") + 
               ", cases: " + casesStr + " })";
    }
};

struct CaseStmt : AstNode {
    unique_ptr<AstNode> value; // null for default case
    vector<unique_ptr<AstNode>> statements;
    
    string toString() const override {
        string stmtsStr = "[";
        for (size_t i = 0; i < statements.size(); ++i) {
            if (i > 0) stmtsStr += ", ";
            stmtsStr += statements[i]->toString();
        }
        stmtsStr += "]";
        
        if (value) {
            return "Case(CaseStmt { value: " + value->toString() + ", stmts: " + stmtsStr + " })";
        } else {
            return "Default(DefaultStmt { stmts: " + stmtsStr + " })";
        }
    }
};

struct ExprStmt : AstNode {
    unique_ptr<AstNode> expr;
    
    string toString() const override {
        return "ExprStmt { expr: " + (expr ? expr->toString() : "null") + " }";
    }
};

struct BinaryExpr : AstNode {
    unique_ptr<AstNode> left;
    string op;
    unique_ptr<AstNode> right;
    
    string toString() const override {
        string opName = "Unknown";
        if (op == "+") opName = "Add";
        else if (op == "-") opName = "Sub";
        else if (op == "*") opName = "Mul";
        else if (op == "/") opName = "Div";
        else if (op == "%") opName = "Mod";
        else if (op == "==") opName = "Comp(EqualsOp)";
        else if (op == "!=") opName = "Comp(NotEqualsOp)";
        else if (op == "<") opName = "Comp(LessThan)";
        else if (op == ">") opName = "Comp(GreaterThan)";
        else if (op == "<=") opName = "Comp(LessEqual)";
        else if (op == ">=") opName = "Comp(GreaterEqual)";
        else if (op == "&&") opName = "LogicalAnd";
        else if (op == "||") opName = "LogicalOr";
        else if (op == "&") opName = "BitwiseAnd";
        else if (op == "|") opName = "BitwiseOr";
        else if (op == "^") opName = "BitwiseXor";
        else if (op == "<<") opName = "LeftShift";
        else if (op == ">>") opName = "RightShift";
        else if (op == "=") opName = "Assign";
        
        return "::" + opName + "(" + op + ") " + (left ? left->toString() : "null") + " " + 
               (right ? right->toString() : "null");
    }
};

struct UnaryExpr : AstNode {
    string op;
    unique_ptr<AstNode> expr;
    bool isPostfix = false;  // to handle ++i vs i++
    
    string toString() const override {
        if (isPostfix) {
            return "Postfix(" + op + ") " + (expr ? expr->toString() : "null");
        } else {
            return "Unary(" + op + ") " + (expr ? expr->toString() : "null");
        }
    }
};

struct CallExpr : AstNode {
    string callee;
    vector<unique_ptr<AstNode>> arguments;
    
    string toString() const override {
        string argsStr = "[";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) argsStr += ", ";
            argsStr += arguments[i] ? arguments[i]->toString() : "null";
        }
        argsStr += "]";
        return "Call(FnCall { ident: \"" + callee + "\", args: " + argsStr + " })";
    }
};

struct LiteralExpr : AstNode {
    string type;
    string value;
    
    string toString() const override {
        return value;
    }
};

struct IdentifierExpr : AstNode {
    string name;
    
    string toString() const override {
        return "\"" + name + "\"";
    }
};

struct IncludeDirective : AstNode {
    string filename;
    bool isSystemInclude; // true for <>, false for ""
    
    string toString() const override {
        string includeType = isSystemInclude ? "SystemInclude" : "UserInclude";
        return "Include(" + includeType + " { file: \"" + filename + "\" })";
    }
};

struct DefineDirective : AstNode {
    string name;
    string value;
    vector<string> parameters; // for function-like macros
    bool isFunctionLike;
    
    string toString() const override {
        if (isFunctionLike) {
            string paramsStr = "[";
            for (size_t i = 0; i < parameters.size(); ++i) {
                if (i > 0) paramsStr += ", ";
                paramsStr += "\"" + parameters[i] + "\"";
            }
            paramsStr += "]";
            return "Define(FunctionMacro { name: \"" + name + "\", params: " + paramsStr + ", value: \"" + value + "\" })";
        } else {
            return "Define(ObjectMacro { name: \"" + name + "\", value: \"" + value + "\" })";
        }
    }
};

struct EnumDecl : AstNode {
    string name;
    vector<string> values;
    
    string toString() const override {
        string valuesStr = "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) valuesStr += ", ";
            valuesStr += "\"" + values[i] + "\"";
        }
        valuesStr += "]";
        return "Enum(EnumDecl { name: \"" + name + "\", values: " + valuesStr + " })";
    }
};

// PARSER CLASS

enum class ParseError {
    // General errors
    UnexpectedEOF,
    UnexpectedToken,
    LexerError,
    
    // Declaration errors
    ExpectedTypeToken,
    ExpectedIdentifier,
    ExpectedFunctionName,
    ExpectedParameterName,
    ExpectedVariableName,
    
    // Statement errors
    ExpectedSemicolon,
    ExpectedLeftParen,
    ExpectedRightParen,
    ExpectedLeftBrace,
    ExpectedRightBrace,
    ExpectedColon,
    ExpectedComma,
    
    // Expression errors
    ExpectedExpr,
    ExpectedPrimaryExpr,
    ExpectedAssignmentOp,
    
    // Literal errors
    ExpectedFloatLit,
    ExpectedIntLit,
    ExpectedStringLit,
    ExpectedBoolLit,
    ExpectedCharLit,
    
    // Control flow errors
    ExpectedCondition,
    ExpectedWhileKeyword,
    ExpectedCaseValue,
    ExpectedReturnValue,
    
    // Include/Define errors
    ExpectedIncludeFile,
    ExpectedMacroName,
    ExpectedAngleBracket,
    
    // Deprecated / misc
    FailedToFindToken,
    
    // Added
    InternalError,
    ExpectedAssignmentOperator,
};

class Parser {
private:
    LexerState lexerState;
    Token currentToken;
    Token previousToken;
    bool hasCurrentToken = false;
    bool hasPreviousToken = false;
    Token lastNonCommentToken; // track last significant token for better EOF reporting

public:
    Parser(LexerState state) : lexerState(state) {
        // Get the first token
        advance();
    }

    vector<unique_ptr<AstNode>> parse();

private:
    unique_ptr<AstNode> declaration();
    unique_ptr<AstNode> varDecl();
    unique_ptr<AstNode> varDeclWithType(const string& type);
    unique_ptr<AstNode> fnDecl();
    unique_ptr<AstNode> fnDeclWithType(const string& type);
    unique_ptr<AstNode> includeDirective();
    unique_ptr<AstNode> defineDirective();
    unique_ptr<AstNode> enumDecl();
    unique_ptr<AstNode> statement();
    unique_ptr<AstNode> exprStmt();
    unique_ptr<AstNode> assignStmt();
    unique_ptr<AstNode> ifStmt();
    unique_ptr<AstNode> whileStmt();
    unique_ptr<AstNode> doWhileStmt();
    unique_ptr<AstNode> forStmt();
    unique_ptr<AstNode> switchStmt();
    unique_ptr<AstNode> returnStmt();
    unique_ptr<AstNode> breakStmt();
    unique_ptr<AstNode> continueStmt();

    unique_ptr<AstNode> expression();    
    unique_ptr<AstNode> assignment();
    unique_ptr<AstNode> logicalOr();
    unique_ptr<AstNode> logicalAnd();
    unique_ptr<AstNode> bitwiseOr();
    unique_ptr<AstNode> bitwiseXor();
    unique_ptr<AstNode> bitwiseAnd();
    unique_ptr<AstNode> equality();
    unique_ptr<AstNode> comparison();
    unique_ptr<AstNode> shift();
    unique_ptr<AstNode> term();
    unique_ptr<AstNode> factor();
    unique_ptr<AstNode> unary();
    unique_ptr<AstNode> postfix();
    unique_ptr<AstNode> call();
    unique_ptr<AstNode> primary();

    // Helper methods
    bool match(initializer_list<TokenType> types);
    Token consume(TokenType type, string message);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    
    void error(ParseError err, const string& message);
    [[noreturn]] void errorAt(ParseError err, const Token& tok, const string& message);
    string errorTypeName(ParseError err) const;
};

// PARSER IMPLEMENTATION

vector<unique_ptr<AstNode>> Parser::parse() {
    vector<unique_ptr<AstNode>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

unique_ptr<AstNode> Parser::declaration() {
    // parse() loop already ensures we are not at end; we avoid redundant EOF error here.
    
    // Check for preprocessor directives first
    if (check(T_INCLUDE)) {
        return includeDirective();
    } else if (check(T_DEFINE)) {
        return defineDirective();
    }
    
    // Check for enum declaration
    if (check(T_ENUM)) {
        return enumDecl();
    }
    
    // Check for type qualifiers and base types
    if (check(T_CONST) || check(T_INT) || check(T_FLOAT) || 
        check(T_DOUBLE) || check(T_CHAR) || check(T_BOOL) || check(T_VOID)) {
        
        // Parse type specifier (optional const + base type)
        string fullType = "";
        
        // Check for const qualifier 
        if (check(T_CONST)) {
            fullType += advance().value;
        }
        
        // Parse base type
        if (check(T_INT) || check(T_FLOAT) || check(T_DOUBLE) || 
            check(T_CHAR) || check(T_BOOL) || check(T_VOID)) {
            if (!fullType.empty()) fullType += " ";
            fullType += advance().value;
        } else if (fullType.empty()) {
            error(ParseError::ExpectedTypeToken, "Expected base type after 'const' or at declaration start");
        }
        
        // Determine function vs variable
        if (check(T_IDENTIFIER)) {
            LexerState savedState = lexerState;
            Token savedCurrent = currentToken;
            Token savedPrevious = previousToken;
            bool savedHasCurrent = hasCurrentToken;
            bool savedHasPrevious = hasPreviousToken;
            
            advance(); // consume identifier
            bool isFunction = check(T_LPAREN);
            
            // Restore
            lexerState = savedState;
            currentToken = savedCurrent;
            previousToken = savedPrevious;
            hasCurrentToken = savedHasCurrent;
            hasPreviousToken = savedHasPrevious;
            
            if (isFunction) {
                return fnDeclWithType(fullType);
            } else {
                return varDeclWithType(fullType);
            }
        } else {
            error(ParseError::ExpectedIdentifier, "Expected identifier after type specifier in declaration");
            return nullptr;
        }
    } else {
        return statement();
    }
}

unique_ptr<AstNode> Parser::varDecl() {
    string type = previous().value;  // GET TYPE FROM PREVIOUS TOKEN
    Token name = consume(T_IDENTIFIER, "Expected variable name");
    
    auto varDecl = make_unique<VarDecl>();
    varDecl->type = type;
    varDecl->name = name.value;
    
    if (match({T_ASSIGNOP})) {
        varDecl->initializer = expression();
    }
    
    consume(T_SEMICOLON, "Expected ';' after variable declaration");
    return varDecl;
}

unique_ptr<AstNode> Parser::varDeclWithType(const string& type) {
    Token name = consume(T_IDENTIFIER, "Expected variable name");
    
    auto varDecl = make_unique<VarDecl>();
    varDecl->type = type;
    varDecl->name = name.value;
    
    if (match({T_ASSIGNOP})) {
        varDecl->initializer = expression();
    }
    
    consume(T_SEMICOLON, "Expected ';' after variable declaration");
    return varDecl;
}

unique_ptr<AstNode> Parser::fnDecl() {
    string returnType = previous().value;  // GET TYPE FROM PREVIOUS TOKEN
    Token name = consume(T_IDENTIFIER, "Expected function name");
    
    auto fnDecl = make_unique<FnDecl>();
    fnDecl->returnType = returnType;
    fnDecl->name = name.value;
    
    consume(T_LPAREN, "Expected '(' after function name");
    
    // Parse parameters
    if (!check(T_RPAREN)) {
        do {
            // Parse parameter type (optional const + base type)
            string paramType = "";
            
            // Check for const qualifier 
            if (check(T_CONST)) {
                paramType += advance().value;
            }
            
            // Parse base type
            if (check(T_INT) || check(T_FLOAT) || check(T_DOUBLE) || 
                check(T_CHAR) || check(T_BOOL) || check(T_VOID)) {
                if (!paramType.empty()) paramType += " ";
                paramType += advance().value;
            } else if (paramType.empty()) {
                error(ParseError::ExpectedTypeToken, "Expected parameter type");
            }
            
            // Parse parameter name
            Token paramName = consume(T_IDENTIFIER, "Expected parameter name");
            
            fnDecl->params.push_back(make_pair(paramType, paramName.value));
            
        } while (match({T_COMMA}));
    }
    
    consume(T_RPAREN, "Expected ')' after parameters");
    consume(T_LBRACE, "Expected '{' before function body");
    
    // Parse function body
    while (!check(T_RBRACE) && !isAtEnd()) {
        fnDecl->body.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after function body");
    return fnDecl;
}

unique_ptr<AstNode> Parser::includeDirective() {
    consume(T_INCLUDE, "Expected '#include'");
    
    auto include = make_unique<IncludeDirective>();
    
    // Check if it's a string literal (user include) or angle brackets (system include)
    if (check(T_STRINGLIT)) {
        Token filename = advance();
        include->filename = filename.value.substr(1, filename.value.length() - 2); // Remove quotes
        include->isSystemInclude = false;
    } else if (check(T_LT)) {
        consume(T_LT, "Expected '<'");
        
        // Collect everything until '>' or error on newline/EOF
        string filename = "";
        int startLine = peek().line; // line after '<'
        while (!check(T_GT) && !isAtEnd()) {
            // If line advanced without finding '>', treat as missing angle bracket
            if (peek().line != startLine && !check(T_GT)) {
                error(ParseError::ExpectedAngleBracket, "Missing closing '>' in #include directive");
            }
            filename += advance().value;
        }
        if (isAtEnd()) {
            error(ParseError::ExpectedAngleBracket, "Unexpected EOF while scanning #include <...>");
        }
        consume(T_GT, "Expected '>' after include filename");
        include->filename = filename;
        include->isSystemInclude = true;
    } else {
        error(ParseError::ExpectedIncludeFile, "Expected string literal \"file.h\" or <system_file.h> after #include");
    }
    
    return include;
}

unique_ptr<AstNode> Parser::defineDirective() {
    consume(T_DEFINE, "Expected '#define'");
    
    auto define = make_unique<DefineDirective>();
    define->isFunctionLike = false;
    
    // Get the macro name
    Token name = consume(T_IDENTIFIER, "Expected macro name after #define");
    define->name = name.value;
    
    // Check if it's a function-like macro (has parameters)
    if (check(T_LPAREN)) {
        define->isFunctionLike = true;
        consume(T_LPAREN, "Expected '(' for function-like macro");
        
        // Parse parameters
        if (!check(T_RPAREN)) {
            do {
                Token param = consume(T_IDENTIFIER, "Expected parameter name");
                define->parameters.push_back(param.value);
            } while (match({T_COMMA}));
        }
        
        consume(T_RPAREN, "Expected ')' after macro parameters");
    }
    
    // Collect the rest of the line as the macro value
    string value = "";
    while (!isAtEnd() && peek().line == name.line) {
        if (!value.empty()) value += " ";
        value += advance().value;
    }
    
    define->value = value;
    return define;
}

unique_ptr<AstNode> Parser::enumDecl() {
    consume(T_ENUM, "Expected 'enum'");
    
    auto enumDecl = make_unique<EnumDecl>();
    
    // Get the enum name
    Token name = consume(T_IDENTIFIER, "Expected enum name");
    enumDecl->name = name.value;
    
    // Parse the enum body
    consume(T_LBRACE, "Expected '{' after enum name");
    
    // Parse enum values
    if (!check(T_RBRACE)) {
        do {
            Token value = consume(T_IDENTIFIER, "Expected enum value");
            enumDecl->values.push_back(value.value);
        } while (match({T_COMMA}));
    }
    
    consume(T_RBRACE, "Expected '}' after enum values");
    consume(T_SEMICOLON, "Expected ';' after enum declaration");
    
    return enumDecl;
}

unique_ptr<AstNode> Parser::fnDeclWithType(const string& returnType) {
    Token name = consume(T_IDENTIFIER, "Expected function name");
    
    auto fnDecl = make_unique<FnDecl>();
    fnDecl->returnType = returnType;
    fnDecl->name = name.value;
    
    consume(T_LPAREN, "Expected '(' after function name");
    
    // Parse parameters
    if (!check(T_RPAREN)) {
        do {
            // Parse parameter type (optional const + base type)
            string paramType = "";
            
            // Check for const qualifier 
            if (check(T_CONST)) {
                paramType += advance().value;
            }
            
            // Parse base type
            if (check(T_INT) || check(T_FLOAT) || check(T_DOUBLE) || 
                check(T_CHAR) || check(T_BOOL) || check(T_VOID)) {
                if (!paramType.empty()) paramType += " ";
                paramType += advance().value;
            } else if (paramType.empty()) {
                error(ParseError::ExpectedTypeToken, "Expected parameter type");
            }
            
            // Parse parameter name
            Token paramName = consume(T_IDENTIFIER, "Expected parameter name");
            
            fnDecl->params.push_back(make_pair(paramType, paramName.value));
            
        } while (match({T_COMMA}));
    }
    
    consume(T_RPAREN, "Expected ')' after parameters");
    consume(T_LBRACE, "Expected '{' before function body");
    
    // Parse function body
    while (!check(T_RBRACE) && !isAtEnd()) {
        fnDecl->body.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after function body");
    return fnDecl;
}

unique_ptr<AstNode> Parser::statement() {
    if (check(T_RETURN)) {
        return returnStmt();
    } else if (check(T_IF)) {
        return ifStmt();
    } else if (check(T_WHILE)) {
        return whileStmt();
    } else if (check(T_DO)) {
        return doWhileStmt();
    } else if (check(T_FOR)) {
        return forStmt();
    } else if (check(T_SWITCH)) {
        return switchStmt();
    } else if (check(T_BREAK)) {
        return breakStmt();
    } else if (check(T_CONTINUE)) {
        return continueStmt();
    } else if (check(T_IDENTIFIER)) {
        // Check if it's an assignment by looking ahead
        LexerState savedState = lexerState;
        Token savedCurrent = currentToken;
        Token savedPrevious = previousToken;
        bool savedHasCurrent = hasCurrentToken;
        bool savedHasPrevious = hasPreviousToken;
        
        advance(); // consume identifier
        bool isAssignment = check(T_ASSIGNOP);
        
        // Restore lexer state
        lexerState = savedState;
        currentToken = savedCurrent;
        previousToken = savedPrevious;
        hasCurrentToken = savedHasCurrent;
        hasPreviousToken = savedHasPrevious;
        
        if (isAssignment) {
            return assignStmt();
        } else {
            return exprStmt();
        }
    } else {
        return exprStmt();
    }
}

unique_ptr<AstNode> Parser::exprStmt() {
    auto expr = expression();
    consume(T_SEMICOLON, "Expected ';' after expression");
    
    auto exprStmt = make_unique<ExprStmt>();
    exprStmt->expr = move(expr);
    return exprStmt;
}

unique_ptr<AstNode> Parser::assignStmt() {
    Token name = consume(T_IDENTIFIER, "Expected identifier");
    consume(T_ASSIGNOP, "Expected '='");
    auto expr = expression();
    consume(T_SEMICOLON, "Expected ';' after assignment");
    
    auto assignStmt = make_unique<AssignStmt>();
    assignStmt->identifier = name.value;
    assignStmt->expr = move(expr);
    return assignStmt;
}

unique_ptr<AstNode> Parser::ifStmt() {
    consume(T_IF, "Expected 'if'");
    consume(T_LPAREN, "Expected '(' after 'if'");
    
    auto condition = expression();
    consume(T_RPAREN, "Expected ')' after condition");
    
    consume(T_LBRACE, "Expected '{' before if body");
    
    auto ifStmt = make_unique<IfStmt>();
    ifStmt->condition = move(condition);
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        ifStmt->thenBranch.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after if body");
    
    if (match({T_ELSE})) {
        consume(T_LBRACE, "Expected '{' before else body");
        
        while (!check(T_RBRACE) && !isAtEnd()) {
            ifStmt->elseBranch.push_back(declaration());
        }
        
        consume(T_RBRACE, "Expected '}' after else body");
    }
    
    return ifStmt;
}

unique_ptr<AstNode> Parser::forStmt() {
    consume(T_FOR, "Expected 'for'");
    consume(T_LPAREN, "Expected '(' after 'for'");
    
    auto forStmt = make_unique<ForStmt>();
    
    // Initializer
    if (check(T_CONST) || check(T_STATIC) || check(T_SIGNED) || check(T_UNSIGNED) || 
        check(T_SHORT) || check(T_LONG) || check(T_INT) || check(T_FLOAT) || 
        check(T_DOUBLE) || check(T_CHAR) || check(T_BOOL)) {
        // Parse type for variable declaration in for loop
        string type = "";
        while (check(T_CONST) || check(T_STATIC) || check(T_SIGNED) || check(T_UNSIGNED) || 
               check(T_SHORT) || check(T_LONG) || check(T_INT) || check(T_FLOAT) || 
               check(T_DOUBLE) || check(T_CHAR) || check(T_BOOL)) {
            if (!type.empty()) type += " ";
            type += advance().value;
        }
        forStmt->initializer = varDeclWithType(type);
    } else if (!check(T_SEMICOLON)) {
        forStmt->initializer = exprStmt();
    } else {
        advance(); // consume ';'
    }
    
    // Condition
    if (!check(T_SEMICOLON)) {
        forStmt->condition = expression();
    }
    consume(T_SEMICOLON, "Expected ';' in for loop");
    
    // Increment
    if (!check(T_RPAREN)) {
        forStmt->increment = expression();  // CHANGED: parse as expression instead of special cases
    }
    
    consume(T_RPAREN, "Expected ')' after for clauses");
    consume(T_LBRACE, "Expected '{' before for body");
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        forStmt->body.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after for body");
    
    return forStmt;
}

unique_ptr<AstNode> Parser::returnStmt() {
    consume(T_RETURN, "Expected 'return'");
    
    auto returnStmt = make_unique<ReturnStmt>();
    
    if (!check(T_SEMICOLON)) {
        returnStmt->expr = expression();
    }
    
    consume(T_SEMICOLON, "Expected ';' after return value");
    return returnStmt;
}

unique_ptr<AstNode> Parser::breakStmt() {
    consume(T_BREAK, "Expected 'break'");
    consume(T_SEMICOLON, "Expected ';' after break");
    return make_unique<BreakStmt>();
}

unique_ptr<AstNode> Parser::continueStmt() {
    consume(T_CONTINUE, "Expected 'continue'");
    consume(T_SEMICOLON, "Expected ';' after continue");
    return make_unique<ContinueStmt>();
}

unique_ptr<AstNode> Parser::whileStmt() {
    consume(T_WHILE, "Expected 'while'");
    consume(T_LPAREN, "Expected '(' after 'while'");
    
    auto condition = expression();
    consume(T_RPAREN, "Expected ')' after condition");
    
    consume(T_LBRACE, "Expected '{' before while body");
    
    auto whileStmt = make_unique<WhileStmt>();
    whileStmt->condition = move(condition);
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        whileStmt->body.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after while body");
    return whileStmt;
}

unique_ptr<AstNode> Parser::doWhileStmt() {
    consume(T_DO, "Expected 'do'");
    consume(T_LBRACE, "Expected '{' after 'do'");
    
    auto doWhileStmt = make_unique<DoWhileStmt>();
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        doWhileStmt->body.push_back(declaration());
    }
    
    consume(T_RBRACE, "Expected '}' after do body");
    consume(T_WHILE, "Expected 'while' after do body");
    consume(T_LPAREN, "Expected '(' after 'while'");
    
    doWhileStmt->condition = expression();
    consume(T_RPAREN, "Expected ')' after condition");
    consume(T_SEMICOLON, "Expected ';' after do-while");
    
    return doWhileStmt;
}

unique_ptr<AstNode> Parser::switchStmt() {
    consume(T_SWITCH, "Expected 'switch'");
    consume(T_LPAREN, "Expected '(' after 'switch'");
    
    auto expr = expression();
    consume(T_RPAREN, "Expected ')' after switch expression");
    consume(T_LBRACE, "Expected '{' before switch body");
    
    auto switchStmt = make_unique<SwitchStmt>();
    switchStmt->expr = move(expr);
    
    while (!check(T_RBRACE) && !isAtEnd()) {
        if (check(T_CASE)) {
            consume(T_CASE, "Expected 'case'");
            auto caseValue = expression();
            consume(T_COLON, "Expected ':' after case value");
            
            auto caseStmt = make_unique<CaseStmt>();
            caseStmt->value = move(caseValue);
            
            while (!check(T_CASE) && !check(T_DEFAULT) && !check(T_RBRACE) && !isAtEnd()) {
                caseStmt->statements.push_back(declaration());
            }
            
            switchStmt->cases.push_back(move(caseStmt));
        } else if (check(T_DEFAULT)) {
            consume(T_DEFAULT, "Expected 'default'");
            consume(T_COLON, "Expected ':' after 'default'");
            
            auto defaultStmt = make_unique<CaseStmt>();
            defaultStmt->value = nullptr; // null indicates default case
            
            while (!check(T_CASE) && !check(T_DEFAULT) && !check(T_RBRACE) && !isAtEnd()) {
                defaultStmt->statements.push_back(declaration());
            }
            
            switchStmt->cases.push_back(move(defaultStmt));
        } else {
            error(ParseError::ExpectedCaseValue, "Expected 'case' or 'default' in switch body, found unexpected token");
        }
    }
    
    consume(T_RBRACE, "Expected '}' after switch body");
    return switchStmt;
}

// Expression parsing with precedence
unique_ptr<AstNode> Parser::expression() {
    return assignment();
}

unique_ptr<AstNode> Parser::assignment() {
    auto expr = logicalOr();
    
    if (match({T_ASSIGNOP})) {
        string op = previous().value;
        auto right = assignment();
        auto assign = make_unique<BinaryExpr>();
        assign->left = move(expr);
        assign->op = op;
        assign->right = move(right);
        return assign;
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match({T_OR})) {
        string op = previous().value;
        auto right = logicalAnd();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::logicalAnd() {
    auto expr = bitwiseOr();
    
    while (match({T_AND})) {
        string op = previous().value;
        auto right = bitwiseOr();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::bitwiseOr() {
    auto expr = bitwiseXor();
    
    while (match({T_BITWISE_OR})) {
        string op = previous().value;
        auto right = bitwiseXor();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::bitwiseXor() {
    auto expr = bitwiseAnd();
    
    while (match({T_BITWISE_XOR})) {
        string op = previous().value;
        auto right = bitwiseAnd();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::bitwiseAnd() {
    auto expr = equality();
    
    while (match({T_BITWISE_AND})) {
        string op = previous().value;
        auto right = equality();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::equality() {
    auto expr = comparison();
    
    while (match({T_EQUALOP, T_NE})) {
        string op = previous().value;
        auto right = comparison();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::comparison() {
    auto expr = shift();
    
    while (match({T_LT, T_GT, T_LE, T_GE})) {
        string op = previous().value;
        auto right = shift();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::shift() {
    auto expr = term();
    
    while (match({T_LEFT_SHIFT, T_RIGHT_SHIFT})) {
        string op = previous().value;
        auto right = term();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::term() {
    auto expr = factor();
    
    while (match({T_PLUS, T_MINUS})) {
        string op = previous().value;
        auto right = factor();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::factor() {
    auto expr = unary();
    
    while (match({T_MULTIPLY, T_DIVIDE, T_MODULO})) {
        string op = previous().value;
        auto right = unary();
        auto binary = make_unique<BinaryExpr>();
        binary->left = move(expr);
        binary->op = op;
        binary->right = move(right);
        expr = move(binary);
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::unary() {
    if (match({T_MINUS, T_PLUS, T_NOT, T_BITWISE_NOT})) {
        string op = previous().value;
        auto expr = unary();
        auto unary = make_unique<UnaryExpr>();
        unary->op = op;
        unary->expr = move(expr);
        return unary;
    }
    
    return postfix();
}

unique_ptr<AstNode> Parser::postfix() {
    auto expr = call();
    
    // Handle postfix increment/decrement
    if (match({T_INCREMENT, T_DECREMENT})) {
        string op = previous().value;
        auto unary = make_unique<UnaryExpr>();
        unary->op = op;
        unary->expr = move(expr);
        unary->isPostfix = true;
        return unary;
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::call() {
    auto expr = primary();
    
    if (match({T_LPAREN})) {
        auto call = make_unique<CallExpr>();
        
        if (auto identExpr = dynamic_cast<IdentifierExpr*>(expr.get())) {
            call->callee = identExpr->name;
        }
        
        if (!check(T_RPAREN)) {
            do {
                call->arguments.push_back(expression());
            } while (match({T_COMMA}));
        }
        
        consume(T_RPAREN, "Expected ')' after arguments");
        return call;
    }
    
    return expr;
}

unique_ptr<AstNode> Parser::primary() {
    if (match({T_INTLIT, T_FLOATLIT, T_BOOLLIT, T_STRINGLIT, T_CHARLIT})) {
        auto literal = make_unique<LiteralExpr>();
        literal->type = tokenTypeToString(previous().type);
        literal->value = previous().value;
        return literal;
    }
    
    if (match({T_IDENTIFIER})) {
        auto ident = make_unique<IdentifierExpr>();
        ident->name = previous().value;
        return ident;
    }
    
    if (match({T_LPAREN})) {
        auto expr = expression();
        consume(T_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    // Handle prefix increment/decrement
    if (match({T_INCREMENT, T_DECREMENT})) {
        string op = previous().value;
        auto expr = unary();
        auto unary = make_unique<UnaryExpr>();
        unary->op = op;
        unary->expr = move(expr);
        unary->isPostfix = false;
        return unary;
    }
    
    error(ParseError::ExpectedPrimaryExpr, "Expected primary expression (identifier, literal, or parenthesized expression)");
    return nullptr;
}

// Helper methods
bool Parser::match(initializer_list<TokenType> types) {
    for (const TokenType& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, string message) {
    if (check(type)) return advance();

    ParseError errorType = ParseError::UnexpectedToken;
    if (type == T_SEMICOLON) errorType = ParseError::ExpectedSemicolon;
    else if (type == T_LPAREN) errorType = ParseError::ExpectedLeftParen;
    else if (type == T_RPAREN) errorType = ParseError::ExpectedRightParen;
    else if (type == T_LBRACE) errorType = ParseError::ExpectedLeftBrace;
    else if (type == T_RBRACE) errorType = ParseError::ExpectedRightBrace;
    else if (type == T_COLON) errorType = ParseError::ExpectedColon;
    else if (type == T_COMMA) errorType = ParseError::ExpectedComma;
    else if (type == T_IDENTIFIER) errorType = ParseError::ExpectedIdentifier;
    else if (type == T_GT) errorType = ParseError::ExpectedAngleBracket;

    Token found = hasCurrentToken ? currentToken : previous();
    Token locationTok = found;
    if (errorType == ParseError::ExpectedSemicolon && hasPreviousToken) {
        locationTok = previousToken;
    }

    string foundStr = string(tokenTypeToString(found.type)) + "('" + found.value + "')";
    if (!message.empty()) message += "; ";
    message += "found " + foundStr;

    if (errorType == ParseError::ExpectedSemicolon) {
        string name = errorTypeName(errorType);
        cout << "Error[" << name << "]: " << message << " at " << locationTok.line << ", " << locationTok.column << endl;
        throw runtime_error(name + ": " + message);
    }

    error(errorType, message);
    return Token();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (hasCurrentToken) {
        previousToken = currentToken;
        hasPreviousToken = true;
    }
    
    if (getNextToken(lexerState, currentToken)) {
        if (currentToken.type == T_ERROR) {
            error(ParseError::LexerError, "Lexer error: " + currentToken.value);
        }
        while (currentToken.type == T_SINGLE_COMMENT || currentToken.type == T_MULTI_COMMENT) {
            if (!getNextToken(lexerState, currentToken)) {
                hasCurrentToken = false;
                return previous();
            }
            if (currentToken.type == T_ERROR) {
                error(ParseError::LexerError, "Lexer error: " + currentToken.value);
            }
        }
        hasCurrentToken = true;
        lastNonCommentToken = currentToken;
    } else {
        hasCurrentToken = false;
    }
    
    return previous();
}

bool Parser::isAtEnd() {
    // End when we failed to fetch further tokens.
    return !hasCurrentToken;
}

Token Parser::peek() {
    if (hasCurrentToken) {
        return currentToken;
    }
    // Return a dummy EOF-like token
    Token eofToken;
    eofToken.type = T_ERROR;
    eofToken.value = "";
    eofToken.line = lexerState.line;
    eofToken.column = lexerState.column;
    return eofToken;
}

Token Parser::previous() {
    if (hasPreviousToken) {
        return previousToken;
    }
    // Return a dummy token
    Token dummy;
    dummy.type = T_ERROR;
    dummy.value = "";
    dummy.line = 1;
    dummy.column = 1;
    return dummy;
}

void Parser::error(ParseError err, const string& message) {
    Token tok;
    if (hasCurrentToken) tok = currentToken;
    else if (hasPreviousToken) tok = previousToken;
    else tok = lastNonCommentToken; // fallback
    errorAt(err, tok, message);
}

void Parser::errorAt(ParseError err, const Token& tok, const string& message) {
    string name = errorTypeName(err);
    cout << "Error[" << name << "]: " << message << " at " << tok.line << ", " << tok.column << endl;
    throw runtime_error(name + ": " + message);
}

string Parser::errorTypeName(ParseError err) const {
    switch (err) {
        case ParseError::UnexpectedEOF: return "UnexpectedEOF";
        case ParseError::UnexpectedToken: return "UnexpectedToken";
        case ParseError::LexerError: return "LexerError";
        case ParseError::ExpectedTypeToken: return "ExpectedType";
        case ParseError::ExpectedIdentifier: return "ExpectedIdentifier";
        case ParseError::ExpectedSemicolon: return "ExpectedSemicolon";
        case ParseError::ExpectedLeftParen: return "ExpectedLeftParen";
        case ParseError::ExpectedRightParen: return "ExpectedRightParen";
        case ParseError::ExpectedLeftBrace: return "ExpectedLeftBrace";
        case ParseError::ExpectedRightBrace: return "ExpectedRightBrace";
        case ParseError::ExpectedColon: return "ExpectedColon";
        case ParseError::ExpectedComma: return "ExpectedComma";
        case ParseError::ExpectedExpr: return "ExpectedExpression";
        case ParseError::ExpectedPrimaryExpr: return "ExpectedPrimaryExpr";
        case ParseError::ExpectedAssignmentOp: return "ExpectedAssignmentOp";
        case ParseError::ExpectedFloatLit: return "ExpectedFloatLiteral";
        case ParseError::ExpectedIntLit: return "ExpectedIntLiteral";
        case ParseError::ExpectedStringLit: return "ExpectedStringLiteral";
        case ParseError::ExpectedBoolLit: return "ExpectedBoolLiteral";
        case ParseError::ExpectedCharLit: return "ExpectedCharLiteral";
        case ParseError::ExpectedCondition: return "ExpectedCondition";
        case ParseError::ExpectedWhileKeyword: return "ExpectedWhile";
        case ParseError::ExpectedCaseValue: return "ExpectedCase";
        case ParseError::ExpectedReturnValue: return "ExpectedReturnValue";
        case ParseError::ExpectedIncludeFile: return "ExpectedIncludeFile";
        case ParseError::ExpectedMacroName: return "ExpectedMacroName";
        case ParseError::ExpectedAngleBracket: return "ExpectedAngleBracket";
        case ParseError::FailedToFindToken: return "FailedToFindToken";
        case ParseError::InternalError: return "InternalError";
        case ParseError::ExpectedAssignmentOperator: return "ExpectedAssignmentOperator";
        default: return "ParseError";
    }
}

// TEST DRIVER 

void runTest(const string& testName, const string& code, bool printSource, bool printTokens) {
    cout << "=== " << testName << " ===" << endl;

    if (printSource) {
        cout << "Source code:" << endl << code << endl;
    }

    try {
        // Create lexer state for tokenization display
        LexerState tokenDisplayState = createLexerState(code.c_str());
        vector<Token> tokens; // Just for display purposes
        Token token;
        bool lexerError = false;
        while (getNextToken(tokenDisplayState, token)) {
            if (token.type == T_ERROR) {
                cout << "Error[LexerError]: Lexer error: " << token.value << " at " << token.line << ", " << token.column << endl;
                lexerError = true;
                break; // Stop processing on first lexer error
            }
            if (token.type != T_SINGLE_COMMENT && token.type != T_MULTI_COMMENT) {
                tokens.push_back(token);
            }
        }
        
        if (lexerError) {
            cout << "Parsing terminated due to lexer error." << endl << endl;
            return;
        }
        
        if (printTokens) {
            cout << endl << "Tokens: ";
            for (const auto& tok : tokens) {
                cout << tokenTypeToString(tok.type) << "('" << tok.value << "') ";
            }
            cout << endl << endl;
        }

        LexerState parsingState = createLexerState(code.c_str());
        Parser parser(parsingState);
        vector<unique_ptr<AstNode>> ast = parser.parse();
        
        cout << "AST Output:" << endl << "[" << endl;
        for (size_t i = 0; i < ast.size(); ++i) {
            if (i > 0) cout << "," << endl;
            cout << "    " << ast[i]->toString();
        }
        cout << endl << "]" << endl;        
    } catch (const exception& e) {
        // Suppress duplicate because errorAt already printed standardized line.
        // Optionally could enable a debug flag to reprint.
    }
}

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("Could not open file");

    stringstream buffer;
    buffer << file.rdbuf();   // read whole file into buffer
    return buffer.str();      // return as string
}

int main() {
    // Test with clean code first
    string cleanCode = readFile("test_code.txt");
    bool printSource = false;
    bool printTokens = false;
    runTest("Testing", cleanCode, printSource, printTokens);

    return 0;
}