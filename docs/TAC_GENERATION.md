# Three-Address Code (TAC) Generation

## Overview
The **TAC Generator** is the first phase of compiler's intermediate representation (IR) pipeline. It traverses the **Abstract Syntax Tree (AST)** produced by the parser and translates high-level language constructs into a flat, linear sequence of instructions. 

This Three-Address Code acts as a bridge between the high-level semantic analysis and low-level target code generation (like Assembly or Machine Code).

---

## Architectural Design

Our TAC adopts a **Standard Quadruple-like Form**, where each instruction typically involves at most three operands (two sources and one destination).

### 1. Operands
The `Operand` enum represents all possible values in the IR:
- **Temp(`usize`)**: Virtual registers (e.g., `t0`, `t1`) generated for intermediate results.
- **Var(`String`)**: Named variables from the source code.
- **Literals**: Native support for `Int`, `Float`, `Bool`, `Char`, and `String`.
- **Label(`String`)**: Symbolic addresses used for jumps and function entry points.

### 2. Instruction Set
The generator produces a variety of instructions tailored for optimization and translation:
| Instruction | Description |
|-------------|-------------|
| `Declare` | Registers a variable with its type, `const`, and `global` qualifiers. |
| `Assign` | Direct value transfer: `dest = src`. |
| `Binary` | Arithmetic/Logical operations: `t1 = a + b`. |
| `Unary` | Single-operand operations: `t2 = !active`. |
| `IfTrue / IfFalse` | Standard conditional jumps: `ifTrue t1 goto L0`. |
| `Goto` | Unconditional jump: `goto L1`. |
| `Call / Param` | Function calling convention using parameter stacking. |
| `Return` | Exits a function with an optional value. |
| `FuncStart / End` | Delimiters for function scopes with typed signatures. |

---

## Key Features & Optimizations

### 1. Enum-to-Integer Mapping
To maintain efficiency, the generator performs a **pre-scan pass** on all `EnumDecl` nodes. Every enum constant is assigned a sequential integer ID (starting at 0). During code generation, these constants are treated as immediate integer literals, eliminating string-based lookups at runtime.

### 2. Control Flow Optimization
Our generator focuses on creating "Dense IR" by minimizing redundant labels and jumps:
- **Short-Circuit Jumps**: Uses `ifFalse` for early exits in `if`, `while`, and `for` loops.
- **Empty Else Avoidance**: If an `if` statement lacks an `else` block, the generator jumps directly to the exit label, avoiding a redundant `goto`.
- **Labels-on-Demand**: Labels are only emitted when they are actual targets of a jump.

### 3. Storage & Mutability Metadata
Unlike primitive IRs that strip away source-level information, our TAC preserves **qualifiers**:
- **`global`**: Identifies variables that should be allocated in the data segment.
- **`const`**: Flags variables for potential constant folding or read-only segment placement.
- **Typed Signatures**: Functions in TAC carry their full parameter types and return types.

---

## Examples

### 1. Nested Control Flow
**Source:**
```c
if (active && x > 5) {
    print("Active");
}
```

**Generated TAC:**
```nginx
t1 = x Gt 5
t2 = active And t1
ifFalse t2 goto L0
print "Active"
L0:
```

### 2. For Loops
**Source:**
```c
for (int i = 0; i < 3; i++) {
    print(i);
}
```

**Generated TAC:**
```nginx
int i = 0
L1:
t4 = i Lt 3
ifFalse t4 goto L2
print i
i = i Plus 1
goto L1
L2:
```

---

## Implementation Details

The generator maintains several internal states to ensure correctness:
- **Temp counter**: Ensures every intermediate result gets a unique virtual register.
- **Label counter**: Simplifies complex nested logic by providing unique jump targets.
- **Break Stack**: A LIFO stack that tracks the exit labels of the current loop/switch nesting, allowing `break` statements to jump to the correct location instantly.

## Usage in Pipeline
The TAC generation occurs after successful **Scope Analysis** and **Type Checking**. If the semantic phase finds no errors, the `TACGenerator` is invoked, and the resulting list of instructions is written to `tac.txt` by default.

### 3. Functions
Functions are represented with entry and exit points, maintaining type safety and calling conventions.

**Source:**
```c
int square(int n) {
    return n * n;
}

main {
    print(square(5));
}
```

**Generated TAC:**
```nginx
int square(int n) begin:
t0 = n Multiply n
return t0
end

void main() begin:
param 5
t1 = call square, 1
print t1
end
```

**Procedure:**
1. **Prologue**: `FuncStart` is emitted with the return type, name, and a list of typed parameters.
2. **Body**: Expressions inside the function are translated. `ReturnStmt` generates a `return` instruction with an optional operand.
3. **Epilogue**: `FuncEnd` is emitted as a closing delimiter.
4. **Invocation**: For function calls, arguments are resolved and emitted as `param` instructions in order. The `call` instruction specifies the function name and the number of stacked parameters.

### 4. Enums
Enums are optimized into integer constants during a pre-processing pass.

**Source:**
```c
enum Color { RED, GREEN, BLUE };
Color c = GREEN;
```

**Generated TAC:**
```nginx
; Enum Color Defined
Color c = 1
```

**Procedure:**
1. **Pre-Scan**: Before code generation, the `TACGenerator` scans the AST for `EnumDecl`.
2. **Value Mapping**: Each identifier in the `EnumValueList` is mapped to an incremental integer ID (e.g., `RED=0`, `GREEN=1`, `BLUE=2`) in an internal `enum_map`.
3. **Replacement**: Whenever an identifier is encountered in the AST, the generator checks the `enum_map`. If found, it emits a constant `Operand::Int(id)` instead of a variable lookup.

### 5. Do-While Loops
Ensures at least one execution of the body before checking the condition.

**Source:**
```c
int x = 10;
do {
    x--;
} while (x > 0);
```

**Generated TAC:**
```nginx
int x = 10
L0:
x = x Minus 1
t0 = x Gt 0
ifTrue t0 goto L0
L1:
```

**Procedure:**
1. **Label Initialization**: Two labels are created: `L_start` (start of body) and `L_end` (for potential `break` statements).
2. **Nesting**: The `L_end` label is pushed onto the `break_stack`.
3. **Body Execution**: The body statements are generated immediately after `L_start`.
4. **Condition Check**: The condition is evaluated. If it results in `true`, an `ifTrue` instruction jumps back to `L_start`.
5. **Exit**: The loop naturally proceeds to the next instruction if the condition is `false`.

---

## Conclusion
The TAC generation phase ensures that high-level structures are simplified into a machine-agnostic representation while preserving essential semantic metadata. This modular approach allows the compiler to support complex language features like nested loops and recursive functions with a clean, linear instruction set.