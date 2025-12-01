
// pub mod lexer;
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
}
