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
           for instr in &mut self.instructions {
            if let Instruction::Binary(dest, op, l, r) = instr {
                if let (Operand::Int(lv), Operand::Int(rv)) = (l, r) {
                    let result = match op {
                        TokenType::Plus => Some(Operand::Int(lv + rv)),
                        TokenType::Minus => Some(Operand::Int(lv - rv)),
                        TokenType::Multiply => Some(Operand::Int(lv * rv)),
                        TokenType::Divide => if *rv != 0 { Some(Operand::Int(lv / rv)) } else { None },
                        _ => None,
                    };

                    if let Some(res_op) = result {
                        *instr = Instruction::Assign(dest.clone(), res_op);
                    }
                }
            }
        }
    }

    /// Tracks variables assigned to constants and replaces their uses.
    /// Example: const int a = 5; t0 = a + 10 -> t0 = 5 + 10
    fn constant_propagation(&mut self) {
        use std::collections::HashMap;
        let mut constants: HashMap<String, Operand> = HashMap::new();

        fn get_key(op: &Operand) -> Option<String> {
            match op {
                Operand::Var(name) => Some(name.clone()),
                Operand::Temp(id) => Some(format!("t{}", id)),
                _ => None,
            }
        }

        fn is_literal(op: &Operand) -> bool {
            matches!(op, Operand::Int(_) | Operand::Float(_) | Operand::Bool(_) | Operand::Char(_) | Operand::String(_))
        }

        for instr in &mut self.instructions {
            match instr {
                // Tracking Definitions
                Instruction::Declare(_, name, init) => {
                    if let Some(val) = init {
                        if let Some(key) = get_key(val) {
                            if let Some(c) = constants.get(&key) { *val = c.clone(); }
                        }
                        if is_literal(val) { constants.insert(name.clone(), val.clone()); } 
                        else { constants.remove(name); }
                    } else { constants.remove(name); }
                }

                Instruction::Assign(dest, src) => {
                    // 1. Replace Use
                    if let Some(key) = get_key(src) {
                        if let Some(c) = constants.get(&key) { *src = c.clone(); }
                    }
                    // 2. Track Definition
                    if let Some(dest_key) = get_key(dest) {
                        if is_literal(src) { constants.insert(dest_key, src.clone()); } 
                        else { constants.remove(&dest_key); }
                    }
                }

                Instruction::Binary(dest, _, l, r) => {
                    if let Some(lk) = get_key(l) { if let Some(c) = constants.get(&lk) { *l = c.clone(); } }
                    if let Some(rk) = get_key(r) { if let Some(c) = constants.get(&rk) { *r = c.clone(); } }
                    if let Some(dk) = get_key(dest) { constants.remove(&dk); }
                }

                Instruction::Unary(dest, _, src) => {
                    if let Some(sk) = get_key(src) { if let Some(c) = constants.get(&sk) { *src = c.clone(); } }
                    if let Some(dk) = get_key(dest) { constants.remove(&dk); }
                }

                // Replacing Uses in Control Flow and IO
                Instruction::IfTrue(cond, _) | Instruction::IfFalse(cond, _) => {
                    if let Some(ck) = get_key(cond) { if let Some(c) = constants.get(&ck) { *cond = c.clone(); } }
                }

                Instruction::Param(p) => {
                    if let Some(pk) = get_key(p) { if let Some(c) = constants.get(&pk) { *p = c.clone(); } }
                }

                Instruction::Call(dest, _, _) => {
                    if let Some(d) = dest {
                        if let Some(dk) = get_key(d) { constants.remove(&dk); }
                    }
                }

                Instruction::Return(val) => {
                    if let Some(v) = val {
                        if let Some(vk) = get_key(v) { if let Some(c) = constants.get(&vk) { *v = c.clone(); } }
                    }
                }

                Instruction::Print(args) => {
                    for arg in args {
                        if let Some(ak) = get_key(arg) { if let Some(c) = constants.get(&ak) { *arg = c.clone(); } }
                    }
                }

                // Invalidate tracking on labels/function boundaries
                Instruction::Label(_) | Instruction::FuncStart(_, _, _) => {
                    constants.clear();
                }

                _ => {}
            }
        }
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
        // TOFO peephole optimzation
        
    }

    pub fn save_to_file(&self, filename: &str) -> std::io::Result<()> {
        let mut content = String::new();
        for instr in &self.instructions {
            content.push_str(&format!("{}\n", instr));
        }
        fs::write(filename, content)
    }
}