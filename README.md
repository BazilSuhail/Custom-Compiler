# Custom Compiler – Lexical Analyzer Project

This repository contains the implementation of a **Lexical Analyzer** (Lexer/Tokenizer) as part of our Compiler Construction course project.  
The lexer is the first phase of a compiler and is responsible for scanning the source code, identifying lexemes, and producing a stream of tokens for further compilation stages.

---

## Main Files
- **`regex_lexer.cpp`** → Lexer implemented using **Regular Expressions**  
- **`manual_lexer.cpp`** → Lexer implemented using **raw string comparisons and state machines** (no regex, no third-party libraries) 

## 📂 Repository Structure

The project is structured by **group member folders < firstName - RollNumber >**, each containing individual implementations.  
Every group member works on their own folder within a dedicated branch (`lexer`), maintaining personal contributions and experimentation.

```
.
├── abdullah-54/
├── ahmad-78/
├── arshad-69/
└── bazil-72/
└── screenshots/

```

Each folder contains:
- **`regex_lexer.cpp`** → Lexer implemented using **Regular Expressions**  
- **`manual_lexer.cpp`** → Lexer implemented using **raw string comparisons and state machines** (no regex, no third-party libraries)   

---

## ✨ Features of the Lexer

1. **Tokenization of Core Constructs**

   * **Keywords**: `int`, `float`, `double`, `char`, `void`, `bool`, `if`, `else`, `while`, `for`, `return`, `break`, `continue`, `switch`, `case`, `default`, `do`, `const`, `static`, `signed`, `unsigned`, `short`, `long`, `enum`, `typedef`
   * **Identifiers**: User-defined names; validated to **not start with digits**
   * **Operators**:

     * Arithmetic: `+ - * / %`
     * Increment/Decrement: `++ --`
     * Relational: `== != <= >= < >`
     * Logical: `&& || !`
     * Bitwise: `& | ^ ~ << >>`
     * Assignment: `= += -=`
   * **Delimiters / Brackets**: `, ; ( ) { } [ ] .`

2. **String and Character Literals**

   * Handles `"..."` strings and `'...'` character literals
   * Supports **escape sequences** like `\n`, `\t`, `\\`, `\"`, etc.
   * Unterminated strings or char literals are flagged as **errors**

3. **Preprocessor Directives**

   * Recognizes `#include` and `#define`

4. **Number Literals**

   * **Integers** and **floating-point numbers** 
   * Detects invalid identifiers that start with a digit

5. **Error Handling**

   * Invalid identifiers (e.g., starting with numbers)
   * Unknown characters
   * Unterminated strings and char literals

6. **Output**

   * Produces a **token stream** with token type, value, and source position:

     ```
     [T_INT, T_IDENTIFIER("myVar"), T_ASSIGNOP("="), T_INTLIT("5"), T_SEMICOLON, ...]
     ```

7. **Comment Handling**

   * Skips single-line (`//`) and multi-line (`/* ... */`) comments

8. **Lexer State Tracking**

   * Tracks **line and column** for each token
   * Maintains a **keyword table** for quick lookup
---

## 🖥️ Example

### Example 1:

**Input Program:**
```c
    #include <stdio.h>
    #define MAX 100
    int main() { 
        // this is a comment
        int x = 42;
        float y = 3.14159f;
        char c = '\n';
        char* str = "Hello\tWorld! \"quoted\"";
        
        /* Multi-line
           comment */
        
        if (x > 0 && x <= MAX) {
            printf("Value: %d\n", x);
        } else if (x == 0 || x >= MAX) {
            printf("Boundary value\n");
        }
        
        for (int i = 0; i < 10; i++) {
            x += i;
            x++;
            x--;
        }
        
        while (x > 0) {
            x >>= 1;  // Right shift
            x &= 0xFF; // Bitwise AND
        }
        return 0;
    }
```

**Output Token Stream:**
```
T_INCLUDE("#include"), T_LT("<"), T_IDENTIFIER("stdio"), T_DOT("."), T_IDENTIFIER("h"), T_GT(">"), T_DEFINE("#define"), T_IDENTIFIER("MAX"), T_INTLIT("100"), T_INT("int"), T_IDENTIFIER("main"), T_LPAREN("("), T_RPAREN(")"), T_LBRACE("{"), T_SINGLE_COMMENT("// this is a comment"), T_INT("int"), T_IDENTIFIER("x"), T_ASSIGNOP("="), T_INTLIT("42"), T_SEMICOLON(";"), T_FLOAT("float"), T_IDENTIFIER("y"), T_ASSIGNOP("="), T_FLOATLIT("3.14159"), T_IDENTIFIER("f"), T_SEMICOLON(";"), T_CHAR("char"), T_IDENTIFIER("c"), T_ASSIGNOP("="), T_CHARLIT("'\n'"), T_SEMICOLON(";"), T_CHAR("char"), T_MULTIPLY("*"), T_IDENTIFIER("str"), T_ASSIGNOP("="), T_STRINGLIT(""Hello\tWorld! \"quoted\"""), T_SEMICOLON(";"), T_MULTI_COMMENT("/* Multi-line comment */"), T_IF("if"), T_LPAREN("("), T_IDENTIFIER("x"), T_GT(">"), T_INTLIT("0"), T_AND("&&"), T_IDENTIFIER("x"), T_LE("<="), T_IDENTIFIER("MAX"), T_RPAREN(")"), T_LBRACE("{"), T_IDENTIFIER("printf"), T_LPAREN("("), T_STRINGLIT(""Value: %d\n""), T_COMMA(","), T_IDENTIFIER("x"), T_RPAREN(")"), T_SEMICOLON(";"), T_RBRACE("}"), T_ELSE("else"), T_IF("if"), T_LPAREN("("), T_IDENTIFIER("x"), T_EQUALOP("=="), T_INTLIT("0"), T_OR("||"), T_IDENTIFIER("x"), T_GE(">="), T_IDENTIFIER("MAX"), T_RPAREN(")"), T_LBRACE("{"), T_IDENTIFIER("printf"), T_LPAREN("("), T_STRINGLIT(""Boundary value\n""), T_RPAREN(")"), T_SEMICOLON(";"), T_RBRACE("}"), T_FOR("for"), T_LPAREN("("), T_INT("int"), T_IDENTIFIER("i"), T_ASSIGNOP("="), T_INTLIT("0"), T_SEMICOLON(";"), T_IDENTIFIER("i"), T_LT("<"), T_INTLIT("10"), T_SEMICOLON(";"), T_IDENTIFIER("i"), T_INCREMENT("++"), T_RPAREN(")"), T_LBRACE("{"), T_IDENTIFIER("x"), T_PLUS("+"), T_ASSIGNOP("="), T_IDENTIFIER("i"), T_SEMICOLON(";"), T_IDENTIFIER("x"), T_INCREMENT("++"), T_SEMICOLON(";"), T_IDENTIFIER("x"), T_DECREMENT("--"), T_SEMICOLON(";"), T_RBRACE("}"), T_WHILE("while"), T_LPAREN("("), T_IDENTIFIER("x"), T_GT(">"), T_INTLIT("0"), T_RPAREN(")"), T_LBRACE("{"), T_IDENTIFIER("x"), T_RIGHT_SHIFT(">>"), T_ASSIGNOP("="), T_INTLIT("1"), T_SEMICOLON(";"), T_SINGLE_COMMENT("// Right shift"), T_IDENTIFIER("x"), T_BITWISE_AND("&"), T_ASSIGNOP("="), ERROR: Invalid identifier: 0xFF at line 28, column 18
T_SEMICOLON(";"), T_SINGLE_COMMENT("// Bitwise AND"), T_RBRACE("}"), T_RETURN("return"), T_INTLIT("0"), T_SEMICOLON(";"), T_RBRACE("}"),
```

---

### Example 2:

**Input Program:**
```c
string var1 = "i am  lexer 💕🙌";
int 123var = 5;
```

**Output Token Stream:**
```
T_IDENTIFIER("string"), T_IDENTIFIER("var1"), T_ASSIGNOP("="), T_STRINGLIT(""i am  lexer 💕🙌""),
T_SEMICOLON(";"), T_INT("int"), ERROR: Invalid identifier: 123var at line 3, column 5
T_ASSIGNOP("="), T_INTLIT("5"), T_SEMICOLON(";"), 
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
   git clone https://github.com/BazilSuhail/Custom-Compiler
   cd Custom-Compiler
   ```

2. Switch to the `lexer` branch:
   ```bash
   git checkout lexer
   ```

3. Navigate to a member’s folder, then into either `regex_lexer` or `manual_lexer`.

4. Run the lexer (example in Python):
   ```bash
   g++ manual_lexer.cpp -o lexer
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

---

## 🔗 Submission Format

- Each group member’s code resides in their folder.  
- Screenshots of **input program** and **output token stream** are included in the **screenshots** folder.   
---

**Maintained by Group Members (Abdullah-54, Ahmad-78, Arshad-69, Bazil-72)**  
Compiler Construction Course – 2025
