mod token;
mod lexer;

use lexer::Lexer;
use std::env;
use std::fs;
use std::process;

fn main() {
    let args: Vec<String> = env::args().collect();

    println!("=== Toy Compiler Lexer ===");
    println!("Arguments received: {:?}", args);

    if args.len() < 2 {
        eprintln!("Usage: {} <input_file> [output_file]", args[0]);
        eprintln!("Example: {} test_input.txt", args[0]);
        process::exit(1);
    }

    let input_filename = &args[1];
    let output_filename = if args.len() >= 3 {
        Some(args[2].clone())
    } else {
        None
    };

    println!("Reading file: {}", input_filename);

    // Read input file
    let source = match fs::read_to_string(input_filename) {
        Ok(content) => {
            println!("File read successfully! Size: {} bytes", content.len());
            if content.is_empty() {
                eprintln!("WARNING: Input file is empty!");
            }
            content
        },
        Err(err) => {
            eprintln!("ERROR: Failed to read file '{}': {}", input_filename, err);
            eprintln!("Make sure the file exists and you have permission to read it.");
            process::exit(1);
        }
    };

    println!("\n=== Source Code ===");
    println!("{}", source);
    println!("===================\n");

    // Create lexer and tokenize
    println!("Starting lexical analysis...");
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    println!("Tokenization complete! Found {} tokens\n", tokens.len());

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

    if has_errors {
        println!("\n⚠️  Lexical errors found!\n");
    }

    // Print tokens to console
    println!("=== TOKENS ===");
    if tokens.is_empty() {
        println!("(No tokens found)");
    } else {
        for (i, token) in tokens.iter().enumerate() {
            println!("{:3}. {}", i + 1, token);
        }
    }
    println!("==============\n");

    // Write to output file if specified
    if let Some(output_file) = output_filename {
        let mut output = String::new();
        for token in &tokens {
            output.push_str(&format!(
                "{}({}),{},{}\n",
                token.token_type, token.value, token.line, token.column
            ));
        }

        match fs::write(&output_file, output) {
            Ok(_) => println!("✓ Tokens written to '{}'", output_file),
            Err(err) => {
                eprintln!("ERROR: Failed to write to file '{}': {}", output_file, err);
                process::exit(1);
            }
        }
    }

    if has_errors {
        eprintln!("\nExiting with errors.");
        process::exit(1);
    } else {
        println!("✓ Lexical analysis completed successfully!");
    }
}