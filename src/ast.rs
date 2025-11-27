use crate::token::TokenType;

// === AST Node Types ===

#[derive(Debug, Clone)]
pub struct IntLiteral {
    pub value: i64,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct FloatLiteral {
    pub value: f64,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct StringLiteral {
    pub value: String,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct CharLiteral {
    pub value: char,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct BoolLiteral {
    pub value: bool,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct Identifier {
    pub name: String,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct BinaryExpr {
    pub op: TokenType,
    pub left: Box<ASTNode>,
    pub right: Box<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct UnaryExpr {
    pub op: TokenType,
    pub operand: Box<ASTNode>,
    pub is_postfix: bool,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct CallExpr {
    pub callee: Box<ASTNode>,
    pub args: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct VarDecl {
    pub var_type: TokenType,
    pub name: String,
    pub initializer: Option<Box<ASTNode>>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct FunctionProto {
    pub return_type: TokenType,
    pub name: String,
    pub params: Vec<(TokenType, String)>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct FunctionDecl {
    pub return_type: TokenType,
    pub name: String,
    pub params: Vec<(TokenType, String)>,
    pub body: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct MainDecl {
    pub body: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct IfStmt {
    pub condition: Box<ASTNode>,
    pub if_body: Vec<ASTNode>,
    pub else_body: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct WhileStmt {
    pub condition: Box<ASTNode>,
    pub body: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct DoWhileStmt {
    pub body: Box<ASTNode>,
    pub condition: Box<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct ForStmt {
    pub init: Option<Box<ASTNode>>,
    pub condition: Option<Box<ASTNode>>,
    pub update: Option<Box<ASTNode>>,
    pub body: Box<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct CaseBlock {
    pub value: Box<ASTNode>,
    pub body: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct SwitchStmt {
    pub expression: Box<ASTNode>,
    pub cases: Vec<ASTNode>,
    pub default_body: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct ReturnStmt {
    pub value: Option<Box<ASTNode>>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct BreakStmt {
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct PrintStmt {
    pub args: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct BlockStmt {
    pub body: Vec<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct ExpressionStmt {
    pub expr: Box<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct IncludeStmt {
    pub header: String,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct EnumValueList {
    pub values: Vec<String>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub struct EnumDecl {
    pub name: String,
    pub values: Box<ASTNode>,
    pub line: usize,
    pub column: usize,
}

#[derive(Debug, Clone)]
pub enum ASTNode {
    IntLiteral(IntLiteral),
    FloatLiteral(FloatLiteral),
    StringLiteral(StringLiteral),
    CharLiteral(CharLiteral),
    BoolLiteral(BoolLiteral),
    Identifier(Identifier),
    BinaryExpr(BinaryExpr),
    UnaryExpr(UnaryExpr),
    CallExpr(CallExpr),
    VarDecl(VarDecl),
    FunctionProto(FunctionProto),
    FunctionDecl(FunctionDecl),
    MainDecl(MainDecl),
    IfStmt(IfStmt),
    WhileStmt(WhileStmt),
    DoWhileStmt(DoWhileStmt),
    ForStmt(ForStmt),
    CaseBlock(CaseBlock),
    SwitchStmt(SwitchStmt),
    ReturnStmt(ReturnStmt),
    BreakStmt(BreakStmt),
    PrintStmt(PrintStmt),
    BlockStmt(BlockStmt),
    ExpressionStmt(ExpressionStmt),
    IncludeStmt(IncludeStmt),
    EnumValueList(EnumValueList),
    EnumDecl(EnumDecl),
}
