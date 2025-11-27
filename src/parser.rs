use crate::token::{Token, TokenType};
use crate::ast::*;

// === Parse Errors ===
#[derive(Debug, Clone)]
pub enum ParseErrorType {
    UnexpectedEOF,
    FailedToFindToken(TokenType),
    ExpectedTypeToken,
    ExpectedIdentifier,
    UnexpectedToken,
    ExpectedExpr,
    InvalidCallTarget,
    MissingSemicolon,
    UnclosedBlock,
}

#[derive(Debug, Clone)]
pub struct ParseError {
    pub error_type: ParseErrorType,
    pub token: Token,
    pub message: String,
}

impl ParseError {
    pub fn new(error_type: ParseErrorType, token: Token) -> Self {
        let message = match &error_type {
            ParseErrorType::UnexpectedEOF => "Unexpected end of file".to_string(),
            ParseErrorType::FailedToFindToken(expected) => {
                format!("Failed to find expected token: {:?}", expected)
            }
            ParseErrorType::ExpectedTypeToken => "Expected type token (int, float, etc.)".to_string(),
            ParseErrorType::ExpectedIdentifier => "Expected identifier".to_string(),
            ParseErrorType::UnexpectedToken => {
                format!("Unexpected token: {} ({})", token.token_type, token.value)
            }
            ParseErrorType::ExpectedExpr => "Expected expression".to_string(),
            ParseErrorType::InvalidCallTarget => {
                "Invalid function call target: only identifiers can be called".to_string()
            }
            ParseErrorType::MissingSemicolon => "Missing semicolon at end of statement".to_string(),
            ParseErrorType::UnclosedBlock => {
                "Unclosed block, expected '}' before end of file".to_string()
            }
        };

        ParseError {
            error_type,
            token: token.clone(),
            message: format!(
                "{} (line: {}, col: {})",
                message, token.line, token.column
            ),
        }
    }
}

impl std::fmt::Display for ParseError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "[Parser Error] {}", self.message)
    }
}

impl std::error::Error for ParseError {}

// === Precedence Levels ===
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
enum Precedence {
    Lowest = 0,
    Assignment = 1,
    LogicalOr = 2,
    LogicalAnd = 3,
    Equality = 4,
    Comparison = 5,
    BitwiseTerm = 6,
    Term = 7,
    Factor = 8,
    Unary = 9,
    Postfix = 10,
    Call = 11,
}

pub struct Parser {
    tokens: Vec<Token>,
    current: usize,
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Self {
        Parser { tokens, current: 0 }
    }

    // === Helper Methods ===
    
    fn current_token(&self) -> &Token {
        if self.current < self.tokens.len() {
            &self.tokens[self.current]
        } else {
            static EOF: Token = Token {
                token_type: TokenType::Eof,
                value: String::new(),
                line: 0,
                column: 0,
            };
            &EOF
        }
    }

    fn peek(&self, offset: usize) -> &Token {
        let idx = self.current + offset;
        if idx < self.tokens.len() {
            &self.tokens[idx]
        } else {
            static EOF: Token = Token {
                token_type: TokenType::Eof,
                value: String::new(),
                line: 0,
                column: 0,
            };
            &EOF
        }
    }

    fn is_at_end(&self) -> bool {
        self.current_token().token_type == TokenType::Eof
    }

    fn advance(&mut self) -> Token {
        let token = self.current_token().clone();
        if self.current < self.tokens.len() {
            self.current += 1;
        }
        token
    }

    fn check(&self, token_type: TokenType) -> bool {
        self.current_token().token_type == token_type
    }

    fn match_token(&mut self, token_type: TokenType) -> bool {
        if self.check(token_type) {
            self.advance();
            true
        } else {
            false
        }
    }

    fn expect(&mut self, token_type: TokenType) -> Result<Token, ParseError> {
        if self.check(token_type) {
            Ok(self.advance())
        } else {
            Err(ParseError::new(
                ParseErrorType::FailedToFindToken(token_type),
                self.current_token().clone(),
            ))
        }
    }

    fn consume_semicolon(&mut self) -> Result<(), ParseError> {
        if !self.match_token(TokenType::Semicolon) {
            return Err(ParseError::new(
                ParseErrorType::MissingSemicolon,
                self.current_token().clone(),
            ));
        }
        Ok(())
    }

    fn is_type_token(&self, token_type: TokenType) -> bool {
        matches!(
            token_type,
            TokenType::Int
                | TokenType::Float
                | TokenType::Double
                | TokenType::Char
                | TokenType::Bool
                | TokenType::Void
                | TokenType::String
        )
    }

    fn get_precedence(&self, token_type: TokenType) -> Precedence {
        match token_type {
            TokenType::AssignOp => Precedence::Assignment,
            TokenType::Or => Precedence::LogicalOr,
            TokenType::And => Precedence::LogicalAnd,
            TokenType::EqualOp | TokenType::Ne => Precedence::Equality,
            TokenType::Lt | TokenType::Gt | TokenType::Le | TokenType::Ge => Precedence::Comparison,
            TokenType::BitAnd | TokenType::BitOr | TokenType::BitXor 
                | TokenType::BitLShift | TokenType::BitRShift => Precedence::BitwiseTerm,
            TokenType::Plus | TokenType::Minus => Precedence::Term,
            TokenType::Multiply | TokenType::Divide | TokenType::Modulo => Precedence::Factor,
            TokenType::Increment | TokenType::Decrement => Precedence::Postfix,
            TokenType::LParen => Precedence::Call,
            _ => Precedence::Lowest,
        }
    }

    fn is_identifier_node(node: &ASTNode) -> bool {
        matches!(node, ASTNode::Identifier(_))
    }

    fn is_function_definition(&self) -> bool {
        if self.current + 1 >= self.tokens.len() {
            return false;
        }

        let mut i = self.current + 1;

        // Look for identifier
        if i < self.tokens.len() && self.tokens[i].token_type == TokenType::Identifier {
            i += 1;
            // Look for LPAREN
            if i < self.tokens.len() && self.tokens[i].token_type == TokenType::LParen {
                i += 1;
                // Skip parameters
                let mut paren_count = 1;
                while i < self.tokens.len() && paren_count > 0 {
                    if self.tokens[i].token_type == TokenType::LParen {
                        paren_count += 1;
                    } else if self.tokens[i].token_type == TokenType::RParen {
                        paren_count -= 1;
                    }
                    i += 1;
                }

                // Check for LBRACE (function definition) vs SEMICOLON (prototype)
                if i < self.tokens.len() && self.tokens[i].token_type == TokenType::LBrace {
                    return true;
                }
            }
        }
        false
    }

    // === Pratt Parser Core ===

    fn parse_expression(&mut self, precedence: Precedence) -> Result<ASTNode, ParseError> {
        let mut left = self.parse_prefix()?;

        while !self.is_at_end()
            && self.get_precedence(self.current_token().token_type) > precedence
        {
            left = self.parse_infix(left)?;
        }

        Ok(left)
    }

    fn parse_prefix(&mut self) -> Result<ASTNode, ParseError> {
        match self.current_token().token_type {
            TokenType::IntLit => self.parse_int_literal(),
            TokenType::FloatLit => self.parse_float_literal(),
            TokenType::StringLit => self.parse_string_literal(),
            TokenType::CharLit => self.parse_char_literal(),
            TokenType::BoolLit => self.parse_bool_literal(),
            TokenType::Identifier => self.parse_identifier(),
            TokenType::LParen => self.parse_grouped_expression(),
            TokenType::Minus | TokenType::Not | TokenType::Increment | TokenType::Decrement => {
                self.parse_unary_expression()
            }
            _ => Err(ParseError::new(
                ParseErrorType::ExpectedExpr,
                self.current_token().clone(),
            )),
        }
    }

    fn parse_infix(&mut self, left: ASTNode) -> Result<ASTNode, ParseError> {
        match self.current_token().token_type {
            TokenType::AssignOp
            | TokenType::Plus
            | TokenType::Minus
            | TokenType::Multiply
            | TokenType::Divide
            | TokenType::Modulo
            | TokenType::EqualOp
            | TokenType::Ne
            | TokenType::Lt
            | TokenType::Gt
            | TokenType::Le
            | TokenType::Ge
            | TokenType::And
            | TokenType::Or
            | TokenType::BitAnd
            | TokenType::BitOr
            | TokenType::BitXor
            | TokenType::BitLShift
            | TokenType::BitRShift => self.parse_binary_expression(left),

            TokenType::Increment | TokenType::Decrement => self.parse_postfix_unary_expression(left),

            TokenType::LParen => {
                if Self::is_identifier_node(&left) {
                    self.parse_call_expression(left)
                } else {
                    Err(ParseError::new(
                        ParseErrorType::InvalidCallTarget,
                        self.current_token().clone(),
                    ))
                }
            }

            _ => Err(ParseError::new(
                ParseErrorType::UnexpectedToken,
                self.current_token().clone(),
            )),
        }
    }

    // === Literal Parsers ===

    fn parse_int_literal(&mut self) -> Result<ASTNode, ParseError> {
        let token = self.advance();
        let value = token.value.parse::<i64>().unwrap_or(0);
        Ok(ASTNode::IntLiteral(IntLiteral {
            value,
            line: token.line,
            column: token.column,
        }))
    }

    fn parse_float_literal(&mut self) -> Result<ASTNode, ParseError> {
        let token = self.advance();
        let value = token.value.parse::<f64>().unwrap_or(0.0);
        Ok(ASTNode::FloatLiteral(FloatLiteral {
            value,
            line: token.line,
            column: token.column,
        }))
    }

    fn parse_string_literal(&mut self) -> Result<ASTNode, ParseError> {
        let token = self.advance();
        let mut value = token.value.clone();
        
        // Remove quotes
        if value.len() >= 2 && value.starts_with('"') && value.ends_with('"') {
            value = value[1..value.len() - 1].to_string();
        }

        Ok(ASTNode::StringLiteral(StringLiteral {
            value,
            line: token.line,
            column: token.column,
        }))
    }

    fn parse_char_literal(&mut self) -> Result<ASTNode, ParseError> {
        let token = self.advance();
        let mut value = '\0';

        if token.value.len() >= 3
            && token.value.starts_with('\'')
            && token.value.ends_with('\'')
        {
            value = token.value.chars().nth(1).unwrap_or('\0');
        }

        Ok(ASTNode::CharLiteral(CharLiteral {
            value,
            line: token.line,
            column: token.column,
        }))
    }

    fn parse_bool_literal(&mut self) -> Result<ASTNode, ParseError> {
        let token = self.advance();
        let value = token.value == "true";
        Ok(ASTNode::BoolLiteral(BoolLiteral {
            value,
            line: token.line,
            column: token.column,
        }))
    }

    fn parse_identifier(&mut self) -> Result<ASTNode, ParseError> {
        let token = self.advance();
        Ok(ASTNode::Identifier(Identifier {
            name: token.value.clone(),
            line: token.line,
            column: token.column,
        }))
    }

    fn parse_grouped_expression(&mut self) -> Result<ASTNode, ParseError> {
        self.expect(TokenType::LParen)?;
        let expr = self.parse_expression(Precedence::Lowest)?;
        self.expect(TokenType::RParen)?;
        Ok(expr)
    }

    fn parse_unary_expression(&mut self) -> Result<ASTNode, ParseError> {
        let op_token = self.advance();
        let right = self.parse_expression(Precedence::Unary)?;

        Ok(ASTNode::UnaryExpr(UnaryExpr {
            op: op_token.token_type,
            operand: Box::new(right),
            is_postfix: false,
            line: op_token.line,
            column: op_token.column,
        }))
    }

    fn parse_postfix_unary_expression(&mut self, left: ASTNode) -> Result<ASTNode, ParseError> {
        let op_token = self.advance();

        Ok(ASTNode::UnaryExpr(UnaryExpr {
            op: op_token.token_type,
            operand: Box::new(left),
            is_postfix: true,
            line: op_token.line,
            column: op_token.column,
        }))
    }

    fn parse_binary_expression(&mut self, left: ASTNode) -> Result<ASTNode, ParseError> {
        let op_token = self.advance();
        let precedence = self.get_precedence(op_token.token_type);
        let right = self.parse_expression(precedence)?;

        // Check for assignment to non-identifier
        if op_token.token_type == TokenType::AssignOp && !Self::is_identifier_node(&left) {
            return Err(ParseError::new(
                ParseErrorType::UnexpectedToken,
                op_token.clone(),
            ));
        }

        Ok(ASTNode::BinaryExpr(BinaryExpr {
            op: op_token.token_type,
            left: Box::new(left),
            right: Box::new(right),
            line: op_token.line,
            column: op_token.column,
        }))
    }

    fn parse_call_expression(&mut self, callee: ASTNode) -> Result<ASTNode, ParseError> {
        let call_token = self.current_token().clone();
        self.expect(TokenType::LParen)?;

        let mut args = Vec::new();
        if !self.check(TokenType::RParen) {
            loop {
                args.push(self.parse_expression(Precedence::Lowest)?);
                if !self.match_token(TokenType::Comma) {
                    break;
                }
            }
        }

        self.expect(TokenType::RParen)?;

        Ok(ASTNode::CallExpr(CallExpr {
            callee: Box::new(callee),
            args,
            line: call_token.line,
            column: call_token.column,
        }))
    }

    // === Statement Parsers ===

    fn parse_statement(&mut self) -> Result<ASTNode, ParseError> {
        let token_type = self.current_token().token_type;

        // Check for enum declaration
        if token_type == TokenType::Enum {
            return self.parse_enum_declaration();
        }

        // Check for type tokens (variable or function)
        if self.is_type_token(token_type) {
            let next = self.peek(1);
            if next.token_type == TokenType::Identifier {
                let i = self.current + 2;
                if i < self.tokens.len() && self.tokens[i].token_type == TokenType::LParen {
                    // Function definition or prototype
                    if self.is_function_definition() {
                        return self.parse_function_declaration();
                    } else {
                        return self.parse_function_prototype();
                    }
                } else {
                    // Variable declaration
                    return self.parse_variable_declaration();
                }
            } else {
                return self.parse_variable_declaration();
            }
        }

        match token_type {
            TokenType::Print => self.parse_print_statement(),
            TokenType::If => self.parse_if_statement(),
            TokenType::While => self.parse_while_statement(),
            TokenType::Do => self.parse_do_while_statement(),
            TokenType::For => self.parse_for_statement(),
            TokenType::Switch => self.parse_switch_statement(),
            TokenType::Return => self.parse_return_statement(),
            TokenType::LBrace => self.parse_block_statement(),
            TokenType::Main => self.parse_main_declaration(),
            TokenType::Break => self.parse_break_statement(),
            _ => {
                // Expression statement
                let expr = self.parse_expression(Precedence::Lowest)?;
                self.consume_semicolon()?;
                Ok(ASTNode::ExpressionStmt(ExpressionStmt {
                    expr: Box::new(expr),
                    line: self.current_token().line,
                    column: self.current_token().column,
                }))
            }
        }
    }

    fn parse_variable_declaration(&mut self) -> Result<ASTNode, ParseError> {
        let type_token = self.advance();
        let ident_token = self.expect(TokenType::Identifier)?;

        let initializer = if self.match_token(TokenType::AssignOp) {
            Some(Box::new(self.parse_expression(Precedence::Lowest)?))
        } else {
            None
        };

        self.consume_semicolon()?;

        Ok(ASTNode::VarDecl(VarDecl {
            var_type: type_token.token_type,
            name: ident_token.value,
            initializer,
            line: type_token.line,
            column: type_token.column,
        }))
    }

    fn parse_function_prototype(&mut self) -> Result<ASTNode, ParseError> {
        let return_token = self.advance();
        let name_token = self.expect(TokenType::Identifier)?;

        self.expect(TokenType::LParen)?;
        let mut params = Vec::new();

        if !self.check(TokenType::RParen) {
            loop {
                let param_type = self.advance();
                let param_name = self.expect(TokenType::Identifier)?;
                params.push((param_type.token_type, param_name.value));

                if !self.match_token(TokenType::Comma) {
                    break;
                }
            }
        }

        self.expect(TokenType::RParen)?;
        self.consume_semicolon()?;

        Ok(ASTNode::FunctionProto(FunctionProto {
            return_type: return_token.token_type,
            name: name_token.value,
            params,
            line: return_token.line,
            column: return_token.column,
        }))
    }

    fn parse_function_declaration(&mut self) -> Result<ASTNode, ParseError> {
        let return_token = self.advance();
        let name_token = self.expect(TokenType::Identifier)?;

        self.expect(TokenType::LParen)?;
        let mut params = Vec::new();

        if !self.check(TokenType::RParen) {
            loop {
                let param_type = self.advance();
                let param_name = self.expect(TokenType::Identifier)?;
                params.push((param_type.token_type, param_name.value));

                if !self.match_token(TokenType::Comma) {
                    break;
                }
            }
        }

        self.expect(TokenType::RParen)?;
        let body = self.parse_block()?;

        Ok(ASTNode::FunctionDecl(FunctionDecl {
            return_type: return_token.token_type,
            name: name_token.value,
            params,
            body,
            line: return_token.line,
            column: return_token.column,
        }))
    }

    fn parse_main_declaration(&mut self) -> Result<ASTNode, ParseError> {
        let main_token = self.advance();
        let body = self.parse_block()?;

        Ok(ASTNode::MainDecl(MainDecl {
            body,
            line: main_token.line,
            column: main_token.column,
        }))
    }

    fn parse_print_statement(&mut self) -> Result<ASTNode, ParseError> {
        let print_token = self.advance();
        self.expect(TokenType::LParen)?;

        let mut args = Vec::new();
        if !self.check(TokenType::RParen) {
            loop {
                args.push(self.parse_expression(Precedence::Lowest)?);
                if !self.match_token(TokenType::Comma) {
                    break;
                }
            }
        }

        self.expect(TokenType::RParen)?;
        self.consume_semicolon()?;

        Ok(ASTNode::PrintStmt(PrintStmt {
            args,
            line: print_token.line,
            column: print_token.column,
        }))
    }

    fn parse_if_statement(&mut self) -> Result<ASTNode, ParseError> {
        let if_token = self.advance();
        self.expect(TokenType::LParen)?;
        let condition = self.parse_expression(Precedence::Lowest)?;
        self.expect(TokenType::RParen)?;

        let if_body = self.parse_block()?;

        let else_body = if self.match_token(TokenType::Else) {
            self.parse_block()?
        } else {
            Vec::new()
        };

        Ok(ASTNode::IfStmt(IfStmt {
            condition: Box::new(condition),
            if_body,
            else_body,
            line: if_token.line,
            column: if_token.column,
        }))
    }

    fn parse_while_statement(&mut self) -> Result<ASTNode, ParseError> {
        let while_token = self.advance();
        self.expect(TokenType::LParen)?;
        let condition = self.parse_expression(Precedence::Lowest)?;
        self.expect(TokenType::RParen)?;

        let body = self.parse_block()?;

        Ok(ASTNode::WhileStmt(WhileStmt {
            condition: Box::new(condition),
            body,
            line: while_token.line,
            column: while_token.column,
        }))
    }

    fn parse_do_while_statement(&mut self) -> Result<ASTNode, ParseError> {
        let do_token = self.current_token().clone();
        let body_vec = self.parse_block()?;
        let body = ASTNode::BlockStmt(BlockStmt {
            body: body_vec,
            line: do_token.line,
            column: do_token.column,
        });

        self.expect(TokenType::While)?;
        self.expect(TokenType::LParen)?;
        let condition = self.parse_expression(Precedence::Lowest)?;
        self.expect(TokenType::RParen)?;
        self.consume_semicolon()?;

        Ok(ASTNode::DoWhileStmt(DoWhileStmt {
            body: Box::new(body),
            condition: Box::new(condition),
            line: do_token.line,
            column: do_token.column,
        }))
    }

    fn parse_for_statement(&mut self) -> Result<ASTNode, ParseError> {
        let for_token = self.current_token().clone();
        self.expect(TokenType::For)?;
        self.expect(TokenType::LParen)?;

        // Init
        let init = if !self.check(TokenType::Semicolon) {
            if self.is_type_token(self.current_token().token_type) {
                Some(Box::new(self.parse_variable_declaration()?))
            } else {
                let expr = self.parse_expression(Precedence::Lowest)?;
                self.consume_semicolon()?;
                Some(Box::new(expr))
            }
        } else {
            self.advance();
            None
        };

        // Condition
        let condition = if !self.check(TokenType::Semicolon) {
            Some(Box::new(self.parse_expression(Precedence::Lowest)?))
        } else {
            None
        };
        self.expect(TokenType::Semicolon)?;

        // Update
        let update = if !self.check(TokenType::RParen) {
            Some(Box::new(self.parse_expression(Precedence::Lowest)?))
        } else {
            None
        };
        self.expect(TokenType::RParen)?;

        let body_vec = self.parse_block()?;
        let body = ASTNode::BlockStmt(BlockStmt {
            body: body_vec,
            line: for_token.line,
            column: for_token.column,
        });

        Ok(ASTNode::ForStmt(ForStmt {
            init,
            condition,
            update,
            body: Box::new(body),
            line: for_token.line,
            column: for_token.column,
        }))
    }

    fn parse_switch_statement(&mut self) -> Result<ASTNode, ParseError> {
        let switch_token = self.advance();
        self.expect(TokenType::LParen)?;
        let expression = self.parse_expression(Precedence::Lowest)?;
        self.expect(TokenType::RParen)?;
        self.expect(TokenType::LBrace)?;

        let mut cases = Vec::new();
        let mut default_body = Vec::new();

        while !self.check(TokenType::RBrace) && !self.is_at_end() {
            if self.match_token(TokenType::Case) {
                let case_token = self.current_token().clone();
                let value = self.parse_expression(Precedence::Lowest)?;
                self.expect(TokenType::Colon)?;
                
                // Parse case body - collect statements until next case/default/closing brace
                let mut case_body = Vec::new();
                while !self.check(TokenType::Case) 
                    && !self.check(TokenType::Default) 
                    && !self.check(TokenType::RBrace) 
                    && !self.is_at_end() {
                    case_body.push(self.parse_statement()?);
                }
                
                cases.push(ASTNode::CaseBlock(CaseBlock {
                    value: Box::new(value),
                    body: case_body,
                    line: case_token.line,
                    column: case_token.column,
                }));
            } else if self.match_token(TokenType::Default) {
                self.expect(TokenType::Colon)?;
                
                // Parse default body - collect statements until closing brace
                while !self.check(TokenType::RBrace) && !self.is_at_end() {
                    default_body.push(self.parse_statement()?);
                }
            } else {
                self.advance();
            }
        }

        self.expect(TokenType::RBrace)?;

        Ok(ASTNode::SwitchStmt(SwitchStmt {
            expression: Box::new(expression),
            cases,
            default_body,
            line: switch_token.line,
            column: switch_token.column,
        }))
    }

    fn parse_return_statement(&mut self) -> Result<ASTNode, ParseError> {
        let return_token = self.advance();

        let value = if !self.check(TokenType::Semicolon) && !self.check(TokenType::RBrace) {
            Some(Box::new(self.parse_expression(Precedence::Lowest)?))
        } else {
            None
        };

        self.consume_semicolon()?;

        Ok(ASTNode::ReturnStmt(ReturnStmt {
            value,
            line: return_token.line,
            column: return_token.column,
        }))
    }

    fn parse_break_statement(&mut self) -> Result<ASTNode, ParseError> {
        let break_token = self.advance();
        self.consume_semicolon()?;

        Ok(ASTNode::BreakStmt(BreakStmt {
            line: break_token.line,
            column: break_token.column,
        }))
    }

    fn parse_block_statement(&mut self) -> Result<ASTNode, ParseError> {
        let block_token = self.current_token().clone();
        let body = self.parse_block()?;

        Ok(ASTNode::BlockStmt(BlockStmt {
            body,
            line: block_token.line,
            column: block_token.column,
        }))
    }

    fn parse_block(&mut self) -> Result<Vec<ASTNode>, ParseError> {
        self.expect(TokenType::LBrace)?;
        let mut statements = Vec::new();

        while !self.check(TokenType::RBrace) && !self.is_at_end() {
            statements.push(self.parse_statement()?);
        }

        if self.is_at_end() {
            return Err(ParseError::new(
                ParseErrorType::UnclosedBlock,
                self.current_token().clone(),
            ));
        }

        self.expect(TokenType::RBrace)?;
        Ok(statements)
    }

    fn parse_enum_declaration(&mut self) -> Result<ASTNode, ParseError> {
        let enum_token = self.advance();
        let name_token = self.expect(TokenType::Identifier)?;
        self.expect(TokenType::LBrace)?;

        let mut values = Vec::new();
        if !self.check(TokenType::RBrace) {
            loop {
                let value_token = self.expect(TokenType::Identifier)?;
                values.push(value_token.value);

                if !self.match_token(TokenType::Comma) {
                    break;
                }
            }
        }

        self.expect(TokenType::RBrace)?;
        self.consume_semicolon()?;

        let value_list = ASTNode::EnumValueList(EnumValueList {
            values,
            line: name_token.line,
            column: name_token.column,
        });

        Ok(ASTNode::EnumDecl(EnumDecl {
            name: name_token.value,
            values: Box::new(value_list),
            line: enum_token.line,
            column: enum_token.column,
        }))
    }

    fn parse_include_statement(&mut self) -> Result<ASTNode, ParseError> {
        let include_token = self.advance();

        if self.match_token(TokenType::Lt) {
            let header = if self.check(TokenType::Main) {
                self.advance();
                "main".to_string()
            } else if self.check(TokenType::Identifier) {
                self.advance().value
            } else {
                return Err(ParseError::new(
                    ParseErrorType::ExpectedIdentifier,
                    self.current_token().clone(),
                ));
            };

            self.expect(TokenType::Gt)?;

            Ok(ASTNode::IncludeStmt(IncludeStmt {
                header,
                line: include_token.line,
                column: include_token.column,
            }))
        } else if self.check(TokenType::StringLit) {
            let header_token = self.advance();
            let mut header = header_token.value;

            // Remove quotes
            if header.len() >= 2 && header.starts_with('"') && header.ends_with('"') {
                header = header[1..header.len() - 1].to_string();
            }

            Ok(ASTNode::IncludeStmt(IncludeStmt {
                header,
                line: include_token.line,
                column: include_token.column,
            }))
        } else {
            Err(ParseError::new(
                ParseErrorType::UnexpectedToken,
                self.current_token().clone(),
            ))
        }
    }

    // === Main Parse Function ===

    pub fn parse_program(&mut self) -> Result<Vec<ASTNode>, ParseError> {
        let mut declarations = Vec::new();

        // Enforce first token must be include
        if !self.check(TokenType::Include) {
            return Err(ParseError::new(
                ParseErrorType::UnexpectedToken,
                self.current_token().clone(),
            ));
        }
        declarations.push(self.parse_include_statement()?);

        // Continue with the rest
        while !self.is_at_end() {
            if self.check(TokenType::Include) {
                declarations.push(self.parse_include_statement()?);
                continue;
            }

            if self.is_type_token(self.current_token().token_type) {
                let next = self.peek(1);
                if next.token_type == TokenType::Identifier {
                    if self.is_function_definition() {
                        declarations.push(self.parse_function_declaration()?);
                    } else {
                        declarations.push(self.parse_function_prototype()?);
                    }
                } else {
                    declarations.push(self.parse_statement()?);
                }
                continue;
            }

            if self.check(TokenType::Main) {
                declarations.push(self.parse_main_declaration()?);
                continue;
            }

            declarations.push(self.parse_statement()?);
        }

        Ok(declarations)
    }
}