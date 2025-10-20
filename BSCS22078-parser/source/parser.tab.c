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
  YYSYMBOL_T_DEFINE = 49,                  /* T_DEFINE  */
  YYSYMBOL_T_IF = 50,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 51,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 52,                   /* T_WHILE  */
  YYSYMBOL_T_FOR = 53,                     /* T_FOR  */
  YYSYMBOL_T_RETURN = 54,                  /* T_RETURN  */
  YYSYMBOL_T_BREAK = 55,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 56,                /* T_CONTINUE  */
  YYSYMBOL_T_SWITCH = 57,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 58,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 59,                 /* T_DEFAULT  */
  YYSYMBOL_T_DO = 60,                      /* T_DO  */
  YYSYMBOL_T_CONST = 61,                   /* T_CONST  */
  YYSYMBOL_T_ENUM = 62,                    /* T_ENUM  */
  YYSYMBOL_T_SINGLE_COMMENT = 63,          /* T_SINGLE_COMMENT  */
  YYSYMBOL_T_MULTI_COMMENT = 64,           /* T_MULTI_COMMENT  */
  YYSYMBOL_T_ERROR = 65,                   /* T_ERROR  */
  YYSYMBOL_UNARY_PREC = 66,                /* UNARY_PREC  */
  YYSYMBOL_YYACCEPT = 67,                  /* $accept  */
  YYSYMBOL_program = 68,                   /* program  */
  YYSYMBOL_declaration_list = 69,          /* declaration_list  */
  YYSYMBOL_declaration = 70,               /* declaration  */
  YYSYMBOL_varDecl = 71,                   /* varDecl  */
  YYSYMBOL_fnDecl = 72,                    /* fnDecl  */
  YYSYMBOL_parameters = 73,                /* parameters  */
  YYSYMBOL_parameter = 74,                 /* parameter  */
  YYSYMBOL_typeSpec = 75,                  /* typeSpec  */
  YYSYMBOL_typeQualifier = 76,             /* typeQualifier  */
  YYSYMBOL_baseType = 77,                  /* baseType  */
  YYSYMBOL_includeDirective = 78,          /* includeDirective  */
  YYSYMBOL_defineDirective = 79,           /* defineDirective  */
  YYSYMBOL_parameterList = 80,             /* parameterList  */
  YYSYMBOL_macroBody = 81,                 /* macroBody  */
  YYSYMBOL_macroBodyElement = 82,          /* macroBodyElement  */
  YYSYMBOL_enumDecl = 83,                  /* enumDecl  */
  YYSYMBOL_enumValueList = 84,             /* enumValueList  */
  YYSYMBOL_statement = 85,                 /* statement  */
  YYSYMBOL_assignStmt = 86,                /* assignStmt  */
  YYSYMBOL_returnStmt = 87,                /* returnStmt  */
  YYSYMBOL_breakStmt = 88,                 /* breakStmt  */
  YYSYMBOL_continueStmt = 89,              /* continueStmt  */
  YYSYMBOL_ifStmt = 90,                    /* ifStmt  */
  YYSYMBOL_whileStmt = 91,                 /* whileStmt  */
  YYSYMBOL_doWhileStmt = 92,               /* doWhileStmt  */
  YYSYMBOL_forStmt = 93,                   /* forStmt  */
  YYSYMBOL_switchStmt = 94,                /* switchStmt  */
  YYSYMBOL_caseStmt_list = 95,             /* caseStmt_list  */
  YYSYMBOL_caseStmt = 96,                  /* caseStmt  */
  YYSYMBOL_block = 97,                     /* block  */
  YYSYMBOL_exprStmt = 98,                  /* exprStmt  */
  YYSYMBOL_expression = 99,                /* expression  */
  YYSYMBOL_assignment = 100,               /* assignment  */
  YYSYMBOL_logicalOr = 101,                /* logicalOr  */
  YYSYMBOL_logicalAnd = 102,               /* logicalAnd  */
  YYSYMBOL_bitwiseOr = 103,                /* bitwiseOr  */
  YYSYMBOL_bitwiseXor = 104,               /* bitwiseXor  */
  YYSYMBOL_bitwiseAnd = 105,               /* bitwiseAnd  */
  YYSYMBOL_equality = 106,                 /* equality  */
  YYSYMBOL_comparison = 107,               /* comparison  */
  YYSYMBOL_shift = 108,                    /* shift  */
  YYSYMBOL_term = 109,                     /* term  */
  YYSYMBOL_factor = 110,                   /* factor  */
  YYSYMBOL_unary = 111,                    /* unary  */
  YYSYMBOL_postfix = 112,                  /* postfix  */
  YYSYMBOL_call = 113,                     /* call  */
  YYSYMBOL_arguments = 114,                /* arguments  */
  YYSYMBOL_primary = 115,                  /* primary  */
  YYSYMBOL_literal = 116,                  /* literal  */
  YYSYMBOL_operator = 117                  /* operator  */
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
typedef yytype_int16 yy_state_t;

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
#define YYFINAL  94
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   750

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  177
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  301

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   321


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
      65,    66
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    90,    90,    94,    98,    99,   104,   105,   106,   107,
     108,   109,   114,   116,   122,   124,   129,   130,   134,   139,
     140,   144,   148,   149,   150,   151,   152,   153,   158,   160,
     165,   171,   173,   175,   177,   179,   181,   186,   188,   193,
     194,   198,   199,   200,   205,   207,   212,   214,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   233,   238,
     239,   243,   247,   251,   253,   258,   263,   268,   270,   272,
     274,   276,   278,   280,   282,   284,   286,   288,   290,   295,
     297,   302,   303,   307,   309,   311,   313,   318,   319,   323,
     328,   332,   333,   338,   339,   344,   345,   350,   351,   356,
     357,   362,   363,   368,   369,   371,   376,   377,   379,   381,
     383,   388,   389,   391,   396,   397,   399,   404,   405,   407,
     409,   414,   415,   417,   419,   421,   423,   425,   430,   431,
     433,   438,   439,   446,   456,   457,   461,   462,   463,   464,
     465,   469,   470,   471,   472,   473,   477,   478,   479,   480,
     481,   482,   483,   484,   485,   486,   487,   488,   489,   490,
     491,   492,   493,   494,   495,   496,   497,   498,   499,   500,
     501,   502,   503,   504,   505,   506,   507,   508
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
  "T_INCLUDE", "T_DEFINE", "T_IF", "T_ELSE", "T_WHILE", "T_FOR",
  "T_RETURN", "T_BREAK", "T_CONTINUE", "T_SWITCH", "T_CASE", "T_DEFAULT",
  "T_DO", "T_CONST", "T_ENUM", "T_SINGLE_COMMENT", "T_MULTI_COMMENT",
  "T_ERROR", "UNARY_PREC", "$accept", "program", "declaration_list",
  "declaration", "varDecl", "fnDecl", "parameters", "parameter",
  "typeSpec", "typeQualifier", "baseType", "includeDirective",
  "defineDirective", "parameterList", "macroBody", "macroBodyElement",
  "enumDecl", "enumValueList", "statement", "assignStmt", "returnStmt",
  "breakStmt", "continueStmt", "ifStmt", "whileStmt", "doWhileStmt",
  "forStmt", "switchStmt", "caseStmt_list", "caseStmt", "block",
  "exprStmt", "expression", "assignment", "logicalOr", "logicalAnd",
  "bitwiseOr", "bitwiseXor", "bitwiseAnd", "equality", "comparison",
  "shift", "term", "factor", "unary", "postfix", "call", "arguments",
  "primary", "literal", "operator", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-198)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     291,  -198,  -198,  -198,  -198,  -198,  -198,   -18,  -198,  -198,
    -198,  -198,  -198,   705,   705,   705,   705,   705,   705,   705,
       1,    89,    92,   119,   132,   112,    36,    82,   133,   143,
    -198,   149,   161,   291,  -198,  -198,  -198,   155,   135,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,   145,  -198,    23,   128,   130,   136,
     137,     4,   141,   -37,    42,    84,  -198,  -198,    73,   168,
    -198,   705,  -198,   170,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,   178,   388,   705,   705,   351,  -198,   169,  -198,  -198,
     705,    32,   139,   177,  -198,  -198,    75,  -198,  -198,   705,
     705,   705,   705,   705,   705,   705,   705,   705,   705,   705,
     705,   705,   705,   705,   705,   705,   705,   705,  -198,  -198,
     466,   174,  -198,   -17,  -198,    88,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,   427,  -198,  -198,
    -198,   181,   183,   476,   513,   191,   527,  -198,   187,  -198,
     231,   189,    40,    18,  -198,   705,  -198,   128,   130,   136,
     137,     4,   141,   141,   -37,   -37,   -37,   -37,    42,    42,
      84,    84,  -198,  -198,  -198,  -198,  -198,    79,  -198,   197,
    -198,  -198,   427,    83,  -198,  -198,   143,   143,   564,   186,
     574,   192,     7,   611,   195,   193,  -198,   705,  -198,   196,
      44,   143,    90,  -198,   203,   198,  -198,   705,   194,   427,
     427,   209,   171,  -198,   143,   204,   621,   143,   205,   658,
     143,   210,   668,     2,   212,  -198,   208,   216,  -198,   143,
      48,  -198,  -198,  -198,  -198,   427,  -198,   143,  -198,   143,
     143,   215,  -198,   143,   143,   217,  -198,   143,   143,   232,
    -198,   705,   223,     9,  -198,   211,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,   143,  -198,  -198,   143,  -198,  -198,   143,
     226,   291,  -198,  -198,  -198,  -198,  -198,  -198,   291,   291,
     291
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,    22,    23,    24,    25,    27,    26,   137,   144,   141,
     142,   145,   143,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,     0,     2,     4,     9,    10,     0,     0,    20,
       6,     7,     8,    11,    56,    48,    54,    55,    49,    50,
      51,    52,    53,    57,     0,    90,    91,    93,    95,    97,
      99,   101,   103,   106,   111,   114,   117,   121,   128,   131,
     136,     0,   137,     0,   122,   123,   126,   127,   124,   125,
      30,     0,    31,     0,     0,     0,    60,     0,    61,    62,
       0,     0,     0,     0,     1,     5,     0,    19,    89,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   129,   130,
       0,     0,   138,     0,    41,   169,   170,   171,   172,   176,
     177,   173,   174,   175,   151,   152,   153,   154,   155,   156,
     157,   146,   147,   148,   149,   150,   167,   168,   158,   159,
     160,   161,   162,   163,   164,   165,   166,    32,    39,    42,
      43,     0,     0,     0,     0,     0,     0,    59,     0,    88,
       0,     0,     0,     0,    12,     0,    92,    94,    96,    98,
     100,   102,   104,   105,   107,   108,   109,   110,   112,   113,
     115,   116,   118,   119,   120,   133,   134,     0,    58,     0,
      28,    37,    35,     0,   169,    40,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    87,     0,    46,     0,
       0,     0,     0,    16,     0,     0,   132,     0,     0,    36,
      33,     0,    63,    65,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    45,     0,     0,    15,     0,
       0,    18,    13,   135,    29,    34,    38,     0,    78,     0,
       0,     0,    70,     0,     0,     0,    74,     0,     0,     0,
      80,     0,     0,     0,    81,     0,    44,    47,    14,    17,
      64,    77,    76,     0,    69,    68,     0,    73,    72,     0,
       0,    86,    79,    82,    66,    75,    67,    71,    84,    85,
      83
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -198,  -198,   -89,   -33,   166,  -198,  -198,     3,   -82,  -198,
     214,  -198,  -198,  -198,  -197,  -153,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,   -19,
     -75,   172,   -12,   156,  -198,   158,   159,   154,   162,   157,
       8,    68,    24,    16,     0,  -198,  -198,  -198,  -198,   -74,
    -198
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    32,    33,    34,    35,    36,   222,   223,    37,    38,
      39,    40,    41,   203,   157,   158,    42,   220,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,   273,   274,
      92,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,   197,    69,    70,
     160
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      95,    73,   170,   165,   205,   229,   199,    71,   159,   111,
     112,    80,   200,    87,    74,    75,    76,    77,    78,    79,
     270,     1,     2,     3,     4,     5,     6,   292,   174,    81,
     105,   106,   175,   255,   221,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    99,   218,
     169,     1,     2,     3,     4,     5,     6,    88,   219,   121,
     271,   272,   246,   100,    14,    15,   247,   271,   272,    16,
      17,   161,   162,    18,   113,   114,   205,    19,   168,    30,
      20,    21,    22,   159,    23,    24,    25,    26,    27,    28,
     173,   224,    29,    30,    31,   226,   174,   201,    82,   230,
     175,   227,   205,    89,   202,   231,   249,    83,   196,    30,
     118,   119,   250,   182,   183,   192,   193,   194,   115,   116,
     117,    72,     8,     9,    10,    11,    12,    13,   159,   190,
     191,   232,   233,    86,    84,   188,   189,    95,     1,     2,
       3,     4,     5,     6,    14,    15,   248,    85,    90,    16,
      17,   209,   211,    18,   214,   159,   159,    19,    93,   258,
      91,    94,   262,   225,    96,   266,    98,   101,   224,   107,
     108,   109,   110,   102,   278,   184,   185,   186,   187,   104,
     103,   159,   280,   120,   281,   282,   122,   123,   284,   285,
     167,   171,   287,   288,   172,   198,   235,   206,   238,   207,
     212,   241,   299,   215,   217,   244,   228,   236,   295,   300,
     243,   296,   251,   239,   297,   253,   242,   245,   256,   252,
     259,   263,   257,   254,   261,   277,   267,   265,   275,   276,
     269,   283,   294,   286,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   291,   289,   216,
     298,   164,    97,   279,   293,   176,   179,   166,   177,   290,
     178,   181,     0,    14,    15,   180,    95,    95,    16,    17,
       0,     0,    18,     0,     0,     0,    19,     0,     0,    20,
      21,    22,     0,    23,    24,    25,    26,    27,    28,     0,
       0,    29,    30,    31,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,    15,     0,     0,     0,    16,    17,
       0,     0,    18,     0,     0,     0,    19,     0,     0,    20,
      21,    22,     0,    23,    24,    25,    26,    27,    28,     0,
       0,    29,    30,    31,     1,     2,     3,     4,     5,     6,
      72,     8,     9,    10,    11,    12,    13,     0,     0,     0,
       0,     0,   163,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,    15,     0,     0,     0,    16,    17,
       0,     0,    18,     0,     0,     0,    19,   124,     8,     9,
      10,    11,    12,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    30,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   124,     8,     9,    10,
      11,    12,   204,   126,   127,   128,   129,   130,   131,   132,
     133,     0,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,    72,     8,     9,    10,    11,
      12,    13,   195,     0,     0,    72,     8,     9,    10,    11,
      12,    13,     0,     0,     0,     0,     0,   208,    14,    15,
       0,     0,     0,    16,    17,     0,     0,    18,    14,    15,
       0,    19,     0,    16,    17,     0,     0,    18,     0,     0,
       0,    19,    72,     8,     9,    10,    11,    12,    13,     0,
       0,     0,     0,     0,   210,     0,    72,     8,     9,    10,
      11,    12,    13,     0,     0,    14,    15,     0,   213,     0,
      16,    17,     0,     0,    18,     0,     0,     0,    19,    14,
      15,     0,     0,     0,    16,    17,     0,     0,    18,     0,
       0,     0,    19,    72,     8,     9,    10,    11,    12,    13,
     234,     0,     0,    72,     8,     9,    10,    11,    12,    13,
     237,     0,     0,     0,     0,     0,    14,    15,     0,     0,
       0,    16,    17,     0,     0,    18,    14,    15,     0,    19,
       0,    16,    17,     0,     0,    18,     0,     0,     0,    19,
      72,     8,     9,    10,    11,    12,    13,   240,     0,     0,
      72,     8,     9,    10,    11,    12,    13,   260,     0,     0,
       0,     0,     0,    14,    15,     0,     0,     0,    16,    17,
       0,     0,    18,    14,    15,     0,    19,     0,    16,    17,
       0,     0,    18,     0,     0,     0,    19,    72,     8,     9,
      10,    11,    12,    13,   264,     0,     0,    72,     8,     9,
      10,    11,    12,    13,   268,     0,     0,     0,     0,     0,
      14,    15,     0,     0,     0,    16,    17,     0,     0,    18,
      14,    15,     0,    19,     0,    16,    17,     0,     0,    18,
       0,     0,     0,    19,    72,     8,     9,    10,    11,    12,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,    15,     0,
       0,     0,    16,    17,     0,     0,    18,     0,     0,     0,
      19
};

static const yytype_int16 yycheck[] =
{
      33,    13,    91,    85,   157,   202,    23,    25,    82,    46,
      47,    10,    29,    25,    14,    15,    16,    17,    18,    19,
      18,     3,     4,     5,     6,     7,     8,    18,    21,    28,
      26,    27,    25,   230,    16,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    25,     9,
      18,     3,     4,     5,     6,     7,     8,    21,    18,    71,
      58,    59,    18,    40,    32,    33,    22,    58,    59,    37,
      38,    83,    84,    41,    32,    33,   229,    45,    90,    61,
      48,    49,    50,   157,    52,    53,    54,    55,    56,    57,
      15,   173,    60,    61,    62,    16,    21,     9,     9,    16,
      25,    22,   255,    21,    16,    22,    16,    15,   120,    61,
      37,    38,    22,   105,   106,   115,   116,   117,    34,    35,
      36,     9,    10,    11,    12,    13,    14,    15,   202,   113,
     114,   206,   207,    21,    15,   111,   112,   170,     3,     4,
       5,     6,     7,     8,    32,    33,   221,    15,    15,    37,
      38,   163,   164,    41,   166,   229,   230,    45,     9,   234,
      17,     0,   237,   175,     9,   240,    21,    39,   250,    28,
      29,    30,    31,    43,   249,   107,   108,   109,   110,    42,
      44,   255,   257,    15,   259,   260,    16,     9,   263,   264,
      21,    52,   267,   268,    17,    21,   208,    16,   210,    16,
       9,   213,   291,    16,    15,   217,     9,    21,   283,   298,
      17,   286,     9,    21,   289,   227,    21,    21,     9,    21,
      16,    16,    51,    29,   236,     9,    16,   239,    16,    21,
     242,    16,    21,    16,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    24,    16,    18,
      24,    85,    38,   250,   273,    99,   102,    85,   100,   271,
     101,   104,    -1,    32,    33,   103,   299,   300,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    -1,    52,    53,    54,    55,    56,    57,    -1,
      -1,    60,    61,    62,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    -1,    52,    53,    54,    55,    56,    57,    -1,
      -1,    60,    61,    62,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    -1,    -1,
      -1,    -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    45,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    61,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    -1,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,     9,    10,    11,    12,    13,
      14,    15,    16,    -1,    -1,     9,    10,    11,    12,    13,
      14,    15,    -1,    -1,    -1,    -1,    -1,    21,    32,    33,
      -1,    -1,    -1,    37,    38,    -1,    -1,    41,    32,    33,
      -1,    45,    -1,    37,    38,    -1,    -1,    41,    -1,    -1,
      -1,    45,     9,    10,    11,    12,    13,    14,    15,    -1,
      -1,    -1,    -1,    -1,    21,    -1,     9,    10,    11,    12,
      13,    14,    15,    -1,    -1,    32,    33,    -1,    21,    -1,
      37,    38,    -1,    -1,    41,    -1,    -1,    -1,    45,    32,
      33,    -1,    -1,    -1,    37,    38,    -1,    -1,    41,    -1,
      -1,    -1,    45,     9,    10,    11,    12,    13,    14,    15,
      16,    -1,    -1,     9,    10,    11,    12,    13,    14,    15,
      16,    -1,    -1,    -1,    -1,    -1,    32,    33,    -1,    -1,
      -1,    37,    38,    -1,    -1,    41,    32,    33,    -1,    45,
      -1,    37,    38,    -1,    -1,    41,    -1,    -1,    -1,    45,
       9,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
       9,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
      -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    37,    38,
      -1,    -1,    41,    32,    33,    -1,    45,    -1,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    45,     9,    10,    11,
      12,    13,    14,    15,    16,    -1,    -1,     9,    10,    11,
      12,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    -1,
      32,    33,    -1,    -1,    -1,    37,    38,    -1,    -1,    41,
      32,    33,    -1,    45,    -1,    37,    38,    -1,    -1,    41,
      -1,    -1,    -1,    45,     9,    10,    11,    12,    13,    14,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    -1,
      -1,    -1,    37,    38,    -1,    -1,    41,    -1,    -1,    -1,
      45
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    32,    33,    37,    38,    41,    45,
      48,    49,    50,    52,    53,    54,    55,    56,    57,    60,
      61,    62,    68,    69,    70,    71,    72,    75,    76,    77,
      78,    79,    83,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   115,
     116,    25,     9,    99,   111,   111,   111,   111,   111,   111,
      10,    28,     9,    15,    15,    15,    21,    99,    21,    21,
      15,    17,    97,     9,     0,    70,     9,    77,    21,    25,
      40,    39,    43,    44,    42,    26,    27,    28,    29,    30,
      31,    46,    47,    32,    33,    34,    35,    36,    37,    38,
      15,    99,    16,     9,     9,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    81,    82,   116,
     117,    99,    99,    21,    71,    75,    98,    21,    99,    18,
      69,    52,    17,    15,    21,    25,   100,   102,   103,   104,
     105,   106,   107,   107,   108,   108,   108,   108,   109,   109,
     110,   110,   111,   111,   111,    16,    99,   114,    21,    23,
      29,     9,    16,    80,    15,    82,    16,    16,    21,    99,
      21,    99,     9,    21,    99,    16,    18,    15,     9,    18,
      84,    16,    73,    74,    75,    99,    16,    22,     9,    81,
      16,    22,    97,    97,    16,    99,    21,    16,    99,    21,
      16,    99,    21,    17,    99,    21,    18,    22,    97,    16,
      22,     9,    21,    99,    29,    81,     9,    51,    97,    16,
      16,    99,    97,    16,    16,    99,    97,    16,    16,    99,
      18,    58,    59,    95,    96,    16,    21,     9,    97,    74,
      97,    97,    97,    16,    97,    97,    16,    97,    97,    16,
      99,    24,    18,    96,    21,    97,    97,    97,    24,    69,
      69
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    67,    68,    68,    69,    69,    70,    70,    70,    70,
      70,    70,    71,    71,    72,    72,    73,    73,    74,    75,
      75,    76,    77,    77,    77,    77,    77,    77,    78,    78,
      78,    79,    79,    79,    79,    79,    79,    80,    80,    81,
      81,    82,    82,    82,    83,    83,    84,    84,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    86,    87,
      87,    88,    89,    90,    90,    91,    92,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    94,
      94,    95,    95,    96,    96,    96,    96,    97,    97,    98,
      99,   100,   100,   101,   101,   102,   102,   103,   103,   104,
     104,   105,   105,   106,   106,   106,   107,   107,   107,   107,
     107,   108,   108,   108,   109,   109,   109,   110,   110,   110,
     110,   111,   111,   111,   111,   111,   111,   111,   112,   112,
     112,   113,   113,   113,   114,   114,   115,   115,   115,   115,
     115,   116,   116,   116,   116,   116,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     3,     5,     6,     5,     1,     3,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     6,
       2,     2,     3,     5,     6,     4,     5,     1,     3,     1,
       2,     1,     1,     1,     6,     5,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     3,
       2,     2,     2,     5,     7,     5,     7,     8,     7,     7,
       6,     8,     7,     7,     6,     8,     7,     7,     6,     7,
       6,     1,     2,     4,     3,     3,     2,     3,     2,     2,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     1,     2,     2,     2,     2,     2,     2,     1,     2,
       2,     1,     4,     3,     1,     3,     1,     1,     3,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1
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
#line 90 "parser.y"
                       { (yyval.node) = create_list(NODE_PROGRAM); 
                         for (int i = 0; i < (yyvsp[0].node)->data.list.count; i++) 
                             list_append((yyval.node), (yyvsp[0].node)->data.list.items[i]);
                         root = (yyval.node); }
#line 1627 "parser.tab.c"
    break;

  case 3: /* program: %empty  */
#line 94 "parser.y"
                  { (yyval.node) = create_list(NODE_PROGRAM); root = (yyval.node); }
#line 1633 "parser.tab.c"
    break;

  case 4: /* declaration_list: declaration  */
#line 98 "parser.y"
                  { (yyval.node) = create_list(NODE_DECLARATION_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 1639 "parser.tab.c"
    break;

  case 5: /* declaration_list: declaration_list declaration  */
#line 99 "parser.y"
                                   { (yyval.node) = (yyvsp[-1].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 1645 "parser.tab.c"
    break;

  case 6: /* declaration: includeDirective  */
#line 104 "parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1651 "parser.tab.c"
    break;

  case 7: /* declaration: defineDirective  */
#line 105 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1657 "parser.tab.c"
    break;

  case 8: /* declaration: enumDecl  */
#line 106 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1663 "parser.tab.c"
    break;

  case 9: /* declaration: varDecl  */
#line 107 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1669 "parser.tab.c"
    break;

  case 10: /* declaration: fnDecl  */
#line 108 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1675 "parser.tab.c"
    break;

  case 11: /* declaration: statement  */
#line 109 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1681 "parser.tab.c"
    break;

  case 12: /* varDecl: typeSpec T_IDENTIFIER T_SEMICOLON  */
#line 115 "parser.y"
        { (yyval.node) = create_var_decl((yyvsp[-2].node), (yyvsp[-1].strval), NULL); free((yyvsp[-1].strval)); }
#line 1687 "parser.tab.c"
    break;

  case 13: /* varDecl: typeSpec T_IDENTIFIER T_ASSIGNOP expression T_SEMICOLON  */
#line 117 "parser.y"
        { (yyval.node) = create_var_decl((yyvsp[-4].node), (yyvsp[-3].strval), (yyvsp[-1].node)); free((yyvsp[-3].strval)); }
#line 1693 "parser.tab.c"
    break;

  case 14: /* fnDecl: typeSpec T_IDENTIFIER T_LPAREN parameters T_RPAREN block  */
#line 123 "parser.y"
        { (yyval.node) = create_func_decl((yyvsp[-5].node), (yyvsp[-4].strval), (yyvsp[-2].node), (yyvsp[0].node)); free((yyvsp[-4].strval)); }
#line 1699 "parser.tab.c"
    break;

  case 15: /* fnDecl: typeSpec T_IDENTIFIER T_LPAREN T_RPAREN block  */
#line 125 "parser.y"
        { (yyval.node) = create_func_decl((yyvsp[-4].node), (yyvsp[-3].strval), NULL, (yyvsp[0].node)); free((yyvsp[-3].strval)); }
#line 1705 "parser.tab.c"
    break;

  case 16: /* parameters: parameter  */
#line 129 "parser.y"
                { (yyval.node) = create_list(NODE_PARAM_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 1711 "parser.tab.c"
    break;

  case 17: /* parameters: parameters T_COMMA parameter  */
#line 130 "parser.y"
                                   { (yyval.node) = (yyvsp[-2].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 1717 "parser.tab.c"
    break;

  case 18: /* parameter: typeSpec T_IDENTIFIER  */
#line 134 "parser.y"
                            { (yyval.node) = create_param((yyvsp[-1].node), (yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 1723 "parser.tab.c"
    break;

  case 19: /* typeSpec: typeQualifier baseType  */
#line 139 "parser.y"
                             { (yyval.node) = create_type_spec((yyvsp[-1].intval), (yyvsp[0].datatype)); }
#line 1729 "parser.tab.c"
    break;

  case 20: /* typeSpec: baseType  */
#line 140 "parser.y"
               { (yyval.node) = create_type_spec(0, (yyvsp[0].datatype)); }
#line 1735 "parser.tab.c"
    break;

  case 21: /* typeQualifier: T_CONST  */
#line 144 "parser.y"
              { (yyval.intval) = 1; }
#line 1741 "parser.tab.c"
    break;

  case 22: /* baseType: T_INT  */
#line 148 "parser.y"
            { (yyval.datatype) = TYPE_INT; }
#line 1747 "parser.tab.c"
    break;

  case 23: /* baseType: T_FLOAT  */
#line 149 "parser.y"
              { (yyval.datatype) = TYPE_FLOAT; }
#line 1753 "parser.tab.c"
    break;

  case 24: /* baseType: T_DOUBLE  */
#line 150 "parser.y"
               { (yyval.datatype) = TYPE_DOUBLE; }
#line 1759 "parser.tab.c"
    break;

  case 25: /* baseType: T_CHAR  */
#line 151 "parser.y"
             { (yyval.datatype) = TYPE_CHAR; }
#line 1765 "parser.tab.c"
    break;

  case 26: /* baseType: T_BOOL  */
#line 152 "parser.y"
             { (yyval.datatype) = TYPE_BOOL; }
#line 1771 "parser.tab.c"
    break;

  case 27: /* baseType: T_VOID  */
#line 153 "parser.y"
             { (yyval.datatype) = TYPE_VOID; }
#line 1777 "parser.tab.c"
    break;

  case 28: /* includeDirective: T_INCLUDE T_LT T_IDENTIFIER T_GT  */
#line 159 "parser.y"
        { (yyval.node) = create_include_directive((yyvsp[-1].strval), 1); free((yyvsp[-1].strval)); }
#line 1783 "parser.tab.c"
    break;

  case 29: /* includeDirective: T_INCLUDE T_LT T_IDENTIFIER T_DOT T_IDENTIFIER T_GT  */
#line 161 "parser.y"
        { char *file = malloc(strlen((yyvsp[-3].strval)) + strlen((yyvsp[-1].strval)) + 2);
          sprintf(file, "%s.%s", (yyvsp[-3].strval), (yyvsp[-1].strval));
          (yyval.node) = create_include_directive(file, 1);
          free((yyvsp[-3].strval)); free((yyvsp[-1].strval)); free(file); }
#line 1792 "parser.tab.c"
    break;

  case 30: /* includeDirective: T_INCLUDE T_STRINGLIT  */
#line 166 "parser.y"
        { (yyval.node) = create_include_directive((yyvsp[0].strval), 0); free((yyvsp[0].strval)); }
#line 1798 "parser.tab.c"
    break;

  case 31: /* defineDirective: T_DEFINE T_IDENTIFIER  */
#line 172 "parser.y"
        { (yyval.node) = create_define_directive((yyvsp[0].strval), NULL, NULL); free((yyvsp[0].strval)); }
#line 1804 "parser.tab.c"
    break;

  case 32: /* defineDirective: T_DEFINE T_IDENTIFIER macroBody  */
#line 174 "parser.y"
        { (yyval.node) = create_define_directive((yyvsp[-1].strval), NULL, "macro_body"); free((yyvsp[-1].strval)); }
#line 1810 "parser.tab.c"
    break;

  case 33: /* defineDirective: T_DEFINE T_IDENTIFIER T_LPAREN parameterList T_RPAREN  */
#line 176 "parser.y"
        { (yyval.node) = create_define_directive((yyvsp[-3].strval), (yyvsp[-1].node), NULL); free((yyvsp[-3].strval)); }
#line 1816 "parser.tab.c"
    break;

  case 34: /* defineDirective: T_DEFINE T_IDENTIFIER T_LPAREN parameterList T_RPAREN macroBody  */
#line 178 "parser.y"
        { (yyval.node) = create_define_directive((yyvsp[-4].strval), (yyvsp[-2].node), "macro_body"); free((yyvsp[-4].strval)); }
#line 1822 "parser.tab.c"
    break;

  case 35: /* defineDirective: T_DEFINE T_IDENTIFIER T_LPAREN T_RPAREN  */
#line 180 "parser.y"
        { (yyval.node) = create_define_directive((yyvsp[-2].strval), NULL, NULL); free((yyvsp[-2].strval)); }
#line 1828 "parser.tab.c"
    break;

  case 36: /* defineDirective: T_DEFINE T_IDENTIFIER T_LPAREN T_RPAREN macroBody  */
#line 182 "parser.y"
        { (yyval.node) = create_define_directive((yyvsp[-3].strval), NULL, "macro_body"); free((yyvsp[-3].strval)); }
#line 1834 "parser.tab.c"
    break;

  case 37: /* parameterList: T_IDENTIFIER  */
#line 186 "parser.y"
                   { (yyval.node) = create_list(NODE_PARAM_LIST); 
                     list_append((yyval.node), create_identifier((yyvsp[0].strval))); free((yyvsp[0].strval)); }
#line 1841 "parser.tab.c"
    break;

  case 38: /* parameterList: parameterList T_COMMA T_IDENTIFIER  */
#line 189 "parser.y"
        { (yyval.node) = (yyvsp[-2].node); list_append((yyval.node), create_identifier((yyvsp[0].strval))); free((yyvsp[0].strval)); }
#line 1847 "parser.tab.c"
    break;

  case 39: /* macroBody: macroBodyElement  */
#line 193 "parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1853 "parser.tab.c"
    break;

  case 40: /* macroBody: macroBody macroBodyElement  */
#line 194 "parser.y"
                                 { (yyval.node) = (yyvsp[-1].node); }
#line 1859 "parser.tab.c"
    break;

  case 41: /* macroBodyElement: T_IDENTIFIER  */
#line 198 "parser.y"
                   { (yyval.node) = create_identifier((yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 1865 "parser.tab.c"
    break;

  case 42: /* macroBodyElement: literal  */
#line 199 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1871 "parser.tab.c"
    break;

  case 43: /* macroBodyElement: operator  */
#line 200 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1877 "parser.tab.c"
    break;

  case 44: /* enumDecl: T_ENUM T_IDENTIFIER T_LBRACE enumValueList T_RBRACE T_SEMICOLON  */
#line 206 "parser.y"
        { (yyval.node) = create_enum_decl((yyvsp[-4].strval), (yyvsp[-2].node)); free((yyvsp[-4].strval)); }
#line 1883 "parser.tab.c"
    break;

  case 45: /* enumDecl: T_ENUM T_IDENTIFIER T_LBRACE T_RBRACE T_SEMICOLON  */
#line 208 "parser.y"
        { (yyval.node) = create_enum_decl((yyvsp[-3].strval), NULL); free((yyvsp[-3].strval)); }
#line 1889 "parser.tab.c"
    break;

  case 46: /* enumValueList: T_IDENTIFIER  */
#line 212 "parser.y"
                   { (yyval.node) = create_list(NODE_ENUM_VALUE_LIST); 
                     list_append((yyval.node), create_identifier((yyvsp[0].strval))); free((yyvsp[0].strval)); }
#line 1896 "parser.tab.c"
    break;

  case 47: /* enumValueList: enumValueList T_COMMA T_IDENTIFIER  */
#line 215 "parser.y"
        { (yyval.node) = (yyvsp[-2].node); list_append((yyval.node), create_identifier((yyvsp[0].strval))); free((yyvsp[0].strval)); }
#line 1902 "parser.tab.c"
    break;

  case 48: /* statement: returnStmt  */
#line 220 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1908 "parser.tab.c"
    break;

  case 49: /* statement: ifStmt  */
#line 221 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1914 "parser.tab.c"
    break;

  case 50: /* statement: whileStmt  */
#line 222 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1920 "parser.tab.c"
    break;

  case 51: /* statement: doWhileStmt  */
#line 223 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 1926 "parser.tab.c"
    break;

  case 52: /* statement: forStmt  */
#line 224 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1932 "parser.tab.c"
    break;

  case 53: /* statement: switchStmt  */
#line 225 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1938 "parser.tab.c"
    break;

  case 54: /* statement: breakStmt  */
#line 226 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1944 "parser.tab.c"
    break;

  case 55: /* statement: continueStmt  */
#line 227 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1950 "parser.tab.c"
    break;

  case 56: /* statement: assignStmt  */
#line 228 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1956 "parser.tab.c"
    break;

  case 57: /* statement: exprStmt  */
#line 229 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1962 "parser.tab.c"
    break;

  case 58: /* assignStmt: T_IDENTIFIER T_ASSIGNOP expression T_SEMICOLON  */
#line 234 "parser.y"
        { (yyval.node) = create_binary_op(OP_ASSIGN, create_identifier((yyvsp[-3].strval)), (yyvsp[-1].node)); free((yyvsp[-3].strval)); }
#line 1968 "parser.tab.c"
    break;

  case 59: /* returnStmt: T_RETURN expression T_SEMICOLON  */
#line 238 "parser.y"
                                      { (yyval.node) = create_return_stmt((yyvsp[-1].node)); }
#line 1974 "parser.tab.c"
    break;

  case 60: /* returnStmt: T_RETURN T_SEMICOLON  */
#line 239 "parser.y"
                           { (yyval.node) = create_return_stmt(NULL); }
#line 1980 "parser.tab.c"
    break;

  case 61: /* breakStmt: T_BREAK T_SEMICOLON  */
#line 243 "parser.y"
                          { (yyval.node) = create_break_stmt(); }
#line 1986 "parser.tab.c"
    break;

  case 62: /* continueStmt: T_CONTINUE T_SEMICOLON  */
#line 247 "parser.y"
                             { (yyval.node) = create_continue_stmt(); }
#line 1992 "parser.tab.c"
    break;

  case 63: /* ifStmt: T_IF T_LPAREN expression T_RPAREN block  */
#line 252 "parser.y"
        { (yyval.node) = create_if_stmt((yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1998 "parser.tab.c"
    break;

  case 64: /* ifStmt: T_IF T_LPAREN expression T_RPAREN block T_ELSE block  */
#line 254 "parser.y"
        { (yyval.node) = create_if_stmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2004 "parser.tab.c"
    break;

  case 65: /* whileStmt: T_WHILE T_LPAREN expression T_RPAREN block  */
#line 259 "parser.y"
        { (yyval.node) = create_while_stmt((yyvsp[-2].node), (yyvsp[0].node)); }
#line 2010 "parser.tab.c"
    break;

  case 66: /* doWhileStmt: T_DO block T_WHILE T_LPAREN expression T_RPAREN T_SEMICOLON  */
#line 264 "parser.y"
        { (yyval.node) = create_do_while_stmt((yyvsp[-5].node), (yyvsp[-2].node)); }
#line 2016 "parser.tab.c"
    break;

  case 67: /* forStmt: T_FOR T_LPAREN varDecl expression T_SEMICOLON expression T_RPAREN block  */
#line 269 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-5].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2022 "parser.tab.c"
    break;

  case 68: /* forStmt: T_FOR T_LPAREN varDecl expression T_SEMICOLON T_RPAREN block  */
#line 271 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), (yyvsp[-3].node), NULL, (yyvsp[0].node)); }
#line 2028 "parser.tab.c"
    break;

  case 69: /* forStmt: T_FOR T_LPAREN varDecl T_SEMICOLON expression T_RPAREN block  */
#line 273 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), NULL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2034 "parser.tab.c"
    break;

  case 70: /* forStmt: T_FOR T_LPAREN varDecl T_SEMICOLON T_RPAREN block  */
#line 275 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-3].node), NULL, NULL, (yyvsp[0].node)); }
#line 2040 "parser.tab.c"
    break;

  case 71: /* forStmt: T_FOR T_LPAREN exprStmt expression T_SEMICOLON expression T_RPAREN block  */
#line 277 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-5].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2046 "parser.tab.c"
    break;

  case 72: /* forStmt: T_FOR T_LPAREN exprStmt expression T_SEMICOLON T_RPAREN block  */
#line 279 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), (yyvsp[-3].node), NULL, (yyvsp[0].node)); }
#line 2052 "parser.tab.c"
    break;

  case 73: /* forStmt: T_FOR T_LPAREN exprStmt T_SEMICOLON expression T_RPAREN block  */
#line 281 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-4].node), NULL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2058 "parser.tab.c"
    break;

  case 74: /* forStmt: T_FOR T_LPAREN exprStmt T_SEMICOLON T_RPAREN block  */
#line 283 "parser.y"
        { (yyval.node) = create_for_stmt((yyvsp[-3].node), NULL, NULL, (yyvsp[0].node)); }
#line 2064 "parser.tab.c"
    break;

  case 75: /* forStmt: T_FOR T_LPAREN T_SEMICOLON expression T_SEMICOLON expression T_RPAREN block  */
#line 285 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2070 "parser.tab.c"
    break;

  case 76: /* forStmt: T_FOR T_LPAREN T_SEMICOLON expression T_SEMICOLON T_RPAREN block  */
#line 287 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, (yyvsp[-3].node), NULL, (yyvsp[0].node)); }
#line 2076 "parser.tab.c"
    break;

  case 77: /* forStmt: T_FOR T_LPAREN T_SEMICOLON T_SEMICOLON expression T_RPAREN block  */
#line 289 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, NULL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2082 "parser.tab.c"
    break;

  case 78: /* forStmt: T_FOR T_LPAREN T_SEMICOLON T_SEMICOLON T_RPAREN block  */
#line 291 "parser.y"
        { (yyval.node) = create_for_stmt(NULL, NULL, NULL, (yyvsp[0].node)); }
#line 2088 "parser.tab.c"
    break;

  case 79: /* switchStmt: T_SWITCH T_LPAREN expression T_RPAREN T_LBRACE caseStmt_list T_RBRACE  */
#line 296 "parser.y"
        { (yyval.node) = create_switch_stmt((yyvsp[-4].node), (yyvsp[-1].node)); }
#line 2094 "parser.tab.c"
    break;

  case 80: /* switchStmt: T_SWITCH T_LPAREN expression T_RPAREN T_LBRACE T_RBRACE  */
#line 298 "parser.y"
        { (yyval.node) = create_switch_stmt((yyvsp[-3].node), NULL); }
#line 2100 "parser.tab.c"
    break;

  case 81: /* caseStmt_list: caseStmt  */
#line 302 "parser.y"
               { (yyval.node) = create_list(NODE_STATEMENT_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 2106 "parser.tab.c"
    break;

  case 82: /* caseStmt_list: caseStmt_list caseStmt  */
#line 303 "parser.y"
                             { (yyval.node) = (yyvsp[-1].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 2112 "parser.tab.c"
    break;

  case 83: /* caseStmt: T_CASE expression T_COLON declaration_list  */
#line 308 "parser.y"
        { (yyval.node) = create_case_stmt((yyvsp[-2].node), (yyvsp[0].node)); }
#line 2118 "parser.tab.c"
    break;

  case 84: /* caseStmt: T_CASE expression T_COLON  */
#line 310 "parser.y"
        { (yyval.node) = create_case_stmt((yyvsp[-1].node), NULL); }
#line 2124 "parser.tab.c"
    break;

  case 85: /* caseStmt: T_DEFAULT T_COLON declaration_list  */
#line 312 "parser.y"
        { (yyval.node) = create_case_stmt(NULL, (yyvsp[0].node)); }
#line 2130 "parser.tab.c"
    break;

  case 86: /* caseStmt: T_DEFAULT T_COLON  */
#line 314 "parser.y"
        { (yyval.node) = create_case_stmt(NULL, NULL); }
#line 2136 "parser.tab.c"
    break;

  case 87: /* block: T_LBRACE declaration_list T_RBRACE  */
#line 318 "parser.y"
                                         { (yyval.node) = create_block((yyvsp[-1].node)); }
#line 2142 "parser.tab.c"
    break;

  case 88: /* block: T_LBRACE T_RBRACE  */
#line 319 "parser.y"
                        { (yyval.node) = create_block(NULL); }
#line 2148 "parser.tab.c"
    break;

  case 89: /* exprStmt: expression T_SEMICOLON  */
#line 323 "parser.y"
                             { (yyval.node) = (yyvsp[-1].node); }
#line 2154 "parser.tab.c"
    break;

  case 90: /* expression: assignment  */
#line 328 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2160 "parser.tab.c"
    break;

  case 91: /* assignment: logicalOr  */
#line 332 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 2166 "parser.tab.c"
    break;

  case 92: /* assignment: logicalOr T_ASSIGNOP assignment  */
#line 334 "parser.y"
        { (yyval.node) = create_binary_op(OP_ASSIGN, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2172 "parser.tab.c"
    break;

  case 93: /* logicalOr: logicalAnd  */
#line 338 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2178 "parser.tab.c"
    break;

  case 94: /* logicalOr: logicalOr T_OR logicalAnd  */
#line 340 "parser.y"
        { (yyval.node) = create_binary_op(OP_OR, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2184 "parser.tab.c"
    break;

  case 95: /* logicalAnd: bitwiseOr  */
#line 344 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 2190 "parser.tab.c"
    break;

  case 96: /* logicalAnd: logicalAnd T_AND bitwiseOr  */
#line 346 "parser.y"
        { (yyval.node) = create_binary_op(OP_AND, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2196 "parser.tab.c"
    break;

  case 97: /* bitwiseOr: bitwiseXor  */
#line 350 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2202 "parser.tab.c"
    break;

  case 98: /* bitwiseOr: bitwiseOr T_BITWISE_OR bitwiseXor  */
#line 352 "parser.y"
        { (yyval.node) = create_binary_op(OP_BIT_OR, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2208 "parser.tab.c"
    break;

  case 99: /* bitwiseXor: bitwiseAnd  */
#line 356 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2214 "parser.tab.c"
    break;

  case 100: /* bitwiseXor: bitwiseXor T_BITWISE_XOR bitwiseAnd  */
#line 358 "parser.y"
        { (yyval.node) = create_binary_op(OP_BIT_XOR, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2220 "parser.tab.c"
    break;

  case 101: /* bitwiseAnd: equality  */
#line 362 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 2226 "parser.tab.c"
    break;

  case 102: /* bitwiseAnd: bitwiseAnd T_BITWISE_AND equality  */
#line 364 "parser.y"
        { (yyval.node) = create_binary_op(OP_BIT_AND, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2232 "parser.tab.c"
    break;

  case 103: /* equality: comparison  */
#line 368 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2238 "parser.tab.c"
    break;

  case 104: /* equality: equality T_EQUALOP comparison  */
#line 370 "parser.y"
        { (yyval.node) = create_binary_op(OP_EQ, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2244 "parser.tab.c"
    break;

  case 105: /* equality: equality T_NE comparison  */
#line 372 "parser.y"
        { (yyval.node) = create_binary_op(OP_NE, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2250 "parser.tab.c"
    break;

  case 106: /* comparison: shift  */
#line 376 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 2256 "parser.tab.c"
    break;

  case 107: /* comparison: comparison T_LT shift  */
#line 378 "parser.y"
        { (yyval.node) = create_binary_op(OP_LT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2262 "parser.tab.c"
    break;

  case 108: /* comparison: comparison T_GT shift  */
#line 380 "parser.y"
        { (yyval.node) = create_binary_op(OP_GT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2268 "parser.tab.c"
    break;

  case 109: /* comparison: comparison T_LE shift  */
#line 382 "parser.y"
        { (yyval.node) = create_binary_op(OP_LE, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2274 "parser.tab.c"
    break;

  case 110: /* comparison: comparison T_GE shift  */
#line 384 "parser.y"
        { (yyval.node) = create_binary_op(OP_GE, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2280 "parser.tab.c"
    break;

  case 111: /* shift: term  */
#line 388 "parser.y"
           { (yyval.node) = (yyvsp[0].node); }
#line 2286 "parser.tab.c"
    break;

  case 112: /* shift: shift T_LEFT_SHIFT term  */
#line 390 "parser.y"
        { (yyval.node) = create_binary_op(OP_LSHIFT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2292 "parser.tab.c"
    break;

  case 113: /* shift: shift T_RIGHT_SHIFT term  */
#line 392 "parser.y"
        { (yyval.node) = create_binary_op(OP_RSHIFT, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2298 "parser.tab.c"
    break;

  case 114: /* term: factor  */
#line 396 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 2304 "parser.tab.c"
    break;

  case 115: /* term: term T_PLUS factor  */
#line 398 "parser.y"
        { (yyval.node) = create_binary_op(OP_ADD, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2310 "parser.tab.c"
    break;

  case 116: /* term: term T_MINUS factor  */
#line 400 "parser.y"
        { (yyval.node) = create_binary_op(OP_SUB, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2316 "parser.tab.c"
    break;

  case 117: /* factor: unary  */
#line 404 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 2322 "parser.tab.c"
    break;

  case 118: /* factor: factor T_MULTIPLY unary  */
#line 406 "parser.y"
        { (yyval.node) = create_binary_op(OP_MUL, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2328 "parser.tab.c"
    break;

  case 119: /* factor: factor T_DIVIDE unary  */
#line 408 "parser.y"
        { (yyval.node) = create_binary_op(OP_DIV, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2334 "parser.tab.c"
    break;

  case 120: /* factor: factor T_MODULO unary  */
#line 410 "parser.y"
        { (yyval.node) = create_binary_op(OP_MOD, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2340 "parser.tab.c"
    break;

  case 121: /* unary: postfix  */
#line 414 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2346 "parser.tab.c"
    break;

  case 122: /* unary: T_PLUS unary  */
#line 416 "parser.y"
        { (yyval.node) = create_unary_op(OP_PLUS_UNARY, (yyvsp[0].node)); }
#line 2352 "parser.tab.c"
    break;

  case 123: /* unary: T_MINUS unary  */
#line 418 "parser.y"
        { (yyval.node) = create_unary_op(OP_MINUS_UNARY, (yyvsp[0].node)); }
#line 2358 "parser.tab.c"
    break;

  case 124: /* unary: T_NOT unary  */
#line 420 "parser.y"
        { (yyval.node) = create_unary_op(OP_NOT, (yyvsp[0].node)); }
#line 2364 "parser.tab.c"
    break;

  case 125: /* unary: T_BITWISE_NOT unary  */
#line 422 "parser.y"
        { (yyval.node) = create_unary_op(OP_BIT_NOT, (yyvsp[0].node)); }
#line 2370 "parser.tab.c"
    break;

  case 126: /* unary: T_INCREMENT unary  */
#line 424 "parser.y"
        { (yyval.node) = create_unary_op(OP_PRE_INC, (yyvsp[0].node)); }
#line 2376 "parser.tab.c"
    break;

  case 127: /* unary: T_DECREMENT unary  */
#line 426 "parser.y"
        { (yyval.node) = create_unary_op(OP_PRE_DEC, (yyvsp[0].node)); }
#line 2382 "parser.tab.c"
    break;

  case 128: /* postfix: call  */
#line 430 "parser.y"
           { (yyval.node) = (yyvsp[0].node); }
#line 2388 "parser.tab.c"
    break;

  case 129: /* postfix: call T_INCREMENT  */
#line 432 "parser.y"
        { (yyval.node) = create_unary_op(OP_POST_INC, (yyvsp[-1].node)); }
#line 2394 "parser.tab.c"
    break;

  case 130: /* postfix: call T_DECREMENT  */
#line 434 "parser.y"
        { (yyval.node) = create_unary_op(OP_POST_DEC, (yyvsp[-1].node)); }
#line 2400 "parser.tab.c"
    break;

  case 131: /* call: primary  */
#line 438 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2406 "parser.tab.c"
    break;

  case 132: /* call: primary T_LPAREN arguments T_RPAREN  */
#line 440 "parser.y"
        { if ((yyvsp[-3].node)->type == NODE_IDENTIFIER) {
              (yyval.node) = create_func_call((yyvsp[-3].node)->data.identifier.name, (yyvsp[-1].node));
              free_ast((yyvsp[-3].node));
          } else {
              (yyval.node) = (yyvsp[-3].node);
          } }
#line 2417 "parser.tab.c"
    break;

  case 133: /* call: primary T_LPAREN T_RPAREN  */
#line 447 "parser.y"
        { if ((yyvsp[-2].node)->type == NODE_IDENTIFIER) {
              (yyval.node) = create_func_call((yyvsp[-2].node)->data.identifier.name, NULL);
              free_ast((yyvsp[-2].node));
          } else {
              (yyval.node) = (yyvsp[-2].node);
          } }
#line 2428 "parser.tab.c"
    break;

  case 134: /* arguments: expression  */
#line 456 "parser.y"
                 { (yyval.node) = create_list(NODE_ARG_LIST); list_append((yyval.node), (yyvsp[0].node)); }
#line 2434 "parser.tab.c"
    break;

  case 135: /* arguments: arguments T_COMMA expression  */
#line 457 "parser.y"
                                   { (yyval.node) = (yyvsp[-2].node); list_append((yyval.node), (yyvsp[0].node)); }
#line 2440 "parser.tab.c"
    break;

  case 136: /* primary: literal  */
#line 461 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2446 "parser.tab.c"
    break;

  case 137: /* primary: T_IDENTIFIER  */
#line 462 "parser.y"
                   { (yyval.node) = create_identifier((yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 2452 "parser.tab.c"
    break;

  case 138: /* primary: T_LPAREN expression T_RPAREN  */
#line 463 "parser.y"
                                   { (yyval.node) = (yyvsp[-1].node); }
#line 2458 "parser.tab.c"
    break;

  case 139: /* primary: T_INCREMENT unary  */
#line 464 "parser.y"
                        { (yyval.node) = create_unary_op(OP_PRE_INC, (yyvsp[0].node)); }
#line 2464 "parser.tab.c"
    break;

  case 140: /* primary: T_DECREMENT unary  */
#line 465 "parser.y"
                        { (yyval.node) = create_unary_op(OP_PRE_DEC, (yyvsp[0].node)); }
#line 2470 "parser.tab.c"
    break;

  case 141: /* literal: T_INTLIT  */
#line 469 "parser.y"
               { (yyval.node) = create_literal_int((yyvsp[0].intval)); }
#line 2476 "parser.tab.c"
    break;

  case 142: /* literal: T_FLOATLIT  */
#line 470 "parser.y"
                 { (yyval.node) = create_literal_float((yyvsp[0].floatval)); }
#line 2482 "parser.tab.c"
    break;

  case 143: /* literal: T_BOOLLIT  */
#line 471 "parser.y"
                { (yyval.node) = create_literal_bool((yyvsp[0].boolval)); }
#line 2488 "parser.tab.c"
    break;

  case 144: /* literal: T_STRINGLIT  */
#line 472 "parser.y"
                  { (yyval.node) = create_literal_string((yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 2494 "parser.tab.c"
    break;

  case 145: /* literal: T_CHARLIT  */
#line 473 "parser.y"
                { (yyval.node) = create_literal_char((yyvsp[0].charval)); }
#line 2500 "parser.tab.c"
    break;

  case 146: /* operator: T_PLUS  */
#line 477 "parser.y"
             { (yyval.node) = create_identifier("+"); }
#line 2506 "parser.tab.c"
    break;

  case 147: /* operator: T_MINUS  */
#line 478 "parser.y"
              { (yyval.node) = create_identifier("-"); }
#line 2512 "parser.tab.c"
    break;

  case 148: /* operator: T_MULTIPLY  */
#line 479 "parser.y"
                 { (yyval.node) = create_identifier("*"); }
#line 2518 "parser.tab.c"
    break;

  case 149: /* operator: T_DIVIDE  */
#line 480 "parser.y"
               { (yyval.node) = create_identifier("/"); }
#line 2524 "parser.tab.c"
    break;

  case 150: /* operator: T_MODULO  */
#line 481 "parser.y"
               { (yyval.node) = create_identifier("%"); }
#line 2530 "parser.tab.c"
    break;

  case 151: /* operator: T_ASSIGNOP  */
#line 482 "parser.y"
                 { (yyval.node) = create_identifier("="); }
#line 2536 "parser.tab.c"
    break;

  case 152: /* operator: T_EQUALOP  */
#line 483 "parser.y"
                { (yyval.node) = create_identifier("=="); }
#line 2542 "parser.tab.c"
    break;

  case 153: /* operator: T_NE  */
#line 484 "parser.y"
           { (yyval.node) = create_identifier("!="); }
#line 2548 "parser.tab.c"
    break;

  case 154: /* operator: T_LT  */
#line 485 "parser.y"
           { (yyval.node) = create_identifier("<"); }
#line 2554 "parser.tab.c"
    break;

  case 155: /* operator: T_GT  */
#line 486 "parser.y"
           { (yyval.node) = create_identifier(">"); }
#line 2560 "parser.tab.c"
    break;

  case 156: /* operator: T_LE  */
#line 487 "parser.y"
           { (yyval.node) = create_identifier("<="); }
#line 2566 "parser.tab.c"
    break;

  case 157: /* operator: T_GE  */
#line 488 "parser.y"
           { (yyval.node) = create_identifier(">="); }
#line 2572 "parser.tab.c"
    break;

  case 158: /* operator: T_AND  */
#line 489 "parser.y"
            { (yyval.node) = create_identifier("&&"); }
#line 2578 "parser.tab.c"
    break;

  case 159: /* operator: T_OR  */
#line 490 "parser.y"
           { (yyval.node) = create_identifier("||"); }
#line 2584 "parser.tab.c"
    break;

  case 160: /* operator: T_NOT  */
#line 491 "parser.y"
            { (yyval.node) = create_identifier("!"); }
#line 2590 "parser.tab.c"
    break;

  case 161: /* operator: T_BITWISE_AND  */
#line 492 "parser.y"
                    { (yyval.node) = create_identifier("&"); }
#line 2596 "parser.tab.c"
    break;

  case 162: /* operator: T_BITWISE_OR  */
#line 493 "parser.y"
                   { (yyval.node) = create_identifier("|"); }
#line 2602 "parser.tab.c"
    break;

  case 163: /* operator: T_BITWISE_XOR  */
#line 494 "parser.y"
                    { (yyval.node) = create_identifier("^"); }
#line 2608 "parser.tab.c"
    break;

  case 164: /* operator: T_BITWISE_NOT  */
#line 495 "parser.y"
                    { (yyval.node) = create_identifier("~"); }
#line 2614 "parser.tab.c"
    break;

  case 165: /* operator: T_LEFT_SHIFT  */
#line 496 "parser.y"
                   { (yyval.node) = create_identifier("<<"); }
#line 2620 "parser.tab.c"
    break;

  case 166: /* operator: T_RIGHT_SHIFT  */
#line 497 "parser.y"
                    { (yyval.node) = create_identifier(">>"); }
#line 2626 "parser.tab.c"
    break;

  case 167: /* operator: T_INCREMENT  */
#line 498 "parser.y"
                  { (yyval.node) = create_identifier("++"); }
#line 2632 "parser.tab.c"
    break;

  case 168: /* operator: T_DECREMENT  */
#line 499 "parser.y"
                  { (yyval.node) = create_identifier("--"); }
#line 2638 "parser.tab.c"
    break;

  case 169: /* operator: T_LPAREN  */
#line 500 "parser.y"
               { (yyval.node) = create_identifier("("); }
#line 2644 "parser.tab.c"
    break;

  case 170: /* operator: T_RPAREN  */
#line 501 "parser.y"
               { (yyval.node) = create_identifier(")"); }
#line 2650 "parser.tab.c"
    break;

  case 171: /* operator: T_LBRACE  */
#line 502 "parser.y"
               { (yyval.node) = create_identifier("{"); }
#line 2656 "parser.tab.c"
    break;

  case 172: /* operator: T_RBRACE  */
#line 503 "parser.y"
               { (yyval.node) = create_identifier("}"); }
#line 2662 "parser.tab.c"
    break;

  case 173: /* operator: T_SEMICOLON  */
#line 504 "parser.y"
                  { (yyval.node) = create_identifier(";"); }
#line 2668 "parser.tab.c"
    break;

  case 174: /* operator: T_COMMA  */
#line 505 "parser.y"
              { (yyval.node) = create_identifier(","); }
#line 2674 "parser.tab.c"
    break;

  case 175: /* operator: T_DOT  */
#line 506 "parser.y"
            { (yyval.node) = create_identifier("."); }
#line 2680 "parser.tab.c"
    break;

  case 176: /* operator: T_LBRACKET  */
#line 507 "parser.y"
                 { (yyval.node) = create_identifier("["); }
#line 2686 "parser.tab.c"
    break;

  case 177: /* operator: T_RBRACKET  */
#line 508 "parser.y"
                 { (yyval.node) = create_identifier("]"); }
#line 2692 "parser.tab.c"
    break;


#line 2696 "parser.tab.c"

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

#line 511 "parser.y"


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
