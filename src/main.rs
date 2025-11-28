mod token;
mod lexer;
mod ast;
mod parser;
mod ast_printer;

use lexer::Lexer;
use parser::Parser;
use ast_printer::print_ast;
//use std::env;
use std::fs;
use std::process;

fn main() {
    // let args: Vec<String> = env::args().collect();

    // if args.len() < 2 {
    //     eprintln!("Usage: {} <input_file>", args[0]);
    //     process::exit(1);
    // }

    // let filename = &args[1];

    // let source = match fs::read_to_string(filename) {
    //     Ok(content) => content,
    //     Err(err) => {
    //         eprintln!("Error reading file '{}': {}", filename, err);
    //         process::exit(1);
    //     }
    // };
    let source = fs::read_to_string("test_input.txt")
.expect("failed to read test_input.txt");
    
    // Lexical Analysis
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    // Check for lexical errors
    let has_lex_errors = tokens.iter().any(|t| t.token_type == token::TokenType::Error);
    if has_lex_errors {
        eprintln!("\n❌ Lexical errors found:");
        for token in &tokens {
            if token.token_type == token::TokenType::Error {
                eprintln!("  Error at line {}, col {}: {}", token.line, token.column, token.value);
            }
        }
        process::exit(1);
    }

    // Syntax Analysis (Parsing)
    let mut parser = Parser::new(tokens);
    match parser.parse_program() {
        Ok(ast) => {
            print_ast(&ast);
            println!("✅ Parsing completed successfully!");
        }
        Err(err) => {
            eprintln!("\n❌ {}", err);
            process::exit(1);
        }
    }
}