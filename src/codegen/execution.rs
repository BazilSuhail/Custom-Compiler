use crate::ir_pipeline::tac::{Instruction, Operand};
use crate::core::token::TokenType;
use std::collections::HashMap;
use std::fmt;

#[derive(Debug, Clone, PartialEq)]
pub enum Value {
    Int(i64),
    Float(f64),
    Bool(bool),
    Char(char),
    String(String),
    Unit,
}

fn unescape(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    let mut chars = s.chars().peekable();
    while let Some(c) = chars.next() {
        if c == '\\' {
            match chars.next() {
                Some('n') => out.push('\n'),
                Some('t') => out.push('\t'),
                Some('r') => out.push('\r'),
                Some('\\') => out.push('\\'),
                Some('\"') => out.push('\"'),
                Some('\'') => out.push('\''),
                Some(other) => {
                    out.push('\\');
                    out.push(other);
                }
                None => out.push('\\'),
            }
        } else {
            out.push(c);
        }
    }
    out
}

#[derive(Debug)]
pub enum RuntimeError {
    UndefinedVariable(String),
    TypeMismatch(String),
    DivisionByZero,
    MainNotFound,
    StackUnderflow,
    Other(String),
}

impl fmt::Display for RuntimeError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::UndefinedVariable(name) => write!(f, "Runtime Error: Undefined variable '{}'", name),
            Self::TypeMismatch(msg) => write!(f, "Runtime Error: Type mismatch - {}", msg),
            Self::DivisionByZero => write!(f, "Runtime Error: Division by zero"),
            Self::MainNotFound => write!(f, "Runtime Error: 'main' function not found"),
            Self::StackUnderflow => write!(f, "Runtime Error: Stack underflow"),
            Self::Other(msg) => write!(f, "Runtime Error: {}", msg),
        }
    }
}

impl fmt::Display for Value {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Value::Int(v) => write!(f, "{}", v),
            Value::Float(v) => write!(f, "{}", v),
            Value::Bool(v) => write!(f, "{}", v),
            Value::Char(v) => write!(f, "{}", v),
            Value::String(v) => write!(f, "{}", unescape(v)),
            Value::Unit => write!(f, "unit"),
        }
    }
}

// Memory and Stack management separated from the main loop logic
struct RuntimeState {
    global_vars: HashMap<String, Value>,
    stack: Vec<HashMap<String, Value>>,
    param_buffer: Vec<Value>,
}

impl RuntimeState {
    fn new() -> Self {
        Self {
            global_vars: HashMap::new(),
            stack: vec![HashMap::new()],
            param_buffer: Vec::new(),
        }
    }

    fn resolve(&self, op: &Operand) -> Result<Value, RuntimeError> {
        match op {
            Operand::Int(v) => Ok(Value::Int(*v)),
            Operand::Float(v) => Ok(Value::Float(*v)),
            Operand::Bool(v) => Ok(Value::Bool(*v)),
            Operand::Char(v) => Ok(Value::Char(*v)),
            Operand::String(v) => Ok(Value::String(v.clone())),
            Operand::Var(name) => {
                self.stack.last().and_then(|frame| frame.get(name))
                    .or_else(|| self.global_vars.get(name))
                    .cloned()
                    .ok_or_else(|| RuntimeError::UndefinedVariable(name.clone()))
            }
            Operand::Temp(id) => {
                let key = format!("t{}", id);
                self.stack.last().and_then(|frame| frame.get(&key))
                    .cloned()
                    .ok_or_else(|| RuntimeError::UndefinedVariable(key))
            }
            _ => Err(RuntimeError::Other(format!("Cannot resolve operand: {:?}", op))),
        }
    }

    fn store(&mut self, dest: &Operand, val: Value) -> Result<(), RuntimeError> {
        let key = match dest {
            Operand::Var(name) => name.clone(),
            Operand::Temp(id) => format!("t{}", id),
            _ => return Err(RuntimeError::Other("Invalid assignment destination".into())),
        };

        // If it's a variable, check if it's already in global_vars
        if let Operand::Var(name) = dest {
            // Check if it shadows in the current stack frame
            if self.stack.last().map(|frame| frame.contains_key(name)).unwrap_or(false) {
                self.stack.last_mut().unwrap().insert(name.clone(), val);
                return Ok(());
            }
            // Otherwise check if it's a global
            if self.global_vars.contains_key(name) {
                self.global_vars.insert(name.clone(), val);
                return Ok(());
            }
        }

        self.stack.last_mut().unwrap().insert(key, val);
        Ok(())
    }
}

pub struct ExecutionEngine {
    instructions: Vec<Instruction>,
    labels: HashMap<String, usize>,
}

impl ExecutionEngine {
    pub fn new(instructions: Vec<Instruction>) -> Self {
        let mut labels = HashMap::new();
        for (i, instr) in instructions.iter().enumerate() {
            match instr {
                Instruction::Label(name) | Instruction::FuncStart(name, _, _) => {
                    labels.insert(name.clone(), i);
                }
                _ => {}
            }
        }
        Self { instructions, labels }
    }

    pub fn execute(&self) -> Result<(), RuntimeError> {
        let mut state = RuntimeState::new();
        
        // Phase 1: Initialize Globals (Top-level scope)
        let mut pc = 0;
        while pc < self.instructions.len() {
            match &self.instructions[pc] {
                Instruction::FuncStart(_, _, _) => {
                    // Skip function bodies
                    let mut depth = 1;
                    pc += 1;
                    while pc < self.instructions.len() && depth > 0 {
                        match &self.instructions[pc] {
                            Instruction::FuncStart(_, _, _) => depth += 1,
                            Instruction::FuncEnd => depth -= 1,
                            _ => {}
                        }
                        pc += 1;
                    }
                    continue; // Continue from the instruction AFTER FuncEnd
                }
                Instruction::Declare(_, name, init) => {
                    let val = match init {
                        Some(op) => state.resolve(op)?,
                        None => Value::Int(0),
                    };
                    state.global_vars.insert(name.clone(), val);
                }
                Instruction::Assign(dest, src) => {
                    if let Operand::Var(name) = dest {
                        let val = state.resolve(src)?;
                        state.global_vars.insert(name.clone(), val);
                    }
                }
                _ => {}
            }
            pc += 1;
        }

        // Phase 2: Run Main
        let main_idx = *self.labels.get("main").ok_or(RuntimeError::MainNotFound)?;
        self.run_from(&mut state, main_idx)?;
        Ok(())
    }

    fn run_from(&self, state: &mut RuntimeState, start_pc: usize) -> Result<Value, RuntimeError> {
        let mut pc = start_pc;

        while pc < self.instructions.len() {
            // Using a reference avoids copying at every step
            match &self.instructions[pc] {
                Instruction::Declare(type_str, name, init) => {
                    let val = match init {
                        Some(op) => state.resolve(op)?,
                        None => Value::Int(0),
                    };
                    if type_str.contains("global") {
                        state.global_vars.insert(name.clone(), val);
                    } else {
                        state.stack.last_mut().unwrap().insert(name.clone(), val);
                    }
                }

                Instruction::Assign(dest, src) => {
                    let val = state.resolve(src)?;
                    state.store(dest, val)?;
                }

                Instruction::Binary(dest, op, l, r) => {
                    let lv = state.resolve(l)?;
                    let rv = state.resolve(r)?;
                    let res = self.evaluate_binary(lv, op, rv)?;
                    state.store(dest, res)?;
                }

                Instruction::Unary(dest, op, src) => {
                    let val = state.resolve(src)?;
                    let res = match (op, val) {
                        (TokenType::Minus, Value::Int(v)) => Value::Int(-v),
                        (TokenType::Minus, Value::Float(v)) => Value::Float(-v),
                        (TokenType::Not, Value::Bool(v)) => Value::Bool(!v),
                        _ => return Err(RuntimeError::TypeMismatch(format!("Invalid unary op {:?} for value", op))),
                    };
                    state.store(dest, res)?;
                }

                Instruction::Print(args) => {
                    let output: Vec<String> = args.iter()
                        .map(|a| state.resolve(a).map(|v| v.to_string()))
                        .collect::<Result<_, _>>()?;
                    // Manual newline control via escape sequences only
                    print!("{}", output.join(" "));
                    use std::io::Write;
                    std::io::stdout().flush().map_err(|e| RuntimeError::Other(e.to_string()))?;
                }

                Instruction::Goto(lbl) => {
                    pc = *self.labels.get(lbl).unwrap();
                    continue;
                }

                Instruction::IfTrue(cond, lbl) => {
                    if let Value::Bool(true) = state.resolve(cond)? {
                        pc = *self.labels.get(lbl).unwrap();
                        continue;
                    }
                }

                Instruction::IfFalse(cond, lbl) => {
                    if let Value::Bool(false) = state.resolve(cond)? {
                        pc = *self.labels.get(lbl).unwrap();
                        continue;
                    }
                }

                Instruction::Param(op) => state.param_buffer.push(state.resolve(op)?),

                Instruction::Call(dest, name, n_args) => {
                    let args: Vec<Value> = state.param_buffer.drain(state.param_buffer.len() - n_args..).collect();
                    let func_idx = *self.labels.get(name).ok_or_else(|| RuntimeError::Other(format!("Function {} not found", name)))?;

                    let mut new_frame = HashMap::new();
                    if let Instruction::FuncStart(_, _, params) = &self.instructions[func_idx] {
                        for (i, arg) in args.into_iter().enumerate() {
                            new_frame.insert(params[i].1.clone(), arg);
                        }
                    }

                    state.stack.push(new_frame);
                    let result = self.run_from(state, func_idx + 1)?;
                    state.stack.pop();

                    if let Some(d) = dest {
                        state.store(d, result)?;
                    }
                }

                Instruction::Return(op) => {
                    return match op {
                        Some(o) => state.resolve(o),
                        None => Ok(Value::Unit),
                    };
                }

                Instruction::FuncEnd => return Ok(Value::Unit),

                _ => {}
            }
            pc += 1;
        }
        Ok(Value::Unit)
    }

    fn evaluate_binary(&self, l: Value, op: &TokenType, r: Value) -> Result<Value, RuntimeError> {
        match (l, r) {
            (Value::Int(a), Value::Int(b)) => match op {
                TokenType::Plus => Ok(Value::Int(a + b)),
                TokenType::Minus => Ok(Value::Int(a - b)),
                TokenType::Multiply => Ok(Value::Int(a * b)),
                TokenType::Divide => if b != 0 { Ok(Value::Int(a / b)) } else { Err(RuntimeError::DivisionByZero) },
                TokenType::Modulo => if b != 0 { Ok(Value::Int(a % b)) } else { Err(RuntimeError::DivisionByZero) },
                TokenType::EqualOp => Ok(Value::Bool(a == b)),
                TokenType::Ne => Ok(Value::Bool(a != b)),
                TokenType::Lt => Ok(Value::Bool(a < b)),
                TokenType::Gt => Ok(Value::Bool(a > b)),
                TokenType::Le => Ok(Value::Bool(a <= b)),
                TokenType::Ge => Ok(Value::Bool(a >= b)),
                TokenType::AssignOp => Ok(Value::Int(b)),
                _ => Err(RuntimeError::TypeMismatch(format!("Invalid int op {:?}", op))),
            },
            (Value::Float(a), Value::Float(b)) => match op {
                TokenType::Plus => Ok(Value::Float(a + b)),
                TokenType::Minus => Ok(Value::Float(a - b)),
                TokenType::Multiply => Ok(Value::Float(a * b)),
                TokenType::Divide => Ok(Value::Float(a / b)),
                TokenType::EqualOp => Ok(Value::Bool(a == b)),
                _ => Err(RuntimeError::TypeMismatch(format!("Invalid float op {:?}", op))),
            },
            (Value::Bool(a), Value::Bool(b)) => match op {
                TokenType::And => Ok(Value::Bool(a && b)),
                TokenType::Or => Ok(Value::Bool(a || b)),
                TokenType::EqualOp => Ok(Value::Bool(a == b)),
                _ => Err(RuntimeError::TypeMismatch(format!("Invalid bool op {:?}", op))),
            },
            (l, r) => Err(RuntimeError::TypeMismatch(format!("Incompatible types for operator {:?}: {} and {}", op, l, r))),
        }
    }
}