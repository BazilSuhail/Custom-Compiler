use crate::core::token::TokenType;
use crate::ir_pipeline::tac::{Instruction, Operand};
use std::fs;

pub struct IROptimizer {
    instructions: Vec<Instruction>,
}

impl IROptimizer {
    pub fn new(instructions: Vec<Instruction>) -> Self {
        Self { instructions }
    }

    pub fn run(&mut self) {
        // Run multiple passes for better results
        self.constant_folding();
        self.constant_propagation();
        self.copy_propagation();
        self.peephole_optimization();
        self.dead_code_elimination();
    }

    pub fn get_instructions(&self) -> Vec<Instruction> {
        self.instructions.clone()
    }

    /// Evaluates expressions with constants at compile time.
    /// Example: t0 = 5 + 10 -> t0 = 15
    fn constant_folding(&mut self) {
     // constant foling
    }

    /// Placeholder for Constant Propagation.
    fn constant_propagation(&mut self) {
        // TODO: Map variables to their constant values and replace uses.
    }

    /// Placeholder for Copy Propagation.
    fn copy_propagation(&mut self) {
        // TODO: Replace temporaries that are direct copies (t1 = t0; t2 = t1 + 5 -> t2 = t0 + 5).
    }

    /// Removes instructions whose results are never used.
    fn dead_code_elimination(&mut self) {
        // TODO: Perform liveness analysis and remove dead assignments.
    }

    /// Simplifies local instruction patterns.
    /// Example: t0 = t1 + 0 -> t0 = t1
    fn peephole_optimization(&mut self) {
        for instr in &mut self.instructions {
            if let Instruction::Binary(dest, op, l, r) = instr {
                let simplified = match (op, l, r) {
                    (TokenType::Plus, val, Operand::Int(0)) => Some(Instruction::Assign(dest.clone(), val.clone())),
                    (TokenType::Plus, Operand::Int(0), val) => Some(Instruction::Assign(dest.clone(), val.clone())),
                    (TokenType::Multiply, val, Operand::Int(1)) => Some(Instruction::Assign(dest.clone(), val.clone())),
                    (TokenType::Multiply, Operand::Int(1), val) => Some(Instruction::Assign(dest.clone(), val.clone())),
                    _ => None,
                };

                if let Some(new_instr) = simplified {
                    *instr = new_instr;
                }
            }
        }
    }

    pub fn save_to_file(&self, filename: &str) -> std::io::Result<()> {
        let mut content = String::new();
        for instr in &self.instructions {
            content.push_str(&format!("{}\n", instr));
        }
        fs::write(filename, content)
    }
}