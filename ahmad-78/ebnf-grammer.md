
```mathematica
program         ::= declaration* EOF ;

declaration     ::= varDecl
                  | statement ;

varDecl         ::= type IDENTIFIER ( "=" expression )? ";" ;

type            ::= "int" | "float" | "double" | "char" | "bool" ;

statement       ::= returnStmt
                  | ifStmt
                  | forStmt
                  | breakStmt
                  | assignStmt
                  | exprStmt ;

assignStmt      ::= IDENTIFIER "=" expression ";" ;

returnStmt      ::= "return" expression? ";" ;

breakStmt       ::= "break" ";" ;

ifStmt          ::= "if" "(" expression ")" block ("else" block)? ;

forStmt         ::= "for" "(" ( varDecl | exprStmt | ";" ) 
                        expression? ";" 
                        expression? ")" block ;

block           ::= "{" declaration* "}" ;

exprStmt        ::= expression ";" ;

expression      ::= assignment ;

assignment      ::= equality ( "=" assignment )? ;

equality        ::= comparison ( ( "==" | "!=" ) comparison )* ;

comparison      ::= term ( ( "<" | ">" | "<=" | ">=" ) term )* ;

term            ::= factor ( ( "+" | "-" ) factor )* ;

factor          ::= unary ( ( "*" | "/" ) unary )* ;

unary           ::= ( "+" | "-" | "++" | "--" ) unary
                  | postfix ;

postfix         ::= call ( "++" | "--" )? ;

call            ::= primary ( "(" arguments? ")" )? ;

arguments       ::= expression ( "," expression )* ;

primary         ::= literal
                  | IDENTIFIER
                  | "(" expression ")" ;

literal         ::= INTLIT | FLOATLIT | BOOLLIT | STRINGLIT | CHARLIT ;
``` 
