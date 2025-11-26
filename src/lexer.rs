use crate::token::{Token, TokenType};
use std::collections::HashMap;

pub struct Lexer {
    input: Vec<char>,
    pos: usize,
    line: usize,
    column: usize,
    keywords: HashMap<String, TokenType>,
}

impl Lexer {
    pub fn new(source: String) -> Self {
        let mut keywords = HashMap::new();
        keywords.insert("int".to_string(), TokenType::Int);
        keywords.insert("float".to_string(), TokenType::Float);
        keywords.insert("double".to_string(), TokenType::Double);
        keywords.insert("char".to_string(), TokenType::Char);
        keywords.insert("void".to_string(), TokenType::Void);
        keywords.insert("bool".to_string(), TokenType::Bool);
        keywords.insert("enum".to_string(), TokenType::Enum);
        keywords.insert("true".to_string(), TokenType::BoolLit);
        keywords.insert("false".to_string(), TokenType::BoolLit);
        keywords.insert("if".to_string(), TokenType::If);
        keywords.insert("else".to_string(), TokenType::Else);
        keywords.insert("while".to_string(), TokenType::While);
        keywords.insert("return".to_string(), TokenType::Return);
        keywords.insert("print".to_string(), TokenType::Print);
        keywords.insert("main".to_string(), TokenType::Main);
        keywords.insert("string".to_string(), TokenType::String);
        keywords.insert("do".to_string(), TokenType::Do);
        keywords.insert("switch".to_string(), TokenType::Switch);
        keywords.insert("include".to_string(), TokenType::Include);
        keywords.insert("const".to_string(), TokenType::Const);
        keywords.insert("global".to_string(), TokenType::Global);
        keywords.insert("break".to_string(), TokenType::Break);
        keywords.insert("for".to_string(), TokenType::For);
        keywords.insert("default".to_string(), TokenType::Default);
        keywords.insert("case".to_string(), TokenType::Case);

        Lexer {
            input: source.chars().collect(),
            pos: 0,
            line: 1,
            column: 1,
            keywords,
        }
    }

    fn current_char(&self) -> Option<char> {
        if self.pos < self.input.len() {
            Some(self.input[self.pos])
        } else {
            None
        }
    }

    fn peek_char(&self, offset: usize) -> Option<char> {
        let peek_pos = self.pos + offset;
        if peek_pos < self.input.len() {
            Some(self.input[peek_pos])
        } else {
            None
        }
    }

    fn advance(&mut self) -> Option<char> {
        if let Some(ch) = self.current_char() {
            self.pos += 1;
            if ch == '\n' {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
            Some(ch)
        } else {
            None
        }
    }

    fn skip_whitespace(&mut self) {
        while let Some(ch) = self.current_char() {
            if ch.is_whitespace() {
                self.advance();
            } else {
                break;
            }
        }
    }

    fn try_match_comment(&mut self) -> Option<Token> {
        let start_col = self.column;
        let start_line = self.line;

        if self.current_char() == Some('/') {
            if self.peek_char(1) == Some('/') {
                // Single-line comment
                let mut comment = String::from("//");
                self.advance();
                self.advance();

                while let Some(ch) = self.current_char() {
                    if ch == '\n' {
                        break;
                    }
                    comment.push(ch);
                    self.advance();
                }

                return Some(Token::new(
                    TokenType::SingleComment,
                    comment,
                    start_line,
                    start_col,
                ));
            } else if self.peek_char(1) == Some('*') {
                // Multi-line comment
                let mut comment = String::from("/*");
                self.advance();
                self.advance();

                while let Some(ch) = self.current_char() {
                    if ch == '*' && self.peek_char(1) == Some('/') {
                        comment.push_str("*/");
                        self.advance();
                        self.advance();
                        return Some(Token::new(
                            TokenType::MultiComment,
                            comment,
                            start_line,
                            start_col,
                        ));
                    }
                    comment.push(ch);
                    self.advance();
                }

                return Some(Token::new(
                    TokenType::Error,
                    "Unterminated multi-line comment".to_string(),
                    start_line,
                    start_col,
                ));
            }
        }
        None
    }

    fn try_match_quoted(&mut self) -> Option<Token> {
        let start_col = self.column;
        let start_line = self.line;

        if let Some(quote) = self.current_char() {
            if quote == '"' || quote == '\'' {
                let mut literal = String::new();
                literal.push(quote);
                self.advance();

                while let Some(ch) = self.current_char() {
                    if ch == quote {
                        literal.push(ch);
                        self.advance();
                        let token_type = if quote == '"' {
                            TokenType::StringLit
                        } else {
                            TokenType::CharLit
                        };
                        return Some(Token::new(token_type, literal, start_line, start_col));
                    } else if ch == '\\' {
                        literal.push(ch);
                        self.advance();
                        if let Some(escaped) = self.current_char() {
                            literal.push(escaped);
                            self.advance();
                        }
                    } else {
                        literal.push(ch);
                        self.advance();
                    }
                }

                return Some(Token::new(
                    TokenType::Error,
                    "Unterminated literal".to_string(),
                    start_line,
                    start_col,
                ));
            }
        }
        None
    }

    fn try_match_operator(&mut self) -> Option<Token> {
        let start_col = self.column;
        let start_line = self.line;

        if let Some(ch) = self.current_char() {
            let next = self.peek_char(1);

            // Two-character operators
            let two_char = format!("{}{}", ch, next.unwrap_or('\0'));
            let token_type = match two_char.as_str() {
                "==" => Some(TokenType::EqualOp),
                "!=" => Some(TokenType::Ne),
                "<=" => Some(TokenType::Le),
                ">=" => Some(TokenType::Ge),
                "&&" => Some(TokenType::And),
                "||" => Some(TokenType::Or),
                "++" => Some(TokenType::Increment),
                "--" => Some(TokenType::Decrement),
                "<<" => Some(TokenType::BitLShift),
                ">>" => Some(TokenType::BitRShift),
                _ => None,
            };

            if let Some(tt) = token_type {
                self.advance();
                self.advance();
                return Some(Token::new(tt, two_char, start_line, start_col));
            }

            // Single-character operators
            let single_token = match ch {
                '(' => Some(TokenType::LParen),
                ')' => Some(TokenType::RParen),
                '{' => Some(TokenType::LBrace),
                '}' => Some(TokenType::RBrace),
                '[' => Some(TokenType::LBracket),
                ']' => Some(TokenType::RBracket),
                ';' => Some(TokenType::Semicolon),
                ',' => Some(TokenType::Comma),
                '.' => Some(TokenType::Dot),
                '+' => Some(TokenType::Plus),
                '-' => Some(TokenType::Minus),
                '*' => Some(TokenType::Multiply),
                '/' => Some(TokenType::Divide),
                '%' => Some(TokenType::Modulo),
                '=' => Some(TokenType::AssignOp),
                '!' => Some(TokenType::Not),
                '<' => Some(TokenType::Lt),
                '>' => Some(TokenType::Gt),
                '&' => Some(TokenType::BitAnd),
                '|' => Some(TokenType::BitOr),
                '^' => Some(TokenType::BitXor),
                ':' => Some(TokenType::Colon),
                _ => None,
            };

            if let Some(tt) = single_token {
                let value = ch.to_string();
                self.advance();
                return Some(Token::new(tt, value, start_line, start_col));
            }
        }
        None
    }

    fn try_match_number(&mut self) -> Option<Token> {
        let start_col = self.column;
        let start_line = self.line;

        let mut value = String::new();
        let mut has_decimal = false;
        let mut has_exponent = false;

        let current = self.current_char()?;
        
        // Check for decimal point followed by digit (like .22)
        let starts_with_decimal = current == '.' 
            && self.peek_char(1).map_or(false, |c| c.is_ascii_digit());
        
        // Must start with digit or decimal point followed by digit
        if !current.is_ascii_digit() && !starts_with_decimal {
            return None;
        }

        // If starts with decimal, consume it immediately
        if starts_with_decimal {
            has_decimal = true;
            value.push('.');
            self.advance();
        }

        while let Some(ch) = self.current_char() {
            if ch.is_ascii_digit() {
                value.push(ch);
                self.advance();
            } else if ch == '.' && !has_decimal {
                has_decimal = true;
                value.push(ch);
                self.advance();
            } else if (ch == 'e' || ch == 'E') && !has_exponent {
                has_exponent = true;
                value.push(ch);
                self.advance();
                if let Some(sign) = self.current_char() {
                    if sign == '+' || sign == '-' {
                        value.push(sign);
                        self.advance();
                    }
                }
            } else {
                break;
            }
        }

        // Check for invalid suffix like 123abc
        if let Some(ch) = self.current_char() {
            if ch.is_alphabetic() || ch == '_' {
                let mut bad_ident = value.clone();
                while let Some(c) = self.current_char() {
                    if c.is_alphanumeric() || c == '_' {
                        bad_ident.push(c);
                        self.advance();
                    } else {
                        break;
                    }
                }
                return Some(Token::new(
                    TokenType::Error,
                    format!("Invalid numeric literal followed by identifier: '{}'", bad_ident),
                    start_line,
                    start_col,
                ));
            }
        }

        let token_type = if has_decimal || has_exponent {
            TokenType::FloatLit
        } else {
            TokenType::IntLit
        };

        Some(Token::new(token_type, value, start_line, start_col))
    }

    fn is_identifier_start(&self, ch: char) -> bool {
        // Allow alphabetic characters (including Unicode), underscore, or any Unicode letter
        ch.is_alphabetic() || ch == '_'
    }

    fn is_identifier_continue(&self, ch: char) -> bool {
        // Allow alphanumeric (including Unicode), underscore, or any Unicode character
        // that is not whitespace, operator, or delimiter
        if ch.is_alphanumeric() || ch == '_' {
            return true;
        }
        
        // Allow any Unicode character that is not:
        // - Whitespace
        // - Common operators and delimiters
        // - ASCII control characters
        if ch.is_whitespace() || ch.is_ascii_control() {
            return false;
        }
        
        // Exclude common operators and delimiters
        let excluded = ['(', ')', '{', '}', '[', ']', ';', ',', '.', 
                       '+', '-', '*', '/', '%', '=', '!', '<', '>', 
                       '&', '|', '^', ':', '"', '\''];
        
        !excluded.contains(&ch)
    }

    fn try_match_identifier(&mut self) -> Option<Token> {
        let start_col = self.column;
        let start_line = self.line;

        if let Some(ch) = self.current_char() {
            if self.is_identifier_start(ch) {
                let mut value = String::new();
                value.push(ch);
                self.advance();

                while let Some(c) = self.current_char() {
                    if self.is_identifier_continue(c) {
                        value.push(c);
                        self.advance();
                    } else {
                        break;
                    }
                }

                let token_type = self
                    .keywords
                    .get(&value)
                    .cloned()
                    .unwrap_or(TokenType::Identifier);

                return Some(Token::new(token_type, value, start_line, start_col));
            }
        }
        None
    }

    pub fn next_token(&mut self) -> Option<Token> {
        self.skip_whitespace();

        if self.pos >= self.input.len() {
            return Some(Token::new(TokenType::Eof, "EOF".to_string(), self.line, self.column));
        }

        let start_col = self.column;
        let start_line = self.line;
        let start_pos = self.pos;

        if let Some(token) = self.try_match_comment() {
            return Some(token);
        }

        if let Some(token) = self.try_match_quoted() {
            return Some(token);
        }

        if let Some(token) = self.try_match_number() {
            return Some(token);
        }

        if let Some(token) = self.try_match_operator() {
            return Some(token);
        }

        if let Some(token) = self.try_match_identifier() {
            return Some(token);
        }

        // Unknown character - ensure we advance to prevent infinite loop
        if let Some(ch) = self.current_char() {
            let error_msg = format!("Unexpected character: '{}'", ch);
            self.advance();
            return Some(Token::new(TokenType::Error, error_msg, start_line, start_col));
        }

        // Safety check: if position hasn't advanced, force it
        if self.pos == start_pos && self.pos < self.input.len() {
            eprintln!("WARNING: Token matching failed to advance position at pos {}", self.pos);
            self.advance();
        }

        None
    }

    pub fn tokenize(&mut self) -> Vec<Token> {
        let mut tokens = Vec::new();
        let mut iterations = 0;
        let max_iterations = self.input.len() * 2; // Safety limit

        loop {
            iterations += 1;
            if iterations > max_iterations {
                eprintln!("ERROR: Tokenizer exceeded maximum iterations. Possible infinite loop.");
                eprintln!("Position: {}, Line: {}, Column: {}", self.pos, self.line, self.column);
                tokens.push(Token::new(
                    TokenType::Error,
                    "Tokenizer loop limit exceeded".to_string(),
                    self.line,
                    self.column
                ));
                break;
            }

            if let Some(token) = self.next_token() {
                let is_eof = token.token_type == TokenType::Eof;
                
                // Skip comments but keep all other tokens
                if token.token_type != TokenType::SingleComment 
                    && token.token_type != TokenType::MultiComment {
                    tokens.push(token);
                }
                
                if is_eof {
                    break;
                }
            } else {
                eprintln!("Warning: next_token() returned None unexpectedly at pos {}", self.pos);
                break;
            }
        }

        tokens
    }
}