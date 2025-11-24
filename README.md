# Intermediate Representation (IR) Generation Module

## Overview

The IR Generation module transforms a type-checked Abstract Syntax Tree (AST) into executable code through a two-stage process. First, it generates Three-Address Code (TAC) as an intermediate representation. Then, it converts TAC into LLVM IR and executes the program using LLVM's JIT compiler. This phase bridges the gap between high-level semantic analysis and low-level machine code execution.

## What It Does

The IR Generation module performs two main tasks:

**Three-Address Code Generation**
- Converts AST nodes into simple, linearized TAC instructions
- Generates unique temporary variables for intermediate results
- Creates labels for control flow structures
- Handles function declarations, parameters, and returns
- Manages scope through indentation for readability
- Outputs TAC to a text file for inspection and further processing

**LLVM IR Generation and Execution**
- Parses TAC instructions from the generated text file
- Builds LLVM IR using the LLVM C++ API
- Creates function signatures with proper type information
- Implements type conversions and operations
- Executes the program using LLVM's JIT execution engine
- Provides runtime output and verification

## Pipeline Architecture

```
Source Code → Lexer → Parser → Type Checker → TAC Generator → LLVM IR Generator → JIT Execution
                                               ↓
                                           tester/tac.txt
```

The pipeline flow:
1. **TAC Generation** (`tac.cpp`): Converts AST to TAC and writes to `tester/tac.txt`
2. **LLVM IR Generation** (`llvm_test.cpp`): Reads TAC, generates LLVM IR, and executes

## Data Structures

### TAC Generator (`tac.cpp`)

**TACGenerator Class**
Core class managing TAC generation:
- `tempCounter`: Generates unique temporary variable names (t0, t1, t2, ...)
- `labelCounter`: Generates unique labels for control flow (L0, L1, L2, ...)
- `indentLevel`: Tracks code nesting for readable output
- `currentFunction`: Maintains context of the function being processed
- `enumValues`: Maps enum identifiers to their integer values
- `breakLabels`: Stack for tracking break statement targets
```cpp
class TACGenerator {
    ofstream outputFile;
    int tempCounter;
    int labelCounter;
    int indentLevel;
    string currentFunction;
    unordered_map<string, string> enumValues;
    stack<string> breakLabels;
};
```

### LLVM IR Generator (`llvm_test.cpp`)

**VariableInfo**
Stores information about allocated variables:
- `alloca`: LLVM AllocaInst pointer for stack allocation
- `type`: Variable type classification
```cpp
struct VariableInfo {
    AllocaInst* alloca;
    VarType type;
};
```

**FunctionInfo**
Maintains function metadata:
- `func`: LLVM Function pointer
- `paramNames`: Parameter identifiers
- `paramTypes`: Type information for each parameter
- `returnType`: Function return type
```cpp
struct FunctionInfo {
    Function* func;
    vector<string> paramNames;
    vector<VarType> paramTypes;
    VarType returnType;
};
```

**TACExecutor Class**
Main class for LLVM IR generation and execution:
- `ctx`: LLVM context for IR construction
- `module`: LLVM module containing all functions and global data
- `builder`: IRBuilder for constructing LLVM instructions
- `namedValues`: Maps variable names to their LLVM allocations
- `tempValues`: Stores temporary values from TAC
- `labels`: Maps label names to BasicBlocks
- `functions`: Registry of all declared functions
```cpp
class TACExecutor {
    LLVMContext ctx;
    unique_ptr<Module> module;
    IRBuilder<> builder;
    map<string, VariableInfo> namedValues;
    map<string, pair<Value*, VarType>> tempValues;
    map<string, BasicBlock*> labels;
    map<string, FunctionInfo> functions;
};
```

## TAC Instruction Format

The generated TAC follows these patterns:

**Variable Declaration**
```
type varname = value
```

**Assignment**
```
varname = value
```

**Binary Operations**
```
t0 = left + right
t1 = left == right
```

**Unary Operations**
```
t2 = -operand
t3 = !operand
```

**Function Declarations**
```
returntype function name begin
  type param1 = param
  type param2 = param
  // function body
function name end
```

**Function Calls**
```
t4 = call funcname(arg1, arg2)
call print(value)
```

**Control Flow**
```
if condition == 0 goto L0
goto L1
L0:
L1:
```

**Return Statements**
```
return value
return
```

## Type System

**VarType Enumeration**
```cpp
enum class VarType {
    INT,      // 32-bit integer
    FLOAT,    // 32-bit floating point
    DOUBLE,   // 64-bit floating point
    CHAR,     // 8-bit character
    STRING,   // Pointer to character array
    BOOL,     // Boolean (stored as int32)
    VOID,     // No return value
    UNKNOWN   // Type inference needed
};
```

**Type Conversions**
The LLVM generator automatically handles:
- Integer to float/double promotion
- Float to double extension
- Double to float truncation
- Float/double to integer conversion
- Boolean (i1) to integer (i32) extension
- Character (i8) to integer (i32) extension

## TAC Generation Process

**Expression Processing**
1. Process operands recursively
2. Generate temporary for intermediate results
3. Emit TAC instruction with operator
4. Return temporary name

**Statement Processing**
1. Traverse AST statement nodes
2. Generate appropriate TAC instructions
3. Manage indentation for nested blocks
4. Track labels for control flow

**Control Flow Handling**
- **If Statements**: Generate conditional jump with else/end labels
- **While Loops**: Create start label, condition check, loop body, back edge
- **Do-While Loops**: Create start label, body, condition at end
- **For Loops**: Handle initialization, condition, body, update
- **Switch Statements**: Generate case comparisons and jump table

## LLVM IR Generation Process

**Two-Pass Processing**

*First Pass: Function Discovery*
1. Scan all TAC lines for function declarations
2. Extract return types and parameter types
3. Create LLVM function signatures
4. Register functions in function table

*Second Pass: Code Generation*
1. Process each function body
2. Create entry basic block
3. Allocate space for parameters and locals
4. Pre-scan for labels and create BasicBlocks
5. Generate LLVM IR for each TAC instruction
6. Handle control flow with conditional/unconditional branches

**Instruction Translation**

Each TAC instruction maps to LLVM IR:
- Assignments → CreateStore
- Arithmetic → CreateAdd/CreateSub/CreateMul/CreateDiv
- Comparisons → CreateICmpEQ/CreateFCmpOEQ
- Function calls → CreateCall
- Returns → CreateRet/CreateRetVoid
- Conditional jumps → CreateCondBr
- Unconditional jumps → CreateBr

## Integration

**Input**
Receives from type checking phase:
- Type-checked AST with verified semantics
- Token list with source location information

**Intermediate Output (TAC)**
Produces human-readable TAC file:
```
tester/tac.txt
```

**Final Output**
- Generated LLVM IR (printed to console)
- Program execution results via JIT compilation

## Usage

**TAC Generation**
```cpp
void generateTAC(const vector<ASTPtr>& ast, const string& outputFilename);
```
Called from main compilation pipeline to generate TAC file.

**LLVM IR Generation and Execution**
```bash
# Compile the LLVM test binary (requires LLVM libraries)
g++ llvm_test.cpp -o llvm_test `llvm-config --cxxflags --ldflags --libs`

# Run with default TAC file
./llvm_test

# Run with custom TAC file
./llvm_test path/to/tac.txt
```

The LLVM executor:
1. Reads TAC file
2. Builds LLVM IR module
3. Verifies module correctness
4. Prints generated IR
5. Executes main function using JIT
6. Displays program output

## Examples

### Example 1: Simple Arithmetic

**Source Code**
```c
main {
    int x = 10;
    int y = 20;
    int sum = x + y;
    print(sum);
    return 0;
}
```

**Generated TAC**
```
function main begin
  int x = 10
  int y = 20
  t0 = x + y
  int sum = t0
  call print(sum)
  return 0
function main end
```

**LLVM IR Output**
```llvm
define i32 @main() {
entry:
  %x = alloca i32
  store i32 10, i32* %x
  %y = alloca i32
  store i32 20, i32* %y
  %0 = load i32, i32* %x
  %1 = load i32, i32* %y
  %2 = add i32 %0, %1
  %sum = alloca i32
  store i32 %2, i32* %sum
  %3 = load i32, i32* %sum
  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %3)
  ret i32 0
}
```

**Program Output**
```
30
```

### Example 2: Function with Parameters

**Source Code**
```c
int multiply(int a, int b) {
    int result = a * b;
    return result;
}

main {
    int answer = multiply(6, 7);
    print(answer);
    return 0;
}
```

**Generated TAC**
```
int function multiply begin
  int a = param
  int b = param
  t0 = a * b
  int result = t0
  return result
function multiply end

function main begin
  t1 = call multiply(6, 7)
  int answer = t1
  call print(answer)
  return 0
function main end
```

### Example 3: Control Flow (If-Else)

**Source Code**
```c
main {
    int x = 15;
    if (x > 10) {
        print(1);
    } else {
        print(0);
    }
    return 0;
}
```

**Generated TAC**
```
function main begin
  int x = 15
  t0 = x > 10
  if t0 == 0 goto L0
    call print(1)
  goto L1
L0:
    call print(0)
L1:
  return 0
function main end
```

### Example 4: While Loop

**Source Code**
```c
main {
    int i = 0;
    while (i < 5) {
        print(i);
        i = i + 1;
    }
    return 0;
}
```

**Generated TAC**
```
function main begin
  int i = 0
L0:
  t0 = i < 5
  if t0 == 0 goto L1
    call print(i)
    t1 = i + 1
    i = t1
  goto L0
L1:
  return 0
function main end
```

### Example 5: Type Conversions

**Source Code**
```c
main {
    int x = 10;
    float y = 3.14;
    float result = x + y;
    print(result);
    return 0;
}
```

**Generated TAC**
```
function main begin
  int x = 10
  float y = 3.14
  t0 = x + y
  float result = t0
  call print(result)
  return 0
function main end
```

The LLVM generator automatically converts `x` from int to float before the addition.

## Features and Optimizations

**TAC Generator Features**
- Readable output with proper indentation
- Type information preserved in TAC
- Efficient temporary variable allocation
- Label management for complex control flow
- Support for all language constructs

**LLVM Generator Features**
- Automatic type inference and conversion
- Proper handling of floating-point operations
- Built-in functions (printf, puts, putchar)
- JIT execution for immediate results
- Module verification for correctness

**Supported Operations**
- Arithmetic: +, -, *, /, %
- Comparison: ==, !=, <, >, <=, >=
- Logical: &&, ||, !
- Bitwise: &, |, ^, <<, >>
- Unary: -, !
- Assignment: =

## Error Handling

**TAC Generation Errors**
- File I/O errors when writing TAC output
- Throws runtime_error with descriptive message

**LLVM IR Errors**
- TAC file not found or cannot be opened
- Module verification failure
- Execution engine creation failure
- Missing main function

All errors are reported with clear messages to aid debugging.

## Extension Points

**Adding New AST Node Types**
1. Add visitor case in `processNode` method
2. Implement processing function for the new node type
3. Generate appropriate TAC instructions

**Adding New TAC Instructions**
1. Extend TAC format documentation
2. Add parsing logic in LLVM executor
3. Implement LLVM IR generation for new instruction

**Adding New Types**
1. Add type to `VarType` enumeration
2. Update `parseType` and `getLLVMType` functions
3. Implement conversion logic in `convertToType`

**Optimizations**
The current implementation prioritizes correctness and readability. Potential optimizations:
- Constant folding in TAC generation
- Dead code elimination
- Register allocation hints
- Common subexpression elimination
- LLVM optimization passes integration

## Verification

The module includes multiple verification stages:

1. **TAC Output Verification**: Inspect `tester/tac.txt` for correctness
2. **LLVM Module Verification**: Automated check using `verifyModule`
3. **IR Inspection**: Generated LLVM IR printed to console
4. **Runtime Verification**: Program execution produces expected output

## Dependencies

**Required Libraries**
- LLVM Development Libraries (for `llvm_test.cpp`)
  - llvm/IR/LLVMContext.h
  - llvm/IR/Module.h
  - llvm/IR/IRBuilder.h
  - llvm/ExecutionEngine/MCJIT.h

**Build Requirements**
```bash
# Install LLVM (version 14+ recommended)
# Linux: sudo apt-get install llvm-dev
# macOS: brew install llvm

# Link with LLVM libraries when compiling
g++ llvm_test.cpp -o llvm_test `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter native`
```

## Testing

**Test Strategy**
1. Generate TAC for various program structures
2. Verify TAC correctness manually or with automated checks
3. Run LLVM executor on generated TAC
4. Compare program output with expected results
5. Inspect generated LLVM IR for optimization opportunities

**Test Categories**
- Basic arithmetic and assignment
- Function calls with multiple parameters
- Control flow (if-else, loops, switch)
- Type conversions and mixed-type operations
- Nested structures and complex expressions
- Break statements in loops and switches
- Return statements in void and non-void functions