```
Program        ::= IncludeStmt { ( FunctionDecl | Statement ) } MainDecl T_EOF

IncludeStmt    ::= "include" ( "<main>" | T_STRINGLIT | T_IDENTIFIER )

FunctionDecl   ::= Type T_IDENTIFIER "(" [ ParamList ] ")" BlockStmt

ParamList      ::= Param { "," Param }
Param          ::= Type T_IDENTIFIER

MainDecl       ::= T_MAIN BlockStmt

BlockStmt      ::= T_LBRACE { Statement } T_RBRACE

Statement      ::= VarDecl
                 | ExpressionStmt
                 | BlockStmt
                 | IfStmt
                 | WhileStmt
                 | DoWhileStmt
                 | ForStmt
                 | SwitchStmt
                 | ReturnStmt
                 | PrintStmt
                 | T_BREAK ";"

VarDecl        ::= Type T_IDENTIFIER [ "=" Expression ] ";"

ExpressionStmt ::= Expression ";"

IfStmt         ::= T_IF "(" Expression ")" BlockStmt [ T_ELSE BlockStmt ]

WhileStmt      ::= T_WHILE "(" Expression ")" BlockStmt

DoWhileStmt    ::= T_DO BlockStmt T_WHILE "(" Expression ")" ";"

ForStmt        ::= T_FOR "(" [ ( VarDecl | ExpressionStmt ) ] [ Expression ] ";" [ Expression ] ")" BlockStmt

SwitchStmt     ::= T_SWITCH "(" Expression ")" T_LBRACE { ( T_CASE Expression BlockStmt | T_DEFAULT BlockStmt ) } T_RBRACE

ReturnStmt     ::= T_RETURN [ Expression ] ";"

PrintStmt      ::= T_PRINT "(" [ ExpressionList ] ")" ";"

ExpressionList ::= Expression { "," Expression }

Expression     ::= Assignment

Assignment     ::= LogicalOr [ T_ASSIGNOP Assignment ]

LogicalOr      ::= LogicalAnd { T_OR LogicalAnd }

LogicalAnd     ::= Equality { T_AND Equality }

Equality       ::= Comparison { ( T_EQUALOP | T_NE ) Comparison }

Comparison     ::= Term { ( T_LT | T_GT | T_LE | T_GE ) Term }

Term           ::= Factor { ( T_PLUS | T_MINUS ) Factor }

Factor         ::= Unary { ( T_MULTIPLY | T_DIVIDE | T_MODULO ) Unary }

Unary          ::= [ ( T_MINUS | T_NOT ) ] Primary

Primary        ::= Literal
                 | T_IDENTIFIER [ CallSuffix ]
                 | "(" Expression ")"

CallSuffix     ::= "(" [ ExpressionList ] ")"

Literal        ::= T_INTLIT
                 | T_FLOATLIT
                 | T_STRINGLIT
                 | T_CHARLIT
                 | T_BOOLLIT

Type           ::= T_INT
                 | T_FLOAT
                 | T_DOUBLE
                 | T_CHAR
                 | T_BOOL
                 | T_VOID
                 | T_STRING

```