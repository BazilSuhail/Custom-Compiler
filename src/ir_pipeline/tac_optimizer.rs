use crate::core::token::TokenType;
use crate::ir_pipeline::tac::{Instruction, Operand};
use std::fs;
use std::collections::HashSet;

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
                        TokenType::Plus => Some(Operand::Int(*lv + *rv)),
                        TokenType::Minus => Some(Operand::Int(*lv - *rv)),
                        TokenType::Multiply => Some(Operand::Int(*lv * *rv)),
                        TokenType::Divide => if *rv != 0 { Some(Operand::Int(*lv / *rv)) } else { None },
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

                Instruction::Binary(dest, op, l, r) => {
                    if let Some(lk) = get_key(l) { 
                        if let Some(c) = constants.get(&lk) { *l = c.clone(); }
                    }
                    if let Some(rk) = get_key(r) { 
                        if let Some(c) = constants.get(&rk) { *r = c.clone(); }
                    }
                    if let Some(dk) = get_key(dest) { constants.remove(&dk); }
                    
                    // If this is an assignment operation, the left operand is modified
                    if *op == TokenType::AssignOp {
                        if let Some(lk) = get_key(l) { constants.remove(&lk); }
                    }
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

    /// Replaces uses of variables/temporaries that are direct copies of others.
    /// Example: t1 = t0; t2 = t1 + 5 -> t2 = t0 + 5
    fn copy_propagation(&mut self) {
        use std::collections::HashMap;
        let mut copies: HashMap<String, Operand> = HashMap::new();

        fn get_key(op: &Operand) -> Option<String> {
            match op {
                Operand::Var(name) => Some(name.clone()),
                Operand::Temp(id) => Some(format!("t{}", id)),
                _ => None,
            }
        }

        for instr in &mut self.instructions {
            match instr {
                Instruction::Declare(_, name, init) => {
                    // Substitue use in init
                    if let Some(val) = init {
                        if let Some(key) = get_key(val) {
                            if let Some(orig) = copies.get(&key) { *val = orig.clone(); }
                        }
                        // Track new copy
                        if let Some(_) = get_key(val) { copies.insert(name.clone(), val.clone()); }
                    }
                    // If name is redefined, it can't be a source for others anymore
                    copies.retain(|_, v| get_key(v) != Some(name.clone()));
                }

                Instruction::Assign(dest, src) => {
                    // 1. Substitute Use
                    if let Some(src_key) = get_key(src) {
                        if let Some(orig) = copies.get(&src_key) { *src = orig.clone(); }
                    }
                    // 2. Track / Invalidate
                    if let Some(d_key) = get_key(dest) {
                        if let Some(_) = get_key(src) { copies.insert(d_key.clone(), src.clone()); } 
                        else { copies.remove(&d_key); }
                        // Anything holding the old dest is no longer a valid copy
                        copies.retain(|_, v| get_key(v) != Some(d_key.clone()));
                    }
                }

                Instruction::Binary(dest, op, l, r) => {
                    if let Some(lk) = get_key(l) { if let Some(orig) = copies.get(&lk) { *l = orig.clone(); } }
                    if let Some(rk) = get_key(r) { if let Some(orig) = copies.get(&rk) { *r = orig.clone(); } }
                    
                    if let Some(dk) = get_key(dest) {
                        copies.remove(&dk);
                        copies.retain(|_, v| get_key(v) != Some(dk.clone()));
                    }

                    // If this is an assignment operation, the left operand is modified
                    if *op == TokenType::AssignOp {
                        if let Some(lk) = get_key(l) {
                            copies.remove(&lk);
                            copies.retain(|_, v| get_key(v) != Some(lk.clone()));
                        }
                    }
                }

                Instruction::Unary(dest, _, src) => {
                    if let Some(sk) = get_key(src) { if let Some(orig) = copies.get(&sk) { *src = orig.clone(); } }
                    if let Some(dk) = get_key(dest) {
                        copies.remove(&dk);
                        copies.retain(|_, v| get_key(v) != Some(dk.clone()));
                    }
                }

                Instruction::IfTrue(cond, _) | Instruction::IfFalse(cond, _) => {
                    if let Some(ck) = get_key(cond) { if let Some(orig) = copies.get(&ck) { *cond = orig.clone(); } }
                }

                Instruction::Param(p) => {
                    if let Some(pk) = get_key(p) { if let Some(orig) = copies.get(&pk) { *p = orig.clone(); } }
                }

                Instruction::Return(val) => {
                    if let Some(v) = val {
                        if let Some(vk) = get_key(v) { if let Some(orig) = copies.get(&vk) { *v = orig.clone(); } }
                    }
                }

                Instruction::Print(args) => {
                    for arg in args {
                        if let Some(ak) = get_key(arg) { if let Some(orig) = copies.get(&ak) { *arg = orig.clone(); } }
                    }
                }

                Instruction::Call(dest, _, _) => {
                    if let Some(d) = dest {
                        if let Some(dk) = get_key(d) { 
                            copies.remove(&dk); 
                            copies.retain(|_, v| get_key(v) != Some(dk.clone()));
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
        use std::collections::HashSet;
        let mut modified = true;

        while modified {
            modified = false;
            let mut used = HashSet::new();

            // Pass 1: Mark all used variables and temporaries
            for instr in &self.instructions {
                match instr {
                    Instruction::Declare(_, _, init) => {
                        if let Some(op) = init { self.mark_used(op, &mut used); }
                    }
                    Instruction::Assign(_, src) => self.mark_used(src, &mut used),
                    Instruction::Binary(_dest, _op, l, r) => {
                        // In an assignment binary op (e.g., t = x AssignOp y), 
                        // x is treated as both a source and a destination conceptually,
                        // but for 'used' calculation, we need to know if the result 't' is used
                        // OR if 'x' is used later. The side-effect is handled in the sweep phase.
                        self.mark_used(l, &mut used);
                        self.mark_used(r, &mut used);
                    }
                    Instruction::Unary(_, _, src) => self.mark_used(src, &mut used),
                    Instruction::IfTrue(cond, _) | Instruction::IfFalse(cond, _) => self.mark_used(cond, &mut used),
                    Instruction::Param(p) => self.mark_used(p, &mut used),
                    // Note: Call arguments are handled by Param instructions produced before the Call.
                    Instruction::Return(val) => {
                        if let Some(v) = val { self.mark_used(v, &mut used); }
                    }
                    Instruction::Print(args) => {
                        for arg in args { self.mark_used(arg, &mut used); }
                    }
                    _ => {}
                }
            }

            // Pass 2: Sweep unused definitions
            let mut i = 0;
            while i < self.instructions.len() {
                let mut is_dead = false;
                match &self.instructions[i] {
                    Instruction::Assign(dest, _) | 
                    Instruction::Unary(dest, _, _) => {
                        if let Some(key) = self.get_op_key(dest) {
                            if !used.contains(&key) { is_dead = true; }
                        }
                    }
                    Instruction::Binary(dest, op, _, _) => {
                        if let Some(key) = self.get_op_key(dest) {
                            // Only consider binary ops dead if they have no side effects
                            if !used.contains(&key) && *op != TokenType::AssignOp { 
                                is_dead = true; 
                            }
                        }
                    }
                    Instruction::Declare(t, name, _) => {
                        // Prismatically keep globals, but locals can be pruned
                        if !t.contains("global") && !used.contains(name) { is_dead = true; }
                    }
                    Instruction::Call(Some(dest), func, n) => {
                        if let Some(key) = self.get_op_key(dest) {
                            if !used.contains(&key) {
                                // Function might have side effects, so keep the call but remove the destination
                                let f_name = func.clone();
                                let n_args = *n;
                                self.instructions[i] = Instruction::Call(None, f_name, n_args);
                                modified = true;
                            }
                        }
                    }
                    _ => {}
                }

                if is_dead {
                    self.instructions.remove(i);
                    modified = true;
                } else {
                    i += 1;
                }
            }
        }
    }

    fn mark_used(&self, op: &Operand, set: &mut HashSet<String>) {
        if let Some(key) = self.get_op_key(op) {
            set.insert(key);
        }
    }

    fn get_op_key(&self, op: &Operand) -> Option<String> {
        match op {
            Operand::Var(name) => Some(name.clone()),
            Operand::Temp(id) => Some(format!("t{}", id)),
            _ => None,
        }
    }

    /// Simplifies local instruction patterns and removes redundant jumps.
    fn peephole_optimization(&mut self) {
        let mut i = 0;
        while i < self.instructions.len() {
            let mut modified = false;

            // 1. Algebraic Identities
            if let Instruction::Binary(dest, op, l, r) = &self.instructions[i] {
                let simplified = match (op, l, r) {
                    // Addition: x + 0 = x, 0 + x = x
                    (TokenType::Plus, val, Operand::Int(0)) | (TokenType::Plus, Operand::Int(0), val) => 
                        Some(Instruction::Assign(dest.clone(), val.clone())),
                    
                    // Subtraction: x - 0 = x, x - x = 0
                    (TokenType::Minus, val, Operand::Int(0)) => Some(Instruction::Assign(dest.clone(), val.clone())),
                    (TokenType::Minus, Operand::Var(v1), Operand::Var(v2)) if v1 == v2 => Some(Instruction::Assign(dest.clone(), Operand::Int(0))),
                    (TokenType::Minus, Operand::Temp(t1), Operand::Temp(t2)) if t1 == t2 => Some(Instruction::Assign(dest.clone(), Operand::Int(0))),

                    // Multiplication: x * 1 = x, 1 * x = x, x * 0 = 0, 0 * x = 0
                    (TokenType::Multiply, val, Operand::Int(1)) | (TokenType::Multiply, Operand::Int(1), val) => 
                        Some(Instruction::Assign(dest.clone(), val.clone())),
                    (TokenType::Multiply, _, Operand::Int(0)) | (TokenType::Multiply, Operand::Int(0), _) => 
                        Some(Instruction::Assign(dest.clone(), Operand::Int(0))),

                    // Division: x / 1 = x
                    (TokenType::Divide, val, Operand::Int(1)) => Some(Instruction::Assign(dest.clone(), val.clone())),
                    
                    _ => None,
                };

                if let Some(new_instr) = simplified {
                    self.instructions[i] = new_instr;
                    modified = true;
                }
            }

            // 2. Redundant Jump Elimination (jump to next instruction)
            if i + 1 < self.instructions.len() {
                if let Instruction::Goto(target) = &self.instructions[i] {
                    if let Instruction::Label(label_name) = &self.instructions[i+1] {
                        if target == label_name {
                            self.instructions.remove(i);
                            continue; // Don't increment i
                        }
                    }
                }
            }

            if !modified { i += 1; }
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