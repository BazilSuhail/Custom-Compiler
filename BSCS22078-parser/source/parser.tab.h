/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_INT = 258,                   /* T_INT  */
    T_FLOAT = 259,                 /* T_FLOAT  */
    T_DOUBLE = 260,                /* T_DOUBLE  */
    T_CHAR = 261,                  /* T_CHAR  */
    T_VOID = 262,                  /* T_VOID  */
    T_BOOL = 263,                  /* T_BOOL  */
    T_IDENTIFIER = 264,            /* T_IDENTIFIER  */
    T_STRINGLIT = 265,             /* T_STRINGLIT  */
    T_INTLIT = 266,                /* T_INTLIT  */
    T_FLOATLIT = 267,              /* T_FLOATLIT  */
    T_CHARLIT = 268,               /* T_CHARLIT  */
    T_BOOLLIT = 269,               /* T_BOOLLIT  */
    T_LPAREN = 270,                /* T_LPAREN  */
    T_RPAREN = 271,                /* T_RPAREN  */
    T_LBRACE = 272,                /* T_LBRACE  */
    T_RBRACE = 273,                /* T_RBRACE  */
    T_LBRACKET = 274,              /* T_LBRACKET  */
    T_RBRACKET = 275,              /* T_RBRACKET  */
    T_SEMICOLON = 276,             /* T_SEMICOLON  */
    T_COMMA = 277,                 /* T_COMMA  */
    T_DOT = 278,                   /* T_DOT  */
    T_COLON = 279,                 /* T_COLON  */
    T_ASSIGNOP = 280,              /* T_ASSIGNOP  */
    T_EQUALOP = 281,               /* T_EQUALOP  */
    T_NE = 282,                    /* T_NE  */
    T_LT = 283,                    /* T_LT  */
    T_GT = 284,                    /* T_GT  */
    T_LE = 285,                    /* T_LE  */
    T_GE = 286,                    /* T_GE  */
    T_PLUS = 287,                  /* T_PLUS  */
    T_MINUS = 288,                 /* T_MINUS  */
    T_MULTIPLY = 289,              /* T_MULTIPLY  */
    T_DIVIDE = 290,                /* T_DIVIDE  */
    T_MODULO = 291,                /* T_MODULO  */
    T_INCREMENT = 292,             /* T_INCREMENT  */
    T_DECREMENT = 293,             /* T_DECREMENT  */
    T_AND = 294,                   /* T_AND  */
    T_OR = 295,                    /* T_OR  */
    T_NOT = 296,                   /* T_NOT  */
    T_BITWISE_AND = 297,           /* T_BITWISE_AND  */
    T_BITWISE_OR = 298,            /* T_BITWISE_OR  */
    T_BITWISE_XOR = 299,           /* T_BITWISE_XOR  */
    T_BITWISE_NOT = 300,           /* T_BITWISE_NOT  */
    T_LEFT_SHIFT = 301,            /* T_LEFT_SHIFT  */
    T_RIGHT_SHIFT = 302,           /* T_RIGHT_SHIFT  */
    T_INCLUDE = 303,               /* T_INCLUDE  */
    T_IF = 304,                    /* T_IF  */
    T_ELSE = 305,                  /* T_ELSE  */
    T_WHILE = 306,                 /* T_WHILE  */
    T_FOR = 307,                   /* T_FOR  */
    T_RETURN = 308,                /* T_RETURN  */
    T_BREAK = 309,                 /* T_BREAK  */
    T_CONTINUE = 310,              /* T_CONTINUE  */
    T_SWITCH = 311,                /* T_SWITCH  */
    T_CASE = 312,                  /* T_CASE  */
    T_DEFAULT = 313,               /* T_DEFAULT  */
    T_DO = 314,                    /* T_DO  */
    T_CONST = 315,                 /* T_CONST  */
    T_ENUM = 316,                  /* T_ENUM  */
    T_SINGLE_COMMENT = 317,        /* T_SINGLE_COMMENT  */
    T_MULTI_COMMENT = 318,         /* T_MULTI_COMMENT  */
    T_ERROR = 319,                 /* T_ERROR  */
    UNARY_PREC = 320               /* UNARY_PREC  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 21 "parser.y"

    int intval;
    float floatval;
    char charval;
    int boolval;
    char *strval;
    ASTNode *node;
    DataType datatype;
    OperatorType optype;

#line 140 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
