
```mathematica
program         ::= declaration* EOF ;

declaration     ::= includeDirective
                  | defineDirective
                  | enumDecl
                  | varDecl
                  | fnDecl
                  | statement ;

varDecl         ::= typeSpec IDENTIFIER ( "=" expression )? ";" ;

fnDecl          ::= typeSpec IDENTIFIER "(" parameters? ")" block ;

parameters      ::= parameter ( "," parameter )* ;

parameter       ::= typeSpec IDENTIFIER ;

typeSpec        ::= typeQualifier? baseType ;

typeQualifier   ::= "const" ;

baseType        ::= "int" | "float" | "double" | "char" | "bool" | "void" ;

includeDirective ::= "#include" ( "<" IDENTIFIER ( "." IDENTIFIER )? ">" 
                                | STRINGLIT ) ;

defineDirective ::= "#define" IDENTIFIER ( "(" parameterList? ")" )? macroBody? ;

parameterList   ::= IDENTIFIER ( "," IDENTIFIER )* ;

macroBody       ::= ( IDENTIFIER | literal | operator )* ;

enumDecl        ::= "enum" IDENTIFIER "{" enumValueList? "}" ";" ;

enumValueList   ::= IDENTIFIER ( "," IDENTIFIER )* ;

statement       ::= returnStmt
                  | ifStmt
                  | whileStmt
                  | doWhileStmt
                  | forStmt
                  | switchStmt
                  | breakStmt
                  | continueStmt
                  | assignStmt
                  | exprStmt ;

assignStmt      ::= IDENTIFIER "=" expression ";" ;

returnStmt      ::= "return" expression? ";" ;

breakStmt       ::= "break" ";" ;

continueStmt    ::= "continue" ";" ;

ifStmt          ::= "if" "(" expression ")" block ("else" block)? ;

whileStmt       ::= "while" "(" expression ")" block ;

doWhileStmt     ::= "do" block "while" "(" expression ")" ";" ;

forStmt         ::= "for" "(" ( varDecl | exprStmt | ";" ) 
                        expression? ";" 
                        expression? ")" block ;

switchStmt      ::= "switch" "(" expression ")" "{" caseStmt* "}" ;

caseStmt        ::= ( "case" expression ":" | "default" ":" ) declaration* ;

block           ::= "{" declaration* "}" ;

exprStmt        ::= expression ";" ;

expression      ::= assignment ;

assignment      ::= logicalOr ( "=" assignment )? ;

logicalOr       ::= logicalAnd ( "||" logicalAnd )* ;

logicalAnd      ::= bitwiseOr ( "&&" bitwiseOr )* ;

bitwiseOr       ::= bitwiseXor ( "|" bitwiseXor )* ;

bitwiseXor      ::= bitwiseAnd ( "^" bitwiseAnd )* ;

bitwiseAnd      ::= equality ( "&" equality )* ;

equality        ::= comparison ( ( "==" | "!=" ) comparison )* ;

comparison      ::= shift ( ( "<" | ">" | "<=" | ">=" ) shift )* ;

shift           ::= term ( ( "<<" | ">>" ) term )* ;

term            ::= factor ( ( "+" | "-" ) factor )* ;

factor          ::= unary ( ( "*" | "/" | "%" ) unary )* ;

unary           ::= ( "+" | "-" | "!" | "~" | "++" | "--" ) unary
                  | postfix ;

postfix         ::= call ( "++" | "--" )? ;

call            ::= primary ( "(" arguments? ")" )? ;

arguments       ::= expression ( "," expression )* ;

primary         ::= literal
                  | IDENTIFIER  
                  | "(" expression ")"
                  | ( "++" | "--" ) unary ;

literal         ::= INTLIT | FLOATLIT | BOOLLIT | STRINGLIT | CHARLIT ;

operator        ::= "+" | "-" | "*" | "/" | "%" | "=" | "==" | "!=" | "<" | ">" 
                  | "<=" | ">=" | "&&" | "||" | "!" | "&" | "|" | "^" | "~" 
                  | "<<" | ">>" | "++" | "--" | "(" | ")" | "{" | "}" | ";" 
                  | "," | "." | "[" | "]" ;
```

### 📝 Grammar Notes:
- All statements require blocks (`{}`) for consistency
- Switch cases support fall-through behavior
- Type specifications are flexible and composable
- Expression precedence follows C language standards
- Preprocessor directives are parsed at the declaration level
- Include directives support both system (`<>`) and user (`""`) includes
- Define macros support both object-like and function-like definitions
- Enum declarations require semicolon termination
- Enum values are comma-separated identifiers 
