use crate::ast::*;
use crate::token::TokenType;

const RESET: &str = "\x1b[0m";
const BOLD: &str = "\x1b[1m";

// Color codes
const MAGENTA: &str = "\x1b[95m";
const YELLOW: &str = "\x1b[93m";
const GREEN: &str = "\x1b[92m";
const CYAN: &str = "\x1b[96m";
const BLUE: &str = "\x1b[94m";
const RED: &str = "\x1b[91m";
const WHITE: &str = "\x1b[97m";
const PURPLE: &str = "\x1b[35m";

pub fn print_ast(nodes: &[ASTNode]) {
    println!("\n{}{}=== Abstract Syntax Tree ==={}", BOLD, CYAN, RESET);
    for node in nodes {
        print_ast_node(node, 0);
    }
    println!("{}{}============================{}\n", BOLD, CYAN, RESET);
}

pub fn print_ast_node(node: &ASTNode, indent: usize) {
    let spacing = " ".repeat(indent);

    match node {
        ASTNode::IntLiteral(n) => {
            println!("{}{}{}IntLiteral({}){}", spacing, BOLD, MAGENTA, n.value, RESET);
        }
        ASTNode::FloatLiteral(n) => {
            println!("{}{}{}FloatLiteral({}){}", spacing, BOLD, MAGENTA, n.value, RESET);
        }
        ASTNode::StringLiteral(n) => {
            println!("{}{}{}StringLiteral(\"{}\"){}", spacing, BOLD, YELLOW, n.value, RESET);
        }
        ASTNode::CharLiteral(n) => {
            println!("{}{}{}CharLiteral('{}')){}", spacing, BOLD, YELLOW, n.value, RESET);
        }
        ASTNode::BoolLiteral(n) => {
            println!("{}{}{}BoolLiteral({}){}", spacing, BOLD, PURPLE, n.value, RESET);
        }
        ASTNode::Identifier(n) => {
            println!("{}{}{}Identifier(\"{}\")){}", spacing, BOLD, GREEN, n.name, RESET);
        }
        ASTNode::BinaryExpr(n) => {
            println!("{}{}{}BinaryExpr({}){}", spacing, BOLD, YELLOW, token_type_to_op(&n.op), RESET);
            print_ast_node(&n.left, indent + 2);
            print_ast_node(&n.right, indent + 2);
        }
        ASTNode::UnaryExpr(n) => {
            if n.is_postfix {
                println!("{}{}{}PostfixExpr({}){}", spacing, BOLD, RED, token_type_to_op(&n.op), RESET);
            } else {
                println!("{}{}{}UnaryExpr({}){}", spacing, BOLD, RED, token_type_to_op(&n.op), RESET);
            }
            print_ast_node(&n.operand, indent + 2);
        }
        ASTNode::CallExpr(n) => {
            println!("{}{}{}CallExpr{}", spacing, BOLD, BLUE, RESET);
            println!("{}  Callee:", spacing);
            print_ast_node(&n.callee, indent + 4);
            if !n.args.is_empty() {
                println!("{}  Args:", spacing);
                for arg in &n.args {
                    print_ast_node(arg, indent + 4);
                }
            }
        }
        ASTNode::VarDecl(n) => {
            println!("{}{}{}VarDecl({}, \"{}\")){}", 
                spacing, BOLD, CYAN, token_type_to_string(&n.var_type), n.name, RESET);
            if let Some(init) = &n.initializer {
                print_ast_node(init, indent + 2);
            }
        }
        ASTNode::FunctionProto(n) => {
            println!("{}{}{}FunctionProto({}, \"{}\")){}", 
                spacing, BOLD, YELLOW, token_type_to_string(&n.return_type), n.name, RESET);
            for (param_type, param_name) in &n.params {
                println!("{}  Param: {} {}", spacing, token_type_to_string(param_type), param_name);
            }
        }
        ASTNode::FunctionDecl(n) => {
            println!("{}{}{}FunctionDecl({}, \"{}\")){}", 
                spacing, BOLD, YELLOW, token_type_to_string(&n.return_type), n.name, RESET);
            for (param_type, param_name) in &n.params {
                println!("{}  Param: {} {}", spacing, token_type_to_string(param_type), param_name);
            }
            println!("{}  Body:", spacing);
            for stmt in &n.body {
                print_ast_node(stmt, indent + 4);
            }
        }
        ASTNode::MainDecl(n) => {
            println!("{}{}{}MainDecl{}", spacing, BOLD, YELLOW, RESET);
            for stmt in &n.body {
                print_ast_node(stmt, indent + 2);
            }
        }
        ASTNode::IfStmt(n) => {
            println!("{}{}{}IfStmt{}", spacing, BOLD, PURPLE, RESET);
            println!("{}  Condition:", spacing);
            print_ast_node(&n.condition, indent + 4);
            println!("{}  IfBody:", spacing);
            for stmt in &n.if_body {
                print_ast_node(stmt, indent + 4);
            }
            if !n.else_body.is_empty() {
                println!("{}  ElseBody:", spacing);
                for stmt in &n.else_body {
                    print_ast_node(stmt, indent + 4);
                }
            }
        }
        ASTNode::WhileStmt(n) => {
            println!("{}{}{}WhileStmt{}", spacing, BOLD, RED, RESET);
            println!("{}  Condition:", spacing);
            print_ast_node(&n.condition, indent + 4);
            println!("{}  Body:", spacing);
            for stmt in &n.body {
                print_ast_node(stmt, indent + 4);
            }
        }
        ASTNode::DoWhileStmt(n) => {
            println!("{}{}{}DoWhileStmt{}", spacing, BOLD, RED, RESET);
            println!("{}  Body:", spacing);
            print_ast_node(&n.body, indent + 4);
            println!("{}  Condition:", spacing);
            print_ast_node(&n.condition, indent + 4);
        }
        ASTNode::ForStmt(n) => {
            println!("{}{}{}ForStmt{}", spacing, BOLD, RED, RESET);
            if let Some(init) = &n.init {
                println!("{}  Init:", spacing);
                print_ast_node(init, indent + 4);
            }
            if let Some(cond) = &n.condition {
                println!("{}  Condition:", spacing);
                print_ast_node(cond, indent + 4);
            }
            if let Some(update) = &n.update {
                println!("{}  Update:", spacing);
                print_ast_node(update, indent + 4);
            }
            println!("{}  Body:", spacing);
            print_ast_node(&n.body, indent + 4);
        }
        ASTNode::CaseBlock(n) => {
            println!("{}{}{}CaseBlock{}", spacing, BOLD, WHITE, RESET);
            println!("{}  Value:", spacing);
            print_ast_node(&n.value, indent + 4);
            println!("{}  Body:", spacing);
            for stmt in &n.body {
                print_ast_node(stmt, indent + 4);
            }
        }
        ASTNode::SwitchStmt(n) => {
            println!("{}{}{}SwitchStmt{}", spacing, BOLD, WHITE, RESET);
            println!("{}  Expression:", spacing);
            print_ast_node(&n.expression, indent + 4);
            println!("{}  Cases:", spacing);
            for case in &n.cases {
                print_ast_node(case, indent + 4);
            }
            if !n.default_body.is_empty() {
                println!("{}  Default:", spacing);
                for stmt in &n.default_body {
                    print_ast_node(stmt, indent + 4);
                }
            }
        }
        ASTNode::ReturnStmt(n) => {
            println!("{}{}{}ReturnStmt{}", spacing, BOLD, BLUE, RESET);
            if let Some(val) = &n.value {
                print_ast_node(val, indent + 2);
            }
        }
        ASTNode::BreakStmt(_) => {
            println!("{}{}BreakStmt{}", spacing, RED, RESET);
        }
        ASTNode::PrintStmt(n) => {
            println!("{}{}{}PrintStmt{}", spacing, BOLD, GREEN, RESET);
            for arg in &n.args {
                print_ast_node(arg, indent + 2);
            }
        }
        ASTNode::BlockStmt(n) => {
            println!("{}{}{}BlockStmt{}", spacing, BOLD, WHITE, RESET);
            for stmt in &n.body {
                print_ast_node(stmt, indent + 2);
            }
        }
        ASTNode::ExpressionStmt(n) => {
            println!("{}{}{}ExpressionStmt{}", spacing, BOLD, WHITE, RESET);
            print_ast_node(&n.expr, indent + 2);
        }
        ASTNode::IncludeStmt(n) => {
            println!("{}{}IncludeStmt(\"{}\")){}", spacing, BLUE, n.header, RESET);
        }
        ASTNode::EnumValueList(n) => {
            print!("{}{}{}EnumValueList(", spacing, BOLD, WHITE);
            for (i, val) in n.values.iter().enumerate() {
                if i > 0 {
                    print!(", ");
                }
                print!("{}", val);
            }
            println!("){}", RESET);
        }
        ASTNode::EnumDecl(n) => {
            println!("{}{}{}EnumDecl(\"{}\")){}", spacing, BOLD, WHITE, n.name, RESET);
            println!("{}  Values:", spacing);
            print_ast_node(&n.values, indent + 4);
        }
    }
}

fn token_type_to_string(token_type: &TokenType) -> &'static str {
    match token_type {
        TokenType::Int => "int",
        TokenType::Float => "float",
        TokenType::Double => "double",
        TokenType::Char => "char",
        TokenType::Bool => "bool",
        TokenType::Void => "void",
        TokenType::String => "string",
        _ => "unknown",
    }
}

fn token_type_to_op(token_type: &TokenType) -> &'static str {
    match token_type {
        TokenType::Plus => "+",
        TokenType::Minus => "-",
        TokenType::Multiply => "*",
        TokenType::Divide => "/",
        TokenType::Modulo => "%",
        TokenType::Increment => "++",
        TokenType::Decrement => "--",
        TokenType::Not => "!",
        TokenType::EqualOp => "==",
        TokenType::Ne => "!=",
        TokenType::Lt => "<",
        TokenType::Gt => ">",
        TokenType::Le => "<=",
        TokenType::Ge => ">=",
        TokenType::And => "&&",
        TokenType::Or => "||",
        TokenType::AssignOp => "=",
        TokenType::BitAnd => "&",
        TokenType::BitOr => "|",
        TokenType::BitXor => "^",
        TokenType::BitLShift => "<<",
        TokenType::BitRShift => ">>",
        _ => "unknown_op",
    }
}
