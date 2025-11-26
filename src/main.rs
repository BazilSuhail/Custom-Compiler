mod token;
mod lexer;

use lexer::Lexer;
use std::env;
use std::fs;
use std::process;

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        eprintln!("Usage: {} <input_file> [output_file]", args[0]);
        process::exit(1);
    }

    let input_filename = &args[1];
    let output_filename = if args.len() >= 3 {
        Some(args[2].clone())
    } else {
        None
    };

    // Read input file
    let source = match fs::read_to_string(input_filename) {
        Ok(content) => content,
        Err(err) => {
            eprintln!("Failed to read file '{}': {}", input_filename, err);
            process::exit(1);
        }
    };

    // Create lexer and tokenize
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    // Display errors
    let mut has_errors = false;
    for token in &tokens {
        if token.token_type == token::TokenType::Error {
            eprintln!(
                "ERROR(line {}, col {}): {}",
                token.line, token.column, token.value
            );
            has_errors = true;
        }
    }

    // Print tokens to console
    println!("\n=== TOKENS ===");
    for token in &tokens {
        println!("{}", token);
    }

    // Write to output file if specified
    if let Some(output_file) = output_filename {
        let mut output = String::new();
        for token in &tokens {
            output.push_str(&format!(
                "{}({}),{},{}\n",
                token.token_type, token.value, token.line, token.column
            ));
        }

        if let Err(err) = fs::write(&output_file, output) {
            eprintln!("Failed to write to file '{}': {}", output_file, err);
            process::exit(1);
        }
        println!("\n=== Tokens written to '{}' ===", output_file);
    }

    if has_errors {
        process::exit(1);
    }
}