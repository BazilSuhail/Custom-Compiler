
```mathematica
program         ::= declaration* EOF ;

declaration     ::= varDecl
                  | fnDecl
                  | statement ;

varDecl         ::= type IDENTIFIER ( "=" expression )? ";" ;

fnDecl          ::= type IDENTIFIER "(" parameters? ")" block ;

parameters      ::= parameter ( "," parameter )* ;

parameter       ::= type IDENTIFIER ;

type            ::= "int" | "float" | "double" | "char" | "bool" | "void" ;

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
