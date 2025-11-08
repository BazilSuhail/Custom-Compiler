```
Program        ::= IncludeStmt { ( FunctionProto | FunctionDecl | EnumDecl | Statement ) } MainDecl T_EOF

IncludeStmt    ::= T_INCLUDE ( "<" T_MAIN ">" | T_STRINGLIT | T_IDENTIFIER )

FunctionProto  ::= Type T_IDENTIFIER "(" [ ParamList ] ")" ";" 

FunctionDecl   ::= Type T_IDENTIFIER "(" [ ParamList ] ")" BlockStmt

ParamList      ::= Param { "," Param }
Param          ::= Type T_IDENTIFIER

EnumDecl       ::= T_ENUM T_IDENTIFIER T_LBRACE [ T_IDENTIFIER { "," T_IDENTIFIER } ] T_RBRACE ";"

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

VarDecl        ::= Type T_IDENTIFIER [ T_ASSIGNOP Expression ] ";"

ExpressionStmt ::= Expression ";"

IfStmt         ::= T_IF T_LPAREN Expression T_RPAREN BlockStmt [ T_ELSE BlockStmt ]

WhileStmt      ::= T_WHILE T_LPAREN Expression T_RPAREN BlockStmt

DoWhileStmt    ::= T_DO BlockStmt T_WHILE T_LPAREN Expression T_RPAREN ";"

ForStmt        ::= T_FOR T_LPAREN [ ( VarDecl | ExpressionStmt ) ] [ Expression ] ";" [ Expression ] T_RPAREN BlockStmt

SwitchStmt     ::= T_SWITCH T_LPAREN Expression T_RPAREN T_LBRACE { ( T_CASE Expression BlockStmt | T_DEFAULT BlockStmt ) } T_RBRACE

ReturnStmt     ::= T_RETURN [ Expression ] ";"

PrintStmt      ::= T_PRINT T_LPAREN [ ExpressionList ] T_RPAREN ";"

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
                 | T_LPAREN Expression T_RPAREN

CallSuffix     ::= T_LPAREN [ ExpressionList ] T_RPAREN

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