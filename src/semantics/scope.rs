use crate::core::ast::*;
use crate::core::token::{TokenType, TypeNode};
use std::collections::{HashMap, HashSet};

// === Symbol Information ===
#[derive(Debug, Clone)]
pub struct SymbolInfo {
    pub symbol_type: TypeNode,
    pub name: String,
    pub line: usize,
    pub column: usize,
    pub is_function: bool,
    pub is_enum: bool,
    pub is_enum_value: bool,
    pub is_prototype: bool,
    pub params: Vec<(TypeNode, String)>,
}

impl SymbolInfo {
    pub fn new_variable(symbol_type: TypeNode, name: String, line: usize, column: usize) -> Self {
        Self::create(symbol_type, name, line, column, false, false, false, false, Vec::new())
    }

    pub fn new_function(return_type: TypeNode, name: String, line: usize, column: usize, params: Vec<(TypeNode, String)>, is_prototype: bool) -> Self {
        Self::create(return_type, name, line, column, true, false, false, is_prototype, params)
    }

    pub fn new_enum(name: String, line: usize, column: usize) -> Self {
        Self::create(TypeNode::Builtin(TokenType::Enum), name, line, column, false, true, false, false, Vec::new())
    }

    pub fn new_enum_value(name: String, line: usize, column: usize) -> Self {
        Self::create(TypeNode::Builtin(TokenType::Int), name, line, column, false, false, true, false, Vec::new())
    }

    fn create(symbol_type: TypeNode, name: String, line: usize, column: usize, is_function: bool, is_enum: bool, is_enum_value: bool, is_prototype: bool, params: Vec<(TypeNode, String)>) -> Self {
        SymbolInfo { symbol_type, name, line, column, is_function, is_enum, is_enum_value, is_prototype, params }
    }
}

// === Scope Frame ===
#[derive(Debug)]
pub struct ScopeFrame {
    pub symbols: HashMap<String, SymbolInfo>,
    pub children: Vec<ScopeFrame>,
    pub level: usize,
}

impl ScopeFrame {
    pub fn new(level: usize) -> Self {
        ScopeFrame { symbols: HashMap::new(), children: Vec::new(), level }
    }

    pub fn has_symbol(&self, name: &str) -> bool { self.symbols.contains_key(name) }
    pub fn find_symbol(&self, name: &str) -> Option<&SymbolInfo> { self.symbols.get(name) }
    pub fn add_symbol(&mut self, sym: SymbolInfo) { self.symbols.insert(sym.name.clone(), sym); }
}

// === Scope Error Handling ===
#[derive(Debug, Clone, PartialEq)]
pub enum ScopeErrorType {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition,
    ConflictingFunctionDefinition,
    ConflictingDeclaration,
    ParameterRedefinition,
    InvalidForwardReference,
    InvalidStorageClassUsage,
    EnumRedefinition,
    EnumVariantRedefinition,
}

#[derive(Debug, Clone)]
pub struct ScopeError {
    pub error_type: ScopeErrorType,
    pub name: String,
    pub line: usize,
    pub column: usize,
    pub message: String,
}

impl ScopeError {
    pub fn new(error_type: ScopeErrorType, name: String, line: usize, column: usize) -> Self {
        let message = match error_type {
            ScopeErrorType::UndeclaredVariableAccessed => format!("Undeclared variable accessed: '{}'", name),
            ScopeErrorType::UndefinedFunctionCalled => format!("Undefined function called: '{}'", name),
            ScopeErrorType::VariableRedefinition => format!("Variable redefinition: '{}'", name),
            ScopeErrorType::FunctionPrototypeRedefinition => format!("Function prototype redefinition: '{}'", name),
            ScopeErrorType::ConflictingFunctionDefinition => format!("Conflicting function definition: '{}'", name),
            ScopeErrorType::ConflictingDeclaration => format!("Conflicting declaration: '{}'", name),
            ScopeErrorType::ParameterRedefinition => format!("Parameter redefinition: '{}'", name),
            ScopeErrorType::InvalidForwardReference => format!("Invalid forward reference: '{}'", name),
            ScopeErrorType::InvalidStorageClassUsage => format!("Invalid storage class usage for: '{}'", name),
            ScopeErrorType::EnumRedefinition => format!("Enum redefinition: '{}'", name),
            ScopeErrorType::EnumVariantRedefinition => format!("Enum variant redefinition: '{}'", name),
        };

        ScopeError { error_type, name, line, column, message }
    }
}

// === Scope Analyzer ===
pub struct ScopeAnalyzer {
    global_scope: ScopeFrame,
    current_scope_stack: Vec<usize>,
    errors: Vec<ScopeError>,
    all_declared_symbols: HashMap<String, Vec<SymbolInfo>>,
}


impl ScopeAnalyzer {
    pub fn new() -> Self {
        ScopeAnalyzer {
            global_scope: ScopeFrame::new(0),
            current_scope_stack: Vec::new(),
            errors: Vec::new(),
            all_declared_symbols: HashMap::new(),
        }
    }

    // --- Navigation Helpers ---
    fn get_scope_at_path(&self, path: &[usize]) -> &ScopeFrame {
        let mut scope = &self.global_scope;
        for &idx in path {
            scope = &scope.children[idx];
        }
        scope
    }

    fn get_current_scope(&self) -> &ScopeFrame {
        self.get_scope_at_path(&self.current_scope_stack)
    }

    fn get_current_scope_mut(&mut self) -> &mut ScopeFrame {
        let mut scope = &mut self.global_scope;
        for &idx in &self.current_scope_stack {
            scope = &mut scope.children[idx];
        }
        scope
    }

    fn enter_scope(&mut self) {
        let level = self.get_current_scope().level + 1;
        let new_index = {
            let current = self.get_current_scope_mut();
            current.children.push(ScopeFrame::new(level));
            current.children.len() - 1
        };
        self.current_scope_stack.push(new_index);
    }

    fn exit_scope(&mut self) {
        self.current_scope_stack.pop();
    }

    // --- Lookup ---
    fn lookup_symbol(&self, name: &str) -> Option<&SymbolInfo> {
        let mut path = self.current_scope_stack.clone();
        
        loop {
            if let Some(sym) = self.get_scope_at_path(&path).find_symbol(name) {
                return Some(sym);
            }
            
            if path.is_empty() {
                break;
            }
            
            path.pop();
        }
        
        None
    }

    fn are_function_signatures_equal(
        &self,
        p1: &[(TypeNode, String)],
        p2: &[(TypeNode, String)],
    ) -> bool {
        p1.len() == p2.len()
            && p1
                .iter()
                .zip(p2)
                .all(|(a, b)| self.are_types_equal(&a.0, &b.0))
    }

    fn are_types_equal(&self, t1: &TypeNode, t2: &TypeNode) -> bool {
        match (t1, t2) {
            (TypeNode::Builtin(a), TypeNode::Builtin(b)) => a == b,
            (TypeNode::UserDefined(a), TypeNode::UserDefined(b)) => a == b,
            _ => false,
        }
    }

    fn add_error(&mut self, err: ScopeErrorType, name: String, ln: usize, col: usize) {
        self.errors
            .push(ScopeError::new(err, name, ln, col));
    }

    // --- Conflict Resolution ---
    fn check_function_conflict(
        &mut self,
        existing: &SymbolInfo,
        name: &str,
        params: &[(TypeNode, String)],
        ln: usize,
        col: usize,
        is_proto: bool,
    ) -> bool {
        if !existing.is_function {
            self.add_error(
                ScopeErrorType::ConflictingDeclaration,
                name.to_string(),
                ln,
                col,
            );
            return true;
        }

        let signatures_match = self.are_function_signatures_equal(&existing.params, params);
        
        if signatures_match {
            if is_proto && existing.is_prototype {
                self.add_error(
                    ScopeErrorType::FunctionPrototypeRedefinition,
                    name.to_string(),
                    ln,
                    col,
                );
                return true;
            } else if !is_proto && !existing.is_prototype {
                self.add_error(
                    ScopeErrorType::ConflictingFunctionDefinition,
                    name.to_string(),
                    ln,
                    col,
                );
                return true;
            }
        } else {
            self.add_error(
                ScopeErrorType::ConflictingFunctionDefinition,
                name.to_string(),
                ln,
                col,
            );
            return true;
        }
        
        false
    }

    // --- Analysis Passes ---
    pub fn analyze(&mut self, ast: &[ASTNode]) -> Result<(), Vec<ScopeError>> {
        self.errors.clear();
        self.all_declared_symbols.clear();

        for node in ast {
            self.collect_declarations(node);
        }
        
        for node in ast {
            self.analyze_ast_node(node);
        }

        if self.errors.is_empty() {
            Ok(())
        } else {
            Err(self.errors.clone())
        }
    }

    fn collect_declarations(&mut self, node: &ASTNode) {
        let sym = match node {
            ASTNode::VarDecl(d) => Some(SymbolInfo::new_variable(
                d.var_type.clone(),
                d.name.clone(),
                d.line,
                d.column,
            )),
            ASTNode::FunctionDecl(d) => Some(SymbolInfo::new_function(
                d.return_type.clone(),
                d.name.clone(),
                d.line,
                d.column,
                d.params.clone(),
                false,
            )),
            ASTNode::FunctionProto(d) => Some(SymbolInfo::new_function(
                d.return_type.clone(),
                d.name.clone(),
                d.line,
                d.column,
                d.params.clone(),
                true,
            )),
            ASTNode::EnumDecl(d) => Some(SymbolInfo::new_enum(
                d.name.clone(),
                d.line,
                d.column,
            )),
            _ => None,
        };
        
        if let Some(s) = sym {
            self.all_declared_symbols
                .entry(s.name.clone())
                .or_default()
                .push(s);
        }
    }

    fn analyze_ast_node(&mut self, node: &ASTNode) {
        match node {
            ASTNode::VarDecl(d) => self.analyze_var_decl(d),
            ASTNode::FunctionDecl(d) => self.analyze_function_decl(d),
            ASTNode::FunctionProto(d) => self.analyze_function_proto(d),
            ASTNode::EnumDecl(d) => self.analyze_enum_decl(d),
            ASTNode::MainDecl(d) => {
                self.enter_scope();
                for s in &d.body {
                    self.analyze_ast_node(s);
                }
                self.exit_scope();
            }
            ASTNode::CallExpr(d) => self.analyze_call_expr(d),
            ASTNode::Identifier(d) => self.analyze_identifier(d),
            ASTNode::BinaryExpr(d) => {
                self.analyze_expression(&d.left);
                self.analyze_expression(&d.right);
            }
            ASTNode::UnaryExpr(d) => self.analyze_expression(&d.operand),
            ASTNode::IfStmt(d) => {
                self.analyze_expression(&d.condition);
                self.enter_scope();
                for s in &d.if_body {
                    self.analyze_ast_node(s);
                }
                self.exit_scope();
                
                if !d.else_body.is_empty() {
                    self.enter_scope();
                    for s in &d.else_body {
                        self.analyze_ast_node(s);
                    }
                    self.exit_scope();
                }
            }
            ASTNode::WhileStmt(d) => {
                self.analyze_expression(&d.condition);
                self.enter_scope();
                for s in &d.body {
                    self.analyze_ast_node(s);
                }
                self.exit_scope();
            }
            ASTNode::DoWhileStmt(d) => {
                self.enter_scope();
                self.analyze_ast_node(&d.body);
                self.exit_scope();
                self.analyze_expression(&d.condition);
            }
            ASTNode::ForStmt(d) => {
                self.enter_scope();
                if let Some(i) = &d.init {
                    self.analyze_ast_node(i);
                }
                if let Some(c) = &d.condition {
                    self.analyze_expression(c);
                }
                if let Some(u) = &d.update {
                    self.analyze_expression(u);
                }
                self.analyze_ast_node(&d.body);
                self.exit_scope();
            }
            ASTNode::SwitchStmt(d) => {
                self.analyze_expression(&d.expression);
                for c in &d.cases {
                    self.enter_scope();
                    self.analyze_ast_node(c);
                    self.exit_scope();
                }
                if !d.default_body.is_empty() {
                    self.enter_scope();
                    for s in &d.default_body {
                        self.analyze_ast_node(s);
                    }
                    self.exit_scope();
                }
            }
            ASTNode::CaseBlock(d) => {
                self.analyze_expression(&d.value);
                for s in &d.body {
                    self.analyze_ast_node(s);
                }
            }
            ASTNode::ReturnStmt(d) => {
                if let Some(v) = &d.value {
                    self.analyze_expression(v);
                }
            }
            ASTNode::PrintStmt(d) => {
                for a in &d.args {
                    self.analyze_expression(a);
                }
            }
            ASTNode::ExpressionStmt(d) => self.analyze_expression(&d.expr),
            ASTNode::BlockStmt(d) => {
                self.enter_scope();
                for s in &d.body {
                    self.analyze_ast_node(s);
                }
                self.exit_scope();
            }
            _ => {}
        }
    }

    fn analyze_var_decl(&mut self, d: &VarDecl) {
        if self.get_current_scope().has_symbol(&d.name) {
            self.add_error(
                ScopeErrorType::VariableRedefinition,
                d.name.clone(),
                d.line,
                d.column,
            );
            return;
        }
        
        if let Some(existing) = self.lookup_symbol(&d.name) {
            if existing.is_function || existing.is_enum || existing.is_enum_value {
                self.add_error(
                    ScopeErrorType::ConflictingDeclaration,
                    d.name.clone(),
                    d.line,
                    d.column,
                );
                return;
            }
        }
        
        self.get_current_scope_mut().add_symbol(SymbolInfo::new_variable(
            d.var_type.clone(),
            d.name.clone(),
            d.line,
            d.column,
        ));
        
        if let Some(i) = &d.initializer {
            self.analyze_expression(i);
        }
    }

    fn analyze_function_decl(&mut self, d: &FunctionDecl) {
        if let Some(local) = self.get_current_scope().find_symbol(&d.name).cloned() {
            if self.check_function_conflict(
                &local,
                &d.name,
                &d.params,
                d.line,
                d.column,
                false,
            ) {
                return;
            }
        } else if let Some(existing) = self.lookup_symbol(&d.name).cloned() {
            if self.check_function_conflict(
                &existing,
                &d.name,
                &d.params,
                d.line,
                d.column,
                false,
            ) {
                return;
            }
        }

        self.get_current_scope_mut().add_symbol(SymbolInfo::new_function(
            d.return_type.clone(),
            d.name.clone(),
            d.line,
            d.column,
            d.params.clone(),
            false,
        ));
        
        self.enter_scope();
        let mut p_names = HashSet::new();
        
        for (pt, pn) in &d.params {
            if !p_names.insert(pn.clone()) {
                self.add_error(
                    ScopeErrorType::ParameterRedefinition,
                    pn.clone(),
                    d.line,
                    d.column,
                );
            } else {
                self.get_current_scope_mut().add_symbol(SymbolInfo::new_variable(
                    pt.clone(),
                    pn.clone(),
                    d.line,
                    d.column,
                ));
            }
        }
        
        for s in &d.body {
            self.analyze_ast_node(s);
        }
        
        self.exit_scope();
    }

    fn analyze_function_proto(&mut self, d: &FunctionProto) {
        if let Some(existing) = self.lookup_symbol(&d.name).cloned() {
            self.check_function_conflict(
                &existing,
                &d.name,
                &d.params,
                d.line,
                d.column,
                true,
            );
        } else {
            self.get_current_scope_mut().add_symbol(SymbolInfo::new_function(
                d.return_type.clone(),
                d.name.clone(),
                d.line,
                d.column,
                d.params.clone(),
                true,
            ));
        }
    }

    fn analyze_enum_decl(&mut self, d: &EnumDecl) {
        if self.get_current_scope().level > 0 {
            self.add_error(
                ScopeErrorType::InvalidStorageClassUsage,
                d.name.clone(),
                d.line,
                d.column,
            );
            return;
        }
        
        if self.get_current_scope().has_symbol(&d.name) {
            self.add_error(
                ScopeErrorType::EnumRedefinition,
                d.name.clone(),
                d.line,
                d.column,
            );
            return;
        }
        
        if self.lookup_symbol(&d.name).is_some() {
            self.add_error(
                ScopeErrorType::ConflictingDeclaration,
                d.name.clone(),
                d.line,
                d.column,
            );
            return;
        }

        self.get_current_scope_mut().add_symbol(SymbolInfo::new_enum(
            d.name.clone(),
            d.line,
            d.column,
        ));
        
        if let ASTNode::EnumValueList(vl) = d.values.as_ref() {
            let mut v_names = HashSet::new();
            
            for v in &vl.values {
                if !v_names.insert(v.clone()) {
                    self.add_error(
                        ScopeErrorType::EnumVariantRedefinition,
                        v.clone(),
                        d.line,
                        d.column,
                    );
                } else if self.lookup_symbol(v).is_some() {
                    self.add_error(
                        ScopeErrorType::ConflictingDeclaration,
                        v.clone(),
                        d.line,
                        d.column,
                    );
                } else {
                    self.get_current_scope_mut().add_symbol(SymbolInfo::new_enum_value(
                        v.clone(),
                        d.line,
                        d.column,
                    ));
                }
            }
        }
    }

    fn analyze_call_expr(&mut self, d: &CallExpr) {
        if let ASTNode::Identifier(i) = d.callee.as_ref() {
            if let Some(s) = self.lookup_symbol(&i.name) {
                if !s.is_function {
                    self.add_error(
                        ScopeErrorType::UndefinedFunctionCalled,
                        i.name.clone(),
                        i.line,
                        i.column,
                    );
                }
            } else if self.all_declared_symbols.contains_key(&i.name) {
                self.add_error(
                    ScopeErrorType::InvalidForwardReference,
                    i.name.clone(),
                    i.line,
                    i.column,
                );
            } else {
                self.add_error(
                    ScopeErrorType::UndefinedFunctionCalled,
                    i.name.clone(),
                    i.line,
                    i.column,
                );
            }
        } else {
            self.analyze_expression(&d.callee);
        }
        
        for a in &d.args {
            self.analyze_expression(a);
        }
    }

    fn analyze_identifier(&mut self, d: &Identifier) {
        if self.lookup_symbol(&d.name).is_none() {
            if self.all_declared_symbols.contains_key(&d.name) {
                self.add_error(
                    ScopeErrorType::InvalidForwardReference,
                    d.name.clone(),
                    d.line,
                    d.column,
                );
            } else {
                self.add_error(
                    ScopeErrorType::UndeclaredVariableAccessed,
                    d.name.clone(),
                    d.line,
                    d.column,
                );
            }
        }
    }

    fn analyze_expression(&mut self, expr: &ASTNode) {
        match expr {
            ASTNode::Identifier(i) => self.analyze_identifier(i),
            ASTNode::BinaryExpr(b) => {
                self.analyze_expression(&b.left);
                self.analyze_expression(&b.right);
            }
            ASTNode::UnaryExpr(u) => self.analyze_expression(&u.operand),
            ASTNode::CallExpr(c) => self.analyze_call_expr(c),
            _ => {}
        }
    }

    pub fn get_global_scope(&self) -> &ScopeFrame {
        &self.global_scope
    }
}