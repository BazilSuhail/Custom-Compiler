Terminals
``` mathematica
T_INT, T_FLOAT, T_DOUBLE, T_CHAR, T_BOOL,
T_IDENTIFIER, T_ASSIGNOP, T_SEMICOLON,
T_RETURN, T_IF, T_ELSE, T_FOR, T_BREAK,
T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,
T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE,
T_EQUALOP, T_NE, T_LT, T_GT, T_LE, T_GE,
T_INCREMENT, T_DECREMENT,
T_COMMA,
T_INTLIT, T_FLOATLIT, T_BOOLLIT, T_STRINGLIT, T_CHARLIT,
EOF

```

Non-Terminals   
``` mathematica
Program, Decls, Decl, VarDecl, Type, Stmt, AssignStmt, ReturnStmt, BreakStmt,
IfStmt, ForStmt, Block, ExprStmt,
Expr, Assignment, Equality, Comparison, Term, Factor, Unary, Postfix, Call,
Args, ArgList, Primary, Literal

```

Productions CFG
```mathematica
Program     → Decls EOF
Decls       → Decl Decls | ε

Decl        → VarDecl
            | Stmt

VarDecl     → Type T_IDENTIFIER VarInit T_SEMICOLON
VarInit     → T_ASSIGNOP Expr | ε
Type        → T_INT | T_FLOAT | T_DOUBLE | T_CHAR | T_BOOL

Stmt        → ReturnStmt
            | IfStmt
            | ForStmt
            | BreakStmt
            | AssignStmt
            | ExprStmt

AssignStmt  → T_IDENTIFIER T_ASSIGNOP Expr T_SEMICOLON
ReturnStmt  → T_RETURN ExprOpt T_SEMICOLON
ExprOpt     → Expr | ε
BreakStmt   → T_BREAK T_SEMICOLON

IfStmt      → T_IF T_LPAREN Expr T_RPAREN Block ElseOpt
ElseOpt     → T_ELSE Block | ε

ForStmt     → T_FOR T_LPAREN ForInit ExprOpt T_SEMICOLON ExprOpt T_RPAREN Block
ForInit     → VarDecl | ExprStmt | T_SEMICOLON

Block       → T_LBRACE Decls T_RBRACE
ExprStmt    → Expr T_SEMICOLON

Expr        → Assignment

Assignment  → Equality AssignTail
AssignTail  → T_ASSIGNOP Assignment | ε

Equality    → Comparison EqualityTail
EqualityTail→ (T_EQUALOP | T_NE) Comparison EqualityTail | ε

Comparison  → Term CompTail
CompTail    → (T_LT | T_GT | T_LE | T_GE) Term CompTail | ε

Term        → Factor TermTail
TermTail    → (T_PLUS | T_MINUS) Factor TermTail | ε

Factor      → Unary FactorTail
FactorTail  → (T_MULTIPLY | T_DIVIDE) Unary FactorTail | ε

Unary       → (T_PLUS | T_MINUS | T_INCREMENT | T_DECREMENT) Unary
            | Postfix

Postfix     → Call PostfixTail
PostfixTail → (T_INCREMENT | T_DECREMENT) | ε

Call        → Primary CallTail
CallTail    → T_LPAREN ArgsOpt T_RPAREN | ε
ArgsOpt     → Args | ε
Args        → Expr ArgList
ArgList     → T_COMMA Expr ArgList | ε

Primary     → Literal
            | T_IDENTIFIER
            | T_LPAREN Expr T_RPAREN

Literal     → T_INTLIT | T_FLOATLIT | T_BOOLLIT | T_STRINGLIT | T_CHARLIT

```