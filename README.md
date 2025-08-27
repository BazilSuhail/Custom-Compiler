# Custom Compiler – Lexical Analyzer Project

This repository contains the implementation of a **Lexical Analyzer** (Lexer/Tokenizer) as part of our Compiler Construction course project.  
The lexer is the first phase of a compiler and is responsible for scanning the source code, identifying lexemes, and producing a stream of tokens for further compilation stages.

---

## 📂 Repository Structure

The project is structured by **group member folders < firstName - RollNumber >**, each containing individual implementations.  
Every group member works on their own folder within a dedicated branch (`lexer`), maintaining personal contributions and experimentation.

```
.
├── abdullah-54/
├── ahmad-78/
├── arshad-69/
└── bazil-72/
```

Each folder contains:
- **`regex_lexer.cpp`** → Lexer implemented using **Regular Expressions**  
- **`manual_lexer.cpp`** → Lexer implemented using **raw string comparisons and state machines** (no regex, no third-party libraries)  
- **`screenshots/`** → Input program snippets and captured token outputs   

---

## ✨ Features of the Lexer

1. **Tokenization of Core Constructs**
   - Keywords: `fn`, `int`, `float`, `string`, `bool`, `return`, `if`, `else`, `for`, `while`
   - Identifiers: User-defined names (validated – cannot start with digits)
   - Operators: Arithmetic (`+ - * / %`), Logical (`&& || !`), Relational (`== != <= >= < >`), Bitwise (`& | ^ ~ << >>`)
   - Assignment and comparison operators (`=`, `==`, `+=`, `-=`, etc.)
   - Delimiters: `, ; ( ) { } [ ]`

2. **String and Character Literals**
   - Supports escaped sequences (`\n`, `\t`, `\\`, `\"`, etc.)
   - Handles Unicode characters (bonus feature: allows emojis and non-Latin scripts)

3. **Error Handling**
   - Invalid identifiers (e.g., names starting with digits) raise explicit error messages
   - Unterminated strings and malformed tokens are flagged

4. **Output**
   - Produces a **token stream/vector** in the format:
     ```
     [T_FUNCTION, T_INT, T_IDENTIFIER("my_fn"), T_PARENL, ... , T_BRACER]
     ```

---

## 🖥️ Example

**Input Program:**
```c
fn int my_fn(int x, float y) {
    string my_str = "hmm";
    bool my_bool = x == 40;
    return x;
}
```

**Output Token Stream:**
```
[T_FUNCTION, T_INT, T_IDENTIFIER("my_fn"), T_PARENL, T_INT, T_IDENTIFIER("x"),
 T_COMMA, T_FLOAT, T_IDENTIFIER("y"), T_PARENR, T_BRACEL,
 T_STRING, T_IDENTIFIER("my_str"), T_ASSIGNOP, T_QUOTES, T_STRINGLIT("hmm"), 
 T_QUOTES, T_SEMICOLON, T_BOOL, T_IDENTIFIER("my_bool"), T_ASSIGNOP, 
 T_IDENTIFIER("x"), T_EQUALSOP, T_INTLIT(40), T_SEMICOLON, 
 T_RETURN, T_IDENTIFIER("x"), T_SEMICOLON, T_BRACER]
```

---

## 👥 Group Members

| Name      | Roll No | Folder       |
|-----------|---------|--------------|
| Abdullah  | 54      | `abdullah-54/` |
| Ahmad     | 78      | `ahmad-78/`   |
| Arshad    | 69      | `arshad-69/`  |
| Bazil     | 72      | `bazil-72/`   |

> Each member maintains their implementation inside their respective folder and branch.

---

## 🚀 How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/<your-org-or-username>/toy-compiler.git
   cd toy-compiler
   ```

2. Switch to the `lexer` branch:
   ```bash
   git checkout lexer
   ```

3. Navigate to a member’s folder, then into either `regex_lexer` or `manual_lexer`.

4. Run the lexer (example in Python):
   ```bash
   g++ lexer.cpp -o lexer
   ```

5. Check the generated **token stream** in console output.
---

## 📖 Notes

- The **regex-based lexer** offers concise implementation, leveraging pattern-matching for fast tokenization.  
- The **manual lexer** provides deeper insight into how compilers handle lexical analysis via **finite state machines** and string scanning.  
- Both approaches are maintained for academic completeness.

---

## 🏆 Bonus Features

- Unicode and emoji support in identifiers and strings  
- Extensive error reporting with descriptive messages  
- Modular structure allowing integration with **future phases**: parsing, semantic analysis, and code generation  

---

## 🔗 Submission Format

- Each group member’s code resides in their folder.  
- Screenshots of **input program** and **output token stream** are included.   
---

## 📌 Future Work

This repository will be extended into a **full mini-compiler project**, with upcoming branches covering:
- Parser (syntax analysis)  
- Semantic analyzer  
- Optimization and final code generation  

---

**Maintained by Group Members (Abdullah-54, Ahmad-78, Arshad-69, Bazil-72)**  
Compiler Construction Course – 2025
