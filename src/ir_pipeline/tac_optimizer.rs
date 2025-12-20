use crate::core::token::TokenType;
use crate::ir_pipeline::tac::{Instruction, Operand};
use std::fs;
use std::collections::{HashSet, HashMap};

pub struct IROptimizer {
    instructions: Vec<Instruction>,
}

impl IROptimizer {
    pub fn new(instructions: Vec<Instruction>) -> Self {
        Self { instructions }
    }

    pub fn run(&mut self) {
        // Run multiple passes for better results
        for _ in 0..2 {
            self.constant_folding();
            self.constant_propagation();
            self.copy_propagation();
            self.peephole_optimization();
        }
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
                        TokenType::Plus => Some(Operand::Int(*lv + *rv)),
                        TokenType::Minus => Some(Operand::Int(*lv - *rv)),
                        TokenType::Multiply => Some(Operand::Int(*lv * *rv)),
                        TokenType::Divide if *rv != 0 => Some(Operand::Int(*lv / *rv)),
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
        let mut constants: HashMap<String, Operand> = HashMap::new();
        let mut immutable_vars: HashSet<String> = HashSet::new();
        let mut global_vars: HashSet<String> = HashSet::new();
        let mut function_depth: usize = 0;

        // Pre-scan for global constants (declarations before first function)
        for instr in &self.instructions {
            match instr {
                Instruction::FuncStart(_, _, _) => break,
                Instruction::Declare(type_str, name, init) => {
                    global_vars.insert(name.clone());
                    if type_str.contains("const") {
                        immutable_vars.insert(name.clone());
                        if let Some(val) = init {
                            if Self::is_literal(val) {
                                constants.insert(name.clone(), val.clone());
                            }
                        }
                    }
                }
                _ => {}
            }
        }

        // Main propagation pass
        for instr in &mut self.instructions {
            match instr {
                Instruction::Declare(type_str, name, init) => {
                    let is_const = type_str.contains("const");
                    let is_global = function_depth == 0;

                    if is_const {
                        immutable_vars.insert(name.clone());
                    }
                    if is_global {
                        global_vars.insert(name.clone());
                    }

                    if let Some(val) = init {
                        Self::replace_operand(val, &constants);
                        
                        if Self::is_literal(val) && is_const {
                            constants.insert(name.clone(), val.clone());
                        } else if !is_const || !is_global {
                            constants.remove(name);
                        }
                    } else if !is_const || !is_global {
                        constants.remove(name);
                    }
                }

                Instruction::Assign(dest, src) => {
                    Self::replace_operand(src, &constants);
                    
                    if let Some(dest_key) = Self::get_key(dest) {
                        if Self::is_literal(src) {
                            constants.insert(dest_key.clone(), src.clone());
                        } else {
                            constants.remove(&dest_key);
                            immutable_vars.remove(&dest_key);
                        }
                    }
                }

                Instruction::Binary(dest, op, l, r) => {
                    Self::replace_operand(l, &constants);
                    Self::replace_operand(r, &constants);
                    
                    if let Some(dk) = Self::get_key(dest) {
                        constants.remove(&dk);
                        immutable_vars.remove(&dk);
                    }
                    
                    if *op == TokenType::AssignOp {
                        if let Some(lk) = Self::get_key(l) {
                            constants.remove(&lk);
                            immutable_vars.remove(&lk);
                        }
                    }
                }

                Instruction::Unary(dest, _, src) => {
                    Self::replace_operand(src, &constants);
                    if let Some(dk) = Self::get_key(dest) {
                        constants.remove(&dk);
                        immutable_vars.remove(&dk);
                    }
                }

                Instruction::IfTrue(cond, _) | Instruction::IfFalse(cond, _) => {
                    Self::replace_operand(cond, &constants);
                }

                Instruction::Param(p) => {
                    Self::replace_operand(p, &constants);
                }

                Instruction::Call(dest, _, _) => {
                    if let Some(d) = dest {
                        if let Some(dk) = Self::get_key(d) {
                            constants.remove(&dk);
                            immutable_vars.remove(&dk);
                        }
                    }
                }

                Instruction::Return(val) => {
                    if let Some(v) = val {
                        Self::replace_operand(v, &constants);
                    }
                }

                Instruction::Print(args) => {
                    for arg in args {
                        Self::replace_operand(arg, &constants);
                    }
                }

                Instruction::Label(_) => {
                    // Preserve global immutable constants across labels
                    constants.retain(|name, _| {
                        global_vars.contains(name) && immutable_vars.contains(name)
                    });
                }

                Instruction::FuncStart(_, _, _) => {
                    function_depth += 1;
                    // Keep only global immutable constants
                    constants.retain(|name, _| {
                        global_vars.contains(name) && immutable_vars.contains(name)
                    });
                    immutable_vars.retain(|name| global_vars.contains(name));
                }

                Instruction::FuncEnd => {
                    function_depth = function_depth.saturating_sub(1);
                }

                _ => {}
            }
        }
    }

    /// Replaces uses of variables/temporaries that are direct copies of others.
    /// Example: t1 = t0; t2 = t1 + 5 -> t2 = t0 + 5
    fn copy_propagation(&mut self) {
        let mut copies: HashMap<String, Operand> = HashMap::new();

        for instr in &mut self.instructions {
            match instr {
                Instruction::Declare(_, name, init) => {
                    if let Some(val) = init {
                        Self::replace_operand(val, &copies);
                        if Self::get_key(val).is_some() {
                            copies.insert(name.clone(), val.clone());
                        }
                    }
                    copies.retain(|_, v| Self::get_key(v) != Some(name.clone()));
                }

                Instruction::Assign(dest, src) => {
                    Self::replace_operand(src, &copies);
                    
                    if let Some(d_key) = Self::get_key(dest) {
                        if Self::get_key(src).is_some() {
                            copies.insert(d_key.clone(), src.clone());
                        } else {
                            copies.remove(&d_key);
                        }
                        copies.retain(|_, v| Self::get_key(v) != Some(d_key.clone()));
                    }
                }

                Instruction::Binary(dest, op, l, r) => {
                    Self::replace_operand(l, &copies);
                    Self::replace_operand(r, &copies);
                    
                    if let Some(dk) = Self::get_key(dest) {
                        copies.remove(&dk);
                        copies.retain(|_, v| Self::get_key(v) != Some(dk.clone()));
                    }

                    if *op == TokenType::AssignOp {
                        if let Some(lk) = Self::get_key(l) {
                            copies.remove(&lk);
                            copies.retain(|_, v| Self::get_key(v) != Some(lk.clone()));
                        }
                    }
                }

                Instruction::Unary(dest, _, src) => {
                    Self::replace_operand(src, &copies);
                    if let Some(dk) = Self::get_key(dest) {
                        copies.remove(&dk);
                        copies.retain(|_, v| Self::get_key(v) != Some(dk.clone()));
                    }
                }

                Instruction::IfTrue(cond, _) | Instruction::IfFalse(cond, _) => {
                    Self::replace_operand(cond, &copies);
                }

                Instruction::Param(p) => {
                    Self::replace_operand(p, &copies);
                }

                Instruction::Return(val) => {
                    if let Some(v) = val {
                        Self::replace_operand(v, &copies);
                    }
                }

                Instruction::Print(args) => {
                    for arg in args {
                        Self::replace_operand(arg, &copies);
                    }
                }

                Instruction::Call(dest, _, _) => {
                    if let Some(d) = dest {
                        if let Some(dk) = Self::get_key(d) {
                            copies.remove(&dk);
                            copies.retain(|_, v| Self::get_key(v) != Some(dk.clone()));
                        }
                    }
                }

                Instruction::Label(_) | Instruction::FuncStart(_, _, _) => {
                    copies.clear();
                }

                _ => {}
            }
        }
    }

    /// Removes instructions whose results are never used.
    fn dead_code_elimination(&mut self) {
        let mut modified = true;

        while modified {
            modified = false;
            let mut used = HashSet::new();

            // Pass 1: Mark all used variables and temporaries
            for instr in &self.instructions {
                match instr {
                    Instruction::Declare(_, _, init) => {
                        if let Some(op) = init {
                            Self::mark_used(op, &mut used);
                        }
                    }
                    Instruction::Assign(_, src) => Self::mark_used(src, &mut used),
                    Instruction::Binary(_dest, _op, l, r) => {
                        Self::mark_used(l, &mut used);
                        Self::mark_used(r, &mut used);
                    }
                    Instruction::Unary(_, _, src) => Self::mark_used(src, &mut used),
                    Instruction::IfTrue(cond, _) | Instruction::IfFalse(cond, _) => {
                        Self::mark_used(cond, &mut used);
                    }
                    Instruction::Param(p) => Self::mark_used(p, &mut used),
                    Instruction::Return(val) => {
                        if let Some(v) = val {
                            Self::mark_used(v, &mut used);
                        }
                    }
                    Instruction::Print(args) => {
                        for arg in args {
                            Self::mark_used(arg, &mut used);
                        }
                    }
                    _ => {}
                }
            }

            // Pass 2: Sweep unused definitions
            let mut i = 0;
            while i < self.instructions.len() {
                let should_remove = match &self.instructions[i] {
                    Instruction::Assign(dest, _) | Instruction::Unary(dest, _, _) => {
                        Self::get_key(dest).map_or(false, |key| !used.contains(&key))
                    }
                    Instruction::Binary(dest, op, _, _) => {
                        if *op == TokenType::AssignOp {
                            false // Has side effects
                        } else {
                            Self::get_key(dest).map_or(false, |key| !used.contains(&key))
                        }
                    }
                    Instruction::Declare(t, name, _) => {
                        !t.contains("global") && !used.contains(name)
                    }
                    Instruction::Call(Some(dest), func, n) => {
                        if Self::get_key(dest).map_or(false, |key| !used.contains(&key)) {
                            // Keep call but remove destination
                            let f_name = func.clone();
                            let n_args = *n;
                            self.instructions[i] = Instruction::Call(None, f_name, n_args);
                            modified = true;
                            false
                        } else {
                            false
                        }
                    }
                    _ => false,
                };

                if should_remove {
                    self.instructions.remove(i);
                    modified = true;
                } else {
                    i += 1;
                }
            }
        }
    }

    /// Simplifies local instruction patterns and removes redundant jumps.
    fn peephole_optimization(&mut self) {
        let mut i = 0;
        while i < self.instructions.len() {
            let mut advance = true;

            // Algebraic Identities
            if let Some(Instruction::Binary(dest, op, l, r)) = self.instructions.get(i) {
                let simplified = match (op, l, r) {
                    // Addition: x + 0 = x, 0 + x = x
                    (TokenType::Plus, val, Operand::Int(0)) | (TokenType::Plus, Operand::Int(0), val) => {
                        Some(Instruction::Assign(dest.clone(), val.clone()))
                    }
                    // Subtraction: x - 0 = x
                    (TokenType::Minus, val, Operand::Int(0)) => {
                        Some(Instruction::Assign(dest.clone(), val.clone()))
                    }
                    // x - x = 0
                    (TokenType::Minus, Operand::Var(v1), Operand::Var(v2)) if v1 == v2 => {
                        Some(Instruction::Assign(dest.clone(), Operand::Int(0)))
                    }
                    (TokenType::Minus, Operand::Temp(t1), Operand::Temp(t2)) if t1 == t2 => {
                        Some(Instruction::Assign(dest.clone(), Operand::Int(0)))
                    }
                    // Multiplication: x * 1 = x, 1 * x = x
                    (TokenType::Multiply, val, Operand::Int(1)) | (TokenType::Multiply, Operand::Int(1), val) => {
                        Some(Instruction::Assign(dest.clone(), val.clone()))
                    }
                    // x * 0 = 0, 0 * x = 0
                    (TokenType::Multiply, _, Operand::Int(0)) | (TokenType::Multiply, Operand::Int(0), _) => {
                        Some(Instruction::Assign(dest.clone(), Operand::Int(0)))
                    }
                    // Division: x / 1 = x
                    (TokenType::Divide, val, Operand::Int(1)) => {
                        Some(Instruction::Assign(dest.clone(), val.clone()))
                    }
                    _ => None,
                };

                if let Some(new_instr) = simplified {
                    self.instructions[i] = new_instr;
                }
            }

            // Redundant Jump Elimination (jump to next instruction)
            if i + 1 < self.instructions.len() {
                if let Instruction::Goto(target) = &self.instructions[i] {
                    if let Instruction::Label(label_name) = &self.instructions[i + 1] {
                        if target == label_name {
                            self.instructions.remove(i);
                            advance = false;
                        }
                    }
                }
            }

            if advance {
                i += 1;
            }
        }
    }

    // Helper functions
    fn get_key(op: &Operand) -> Option<String> {
        match op {
            Operand::Var(name) => Some(name.clone()),
            Operand::Temp(id) => Some(format!("t{}", id)),
            _ => None,
        }
    }

    fn is_literal(op: &Operand) -> bool {
        matches!(
            op,
            Operand::Int(_)
                | Operand::Float(_)
                | Operand::Bool(_)
                | Operand::Char(_)
                | Operand::String(_)
        )
    }

    fn mark_used(op: &Operand, set: &mut HashSet<String>) {
        if let Some(key) = Self::get_key(op) {
            set.insert(key);
        }
    }

    fn replace_operand(op: &mut Operand, map: &HashMap<String, Operand>) {
        if let Some(key) = Self::get_key(op) {
            if let Some(replacement) = map.get(&key) {
                *op = replacement.clone();
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