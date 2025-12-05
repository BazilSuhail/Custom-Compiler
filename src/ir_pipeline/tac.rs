use crate::core::ast::*;
use crate::core::token::{TokenType, TypeNode};
use std::fmt;
use std::collections::HashMap;

#[derive(Debug, Clone)]
pub enum Operand {
    Temp(usize),
    Var(String),
    Int(i64),
    Float(f64),
    Bool(bool),
    Char(char),
    String(String),
    Label(String),
}

impl fmt::Display for Operand {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Operand::Temp(i) => write!(f, "t{}", i),
            Operand::Var(s) => write!(f, "{}", s),
            Operand::Int(i) => write!(f, "{}", i),
            Operand::Float(fl) => write!(f, "{}", fl),
            Operand::Bool(b) => write!(f, "{}", b),
            Operand::Char(c) => write!(f, "'{}'", c),
            Operand::String(s) => write!(f, "\"{}\"", s),
            Operand::Label(l) => write!(f, "{}", l),
        }
    }
}

#[derive(Debug, Clone)]
pub enum Instruction {
    // Typed assignment for declarations: "type var = src"
    Declare(String, String, Option<Operand>),
    // Standard assignment: "dest = src"
    Assign(Operand, Operand),
    Binary(Operand, TokenType, Operand, Operand),
    Unary(Operand, TokenType, Operand),
    Label(String),
    Goto(String),
    IfGoto(Operand, TokenType, Operand, String),
    Param(Operand),
    // Call(Dest, FuncName, ArgCount)
    Call(Option<Operand>, String, usize),
    Return(Option<Operand>),
    // FuncStart(Name, ReturnType, Params)
    FuncStart(String, String, Vec<(String, String)>),
    FuncEnd,
    Print(Vec<Operand>),
    Comment(String),
}

fn type_node_to_str(t: &TypeNode) -> String {
    match t {
        TypeNode::Builtin(tok) => match tok {
            TokenType::Int => "int".to_string(),
            TokenType::Float => "float".to_string(),
            TokenType::Double => "double".to_string(),
            TokenType::Char => "char".to_string(),
            TokenType::Bool => "bool".to_string(),
            TokenType::Void => "void".to_string(),
            TokenType::String => "string".to_string(),
            _ => "unknown".to_string(),
        },
        TypeNode::UserDefined(name) => name.clone(),
    }
}

impl fmt::Display for Instruction {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Instruction::Declare(t, name, init) => {
                if let Some(val) = init {
                    write!(f, "{} {} = {}", t, name, val)
                } else {
                    write!(f, "{} {}", t, name)
                }
            }
            Instruction::Assign(dest, src) => write!(f, "{} = {}", dest, src),
            Instruction::Binary(dest, op, l, r) => write!(f, "{} = {} {:?} {}", dest, l, op, r),
            Instruction::Unary(dest, op, src) => write!(f, "{} = {:?} {}", dest, op, src),
            Instruction::Label(l) => write!(f, "{}:", l),
            Instruction::Goto(l) => write!(f, "goto {}", l),
            Instruction::IfGoto(l, op, r, lbl) => write!(f, "if {} {:?} {} goto {}", l, op, r, lbl),
            Instruction::Param(p) => write!(f, "param {}", p),
            Instruction::Call(dest, func, n) => {
                if let Some(d) = dest {
                    write!(f, "{} = call {}, {}", d, func, n)
                } else {
                    write!(f, "call {}, {}", func, n)
                }
            }
            Instruction::Return(val) => {
                if let Some(v) = val {
                    write!(f, "return {}", v)
                } else {
                    write!(f, "return")
                }
            }
            Instruction::FuncStart(name, ret, params) => {
                let p_str: Vec<String> = params.iter().map(|(t, n)| format!("{} {}", t, n)).collect();
                write!(f, "\n{} {}({}) begin:", ret, name, p_str.join(", "))
            }
            Instruction::FuncEnd => write!(f, "end\n"),
            Instruction::Print(args) => {
                let args_str: Vec<String> = args.iter().map(|a| a.to_string()).collect();
                write!(f, "print {}", args_str.join(", "))
            }
            Instruction::Comment(c) => write!(f, "; {}", c),
        }
    }
}

pub struct TACGenerator {
    instructions: Vec<Instruction>,
    temp_count: usize,
    label_count: usize,
    break_stack: Vec<String>,
    // Enum value mapping: ConstantName -> IntegerID
    enum_map: HashMap<String, i64>,
}

impl TACGenerator {
    pub fn new() -> Self {
        Self {
            instructions: Vec::new(),
            temp_count: 0,
            label_count: 0,
            break_stack: Vec::new(),
            enum_map: HashMap::new(),
        }
    }

    fn new_temp(&mut self) -> Operand {
        let t = Operand::Temp(self.temp_count);
        self.temp_count += 1;
        t
    }

    fn new_label(&mut self) -> String {
        let l = format!("L{}", self.label_count);
        self.label_count += 1;
        l
    }

    fn emit(&mut self, instr: Instruction) {
        self.instructions.push(instr);
    }

    pub fn generate(&mut self, ast: &[ASTNode]) -> Vec<Instruction> {
        // Pass 1: Collect Enums
        self.pre_scan_enums(ast);

        // Pass 2: Generate Code
        for node in ast {
            self.gen_node(node);
        }
        self.instructions.clone()
    }

    fn pre_scan_enums(&mut self, ast: &[ASTNode]) {
        for node in ast {
            if let ASTNode::EnumDecl(d) = node {
                if let ASTNode::EnumValueList(list) = d.values.as_ref() {
                    for (i, val) in list.values.iter().enumerate() {
                        self.enum_map.insert(val.clone(), i as i64);
                    }
                }
            }
        }
    }

    fn gen_node(&mut self, node: &ASTNode) -> Option<Operand> {
        match node {
            ASTNode::IntLiteral(n) => Some(Operand::Int(n.value)),
            ASTNode::FloatLiteral(n) => Some(Operand::Float(n.value)),
            ASTNode::StringLiteral(n) => Some(Operand::String(n.value.clone())),
            ASTNode::CharLiteral(n) => Some(Operand::Char(n.value)),
            ASTNode::BoolLiteral(n) => Some(Operand::Bool(n.value)),
            ASTNode::Identifier(n) => {
                // If it's an enum constant, replace with its ID
                if let Some(&id) = self.enum_map.get(&n.name) {
                    Some(Operand::Int(id))
                } else {
                    Some(Operand::Var(n.name.clone()))
                }
            }

            ASTNode::BinaryExpr(b) => {
                let left = self.gen_node(&b.left)?;
                let right = self.gen_node(&b.right)?;
                let dest = self.new_temp();
                self.emit(Instruction::Binary(dest.clone(), b.op, left, right));
                Some(dest)
            }

            ASTNode::UnaryExpr(u) => {
                if u.is_postfix {
                    let operand = self.gen_node(&u.operand)?;
                    let t = self.new_temp();
                    self.emit(Instruction::Assign(t.clone(), operand.clone()));
                    let op = if u.op == TokenType::Increment { TokenType::Plus } else { TokenType::Minus };
                    self.emit(Instruction::Binary(operand.clone(), op, operand, Operand::Int(1)));
                    Some(t)
                } else {
                    let operand = self.gen_node(&u.operand)?;
                    if u.op == TokenType::Increment || u.op == TokenType::Decrement {
                        let op = if u.op == TokenType::Increment { TokenType::Plus } else { TokenType::Minus };
                        self.emit(Instruction::Binary(operand.clone(), op, operand.clone(), Operand::Int(1)));
                        Some(operand)
                    } else {
                        let dest = self.new_temp();
                        self.emit(Instruction::Unary(dest.clone(), u.op, operand));
                        Some(dest)
                    }
                }
            }

            ASTNode::VarDecl(d) => {
                let t_str = type_node_to_str(&d.var_type);
                let init = if let Some(init_node) = &d.initializer {
                    self.gen_node(init_node)
                } else {
                    None
                };
                self.emit(Instruction::Declare(t_str, d.name.clone(), init));
                None
            }

            ASTNode::CallExpr(c) => {
                let mut arg_ops = Vec::new();
                for arg in &c.args {
                    if let Some(op) = self.gen_node(arg) { arg_ops.push(op); }
                }
                for arg in &arg_ops { self.emit(Instruction::Param(arg.clone())); }
                
                let func_name = if let ASTNode::Identifier(i) = c.callee.as_ref() { i.name.clone() } else { "unknown".to_string() };
                let dest = self.new_temp();
                self.emit(Instruction::Call(Some(dest.clone()), func_name, arg_ops.len()));
                Some(dest)
            }

            ASTNode::FunctionDecl(d) => {
                let ret_t = type_node_to_str(&d.return_type);
                let params = d.params.iter().map(|(t, n)| (type_node_to_str(t), n.clone())).collect();
                self.emit(Instruction::FuncStart(d.name.clone(), ret_t, params));
                for stmt in &d.body { self.gen_node(stmt); }
                self.emit(Instruction::FuncEnd);
                None
            }

            ASTNode::MainDecl(d) => {
                self.emit(Instruction::FuncStart("main".to_string(), "void".into(), vec![]));
                for stmt in &d.body { self.gen_node(stmt); }
                self.emit(Instruction::FuncEnd);
                None
            }

            ASTNode::IfStmt(d) => {
                let l_else = self.new_label();
                let l_end = self.new_label();
                let cond = self.gen_node(&d.condition)?;
                self.emit(Instruction::IfGoto(cond, TokenType::EqualOp, Operand::Bool(false), l_else.clone()));
                for stmt in &d.if_body { self.gen_node(stmt); }
                self.emit(Instruction::Goto(l_end.clone()));
                self.emit(Instruction::Label(l_else));
                for stmt in &d.else_body { self.gen_node(stmt); }
                self.emit(Instruction::Label(l_end));
                None
            }

            ASTNode::WhileStmt(d) => {
                let l_start = self.new_label();
                let l_end = self.new_label();
                self.break_stack.push(l_end.clone());
                self.emit(Instruction::Label(l_start.clone()));
                let cond = self.gen_node(&d.condition)?;
                self.emit(Instruction::IfGoto(cond, TokenType::EqualOp, Operand::Bool(false), l_end.clone()));
                for stmt in &d.body { self.gen_node(stmt); }
                self.emit(Instruction::Goto(l_start));
                self.emit(Instruction::Label(l_end));
                self.break_stack.pop();
                None
            }

            ASTNode::DoWhileStmt(d) => {
                let l_start = self.new_label();
                let l_end = self.new_label();
                self.break_stack.push(l_end.clone());
                self.emit(Instruction::Label(l_start.clone()));
                self.gen_node(&d.body);
                let cond = self.gen_node(&d.condition)?;
                self.emit(Instruction::IfGoto(cond, TokenType::EqualOp, Operand::Bool(true), l_start));
                self.emit(Instruction::Label(l_end));
                self.break_stack.pop();
                None
            }

            ASTNode::ForStmt(d) => {
                let l_start = self.new_label();
                let l_end = self.new_label();
                if let Some(init) = &d.init { self.gen_node(init); }
                self.break_stack.push(l_end.clone());
                self.emit(Instruction::Label(l_start.clone()));
                if let Some(cond) = &d.condition {
                    let c = self.gen_node(cond)?;
                    self.emit(Instruction::IfGoto(c, TokenType::EqualOp, Operand::Bool(false), l_end.clone()));
                }
                self.gen_node(&d.body);
                if let Some(update) = &d.update { self.gen_node(update); }
                self.emit(Instruction::Goto(l_start));
                self.emit(Instruction::Label(l_end));
                self.break_stack.pop();
                None
            }

            ASTNode::SwitchStmt(d) => {
                let l_end = self.new_label();
                let expr = self.gen_node(&d.expression)?;
                self.break_stack.push(l_end.clone());
                for case in &d.cases {
                    if let ASTNode::CaseBlock(cb) = case {
                        let val = self.gen_node(&cb.value)?;
                        let l_next = self.new_label();
                        self.emit(Instruction::IfGoto(expr.clone(), TokenType::Ne, val, l_next.clone()));
                        for stmt in &cb.body { self.gen_node(stmt); }
                        self.emit(Instruction::Label(l_next));
                    }
                }
                for stmt in &d.default_body { self.gen_node(stmt); }
                self.emit(Instruction::Label(l_end));
                self.break_stack.pop();
                None
            }

            ASTNode::ReturnStmt(d) => {
                let val = if let Some(v) = &d.value { self.gen_node(v) } else { None };
                self.emit(Instruction::Return(val));
                None
            }

            ASTNode::BreakStmt(_) => {
                if let Some(l) = self.break_stack.last() {
                    let lbl = l.clone();
                    self.emit(Instruction::Goto(lbl));
                }
                None
            }

            ASTNode::PrintStmt(d) => {
                let mut ops = Vec::new();
                for arg in &d.args {
                    if let Some(op) = self.gen_node(arg) { ops.push(op); }
                }
                self.emit(Instruction::Print(ops));
                None
            }

            ASTNode::BlockStmt(d) => {
                for stmt in &d.body { self.gen_node(stmt); }
                None
            }

            ASTNode::ExpressionStmt(d) => {
                self.gen_node(&d.expr);
                None
            }

            ASTNode::EnumDecl(d) => {
                self.emit(Instruction::Comment(format!("Enum {} Defined", d.name)));
                None
            }
            
            _ => None,
        }
    }

    pub fn save_to_file(&self, filename: &str) -> std::io::Result<()> {
        let mut content = String::new();
        for instr in &self.instructions {
            content.push_str(&format!("{}\n", instr));
        }
        std::fs::write(filename, content)
    }
}