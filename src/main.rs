mod token;
mod lexer;

use lexer::Lexer;
use std::env;
use std::fs;

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        return;
    }

    let filename = &args[1];
    println!("Reading file: {}", filename);

    let source = fs::read_to_string(filename).expect("Failed to read file");
    
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    for token in tokens {
        println!("{}", token);
    }
}