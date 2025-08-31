# Custom Language Lexer / Tokenizer

This project provides a **lexer/tokenizer** for a simple C-like programming language.  
It tokenizes source code into meaningful tokens for further parsing or interpretation.

---

## Supported Tokens

### 1. **Keywords**

| Keyword    | Token Type     |
| ---------- | -------------- |
| `fn`       | `T_FUNCTION`   |
| `int`      | `T_INT`        |
| `float`    | `T_FLOAT`      |
| `bool`     | `T_BOOL`       |
| `string`   | `T_STRING`     |
| `return`   | `T_RETURN`     |
| `if`       | `T_IF`         |
| `else`     | `T_ELSE`       |
| `for`      | `T_FOR`        |
| `while`    | `T_WHILE`      |
| `break`    | `T_BREAK`      |
| `continue` | `T_CONTINUE`   |
| `true`     | `T_BOOLLIT`    |
| `false`    | `T_BOOLLIT`    |

---

### 2. **Identifiers**

* Must start with **alphabetic character or underscore (`_`)**.  
* Can contain **letters, digits, and underscores**.  
* Token type: `T_IDENTIFIER`  
* Invalid examples (cause `T_ERROR`):  
  * `1var` (cannot start with digit)  
  * `@foo` (invalid character)  

---

### 3. **Literals**

| Literal Type          | Examples            | Token Type    |
| --------------------- | ------------------- | ------------- |
| Integer               | `0`, `123`          | `T_INTLIT`    |
| Float                 | `3.14`, `0.5`       | `T_FLOATLIT`  |
| Boolean               | `true`, `false`     | `T_BOOLLIT`   |
| String (with escapes) | `"Hello\nWorld"`    | `T_STRINGLIT` |

**Escape sequences supported in strings:**

* `\n` → newline  
* `\t` → tab  
* `\"` → double quote  
* `\\` → backslash  

---

### 4. **Operators**

#### Arithmetic

| Symbol | Token Type |
| ------ | ---------- |
| `+`    | `T_PLUS`   |
| `-`    | `T_MINUS`  |
| `*`    | `T_STAR`   |
| `/`    | `T_SLASH`  |
| `%`    | `T_MOD`    |

#### Bitwise

| Symbol | Token Type |
| ------ | ---------- |
| `&`      | `T_BITAND`  |
| `\|`     | `T_BITOR`   |
| `^`      | `T_BITXOR`  |
| `~`      | `T_BITNOT`  |
| `<<`     | `T_BITSHIFTLEFT`     |
| `>>`     | `T_BITSHIFTRIGHT`     |

#### Comparison

| Symbol | Token Type     |
| ------ | -------------- |
| `==`   | `T_EQUALSOP`         |
| `!=`   | `T_NEQ`        |
| `<`    | `T_LT`         |
| `>`    | `T_GT`         |
| `<=`   | `T_LTE`        |
| `>=`   | `T_GTE`        |

#### Logical

| Symbol | Token Type    |
| ------ | ------------- |
| `&&`   | `T_AND`       |
| `\|\|`   | `T_OR`        |
| `!`    | `T_NOT`       |

#### Assignment

| Symbol | Token Type      |
| ------ | --------------- |
| `=`    | `T_ASSIGN`      |

---

### 5. **Punctuation**

| Symbol | Token Type    |
| ------ | ------------- |
| `;`    | `T_SEMICOLON` |
| `,`    | `T_COMMA`     |
| `(`    | `T_PARENL`    |
| `)`    | `T_PARENR`    |
| `{`    | `T_LBRACET`    |
| `}`    | `T_RBRACET`    |

---

## Example Code

```c
fn main() {
    int x = 10;
    float y = 3.14;
    string msg = "Hello\nWorld";

    for (int i = 0; i < 5; i++) {
        if (i == 3) {
            break;
        }
        continue;
    }

    bool flag = true;
    return 0;
}
