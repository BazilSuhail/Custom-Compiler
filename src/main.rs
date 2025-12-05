// main.rs - using lib.rs for module organization
use compiler::lexer::lexer::Lexer;
use compiler::core::token::TokenType;
use compiler::parser::parser::Parser;
use compiler::parser::ast_printer::print_ast;
use compiler::semantics::scope::ScopeAnalyzer;
use compiler::semantics::typeChecker::TypeChecker;
use compiler::ir_pipeline::tac::TACGenerator;

use std::fs;
use std::process;

fn main() {
    let source = fs::read_to_string("test_input.txt") 
        .expect("failed to read test_input.txt");
    
    // Lexical Analysis
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();

    // Check for lexical errors
    let has_lex_errors = tokens.iter().any(|t| t.token_type == TokenType::Error);
    if has_lex_errors {
        eprintln!("\n❌ Lexical errors found:");
        for token in &tokens {
            if token.token_type == TokenType::Error {
                eprintln!("  Error at line {}, col {}: {}", token.line, token.column, token.value);
            }
        }
        process::exit(1);
    }

    // Syntax Analysis (Parsing)
    let mut parser = Parser::new(tokens);
    let ast = match parser.parse_program() {
        Ok(ast) => {
            print_ast(&ast);
            println!("✅ Parsing completed successfully!");
            ast
        }
        Err(err) => {
            eprintln!("\n❌ {}", err);
            process::exit(1);
        }
    };

    // Scope Analysis
    let mut scope_analyzer = ScopeAnalyzer::new();
    match scope_analyzer.analyze(&ast) {
        Ok(()) => {
            println!("\n✅ Scope analysis completed successfully!");
            
            // Type Analysis
            let mut type_checker = TypeChecker::new(scope_analyzer.get_global_scope());
            match type_checker.check(&ast) {
                Ok(()) => {
                    println!("✅ Type checking completed successfully!");
                    println!("No semantic errors found.");

                    // IR Generation (Three-Address Code)
                    println!("\n🚀 Generating Three-Address Code (TAC)...");
                    let mut tac_gen = TACGenerator::new();
                    tac_gen.generate(&ast);
                    
                    match tac_gen.save_to_file("tac.txt") {
                        Ok(()) => println!("✅ IR generation completed! Output saved to 'tac.txt'"),
                        Err(e) => eprintln!("❌ Failed to save TAC to file: {}", e),
                    }
                }
                Err(errors) => {
                    eprintln!("\n❌ Type checking errors found:");
                    for error in &errors {
                        eprintln!("  [Type Error] {}", error.message);
                    }
                    eprintln!("Type checking failed with {} error(s)", errors.len());
                    process::exit(1);
                }
            }
        }
        Err(errors) => {
            eprintln!("\n❌ Scope analysis errors found:");
            for error in &errors {
                eprintln!("  [Scope Error] {}", error.message);
            }
            eprintln!("Scope analysis failed with {} error(s)", errors.len());
            process::exit(1);
        }
    }
}
