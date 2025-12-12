# Intermediate Representation (IR) Optimization Engine

## Overview
The **IR Optimizer** is a sophisticated pipeline component designed to transform raw **Three-Address Code (TAC)** into a more efficient, streamlined version. By analyzing data flow and instruction patterns, the optimizer reduces the instruction count, minimizes temporary variable usage, and resolves computations at compile-time.

The optimizer is implemented as a multi-pass engine that processes a linear vector of TAC instructions.

---

## The Optimization Pipeline

The `IROptimizer` executes several distinct passes in a strategic sequence to maximize the "cascade effect" (where one optimization unlocks another).

### 1. Constant Folding
**Logic**: Performs compile-time evaluation of arithmetic expressions where all operands are numeric literals.
- **Mechanism**: Scans `Instruction::Binary`. If both operands are `Operand::Int`, the optimizer applies the operator (`+`, `-`, `*`, `/`) and replaces the entire binary operation with a single `Instruction::Assign`.
- **Safety**: Includes a division-by-zero check to prevent optimizer crashes on invalid source code.
- **Example**:
  - *Input*: `t0 = 20 * 5`
  - *Output*: `t0 = 100`

### 2. Constant Propagation
**Logic**: Substitutes variables known to contain a constant value with that constant in subsequent instructions.
- **Mechanism**: 
  - Uses a `HashMap<String, Operand>` to track the state of variables.
  - When a variable is assigned a literal, it is added to the map.
  - When a variable is used (in `Binary`, `Unary`, `If`, `Print`, etc.), the optimizer checks the map and replaces the `Operand::Var` or `Operand::Temp` with the literal.
- **Boundary Handling**: The tracking map is **cleared** at `Label` and `FuncStart` instructions. This ensures that optimistic assumptions don't cross potentially complex control-flow boundaries where a variable's state might be ambiguous.
- **Example**:
  - *Input*: 
    ```nginx
    x = 10
    t0 = x + 5
    ```
  - *Output*:
    ```nginx
    x = 10
    t0 = 10 + 5
    ```

### 3. Copy Propagation
**Logic**: Similar to constant propagation, but replaces uses of a variable with its "original" source if it was direct copy.
- **Mechanism**: 
  - Tracks assignments like `a = b` in a `copies` map.
  - If `a` is used later as `t = a + 1`, it is transformed to `t = b + 1`.
- **Invalidation**: If the source variable (`b`) is redefined, it is removed from the tracking map to prevent stale data usage.
- **Example**:
  - *Input*: 
    ```nginx
    val = original
    t0 = val * 2
    ```
  - *Output*:
    ```nginx
    val = original
    t0 = original * 2
    ```

### 4. Peephole Optimization
**Logic**: A "sliding window" analysis that looks for local patterns and algebraic simplifications.
- **Algebraic Identities**:
  - `x + 0` or `0 + x` $\rightarrow$ `x`
  - `x - 0` $\rightarrow$ `x`
  - `x - x` $\rightarrow$ `0`
  - `x * 1` or `1 * x` $\rightarrow$ `x`
  - `x * 0` or `0 * x` $\rightarrow$ `0`
  - `x / 1` $\rightarrow$ `x`
- **Control Flow**:
  - **Redundant Jump Elimination**: If a `Goto L1` is immediately followed by the label `L1:`, the `Goto` is removed as the execution would have fallen through to that label anyway.

### 5. Dead Code Elimination (DCE)
**Logic**: Removes any instruction whose result is never consumed by the program.
- **Mechanism**: A **Mark-and-Sweep** algorithm.
  1. **Mark**: Scans every instruction to collect all `Operand::Var` and `Operand::Temp` used as *sources*.
  2. **Sweep**: Iterates through instructions. If a definition (`Assign`, `Binary`, `Unary`) targets a variable not in the "used" set, the instruction is deleted.
- **Side-Effect Preservation**:
  - **Prism Awareness**: Global declarations are preserved by default.
  - **Function Calls**: If a function call's result is unused, the assignment is removed, but the `Call` remains (as `Call(None, ...)`). This ensures side effects within the function (like I/O or global mutations) still occur.
  - **I/O**: `Print` instructions are never considered dead.

---

## Iterative Fixed-Point
The `dead_code_elimination` pass runs in a `while modified` loop. Since removing one dead instruction (e.g., `t1 = t0 + 5`) might make the source variable (`t0`) dead as well, the optimizer repeats the sweep until no further changes are possible.

---

## Detailed Transformation Trace

### Source Code
```c
main {
    int a = 10;
    int b = a + 5;
    int c = b * 1 + 0;
    int d = 999; // Never used
    print(c);
}
```

### Step 1: Raw TAC Generation
```nginx
void main() begin:
int a = 10
t0 = a Plus 5
int b = t0
t1 = b Multiply 1
t2 = t1 Plus 0
int c = t2
int d = 999
print c
end
```

### Step 2: Constant & Copy Propagation
- `a` is replaced by `10` in `t0`.
- `t0` is a copy, `b` is replaced by `t0` in `t1`.
- `t1` is replaced by `t1`... (Propagation continues).
```nginx
void main() begin:
int a = 10
t0 = 10 Plus 5
int b = t0
t1 = t0 Multiply 1
t2 = t1 Plus 0
int c = t2
int d = 999
print c
end
```

### Step 3: Constant Folding & Peephole
- `10 Plus 5` becomes `15`.
- `t0 Multiply 1` simplifies to `t0`.
- `t1 Plus 0` simplifies to `t1`.
```nginx
void main() begin:
int a = 10
t0 = 15
int b = t0
t1 = t0
t2 = t1
int c = t2
int d = 999
print c
end
```

### Step 4: Final Dead Code Elimination
1. `c` is used in `print`.
2. `c` is defined by `t2` $\rightarrow$ `t2` is used.
3. `t2` is defined by `t1` $\rightarrow$ `t1` is used.
4. `t1` is defined by `t0` $\rightarrow$ `t0` is used.
5. Variables `a`, `b`, and `d` are never used as sources.
```nginx
void main() begin:
t0 = 15
t1 = t0
t2 = t1
int c = t2
print c
end
```
*(Note: Further passes of Copy Prop + DCE would ideally reduce this to `print 15`, depending on the iteration count and order).*

---

## Optimization Summary
| Pass | Techniques Used | Key Benefit |
| :--- | :--- | :--- |
| **Folding** | Arithmetic Evaluation | Saves CPU cycles at runtime |
| **Propagation** | Value Substitution | Enables further folding/DCE |
| **Peephole** | Identity matching, Jump pruning | Removes redundant work |
| **DCE** | Dependency Analysis | Reduces binary size & memory usage |
