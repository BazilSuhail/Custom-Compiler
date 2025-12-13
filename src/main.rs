// main.rs - using lib.rs for module organization
use compiler::lexer::lexer::Lexer;
use compiler::core::token::TokenType;
use compiler::parser::parser::Parser;
//use compiler::parser::ast_printer::print_ast;
use compiler::semantics::scope::ScopeAnalyzer;
use compiler::semantics::type_checker::TypeChecker;
use compiler::ir_pipeline::tac::TACGenerator;
use compiler::ir_pipeline::tac_optimizer::IROptimizer;
use compiler::codegen::execution::ExecutionEngine;
use compiler::error::ErrorReporter;

use std::fs;

fn main() {
    let source = fs::read_to_string("test_input.txt") 
        .expect("failed to read test_input.txt");
    
    let reporter = ErrorReporter::new(&source);
    
    // --- Lexical Analysis ---
    let mut lexer = Lexer::new(source.clone());
    let tokens = lexer.tokenize();

    let has_lex_errors = tokens.iter().any(|t| t.token_type == TokenType::Error);
    if has_lex_errors {
        for token in &tokens {
            if token.token_type == TokenType::Error {
                reporter.report_lexical(&token.value, token.line, token.column);
            }
        }
        return;
    }

    // --- Syntax Analysis ---
    let mut parser = Parser::new(tokens);
    let ast = match parser.parse_program() {
        Ok(ast) => {
            println!("✅ Parsing completed successfully!");
            ast
        }
        Err(err) => {
            reporter.report_syntax(&err.message, err.token.line, err.token.column);
            return;
        }
    };

    // --- Scope Analysis ---
    let mut scope_analyzer = ScopeAnalyzer::new();
    if let Err(errors) = scope_analyzer.analyze(&ast) {
        for error in &errors {
            reporter.report_scope(&error.message, error.line, error.column);
        }
        eprintln!("Scope analysis failed with \x1b[1;31m{} error(s)\x1b[0m", errors.len());
        return;
    }
    println!("✅ Scope analysis completed successfully!");

    // --- Type Analysis ---
    let mut type_checker = TypeChecker::new(scope_analyzer.get_global_scope());
    if let Err(errors) = type_checker.check(&ast) {
        for error in &errors {
            reporter.report_type(&error.message, error.line, error.column);
        }
        eprintln!("Type checking failed with \x1b[1;31m{} error(s)\x1b[0m", errors.len());
        return;
    }
    println!("✅ Type checking completed successfully!");
    println!("No semantic errors found.");

    // --- IR Generation ---
    let mut tac_gen = TACGenerator::new();
    let raw_tac = tac_gen.generate(&ast);
    
    if let Err(e) = tac_gen.save_to_file("three-address-code.txt") {
        eprintln!("❌ Failed to save raw TAC: {}", e);
    } else {
        println!("✅ Raw TAC saved to 'three-address-code.txt'");
    }

    // --- IR Optimization ---
    let mut optimizer = IROptimizer::new(raw_tac);
    optimizer.run();
    let optimized_tac = optimizer.get_instructions();
    
    if let Err(e) = optimizer.save_to_file("optimal-three-address-code.txt") {
        eprintln!("❌ Failed to save optimized TAC: {}", e);
    } else {
        println!("✅ Optimization completed! Saved to 'optimal-three-address-code.txt'");
    }

    // --- Execution ---
    println!("\nExecuting Program Output:");
    let engine = ExecutionEngine::new(optimized_tac);
    if let Err(e) = engine.execute() {
        eprintln!("\n❌ Execution Error: {}", e);
        return;
    }
    println!("✅ Execution finished.");
}
