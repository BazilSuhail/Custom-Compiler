use std::fmt;

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum TokenType {
    // Types
    Int, Float, Double, Char, Void, Bool, Enum,

    // Literals
    Identifier, IntLit, FloatLit, StringLit, CharLit, BoolLit,

    // Brackets & Separators
    LParen, RParen, LBrace, RBrace, LBracket, RBracket,
    Semicolon, Comma, Dot,

    // Operators
    AssignOp, EqualOp, Ne, Lt, Gt, Le, Ge,
    Plus, Minus, Multiply, Divide, Modulo,
    Increment, Decrement,
    And, Or, Not,

    // Bitwise operators
    BitAnd, BitOr, BitXor, BitLShift, BitRShift,

    // Keywords
    If, Else, While, Return, Print, Main, Include, Const, Global,

    // New keywords
    String, Do, Switch, Break, For, Default, Case, Colon,

    // Comments
    SingleComment, MultiComment,

    // Error & EOF
    Error, Eof,
}

#[derive(Debug, Clone, PartialEq)]
pub enum TypeNode {
    Builtin(TokenType),
    UserDefined(String),
}

impl fmt::Display for TokenType {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let s = match self {
            TokenType::Int => "T_INT",
            TokenType::Float => "T_FLOAT",
            TokenType::Double => "T_DOUBLE",
            TokenType::Char => "T_CHAR",
            TokenType::Void => "T_VOID",
            TokenType::Bool => "T_BOOL",
            TokenType::Enum => "T_ENUM",
            TokenType::Identifier => "T_IDENTIFIER",
            TokenType::IntLit => "T_INTLIT",
            TokenType::FloatLit => "T_FLOATLIT",
            TokenType::StringLit => "T_STRINGLIT",
            TokenType::CharLit => "T_CHARLIT",
            TokenType::BoolLit => "T_BOOLLIT",
            TokenType::LParen => "T_LPAREN",
            TokenType::RParen => "T_RPAREN",
            TokenType::LBrace => "T_LBRACE",
            TokenType::RBrace => "T_RBRACE",
            TokenType::LBracket => "T_LBRACKET",
            TokenType::RBracket => "T_RBRACKET",
            TokenType::Semicolon => "T_SEMICOLON",
            TokenType::Comma => "T_COMMA",
            TokenType::Dot => "T_DOT",
            TokenType::AssignOp => "T_ASSIGNOP",
            TokenType::EqualOp => "T_EQUALOP",
            TokenType::Ne => "T_NE",
            TokenType::Lt => "T_LT",
            TokenType::Gt => "T_GT",
            TokenType::Le => "T_LE",
            TokenType::Ge => "T_GE",
            TokenType::Plus => "T_PLUS",
            TokenType::Minus => "T_MINUS",
            TokenType::Multiply => "T_MULTIPLY",
            TokenType::Divide => "T_DIVIDE",
            TokenType::Modulo => "T_MODULO",
            TokenType::Increment => "T_INCREMENT",
            TokenType::Decrement => "T_DECREMENT",
            TokenType::And => "T_AND",
            TokenType::Or => "T_OR",
            TokenType::Not => "T_NOT",
            TokenType::BitAnd => "T_BITAND",
            TokenType::BitOr => "T_BITOR",
            TokenType::BitXor => "T_BITXOR",
            TokenType::BitLShift => "T_BITLSHIFT",
            TokenType::BitRShift => "T_BITRSHIFT",
            TokenType::If => "T_IF",
            TokenType::Else => "T_ELSE",
            TokenType::While => "T_WHILE",
            TokenType::Return => "T_RETURN",
            TokenType::Print => "T_PRINT",
            TokenType::Main => "T_MAIN",
            TokenType::Include => "T_INCLUDE",
            TokenType::Const => "T_CONST",
            TokenType::Global => "T_GLOBAL",
            TokenType::String => "T_STRING",
            TokenType::Do => "T_DO",
            TokenType::Switch => "T_SWITCH",
            TokenType::Break => "T_BREAK",
            TokenType::For => "T_FOR",
            TokenType::Default => "T_DEFAULT",
            TokenType::Case => "T_CASE",
            TokenType::Colon => "T_COLON",
            TokenType::SingleComment => "T_SINGLE_COMMENT",
            TokenType::MultiComment => "T_MULTI_COMMENT",
            TokenType::Error => "T_ERROR",
            TokenType::Eof => "T_EOF",
        };
        write!(f, "{}", s)
    }
}

#[derive(Debug, Clone)]
pub struct Token {
    pub token_type: TokenType,
    pub value: String,
    pub line: usize,
    pub column: usize,
}

impl Token {
    pub fn new(token_type: TokenType, value: String, line: usize, column: usize) -> Self {
        Token {
            token_type,
            value,
            line,
            column,
        }
    }
}

impl fmt::Display for Token {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{}({}), line: {}, col: {}",
            self.token_type, self.value, self.line, self.column
        )
    }
}