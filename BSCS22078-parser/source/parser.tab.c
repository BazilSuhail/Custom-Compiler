/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int line_number;
extern int column_number;

void yyerror(const char *s);

ASTNode *root = NULL;  // Global AST root


#line 89 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_INT = 3,                      /* T_INT  */
  YYSYMBOL_T_FLOAT = 4,                    /* T_FLOAT  */
  YYSYMBOL_T_DOUBLE = 5,                   /* T_DOUBLE  */
  YYSYMBOL_T_CHAR = 6,                     /* T_CHAR  */
  YYSYMBOL_T_VOID = 7,                     /* T_VOID  */
  YYSYMBOL_T_BOOL = 8,                     /* T_BOOL  */
  YYSYMBOL_T_IDENTIFIER = 9,               /* T_IDENTIFIER  */
  YYSYMBOL_T_STRINGLIT = 10,               /* T_STRINGLIT  */
  YYSYMBOL_T_INTLIT = 11,                  /* T_INTLIT  */
  YYSYMBOL_T_FLOATLIT = 12,                /* T_FLOATLIT  */
  YYSYMBOL_T_CHARLIT = 13,                 /* T_CHARLIT  */
  YYSYMBOL_T_BOOLLIT = 14,                 /* T_BOOLLIT  */
  YYSYMBOL_T_LPAREN = 15,                  /* T_LPAREN  */
  YYSYMBOL_T_RPAREN = 16,                  /* T_RPAREN  */
  YYSYMBOL_T_LBRACE = 17,                  /* T_LBRACE  */
  YYSYMBOL_T_RBRACE = 18,                  /* T_RBRACE  */
  YYSYMBOL_T_LBRACKET = 19,                /* T_LBRACKET  */
  YYSYMBOL_T_RBRACKET = 20,                /* T_RBRACKET  */
  YYSYMBOL_T_SEMICOLON = 21,               /* T_SEMICOLON  */
  YYSYMBOL_T_COMMA = 22,                   /* T_COMMA  */
  YYSYMBOL_T_DOT = 23,                     /* T_DOT  */
  YYSYMBOL_T_COLON = 24,                   /* T_COLON  */
  YYSYMBOL_T_ASSIGNOP = 25,                /* T_ASSIGNOP  */
  YYSYMBOL_T_EQUALOP = 26,                 /* T_EQUALOP  */
  YYSYMBOL_T_NE = 27,                      /* T_NE  */
  YYSYMBOL_T_LT = 28,                      /* T_LT  */
  YYSYMBOL_T_GT = 29,                      /* T_GT  */
  YYSYMBOL_T_LE = 30,                      /* T_LE  */
  YYSYMBOL_T_GE = 31,                      /* T_GE  */
  YYSYMBOL_T_PLUS = 32,                    /* T_PLUS  */
  YYSYMBOL_T_MINUS = 33,                   /* T_MINUS  */
  YYSYMBOL_T_MULTIPLY = 34,                /* T_MULTIPLY  */
  YYSYMBOL_T_DIVIDE = 35,                  /* T_DIVIDE  */
  YYSYMBOL_T_MODULO = 36,                  /* T_MODULO  */
  YYSYMBOL_T_INCREMENT = 37,               /* T_INCREMENT  */
  YYSYMBOL_T_DECREMENT = 38,               /* T_DECREMENT  */
  YYSYMBOL_T_AND = 39,                     /* T_AND  */
  YYSYMBOL_T_OR = 40,                      /* T_OR  */
  YYSYMBOL_T_NOT = 41,                     /* T_NOT  */
  YYSYMBOL_T_BITWISE_AND = 42,             /* T_BITWISE_AND  */
  YYSYMBOL_T_BITWISE_OR = 43,              /* T_BITWISE_OR  */
  YYSYMBOL_T_BITWISE_XOR = 44,             /* T_BITWISE_XOR  */
  YYSYMBOL_T_BITWISE_NOT = 45,             /* T_BITWISE_NOT  */
  YYSYMBOL_T_LEFT_SHIFT = 46,              /* T_LEFT_SHIFT  */
  YYSYMBOL_T_RIGHT_SHIFT = 47,             /* T_RIGHT_SHIFT  */
  YYSYMBOL_T_INCLUDE = 48,                 /* T_INCLUDE  */
  YYSYMBOL_T_IF = 49,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 50,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 51,                   /* T_WHILE  */
  YYSYMBOL_T_FOR = 52,                     /* T_FOR  */
  YYSYMBOL_T_RETURN = 53,                  /* T_RETURN  */
  YYSYMBOL_T_BREAK = 54,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 55,                /* T_CONTINUE  */
  YYSYMBOL_T_SWITCH = 56,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 57,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 58,                 /* T_DEFAULT  */
  YYSYMBOL_T_DO = 59,                      /* T_DO  */
  YYSYMBOL_T_CONST = 60,                   /* T_CONST  */
  YYSYMBOL_T_ENUM = 61,                    /* T_ENUM  */
  YYSYMBOL_T_SINGLE_COMMENT = 62,          /* T_SINGLE_COMMENT  */
  YYSYMBOL_T_MULTI_COMMENT = 63,           /* T_MULTI_COMMENT  */
  YYSYMBOL_T_ERROR = 64,                   /* T_ERROR  */
  YYSYMBOL_UNARY_PREC = 65,                /* UNARY_PREC  */
  YYSYMBOL_YYACCEPT = 66,                  /* $accept  */
  YYSYMBOL_program = 67,                   /* program  */
  YYSYMBOL_declaration_list = 68,          /* declaration_list  */
  YYSYMBOL_declaration = 69,               /* declaration  */
  YYSYMBOL_varDecl = 70,                   /* varDecl  */
  YYSYMBOL_fnDecl = 71,                    /* fnDecl  */
  YYSYMBOL_parameters = 72,                /* parameters  */
  YYSYMBOL_parameter = 73,                 /* parameter  */
  YYSYMBOL_typeSpec = 74,                  /* typeSpec  */
  YYSYMBOL_typeQualifier = 75,             /* typeQualifier  */
  YYSYMBOL_baseType = 76,                  /* baseType  */
  YYSYMBOL_includeDirective = 77,          /* includeDirective  */
  YYSYMBOL_enumDecl = 78,                  /* enumDecl  */
  YYSYMBOL_enumValueList = 79,             /* enumValueList  */
  YYSYMBOL_statement = 80,                 /* statement  */
  YYSYMBOL_assignStmt = 81,                /* assignStmt  */
  YYSYMBOL_returnStmt = 82,                /* returnStmt  */
  YYSYMBOL_breakStmt = 83,                 /* breakStmt  */
  YYSYMBOL_continueStmt = 84,              /* continueStmt  */
  YYSYMBOL_ifStmt = 85,                    /* ifStmt  */
  YYSYMBOL_whileStmt = 86,                 /* whileStmt  */
  YYSYMBOL_doWhileStmt = 87,               /* doWhileStmt  */
  YYSYMBOL_forStmt = 88,                   /* forStmt  */
  YYSYMBOL_switchStmt = 89,                /* switchStmt  */
  YYSYMBOL_caseStmt_list = 90,             /* caseStmt_list  */
  YYSYMBOL_caseStmt = 91,                  /* caseStmt  */
  YYSYMBOL_block = 92,                     /* block  */
  YYSYMBOL_exprStmt = 93,                  /* exprStmt  */
  YYSYMBOL_expression = 94,                /* expression  */
  YYSYMBOL_assignment = 95,                /* assignment  */
  YYSYMBOL_logicalOr = 96,                 /* logicalOr  */
  YYSYMBOL_logicalAnd = 97,                /* logicalAnd  */
  YYSYMBOL_bitwiseOr = 98,                 /* bitwiseOr  */
  YYSYMBOL_bitwiseXor = 99,                /* bitwiseXor  */
  YYSYMBOL_bitwiseAnd = 100,               /* bitwiseAnd  */
  YYSYMBOL_equality = 101,                 /* equality  */
  YYSYMBOL_comparison = 102,               /* comparison  */
  YYSYMBOL_shift = 103,                    /* shift  */
  YYSYMBOL_term = 104,                     /* term  */
  YYSYMBOL_factor = 105,                   /* factor  */
  YYSYMBOL_unary = 106,                    /* unary  */
  YYSYMBOL_postfix = 107,                  /* postfix  */
  YYSYMBOL_call = 108,                     /* call  */
  YYSYMBOL_arguments = 109,                /* arguments  */
  YYSYMBOL_primary = 110,                  /* primary  */
  YYSYMBOL_literal = 111                   /* literal  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  91
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   662

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  66
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  46
/* YYNRULES -- Number of rules.  */
#define YYNRULES  131
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  251

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   320


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    89,    89,    93,    97,    98,   103,   104,   105,   106,
     107,   112,   114,   120,   122,   127,   128,   132,   137,   138,
     142,   146,   147,   148,   149,   150,   151,   156,   158,   163,
     169,   171,   176,   178,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   197,   202,   203,   207,   211,   215,
     217,   222,   227,   232,   234,   236,   238,   240,   242,   244,
     246,   248,   250,   252,   254,   259,   261,   266,   267,   271,
     273,   275,   277,   282,   283,   287,   292,   296,   297,   302,
     303,   308,   309,   314,   315,   320,   321,   326,   327,   332,
     333,   335,   340,   341,   343,   345,   347,   352,   353,   355,
     360,   361,   363,   368,   369,   371,   373,   378,   379,   381,
     383,   385,   387,   389,   394,   395,   397,   402,   403,   410,
     420,   421,   425,   426,   427,   428,   429,   433,   434,   435,
     436,   437
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "T_INT", "T_FLOAT",
  "T_DOUBLE", "T_CHAR", "T_VOID", "T_BOOL", "T_IDENTIFIER", "T_STRINGLIT",
  "T_INTLIT", "T_FLOATLIT", "T_CHARLIT", "T_BOOLLIT", "T_LPAREN",
  "T_RPAREN", "T_LBRACE", "T_RBRACE", "T_LBRACKET", "T_RBRACKET",
  "T_SEMICOLON", "T_COMMA", "T_DOT", "T_COLON", "T_ASSIGNOP", "T_EQUALOP",
  "T_NE", "T_LT", "T_GT", "T_LE", "T_GE", "T_PLUS", "T_MINUS",
  "T_MULTIPLY", "T_DIVIDE", "T_MODULO", "T_INCREMENT", "T_DECREMENT",
  "T_AND", "T_OR", "T_NOT", "T_BITWISE_AND", "T_BITWISE_OR",
  "T_BITWISE_XOR", "T_BITWISE_NOT", "T_LEFT_SHIFT", "T_RIGHT_SHIFT",
  "T_INCLUDE", "T_IF", "T_ELSE", "T_WHILE", "T_FOR", "T_RETURN", "T_BREAK",
  "T_CONTINUE", "T_SWITCH", "T_CASE", "T_DEFAULT", "T_DO", "T_CONST",
  "T_ENUM", "T_SINGLE_COMMENT", "T_MULTI_COMMENT", "T_ERROR", "UNARY_PREC",
  "$accept", "program", "declaration_list", "declaration", "varDecl",
  "fnDecl", "parameters", "parameter", "typeSpec", "typeQualifier",
  "baseType", "includeDirective", "enumDecl", "enumValueList", "statement",
  "assignStmt", "returnStmt", "breakStmt", "continueStmt", "ifStmt",
  "whileStmt", "doWhileStmt", "forStmt", "switchStmt", "caseStmt_list",
  "caseStmt", "block", "exprStmt", "expression", "assignment", "logicalOr",
  "logicalAnd", "bitwiseOr", "bitwiseXor", "bitwiseAnd", "equality",
  "comparison", "shift", "term", "factor", "unary", "postfix", "call",
  "arguments", "primary", "literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-152)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     300,  -152,  -152,  -152,  -152,  -152,  -152,   -22,  -152,  -152,
    -152,  -152,  -152,   617,   617,   617,   617,   617,   617,   617,
      -4,   -10,    36,    85,   126,    44,    63,    95,    59,  -152,
      52,   113,   300,  -152,  -152,  -152,   111,    86,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,   100,  -152,   -17,    83,    84,    87,    88,    46,
      50,    40,   -18,    61,  -152,  -152,    69,   114,  -152,   617,
    -152,   112,  -152,  -152,  -152,  -152,  -152,  -152,  -152,   123,
     617,   617,   359,  -152,   121,  -152,  -152,   617,   182,    82,
     117,  -152,  -152,    49,  -152,  -152,   617,   617,   617,   617,
     617,   617,   617,   617,   617,   617,   617,   617,   617,   617,
     617,   617,   617,   617,   617,  -152,  -152,   373,   122,  -152,
      11,   128,   129,   411,   425,   137,   462,  -152,   132,  -152,
     241,   134,    -2,    23,  -152,   617,  -152,    83,    84,    87,
      88,    46,    50,    50,    40,    40,    40,    40,   -18,   -18,
      61,    61,  -152,  -152,  -152,  -152,  -152,    38,  -152,   141,
    -152,    59,    59,   476,   131,   513,   133,    12,   523,   136,
     144,  -152,   617,  -152,   145,    14,    59,    55,  -152,   156,
     147,  -152,   617,   140,   124,  -152,    59,   157,   560,    59,
     159,   570,    59,   161,   607,    -9,   162,  -152,   151,   171,
    -152,    59,    39,  -152,  -152,  -152,  -152,    59,  -152,    59,
      59,   165,  -152,    59,    59,   167,  -152,    59,    59,   168,
    -152,   617,   174,    -5,  -152,   178,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,    59,  -152,  -152,    59,  -152,  -152,    59,
     177,   300,  -152,  -152,  -152,  -152,  -152,  -152,   300,   300,
     300
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,    21,    22,    23,    24,    26,    25,   123,   130,   127,
     128,   131,   129,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
       0,     0,     2,     4,     8,     9,     0,     0,    19,     6,
       7,    10,    42,    34,    40,    41,    35,    36,    37,    38,
      39,    43,     0,    76,    77,    79,    81,    83,    85,    87,
      89,    92,    97,   100,   103,   107,   114,   117,   122,     0,
     123,     0,   108,   109,   112,   113,   110,   111,    29,     0,
       0,     0,     0,    46,     0,    47,    48,     0,     0,     0,
       0,     1,     5,     0,    18,    75,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   115,   116,     0,     0,   124,
       0,     0,     0,     0,     0,     0,     0,    45,     0,    74,
       0,     0,     0,     0,    11,     0,    78,    80,    82,    84,
      86,    88,    90,    91,    93,    94,    95,    96,    98,    99,
     101,   102,   104,   105,   106,   119,   120,     0,    44,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    73,     0,    32,     0,     0,     0,     0,    15,     0,
       0,   118,     0,     0,    49,    51,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,     0,     0,
      14,     0,     0,    17,    12,   121,    28,     0,    64,     0,
       0,     0,    56,     0,     0,     0,    60,     0,     0,     0,
      66,     0,     0,     0,    67,     0,    30,    33,    13,    16,
      50,    63,    62,     0,    55,    54,     0,    59,    58,     0,
       0,    72,    65,    68,    52,    61,    53,    57,    70,    71,
      69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,  -152,   -86,   -32,   120,  -152,  -152,     1,   -78,  -152,
     169,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,   -19,  -151,   125,   -12,   109,
    -152,   115,   110,   127,   116,   138,     6,    -3,    10,    15,
       3,  -152,  -152,  -152,  -152,  -152
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    31,    32,    33,    34,    35,   177,   178,    36,    37,
      38,    39,    40,   175,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,   223,   224,    89,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,   157,    67,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      92,    71,   130,    69,   125,    80,    78,   173,    96,   220,
     184,   185,    84,   242,   110,   111,   174,    72,    73,    74,
      75,    76,    77,    97,    79,   200,     1,     2,     3,     4,
       5,     6,   198,   134,   159,   208,   199,   135,   212,   176,
     160,   216,     1,     2,     3,     4,     5,     6,   221,   222,
     228,    81,   221,   222,   181,   179,   230,   118,   231,   232,
     182,    90,   234,   235,   133,    85,   237,   238,   121,   122,
     134,   201,   102,   103,   135,   128,    88,   202,   104,   105,
     106,   107,   245,    29,    86,   246,   108,   109,   247,     1,
       2,     3,     4,     5,     6,   112,   113,   114,    92,    29,
      82,   144,   145,   146,   147,   156,   115,   116,   142,   143,
      87,   164,   166,    91,   169,   152,   153,   154,   148,   149,
      93,    95,    98,   180,   179,   150,   151,    99,   119,   117,
     101,   100,   120,   131,   132,    70,     8,     9,    10,    11,
      12,    13,   127,   158,   161,   162,   167,    83,   170,   172,
     183,   187,   188,   190,   191,   249,   193,   194,    14,    15,
     196,   195,   250,    16,    17,   203,   197,    18,   204,   206,
     205,    19,   226,   209,   207,   213,   211,   217,   225,   215,
     227,   233,   219,   236,   239,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   241,   244,
     129,   248,   124,   229,   243,   136,    94,   126,   138,   240,
       0,     0,   137,     0,    14,    15,   140,    92,    92,    16,
      17,     0,     0,    18,     0,     0,   139,    19,     0,     0,
      20,    21,     0,    22,    23,    24,    25,    26,    27,   141,
       0,    28,    29,    30,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,     0,     0,   171,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,    15,     0,     0,     0,    16,    17,
       0,     0,    18,     0,     0,     0,    19,     0,     0,    20,
      21,     0,    22,    23,    24,    25,    26,    27,     0,     0,
      28,    29,    30,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,    15,     0,     0,     0,    16,    17,     0,
       0,    18,     0,     0,     0,    19,     0,     0,    20,    21,
       0,    22,    23,    24,    25,    26,    27,     0,     0,    28,
      29,    30,     1,     2,     3,     4,     5,     6,    70,     8,
       9,    10,    11,    12,    13,     0,     0,     0,     0,     0,
     123,     0,    70,     8,     9,    10,    11,    12,    13,   155,
       0,    14,    15,     0,     0,     0,    16,    17,     0,     0,
      18,     0,     0,     0,    19,    14,    15,     0,     0,     0,
      16,    17,     0,     0,    18,     0,     0,     0,    19,    29,
      70,     8,     9,    10,    11,    12,    13,     0,     0,     0,
       0,     0,   163,     0,    70,     8,     9,    10,    11,    12,
      13,     0,     0,    14,    15,     0,   165,     0,    16,    17,
       0,     0,    18,     0,     0,     0,    19,    14,    15,     0,
       0,     0,    16,    17,     0,     0,    18,     0,     0,     0,
      19,    70,     8,     9,    10,    11,    12,    13,     0,     0,
       0,     0,     0,   168,     0,    70,     8,     9,    10,    11,
      12,    13,   186,     0,    14,    15,     0,     0,     0,    16,
      17,     0,     0,    18,     0,     0,     0,    19,    14,    15,
       0,     0,     0,    16,    17,     0,     0,    18,     0,     0,
       0,    19,    70,     8,     9,    10,    11,    12,    13,   189,
       0,     0,    70,     8,     9,    10,    11,    12,    13,   192,
       0,     0,     0,     0,     0,    14,    15,     0,     0,     0,
      16,    17,     0,     0,    18,    14,    15,     0,    19,     0,
      16,    17,     0,     0,    18,     0,     0,     0,    19,    70,
       8,     9,    10,    11,    12,    13,   210,     0,     0,    70,
       8,     9,    10,    11,    12,    13,   214,     0,     0,     0,
       0,     0,    14,    15,     0,     0,     0,    16,    17,     0,
       0,    18,    14,    15,     0,    19,     0,    16,    17,     0,
       0,    18,     0,     0,     0,    19,    70,     8,     9,    10,
      11,    12,    13,   218,     0,     0,    70,     8,     9,    10,
      11,    12,    13,     0,     0,     0,     0,     0,     0,    14,
      15,     0,     0,     0,    16,    17,     0,     0,    18,    14,
      15,     0,    19,     0,    16,    17,     0,     0,    18,     0,
       0,     0,    19
};

static const yytype_int16 yycheck[] =
{
      32,    13,    88,    25,    82,    15,    10,     9,    25,    18,
     161,   162,    24,    18,    32,    33,    18,    14,    15,    16,
      17,    18,    19,    40,    28,   176,     3,     4,     5,     6,
       7,     8,    18,    21,    23,   186,    22,    25,   189,    16,
      29,   192,     3,     4,     5,     6,     7,     8,    57,    58,
     201,    15,    57,    58,    16,   133,   207,    69,   209,   210,
      22,     9,   213,   214,    15,    21,   217,   218,    80,    81,
      21,    16,    26,    27,    25,    87,    17,    22,    28,    29,
      30,    31,   233,    60,    21,   236,    46,    47,   239,     3,
       4,     5,     6,     7,     8,    34,    35,    36,   130,    60,
      15,   104,   105,   106,   107,   117,    37,    38,   102,   103,
      15,   123,   124,     0,   126,   112,   113,   114,   108,   109,
       9,    21,    39,   135,   202,   110,   111,    43,    16,    15,
      42,    44,     9,    51,    17,     9,    10,    11,    12,    13,
      14,    15,    21,    21,    16,    16,     9,    21,    16,    15,
       9,   163,    21,   165,    21,   241,   168,    21,    32,    33,
     172,    17,   248,    37,    38,     9,    21,    41,    21,    29,
     182,    45,    21,    16,    50,    16,   188,    16,    16,   191,
       9,    16,   194,    16,    16,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    24,    21,
      18,    24,    82,   202,   223,    96,    37,    82,    98,   221,
      -1,    -1,    97,    -1,    32,    33,   100,   249,   250,    37,
      38,    -1,    -1,    41,    -1,    -1,    99,    45,    -1,    -1,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,   101,
      -1,    59,    60,    61,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    -1,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    -1,    -1,
      59,    60,    61,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    33,    -1,    -1,    -1,    37,    38,    -1,
      -1,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    -1,    -1,    59,
      60,    61,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    -1,    -1,    -1,    -1,    -1,
      21,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      -1,    32,    33,    -1,    -1,    -1,    37,    38,    -1,    -1,
      41,    -1,    -1,    -1,    45,    32,    33,    -1,    -1,    -1,
      37,    38,    -1,    -1,    41,    -1,    -1,    -1,    45,    60,
       9,    10,    11,    12,    13,    14,    15,    -1,    -1,    -1,
      -1,    -1,    21,    -1,     9,    10,    11,    12,    13,    14,
      15,    -1,    -1,    32,    33,    -1,    21,    -1,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    45,    32,    33,    -1,
      -1,    -1,    37,    38,    -1,    -1,    41,    -1,    -1,    -1,
      45,     9,    10,    11,    12,    13,    14,    15,    -1,    -1,
      -1,    -1,    -1,    21,    -1,     9,    10,    11,    12,    13,
      14,    15,    16,    -1,    32,    33,    -1,    -1,    -1,    37,
      38,    -1,    -1,    41,    -1,    -1,    -1,    45,    32,    33,
      -1,    -1,    -1,    37,    38,    -1,    -1,    41,    -1,    -1,
      -1,    45,     9,    10,    11,    12,    13,    14,    15,    16,
      -1,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      -1,    -1,    -1,    -1,    -1,    32,    33,    -1,    -1,    -1,
      37,    38,    -1,    -1,    41,    32,    33,    -1,    45,    -1,
      37,    38,    -1,    -1,    41,    -1,    -1,    -1,    45,     9,
      10,    11,    12,    13,    14,    15,    16,    -1,    -1,     9,
      10,    11,    12,    13,    14,    15,    16,    -1,    -1,    -1,
      -1,    -1,    32,    33,    -1,    -1,    -1,    37,    38,    -1,
      -1,    41,    32,    33,    -1,    45,    -1,    37,    38,    -1,
      -1,    41,    -1,    -1,    -1,    45,     9,    10,    11,    12,
      13,    14,    15,    16,    -1,    -1,     9,    10,    11,    12,
      13,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      33,    -1,    -1,    -1,    37,    38,    -1,    -1,    41,    32,
      33,    -1,    45,    -1,    37,    38,    -1,    -1,    41,    -1,
      -1,    -1,    45
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    32,    33,    37,    38,    41,    45,
      48,    49,    51,    52,    53,    54,    55,    56,    59,    60,
      61,    67,    68,    69,    70,    71,    74,    75,    76,    77,
      78,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   110,   111,    25,
       9,    94,   106,   106,   106,   106,   106,   106,    10,    28,
      15,    15,    15,    21,    94,    21,    21,    15,    17,    92,
       9,     0,    69,     9,    76,    21,    25,    40,    39,    43,
      44,    42,    26,    27,    28,    29,    30,    31,    46,    47,
      32,    33,    34,    35,    36,    37,    38,    15,    94,    16,
       9,    94,    94,    21,    70,    74,    93,    21,    94,    18,
      68,    51,    17,    15,    21,    25,    95,    97,    98,    99,
     100,   101,   102,   102,   103,   103,   103,   103,   104,   104,
     105,   105,   106,   106,   106,    16,    94,   109,    21,    23,
      29,    16,    16,    21,    94,    21,    94,     9,    21,    94,
      16,    18,    15,     9,    18,    79,    16,    72,    73,    74,
      94,    16,    22,     9,    92,    92,    16,    94,    21,    16,
      94,    21,    16,    94,    21,    17,    94,    21,    18,    22,
      92,    16,    22,     9,    21,    94,    29,    50,    92,    16,
      16,    94,    92,    16,    16,    94,    92,    16,    16,    94,
      18,    57,    58,    90,    91,    16,    21,     9,    92,    73,
      92,    92,    92,    16,    92,    92,    16,    92,    92,    16,
      94,    24,    18,    91,    21,    92,    92,    92,    24,    68,
      68
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    66,    67,    67,    68,    68,    69,    69,    69,    69,
      69,    70,    70,    71,    71,    72,    72,    73,    74,    74,
      75,    76,    76,    76,    76,    76,    76,    77,    77,    77,
      78,    78,    79,    79,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    81,    82,    82,    83,    84,    85,
      85,    86,    87,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    89,    89,    90,    90,    91,
      91,    91,    91,    92,    92,    93,    94,    95,    95,    96,
      96,    97,    97,    98,    98,    99,    99,   100,   100,   101,
     101,   101,   102,   102,   102,   102,   102,   103,   103,   103,
     104,   104,   104,   105,   105,   105,   105,   106,   106,   106,
     106,   106,   106,   106,   107,   107,   107,   108,   108,   108,
     109,   109,   110,   110,   110,   110,   110,   111,   111,   111,
     111,   111
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     1,     2,     1,     1,     1,     1,
       1,     3,     5,     6,     5,     1,     3,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     6,     2,
       6,     5,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     3,     2,     2,     2,     5,
       7,     5,     7,     8,     7,     7,     6,     8,     7,     7,
       6,     8,     7,     7,     6,     7,     6,     1,     2,     4,
       3,     3,     2,     3,     2,     2,     1,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     1,     4,     3,
       1,     3,     1,     1,     3,     2,     2,     1,     1,     1,
       1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: declaration_list  */
#line 89 "parser.y"
                       { (yyval.node) = create_list(NODE_PROGRAM); 
                         for (int i = 0; i < (yyvsp[0].node)->data.list.count; i++) 
                             list_append((yyval.node), (yyvsp[0].node)->data.list.items[i]);
                         root = (yyval.node); }
#line 1572 "parser.tab.c"
    break;

  case 3: /* program: %empty  */
#line 93 "parser.y"
                  { (yyval.node) = create_list(NODE_PROGRAM); root = (yyval.node); }
#line 1578 "parser.tab.c"
    break;

  case 4: /* declaration_list: declaration  */
#line 97 "parser.y"
                  { (yyval.node) = create_list(NODE_DECLARATION_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 1584 "parser.tab.c"
    break;

  case 5: /* declaration_list: declaration_list declaration  */
#line 98 "parser.y"
                                   { (yyval.node) = (yyvsp[-1].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 1590 "parser.tab.c"
    break;

  case 6: /* declaration: includeDirective  */
#line 103 "parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1596 "parser.tab.c"
    break;

  case 7: /* declaration: enumDecl  */
#line 104 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1602 "parser.tab.c"
    break;

  case 8: /* declaration: varDecl  */
#line 105 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1608 "parser.tab.c"
    break;

  case 9: /* declaration: fnDecl  */
#line 106 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1614 "parser.tab.c"
    break;

  case 10: /* declaration: statement  */
#line 107 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1620 "parser.tab.c"
    break;

  case 11: /* varDecl: typeSpec T_IDENTIFIER T_SEMICOLON  */
#line 113 "parser.y"
        { (yyval.node) = create_var_decl((yyvsp[-2].node), (yyvsp[-1].strval), NULL); free((yyvsp[-1].strval)); }
#line 1626 "parser.tab.c"
    break;

  case 12: /* varDecl: typeSpec T_IDENTIFIER T_ASSIGNOP expression T_SEMICOLON  */
#line 115 "parser.y"
        { (yyval.node) = create_var_decl((yyvsp[-4].node), (yyvsp[-3].strval), (yyvsp[-1].node)); free((yyvsp[-3].strval)); }
#line 1632 "parser.tab.c"
    break;

  case 13: /* fnDecl: typeSpec T_IDENTIFIER T_LPAREN parameters T_RPAREN block  */
#line 121 "parser.y"
        { (yyval.node) = create_func_decl((yyvsp[-5].node), (yyvsp[-4].strval), (yyvsp[-2].node), (yyvsp[0].node)); free((yyvsp[-4].strval)); }
#line 1638 "parser.tab.c"
    break;

  case 14: /* fnDecl: typeSpec T_IDENTIFIER T_LPAREN T_RPAREN block  */
#line 123 "parser.y"
        { (yyval.node) = create_func_decl((yyvsp[-4].node), (yyvsp[-3].strval), NULL, (yyvsp[0].node)); free((yyvsp[-3].strval)); }
#line 1644 "parser.tab.c"
    break;

  case 15: /* parameters: parameter  */
#line 127 "parser.y"
                { (yyval.node) = create_list(NODE_PARAM_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 1650 "parser.tab.c"
    break;

  case 16: /* parameters: parameters T_COMMA parameter  */
#line 128 "parser.y"
                                   { (yyval.node) = (yyvsp[-2].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 1656 "parser.tab.c"
    break;

  case 17: /* parameter: typeSpec T_IDENTIFIER  */
#line 132 "parser.y"
                            { (yyval.node) = create_param((yyvsp[-1].node), (yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 1662 "parser.tab.c"
    break;

  case 18: /* typeSpec: typeQualifier baseType  */
#line 137 "parser.y"
                             { (yyval.node) = create_type_spec((yyvsp[-1].intval), (yyvsp[0].datatype)); }
#line 1668 "parser.tab.c"
    break;

  case 19: /* typeSpec: baseType  */
#line 138 "parser.y"
               { (yyval.node) = create_type_spec(0, (yyvsp[0].datatype)); }
#line 1674 "parser.tab.c"
    break;

  case 20: /* typeQualifier: T_CONST  */
#line 142 "parser.y"
              { (yyval.intval) = 1; }
#line 1680 "parser.tab.c"
    break;

  case 21: /* baseType: T_INT  */
#line 146 "parser.y"
            { (yyval.datatype) = TYPE_INT; }
#line 1686 "parser.tab.c"
    break;

  case 22: /* baseType: T_FLOAT  */
#line 147 "parser.y"
              { (yyval.datatype) = TYPE_FLOAT; }
#line 1692 "parser.tab.c"
    break;

  case 23: /* baseType: T_DOUBLE  */
#line 148 "parser.y"
               { (yyval.datatype) = TYPE_DOUBLE; }
#line 1698 "parser.tab.c"
    break;

  case 24: /* baseType: T_CHAR  */
#line 149 "parser.y"
             { (yyval.datatype) = TYPE_CHAR; }
#line 1704 "parser.tab.c"
    break;

  case 25: /* baseType: T_BOOL  */
#line 150 "parser.y"
             { (yyval.datatype) = TYPE_BOOL; }
#line 1710 "parser.tab.c"
    break;

  case 26: /* baseType: T_VOID  */
#line 151 "parser.y"
             { (yyval.datatype) = TYPE_VOID; }
#line 1716 "parser.tab.c"
    break;

  case 27: /* includeDirective: T_INCLUDE T_LT T_IDENTIFIER T_GT  */
#line 157 "parser.y"
        { (yyval.node) = create_include_directive((yyvsp[-1].strval), 1); free((yyvsp[-1].strval)); }
#line 1722 "parser.tab.c"
    break;

  case 28: /* includeDirective: T_INCLUDE T_LT T_IDENTIFIER T_DOT T_IDENTIFIER T_GT  */
#line 159 "parser.y"
        { char *file = malloc(strlen((yyvsp[-3].strval)) + strlen((yyvsp[-1].strval)) + 2);
          sprintf(file, "%s.%s", (yyvsp[-3].strval), (yyvsp[-1].strval));
          (yyval.node) = create_include_directive(file, 1);
          free((yyvsp[-3].strval)); free((yyvsp[-1].strval)); free(file); }
#line 1731 "parser.tab.c"
    break;

  case 29: /* includeDirective: T_INCLUDE T_STRINGLIT  */
#line 164 "parser.y"
        { (yyval.node) = create_include_directive((yyvsp[0].strval), 0); free((yyvsp[0].strval)); }
#line 1737 "parser.tab.c"
    break;

  case 30: /* enumDecl: T_ENUM T_IDENTIFIER T_LBRACE enumValueList T_RBRACE T_SEMICOLON  */
#line 170 "parser.y"
        { (yyval.node) = create_enum_decl((yyvsp[-4].strval), (yyvsp[-2].node)); free((yyvsp[-4].strval)); }
#line 1743 "parser.tab.c"
    break;

  case 31: /* enumDecl: T_ENUM T_IDENTIFIER T_LBRACE T_RBRACE T_SEMICOLON  */
#line 172 "parser.y"
        { (yyval.node) = create_enum_decl((yyvsp[-3].strval), NULL); free((yyvsp[-3].strval)); }
#line 1749 "parser.tab.c"
    break;

  case 32: /* enumValueList: T_IDENTIFIER  */
#line 176 "parser.y"
                   { (yyval.node) = create_list(NODE_ENUM_VALUE_LIST); 
                     list_append((yyval.node), create_identifier((yyvsp[0].strval))); free((yyvsp[0].strval)); }
#line 1756 "parser.tab.c"
    break;

  case 33: /* enumValueList: enumValueList T_COMMA T_IDENTIFIER  */
#line 179 "parser.y"
        { (yyval.node) = (yyvsp[-2].node); list_append((yyval.node), create_identifier((yyvsp[0].strval))); free((yyvsp[0].strval)); }
#line 1762 "parser.tab.c"
    break;

  case 34: /* statement: returnStmt  */
#line 184 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1768 "parser.tab.c"
    break;

  case 35: /* statement: ifStmt  */
#line 185 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1774 "parser.tab.c"
    break;

  case 36: /* statement: whileStmt  */
#line 186 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1780 "parser.tab.c"
    break;

  case 37: /* statement: doWhileStmt  */
#line 187 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1786 "parser.tab.c"
    break;

  case 38: /* statement: forStmt  */
#line 188 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1792 "parser.tab.c"
    break;

  case 39: /* statement: switchStmt  */
#line 189 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1798 "parser.tab.c"
    break;

  case 40: /* statement: breakStmt  */
#line 190 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1804 "parser.tab.c"
    break;

  case 41: /* statement: continueStmt  */
#line 191 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1810 "parser.tab.c"
    break;

  case 42: /* statement: assignStmt  */
#line 192 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1816 "parser.tab.c"
    break;

  case 43: /* statement: exprStmt  */
#line 193 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1822 "parser.tab.c"
    break;

  case 44: /* assignStmt: T_IDENTIFIER T_ASSIGNOP expression T_SEMICOLON  */
#line 198 "parser.y"
        { (yyval.node) = create_binary_op(OP_ASSIGN, create_identifier((yyvsp[-3].strval)), (yyvsp[-1].node)); free((yyvsp[-3].strval)); }
#line 1828 "parser.tab.c"
    break;

  case 45: /* returnStmt: T_RETURN expression T_SEMICOLON  */
#line 202 "parser.y"
                                      { (yyval.node) = create_return_stmt((yyvsp[-1].node)); }
#line 1834 "parser.tab.c"
    break;

  case 46: /* returnStmt: T_RETURN T_SEMICOLON  */
#line 203 "parser.y"
                           { (yyval.node) = create_return_stmt(NULL); }
#line 1840 "parser.tab.c"
    break;

  case 47: /* breakStmt: T_BREAK T_SEMICOLON  */
#line 207 "parser.y"
                          { (yyval.node) = create_break_stmt(); }
#line 1846 "parser.tab.c"
    break;

  case 48: /* continueStmt: T_CONTINUE T_SEMICOLON  */
#line 211 "parser.y"
                             { (yyval.node) = create_continue_stmt(); }
#line 1852 "parser.tab.c"
    break;

  case 49: /* ifStmt: T_IF T_LPAREN expression T_RPAREN block  */
#line 216 "parser.y"
        { (yyval.node) = create_if_stmt((yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1858 "parser.tab.c"
    break;

  case 50: /* ifStmt: T_IF T_LPAREN expression T_RPAREN block T_ELSE block  */
#line 218 "parser.y"
        { (yyval.node) = create_if_stmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1864 "parser.tab.c"
    break;

  case 51: /* whileStmt: T_WHILE T_LPAREN expression T_RPAREN block  */
#line 223 "parser.y"
        { (yyval.node) = create_while_stmt((yyvsp[-2].node), (yyvsp[0].node)); }
#line 1870 "parser.tab.c"
    break;

  case 52: /* doWhileStmt: T_DO block T_WHILE T_LPAREN expression T_RPAREN T_SEMICOLON  */
#line 228 "parser.y"
        { (yyval.node) = create_do_while_stmt((yyvsp[-5].node), (yyvsp[-2].node)); }
#line 1876 "parser.tab.c"
    break;

  case 53: /* forStmt: T_FOR T_LPAREN varDecl expression T_SEMICOLON expression T_RPAREN block  */
#line 233 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-5].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1882 "parser.tab.c"
    break;

  case 54: /* forStmt: T_FOR T_LPAREN varDecl expression T_SEMICOLON T_RPAREN block  */
#line 235 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), (yyvsp[-3].node), NULL, (yyvsp[0].node)); }
#line 1888 "parser.tab.c"
    break;

  case 55: /* forStmt: T_FOR T_LPAREN varDecl T_SEMICOLON expression T_RPAREN block  */
#line 237 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), NULL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1894 "parser.tab.c"
    break;

  case 56: /* forStmt: T_FOR T_LPAREN varDecl T_SEMICOLON T_RPAREN block  */
#line 239 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-3].node), NULL, NULL, (yyvsp[0].node)); }
#line 1900 "parser.tab.c"
    break;

  case 57: /* forStmt: T_FOR T_LPAREN exprStmt expression T_SEMICOLON expression T_RPAREN block  */
#line 241 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-5].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1906 "parser.tab.c"
    break;

  case 58: /* forStmt: T_FOR T_LPAREN exprStmt expression T_SEMICOLON T_RPAREN block  */
#line 243 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), (yyvsp[-3].node), NULL, (yyvsp[0].node)); }
#line 1912 "parser.tab.c"
    break;

  case 59: /* forStmt: T_FOR T_LPAREN exprStmt T_SEMICOLON expression T_RPAREN block  */
#line 245 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), NULL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1918 "parser.tab.c"
    break;

  case 60: /* forStmt: T_FOR T_LPAREN exprStmt T_SEMICOLON T_RPAREN block  */
#line 247 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-3].node), NULL, NULL, (yyvsp[0].node)); }
#line 1924 "parser.tab.c"
    break;

  case 61: /* forStmt: T_FOR T_LPAREN T_SEMICOLON expression T_SEMICOLON expression T_RPAREN block  */
#line 249 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1930 "parser.tab.c"
    break;

  case 62: /* forStmt: T_FOR T_LPAREN T_SEMICOLON expression T_SEMICOLON T_RPAREN block  */
#line 251 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, (yyvsp[-3].node), NULL, (yyvsp[0].node)); }
#line 1936 "parser.tab.c"
    break;

  case 63: /* forStmt: T_FOR T_LPAREN T_SEMICOLON T_SEMICOLON expression T_RPAREN block  */
#line 253 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, NULL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1942 "parser.tab.c"
    break;

  case 64: /* forStmt: T_FOR T_LPAREN T_SEMICOLON T_SEMICOLON T_RPAREN block  */
#line 255 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, NULL, NULL, (yyvsp[0].node)); }
#line 1948 "parser.tab.c"
    break;

  case 65: /* switchStmt: T_SWITCH T_LPAREN expression T_RPAREN T_LBRACE caseStmt_list T_RBRACE  */
#line 260 "parser.y"
        { (yyval.node) = create_switch_stmt((yyvsp[-4].node), (yyvsp[-1].node)); }
#line 1954 "parser.tab.c"
    break;

  case 66: /* switchStmt: T_SWITCH T_LPAREN expression T_RPAREN T_LBRACE T_RBRACE  */
#line 262 "parser.y"
        { (yyval.node) = create_switch_stmt((yyvsp[-3].node), NULL); }
#line 1960 "parser.tab.c"
    break;

  case 67: /* caseStmt_list: caseStmt  */
#line 266 "parser.y"
               { (yyval.node) = create_list(NODE_STATEMENT_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 1966 "parser.tab.c"
    break;

  case 68: /* caseStmt_list: caseStmt_list caseStmt  */
#line 267 "parser.y"
                             { (yyval.node) = (yyvsp[-1].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 1972 "parser.tab.c"
    break;

  case 69: /* caseStmt: T_CASE expression T_COLON declaration_list  */
#line 272 "parser.y"
        { (yyval.node) = create_case_stmt((yyvsp[-2].node), (yyvsp[0].node)); }
#line 1978 "parser.tab.c"
    break;

  case 70: /* caseStmt: T_CASE expression T_COLON  */
#line 274 "parser.y"
        { (yyval.node) = create_case_stmt((yyvsp[-1].node), NULL); }
#line 1984 "parser.tab.c"
    break;

  case 71: /* caseStmt: T_DEFAULT T_COLON declaration_list  */
#line 276 "parser.y"
        { (yyval.node) = create_case_stmt(NULL, (yyvsp[0].node)); }
#line 1990 "parser.tab.c"
    break;

  case 72: /* caseStmt: T_DEFAULT T_COLON  */
#line 278 "parser.y"
        { (yyval.node) = create_case_stmt(NULL, NULL); }
#line 1996 "parser.tab.c"
    break;

  case 73: /* block: T_LBRACE declaration_list T_RBRACE  */
#line 282 "parser.y"
                                         { (yyval.node) = create_block((yyvsp[-1].node)); }
#line 2002 "parser.tab.c"
    break;

  case 74: /* block: T_LBRACE T_RBRACE  */
#line 283 "parser.y"
                        { (yyval.node) = create_block(NULL); }
#line 2008 "parser.tab.c"
    break;

  case 75: /* exprStmt: expression T_SEMICOLON  */
#line 287 "parser.y"
                             { (yyval.node) = (yyvsp[-1].node); }
#line 2014 "parser.tab.c"
    break;

  case 76: /* expression: assignment  */
#line 292 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2020 "parser.tab.c"
    break;

  case 77: /* assignment: logicalOr  */
#line 296 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 2026 "parser.tab.c"
    break;

  case 78: /* assignment: logicalOr T_ASSIGNOP assignment  */
#line 298 "parser.y"
        { (yyval.node) = create_binary_op(OP_ASSIGN, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2032 "parser.tab.c"
    break;

  case 79: /* logicalOr: logicalAnd  */
#line 302 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2038 "parser.tab.c"
    break;

  case 80: /* logicalOr: logicalOr T_OR logicalAnd  */
#line 304 "parser.y"
        { (yyval.node) = create_binary_op(OP_OR, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2044 "parser.tab.c"
    break;

  case 81: /* logicalAnd: bitwiseOr  */
#line 308 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 2050 "parser.tab.c"
    break;

  case 82: /* logicalAnd: logicalAnd T_AND bitwiseOr  */
#line 310 "parser.y"
        { (yyval.node) = create_binary_op(OP_AND, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2056 "parser.tab.c"
    break;

  case 83: /* bitwiseOr: bitwiseXor  */
#line 314 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2062 "parser.tab.c"
    break;

  case 84: /* bitwiseOr: bitwiseOr T_BITWISE_OR bitwiseXor  */
#line 316 "parser.y"
        { (yyval.node) = create_binary_op(OP_BIT_OR, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2068 "parser.tab.c"
    break;

  case 85: /* bitwiseXor: bitwiseAnd  */
#line 320 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2074 "parser.tab.c"
    break;

  case 86: /* bitwiseXor: bitwiseXor T_BITWISE_XOR bitwiseAnd  */
#line 322 "parser.y"
        { (yyval.node) = create_binary_op(OP_BIT_XOR, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2080 "parser.tab.c"
    break;

  case 87: /* bitwiseAnd: equality  */
#line 326 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 2086 "parser.tab.c"
    break;

  case 88: /* bitwiseAnd: bitwiseAnd T_BITWISE_AND equality  */
#line 328 "parser.y"
        { (yyval.node) = create_binary_op(OP_BIT_AND, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2092 "parser.tab.c"
    break;

  case 89: /* equality: comparison  */
#line 332 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2098 "parser.tab.c"
    break;

  case 90: /* equality: equality T_EQUALOP comparison  */
#line 334 "parser.y"
        { (yyval.node) = create_binary_op(OP_EQ, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2104 "parser.tab.c"
    break;

  case 91: /* equality: equality T_NE comparison  */
#line 336 "parser.y"
        { (yyval.node) = create_binary_op(OP_NE, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2110 "parser.tab.c"
    break;

  case 92: /* comparison: shift  */
#line 340 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 2116 "parser.tab.c"
    break;

  case 93: /* comparison: comparison T_LT shift  */
#line 342 "parser.y"
        { (yyval.node) = create_binary_op(OP_LT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2122 "parser.tab.c"
    break;

  case 94: /* comparison: comparison T_GT shift  */
#line 344 "parser.y"
        { (yyval.node) = create_binary_op(OP_GT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2128 "parser.tab.c"
    break;

  case 95: /* comparison: comparison T_LE shift  */
#line 346 "parser.y"
        { (yyval.node) = create_binary_op(OP_LE, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2134 "parser.tab.c"
    break;

  case 96: /* comparison: comparison T_GE shift  */
#line 348 "parser.y"
        { (yyval.node) = create_binary_op(OP_GE, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2140 "parser.tab.c"
    break;

  case 97: /* shift: term  */
#line 352 "parser.y"
           { (yyval.node) = (yyvsp[0].node); }
#line 2146 "parser.tab.c"
    break;

  case 98: /* shift: shift T_LEFT_SHIFT term  */
#line 354 "parser.y"
        { (yyval.node) = create_binary_op(OP_LSHIFT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2152 "parser.tab.c"
    break;

  case 99: /* shift: shift T_RIGHT_SHIFT term  */
#line 356 "parser.y"
        { (yyval.node) = create_binary_op(OP_RSHIFT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2158 "parser.tab.c"
    break;

  case 100: /* term: factor  */
#line 360 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 2164 "parser.tab.c"
    break;

  case 101: /* term: term T_PLUS factor  */
#line 362 "parser.y"
        { (yyval.node) = create_binary_op(OP_ADD, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2170 "parser.tab.c"
    break;

  case 102: /* term: term T_MINUS factor  */
#line 364 "parser.y"
        { (yyval.node) = create_binary_op(OP_SUB, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2176 "parser.tab.c"
    break;

  case 103: /* factor: unary  */
#line 368 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 2182 "parser.tab.c"
    break;

  case 104: /* factor: factor T_MULTIPLY unary  */
#line 370 "parser.y"
        { (yyval.node) = create_binary_op(OP_MUL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2188 "parser.tab.c"
    break;

  case 105: /* factor: factor T_DIVIDE unary  */
#line 372 "parser.y"
        { (yyval.node) = create_binary_op(OP_DIV, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2194 "parser.tab.c"
    break;

  case 106: /* factor: factor T_MODULO unary  */
#line 374 "parser.y"
        { (yyval.node) = create_binary_op(OP_MOD, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2200 "parser.tab.c"
    break;

  case 107: /* unary: postfix  */
#line 378 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2206 "parser.tab.c"
    break;

  case 108: /* unary: T_PLUS unary  */
#line 380 "parser.y"
        { (yyval.node) = create_unary_op(OP_PLUS_UNARY, (yyvsp[0].node)); }
#line 2212 "parser.tab.c"
    break;

  case 109: /* unary: T_MINUS unary  */
#line 382 "parser.y"
        { (yyval.node) = create_unary_op(OP_MINUS_UNARY, (yyvsp[0].node)); }
#line 2218 "parser.tab.c"
    break;

  case 110: /* unary: T_NOT unary  */
#line 384 "parser.y"
        { (yyval.node) = create_unary_op(OP_NOT, (yyvsp[0].node)); }
#line 2224 "parser.tab.c"
    break;

  case 111: /* unary: T_BITWISE_NOT unary  */
#line 386 "parser.y"
        { (yyval.node) = create_unary_op(OP_BIT_NOT, (yyvsp[0].node)); }
#line 2230 "parser.tab.c"
    break;

  case 112: /* unary: T_INCREMENT unary  */
#line 388 "parser.y"
        { (yyval.node) = create_unary_op(OP_PRE_INC, (yyvsp[0].node)); }
#line 2236 "parser.tab.c"
    break;

  case 113: /* unary: T_DECREMENT unary  */
#line 390 "parser.y"
        { (yyval.node) = create_unary_op(OP_PRE_DEC, (yyvsp[0].node)); }
#line 2242 "parser.tab.c"
    break;

  case 114: /* postfix: call  */
#line 394 "parser.y"
           { (yyval.node) = (yyvsp[0].node); }
#line 2248 "parser.tab.c"
    break;

  case 115: /* postfix: call T_INCREMENT  */
#line 396 "parser.y"
        { (yyval.node) = create_unary_op(OP_POST_INC, (yyvsp[-1].node)); }
#line 2254 "parser.tab.c"
    break;

  case 116: /* postfix: call T_DECREMENT  */
#line 398 "parser.y"
        { (yyval.node) = create_unary_op(OP_POST_DEC, (yyvsp[-1].node)); }
#line 2260 "parser.tab.c"
    break;

  case 117: /* call: primary  */
#line 402 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2266 "parser.tab.c"
    break;

  case 118: /* call: primary T_LPAREN arguments T_RPAREN  */
#line 404 "parser.y"
        { if ((yyvsp[-3].node)->type == NODE_IDENTIFIER) {
              (yyval.node) = create_func_call((yyvsp[-3].node)->data.identifier.name, (yyvsp[-1].node));
              free_ast((yyvsp[-3].node));
          } else {
              (yyval.node) = (yyvsp[-3].node);
          } }
#line 2277 "parser.tab.c"
    break;

  case 119: /* call: primary T_LPAREN T_RPAREN  */
#line 411 "parser.y"
        { if ((yyvsp[-2].node)->type == NODE_IDENTIFIER) {
              (yyval.node) = create_func_call((yyvsp[-2].node)->data.identifier.name, NULL);
              free_ast((yyvsp[-2].node));
          } else {
              (yyval.node) = (yyvsp[-2].node);
          } }
#line 2288 "parser.tab.c"
    break;

  case 120: /* arguments: expression  */
#line 420 "parser.y"
                 { (yyval.node) = create_list(NODE_ARG_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 2294 "parser.tab.c"
    break;

  case 121: /* arguments: arguments T_COMMA expression  */
#line 421 "parser.y"
                                   { (yyval.node) = (yyvsp[-2].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 2300 "parser.tab.c"
    break;

  case 122: /* primary: literal  */
#line 425 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2306 "parser.tab.c"
    break;

  case 123: /* primary: T_IDENTIFIER  */
#line 426 "parser.y"
                   { (yyval.node) = create_identifier((yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 2312 "parser.tab.c"
    break;

  case 124: /* primary: T_LPAREN expression T_RPAREN  */
#line 427 "parser.y"
                                   { (yyval.node) = (yyvsp[-1].node); }
#line 2318 "parser.tab.c"
    break;

  case 125: /* primary: T_INCREMENT unary  */
#line 428 "parser.y"
                        { (yyval.node) = create_unary_op(OP_PRE_INC, (yyvsp[0].node)); }
#line 2324 "parser.tab.c"
    break;

  case 126: /* primary: T_DECREMENT unary  */
#line 429 "parser.y"
                        { (yyval.node) = create_unary_op(OP_PRE_DEC, (yyvsp[0].node)); }
#line 2330 "parser.tab.c"
    break;

  case 127: /* literal: T_INTLIT  */
#line 433 "parser.y"
               { (yyval.node) = create_literal_int((yyvsp[0].intval)); }
#line 2336 "parser.tab.c"
    break;

  case 128: /* literal: T_FLOATLIT  */
#line 434 "parser.y"
                 { (yyval.node) = create_literal_float((yyvsp[0].floatval)); }
#line 2342 "parser.tab.c"
    break;

  case 129: /* literal: T_BOOLLIT  */
#line 435 "parser.y"
                { (yyval.node) = create_literal_bool((yyvsp[0].boolval)); }
#line 2348 "parser.tab.c"
    break;

  case 130: /* literal: T_STRINGLIT  */
#line 436 "parser.y"
                  { (yyval.node) = create_literal_string((yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 2354 "parser.tab.c"
    break;

  case 131: /* literal: T_CHARLIT  */
#line 437 "parser.y"
                { (yyval.node) = create_literal_char((yyvsp[0].charval)); }
#line 2360 "parser.tab.c"
    break;


#line 2364 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 439 "parser.y"


void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d, column %d: %s\n", 
            line_number, column_number, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    }
    
    printf("Starting parse...\n");
    int result = yyparse();
    
    if (result == 0) {
        printf("Parse successful!\n\n");
        printf("==================== ABSTRACT SYNTAX TREE ====================\n\n");
        print_ast(root, 0);
        printf("\n==============================================================\n");
        free_ast(root);
    } else {
        printf("Parse failed!\n");
    }
    
    if (argc > 1) {
        fclose(yyin);
    }
    
    return result;
}
