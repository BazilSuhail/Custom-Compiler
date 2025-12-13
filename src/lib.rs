pub mod error;

pub mod lexer {
    pub mod lexer; 
}

pub mod core {
    pub mod ast;
    pub mod token;
}

pub mod parser {
    pub mod parser;
    pub mod ast_printer;
}

pub mod semantics {
    pub mod scope;
    pub mod type_checker;
}

pub mod ir_pipeline {
    pub mod tac;
    pub mod tac_optimizer;
}

pub mod codegen {
    pub mod execution;
}
