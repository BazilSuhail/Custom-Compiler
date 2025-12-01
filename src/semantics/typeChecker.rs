use crate::core::ast::*;
use crate::core::token::{TokenType, TypeNode};
use crate::semantics::scope::{SymbolInfo, ScopeFrame};

#[derive(Debug, Clone, PartialEq)]
pub enum TypeErrorType {
    ErroneousVarDecl,
    FnCallParamCount,
    FnCallParamType,
    ErroneousReturnType,
    ExpressionTypeMismatch,
    ExpectedBooleanExpression,
    ErroneousBreak,
    NonBooleanCondStmt,
    AttemptedBoolOpOnNonBools,
    AttemptedBitOpOnNonInt,
    AttemptedShiftOnNonInt,
    AttemptedAddOpOnNonNumeric,
    ReturnStmtInVoid,
    NonBooleanSwitchExpression,
    InvalidCaseValueType,
    IncrementDecrementOnNonInt,
    NotOnNonBool,
}

#[derive(Debug, Clone)]
pub struct TypeError {
    pub error_type: TypeErrorType,
    pub line: usize,
    pub column: usize,
    pub message: String,
}

pub struct TypeChecker<'a> {
    global_scope: &'a ScopeFrame,
    current_scope_path: Vec<usize>,
    scope_child_indices: Vec<usize>,
    errors: Vec<TypeError>,
    
    // Context tracking
    current_fn_return_type: Option<TypeNode>,
    current_fn_name: String,
    loop_depth: usize,
    switch_depth: usize,
    found_return_stmt: bool,
}

impl<'a> TypeChecker<'a> {
    pub fn new(global_scope: &'a ScopeFrame) -> Self {
        Self {
            global_scope,
            current_scope_path: Vec::new(),
            scope_child_indices: vec![0],
            errors: Vec::new(),
            current_fn_return_type: None,
            current_fn_name: String::new(),
            loop_depth: 0,
            switch_depth: 0,
            found_return_stmt: false,
        }
    }

    pub fn check(&mut self, ast: &[ASTNode]) -> Result<(), Vec<TypeError>> {
        for node in ast { self.check_node(node); }
        if self.errors.is_empty() { Ok(()) } else { Err(self.errors.clone()) }
    }

    // --- Navigation ---
    fn enter_scope(&mut self) {
        let current_child_idx = *self.scope_child_indices.last().unwrap();
        self.current_scope_path.push(current_child_idx);
        self.scope_child_indices.push(0);
    }

    fn exit_scope(&mut self) {
        self.current_scope_path.pop();
        self.scope_child_indices.pop();
        if let Some(idx) = self.scope_child_indices.last_mut() { *idx += 1; }
    }

    fn lookup_symbol(&self, name: &str) -> Option<&'a SymbolInfo> {
        let mut path = self.current_scope_path.clone();
        loop {
            let mut scope = self.global_scope;
            for &idx in &path { scope = &scope.children[idx]; }
            if let Some(sym) = scope.find_symbol(name) { return Some(sym); }
            if path.is_empty() { break; }
            path.pop();
        }
        None
    }

    // --- Type Helpers ---
    fn is_numeric(&self, t: &TypeNode) -> bool {
        matches!(t, TypeNode::Builtin(TokenType::Int) | TypeNode::Builtin(TokenType::Float))
    }

    fn are_types_equal(&self, t1: &TypeNode, t2: &TypeNode) -> bool {
        match (t1, t2) {
            (TypeNode::Builtin(a), TypeNode::Builtin(b)) => a == b,
            (TypeNode::UserDefined(a), TypeNode::UserDefined(b)) => a == b,
            _ => false,
        }
    }

    fn add_error(&mut self, et: TypeErrorType, ln: usize, col: usize, msg: String) {
        let formatted_msg = format!("{} at line {}, col {}", msg, ln, col);
        self.errors.push(TypeError { error_type: et, line: ln, column: col, message: formatted_msg });
    }

    // --- Type Inference ---
    fn infer(&self, expr: &ASTNode) -> TypeNode {
        match expr {
            ASTNode::IntLiteral(_) => TypeNode::Builtin(TokenType::Int),
            ASTNode::FloatLiteral(_) => TypeNode::Builtin(TokenType::Float),
            ASTNode::StringLiteral(_) => TypeNode::Builtin(TokenType::String),
            ASTNode::CharLiteral(_) => TypeNode::Builtin(TokenType::Char),
            ASTNode::BoolLiteral(_) => TypeNode::Builtin(TokenType::Bool),
            ASTNode::Identifier(i) => self.lookup_symbol(&i.name).map(|s| s.symbol_type.clone()).unwrap_or(TypeNode::Builtin(TokenType::Error)),
            ASTNode::BinaryExpr(b) => {
                if matches!(b.op, TokenType::EqualOp | TokenType::Ne | TokenType::Gt | TokenType::Lt | TokenType::Ge | TokenType::Le | TokenType::And | TokenType::Or) {
                    TypeNode::Builtin(TokenType::Bool)
                } else { self.infer(&b.left) }
            }
            ASTNode::UnaryExpr(u) => if u.op == TokenType::Not { TypeNode::Builtin(TokenType::Bool) } else { self.infer(&u.operand) },
            ASTNode::CallExpr(c) => if let ASTNode::Identifier(i) = c.callee.as_ref() {
                self.lookup_symbol(&i.name).map(|s| s.symbol_type.clone()).unwrap_or(TypeNode::Builtin(TokenType::Error))
            } else { TypeNode::Builtin(TokenType::Error) },
            _ => TypeNode::Builtin(TokenType::Error),
        }
    }

    // --- Checks ---
    fn check_node(&mut self, node: &ASTNode) {
        match node {
            ASTNode::VarDecl(d) => {
                if matches!(d.var_type, TypeNode::Builtin(TokenType::Void)) {
                    self.add_error(TypeErrorType::ErroneousVarDecl, d.line, d.column, format!("Variable '{}' cannot be void", d.name));
                }
                if let Some(init) = &d.initializer {
                    self.check_expr(init);
                    if !self.are_types_equal(&d.var_type, &self.infer(init)) {
                        self.add_error(TypeErrorType::ExpressionTypeMismatch, d.line, d.column, format!("Type mismatch in declaration of '{}'", d.name));
                    }
                }
            }
            ASTNode::FunctionDecl(d) => {
                let prev_ret = self.current_fn_return_type.take();
                let prev_name = std::mem::replace(&mut self.current_fn_name, d.name.clone());
                let prev_found = self.found_return_stmt;
                self.current_fn_return_type = Some(d.return_type.clone());
                self.found_return_stmt = false;
                self.enter_scope();
                for s in &d.body { self.check_node(s); }
                if !self.are_types_equal(&d.return_type, &TypeNode::Builtin(TokenType::Void)) && !self.found_return_stmt {
                    self.add_error(TypeErrorType::ErroneousReturnType, d.line, d.column, format!("Function '{}' requires return statement", d.name));
                }
                self.exit_scope();
                self.current_fn_return_type = prev_ret;
                self.current_fn_name = prev_name;
                self.found_return_stmt = prev_found;
            }
            ASTNode::MainDecl(d) => { self.enter_scope(); for s in &d.body { self.check_node(s); } self.exit_scope(); }
            ASTNode::IfStmt(d) => {
                self.check_expr(&d.condition);
                if !matches!(self.infer(&d.condition), TypeNode::Builtin(TokenType::Bool)) {
                    self.add_error(TypeErrorType::NonBooleanCondStmt, d.line, d.column, "If condition must be boolean".into());
                }
                self.enter_scope(); for s in &d.if_body { self.check_node(s); } self.exit_scope();
                if !d.else_body.is_empty() { self.enter_scope(); for s in &d.else_body { self.check_node(s); } self.exit_scope(); }
            }
            ASTNode::WhileStmt(d) => {
                self.check_expr(&d.condition);
                if !matches!(self.infer(&d.condition), TypeNode::Builtin(TokenType::Bool)) {
                    self.add_error(TypeErrorType::NonBooleanCondStmt, d.line, d.column, "While condition must be boolean".into());
                }
                self.loop_depth += 1; self.enter_scope(); for s in &d.body { self.check_node(s); } self.exit_scope(); self.loop_depth -= 1;
            }
            ASTNode::ForStmt(d) => {
                self.enter_scope();
                if let Some(init) = &d.init { self.check_node(init); }
                if let Some(cond) = &d.condition {
                    self.check_expr(cond);
                    if !matches!(self.infer(cond), TypeNode::Builtin(TokenType::Bool)) {
                        self.add_error(TypeErrorType::NonBooleanCondStmt, d.line, d.column, "For condition must be boolean".into());
                    }
                }
                if let Some(update) = &d.update { self.check_expr(update); }
                self.loop_depth += 1; self.check_node(&d.body); self.loop_depth -= 1;
                self.exit_scope();
            }
            ASTNode::SwitchStmt(d) => {
                self.check_expr(&d.expression);
                let et = self.infer(&d.expression);
                if !matches!(et, TypeNode::Builtin(TokenType::Int | TokenType::Char)) {
                    self.add_error(TypeErrorType::NonBooleanSwitchExpression, d.line, d.column, "Switch expression must be int/char".into());
                }
                self.switch_depth += 1;
                for case in &d.cases {
                    if let ASTNode::CaseBlock(cb) = case {
                        if !self.are_types_equal(&et, &self.infer(&cb.value)) {
                            self.add_error(TypeErrorType::InvalidCaseValueType, cb.line, cb.column, "Case value mismatch".into());
                        }
                        self.enter_scope(); for s in &cb.body { self.check_node(s); } self.exit_scope();
                    }
                }
                if !d.default_body.is_empty() { self.enter_scope(); for s in &d.default_body { self.check_node(s); } self.exit_scope(); }
                self.switch_depth -= 1;
            }
            ASTNode::ReturnStmt(d) => {
                self.found_return_stmt = true;
                let expected = self.current_fn_return_type.clone().unwrap_or(TypeNode::Builtin(TokenType::Void));
                if let Some(val) = &d.value {
                    self.check_expr(val);
                    if matches!(expected, TypeNode::Builtin(TokenType::Void)) {
                        self.add_error(TypeErrorType::ReturnStmtInVoid, d.line, d.column, "Void function returning value".into());
                    } else if !self.are_types_equal(&expected, &self.infer(val)) {
                        self.add_error(TypeErrorType::ErroneousReturnType, d.line, d.column, "Incorrect return type".into());
                    }
                } else if !matches!(expected, TypeNode::Builtin(TokenType::Void)) {
                    self.add_error(TypeErrorType::ErroneousReturnType, d.line, d.column, "Function requires return value".into());
                }
            }
            ASTNode::BreakStmt(d) => if self.loop_depth == 0 && self.switch_depth == 0 {
                self.add_error(TypeErrorType::ErroneousBreak, d.line, d.column, "Break outside loop/switch".into());
            }
            ASTNode::ExpressionStmt(d) => self.check_expr(&d.expr),
            ASTNode::BlockStmt(d) => { self.enter_scope(); for s in &d.body { self.check_node(s); } self.exit_scope(); }
            _ => {}
        }
    }

    fn check_expr(&mut self, expr: &ASTNode) {
        match expr {
            ASTNode::BinaryExpr(b) => {
                self.check_expr(&b.left); self.check_expr(&b.right);
                let lt = self.infer(&b.left); let rt = self.infer(&b.right);
                match b.op {
                    TokenType::And | TokenType::Or => if !self.are_types_equal(&lt, &TypeNode::Builtin(TokenType::Bool)) || !self.are_types_equal(&rt, &TypeNode::Builtin(TokenType::Bool)) {
                        self.add_error(TypeErrorType::AttemptedBoolOpOnNonBools, b.line, b.column, "Logic op on non-bools".into());
                    }
                    TokenType::BitAnd | TokenType::BitOr | TokenType::BitXor => if !self.are_types_equal(&lt, &TypeNode::Builtin(TokenType::Int)) || !self.are_types_equal(&rt, &TypeNode::Builtin(TokenType::Int)) {
                        self.add_error(TypeErrorType::AttemptedBitOpOnNonInt, b.line, b.column, "Bitwise op on non-int".into());
                    }
                    TokenType::BitLShift | TokenType::BitRShift => if !self.are_types_equal(&lt, &TypeNode::Builtin(TokenType::Int)) || !self.are_types_equal(&rt, &TypeNode::Builtin(TokenType::Int)) {
                        self.add_error(TypeErrorType::AttemptedShiftOnNonInt, b.line, b.column, "Shift op on non-int".into());
                    }
                    TokenType::Plus | TokenType::Minus | TokenType::Multiply | TokenType::Divide | TokenType::Modulo => {
                        if !self.is_numeric(&lt) || !self.is_numeric(&rt) { self.add_error(TypeErrorType::AttemptedAddOpOnNonNumeric, b.line, b.column, "Arithmetic on non-numeric".into()); }
                        else if !self.are_types_equal(&lt, &rt) { self.add_error(TypeErrorType::ExpressionTypeMismatch, b.line, b.column, "Arithmetic type mismatch".into()); }
                    }
                    TokenType::EqualOp | TokenType::Ne | TokenType::Gt | TokenType::Lt | TokenType::Ge | TokenType::Le => {
                        if !self.are_types_equal(&lt, &rt) { self.add_error(TypeErrorType::ExpressionTypeMismatch, b.line, b.column, "Comparison type mismatch".into()); }
                    }
                    TokenType::AssignOp => if !self.are_types_equal(&lt, &rt) { self.add_error(TypeErrorType::ExpressionTypeMismatch, b.line, b.column, "Assignment type mismatch".into()); }
                    _ => {}
                }
            }
            ASTNode::UnaryExpr(u) => {
                self.check_expr(&u.operand);
                let t = self.infer(&u.operand);
                match u.op {
                    TokenType::Increment | TokenType::Decrement => if !matches!(t, TypeNode::Builtin(TokenType::Int)) { self.add_error(TypeErrorType::IncrementDecrementOnNonInt, u.line, u.column, "Inc/Dec on non-int".into()); }
                    TokenType::Not => if !matches!(t, TypeNode::Builtin(TokenType::Bool)) { self.add_error(TypeErrorType::NotOnNonBool, u.line, u.column, "NOT on non-bool".into()); }
                    TokenType::Minus => if !self.is_numeric(&t) { self.add_error(TypeErrorType::AttemptedAddOpOnNonNumeric, u.line, u.column, "Unary minus on non-numeric".into()); }
                    _ => {}
                }
            }
            ASTNode::CallExpr(c) => {
                let name = if let ASTNode::Identifier(i) = c.callee.as_ref() { &i.name } else { return };
                if let Some(s) = self.lookup_symbol(name) {
                    if c.args.len() != s.params.len() { self.add_error(TypeErrorType::FnCallParamCount, c.line, c.column, "Arg count mismatch".into()); }
                    else {
                        for (i, arg) in c.args.iter().enumerate() {
                            self.check_expr(arg);
                            if !self.are_types_equal(&self.infer(arg), &s.params[i].0) { self.add_error(TypeErrorType::FnCallParamType, c.line, c.column, format!("Arg {} type mismatch", i+1)); }
                        }
                    }
                }
            }
            _ => {}
        }
    }
}