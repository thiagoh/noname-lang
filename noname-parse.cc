/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "noname.y" /* yacc.c:339  */

#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <stack>
#include <memory>
#include <string>
#include <vector>
#include <math.h>
#include "noname-parse.h"
#include "noname-types.h"

//# define YY_SYMBOL_PRINT(Title, Type, Value, Location) 

extern ASTContext* context;
extern std::stack<ASTContext*> context_stack;
extern int yylex(void);
extern int yydebug;
extern void yyerror(const char *error_msg);
extern void division_by_zero(YYLTYPE &yylloc);
extern void eval(ASTNode* ast_node);


#line 98 "noname.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "noname.tab.h".  */
#ifndef YY_YY_NONAME_TAB_H_INCLUDED
# define YY_YY_NONAME_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LINE_BREAK = 258,
    STMT_SEP = 259,
    LETTER = 260,
    DIGIT = 261,
    DIGITS = 262,
    DARROW = 263,
    ELSE = 264,
    FALSE = 265,
    IF = 266,
    IN = 267,
    LET = 268,
    DEF = 269,
    LOOP = 270,
    THEN = 271,
    WHILE = 272,
    BREAK = 273,
    CASE = 274,
    NEW = 275,
    NOT = 276,
    RETURN = 277,
    TRUE = 278,
    NEWLINE = 279,
    NOTNEWLINE = 280,
    WHITESPACE = 281,
    LE = 282,
    ASSIGN = 283,
    NULLCH = 284,
    BACKSLASH = 285,
    STAR = 286,
    NOTSTAR = 287,
    LEFTPAREN = 288,
    NOTLEFTPAREN = 289,
    RIGHTPAREN = 290,
    NOTRIGHTPAREN = 291,
    LINE_COMMENT = 292,
    START_COMMENT = 293,
    END_COMMENT = 294,
    QUOTES = 295,
    ERROR = 296,
    ID = 297,
    STR_CONST = 298,
    DOUBLE = 299,
    LONG = 300,
    NEG = 314
  };
#endif
/* Tokens.  */
#define LINE_BREAK 258
#define STMT_SEP 259
#define LETTER 260
#define DIGIT 261
#define DIGITS 262
#define DARROW 263
#define ELSE 264
#define FALSE 265
#define IF 266
#define IN 267
#define LET 268
#define DEF 269
#define LOOP 270
#define THEN 271
#define WHILE 272
#define BREAK 273
#define CASE 274
#define NEW 275
#define NOT 276
#define RETURN 277
#define TRUE 278
#define NEWLINE 279
#define NOTNEWLINE 280
#define WHITESPACE 281
#define LE 282
#define ASSIGN 283
#define NULLCH 284
#define BACKSLASH 285
#define STAR 286
#define NOTSTAR 287
#define LEFTPAREN 288
#define NOTLEFTPAREN 289
#define RIGHTPAREN 290
#define NOTRIGHTPAREN 291
#define LINE_COMMENT 292
#define START_COMMENT 293
#define END_COMMENT 294
#define QUOTES 295
#define ERROR 296
#define ID 297
#define STR_CONST 298
#define DOUBLE 299
#define LONG 300
#define NEG 314

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 37 "noname.y" /* yacc.c:355  */

  char* id_v;
  double double_v;
  long long_v;
  ASTContext* context;
  stmtlist* stmt_list;
  explist* exp_list;
  arglist* arg_list;
  arg* arg;

  ASTNode* ast_node;
  ExpNode* exp_node;
  char* error_msg;

#line 245 "noname.tab.c" /* yacc.c:355  */
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

#endif /* !YY_YY_NONAME_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 52 "noname.y" /* yacc.c:358  */


  template<class Ret, class In>
  inline Ret _cast(void* v) {
    return (Ret) *(In*)v;
  }


#line 284 "noname.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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


#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
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
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   128

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  17
/* YYNRULES -- Number of rules.  */
#define YYNRULES  47
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  74

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   314

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      65,    66,    45,    42,    69,    43,     2,    44,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    46,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    67,     2,    68,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   138,   138,   139,   142,   146,   147,   148,   152,   153,
     157,   163,   169,   175,   189,   196,   189,   218,   219,   223,
     227,   233,   239,   242,   245,   248,   251,   254,   257,   260,
     263,   266,   269,   272,   282,   283,   285,   290,   292,   297,
     298,   299,   300,   304,   305,   306,   310,   311
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"line_break\"", "\"stmt_sep\"",
  "\"letter\"", "\"digit\"", "\"digits\"", "\"darrow\"", "\"else\"",
  "\"false\"", "\"if\"", "\"in\"", "\"let\"", "\"def\"", "\"loop\"",
  "\"then\"", "\"while\"", "\"break\"", "\"case\"", "\"new\"", "\"not\"",
  "\"return\"", "\"true\"", "\"newline\"", "\"notnewline\"",
  "\"whitespace\"", "\"le\"", "\"assign\"", "\"nullch\"", "\"backslash\"",
  "\"star\"", "\"notstar\"", "\"leftparen\"", "\"notleftparen\"",
  "\"rightparen\"", "\"notrightparen\"", "\"line_comment\"",
  "\"start_comment\"", "\"end_comment\"", "\"quotes\"", "\"error\"",
  "\"+\"", "\"-\"", "\"/\"", "\"*\"", "\"^\"", "\"identifier\"",
  "\"string_constant\"", "\"double\"", "\"long\"", "\"declaration\"",
  "\"assignment\"", "\"optional_ret_stmt\"", "\"expression\"",
  "\"function_def\"", "\"stmt_list\"", "\"ne_stmt_list\"", "\"exp_list\"",
  "\"ne_exp_list\"", "\"arg_list\"", "\"ne_arg_list\"", "\"arg\"",
  "\"statement\"", "NEG", "'('", "')'", "'{'", "'}'", "','", "$accept",
  "prog", "stmt_list", "ne_stmt_list", "stmt", "function_def", "@1", "$@2",
  "optional_ret_stmt", "assignment", "declaration", "exp", "arg_list",
  "ne_arg_list", "arg", "exp_list", "ne_exp_list", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,    43,    45,    47,    42,    94,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,    40,    41,   123,   125,    44
};
# endif

#define YYPACT_NINF -54

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-54)))

#define YYTABLE_NINF -46

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       0,     1,     8,   -54,   -54,   -43,   -31,    63,   -25,   -54,
     -54,   -54,    63,   -54,    15,    16,    26,    32,     3,   -54,
     -53,   -54,    63,    63,    43,   -54,   -54,   -54,   -54,    63,
      63,    63,    63,    63,    63,   -42,    72,    37,   -21,    63,
     -54,   -35,   -35,    14,    14,    14,    72,   -12,   -20,   -54,
      49,    36,     4,   -15,     2,     6,   -11,   -54,   -12,   -54,
     -54,   -54,    23,    27,    19,    80,    19,   -16,    63,    35,
      -7,    55,   -54,   -54
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     4,     1,     0,     0,     0,    22,    23,
      25,    24,     0,     3,     0,     0,     0,     0,    21,    14,
      22,    30,     0,    43,     0,    12,    11,    10,    13,     0,
       0,     0,     0,     0,     0,     0,    19,     0,     0,     0,
      32,    26,    27,    29,    28,    31,    20,    34,    46,    33,
       0,    39,     0,     0,    35,    47,     0,    15,     0,    42,
      40,    41,     0,    36,     5,    17,     0,     8,     0,     0,
       9,     0,    16,    18
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -54,   -54,   -54,   -54,   -22,   -54,   -54,   -54,   -54,   -54,
     -54,    -5,   -54,   -54,    46,   -54,   -54
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,    65,    66,    13,    14,    35,    62,    69,    15,
      16,    17,    52,    53,    54,    38,    39
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      -2,     1,    21,    22,    18,     3,    -6,    24,     4,    31,
      32,    33,    23,    -2,    -2,    -7,    19,    36,    37,    25,
      26,     5,     6,    47,    41,    42,    43,    44,    45,    46,
      27,    34,     5,     6,    50,    51,    28,    59,    60,    61,
      23,    48,    67,    -2,    70,    49,   -44,    -2,    -2,    -2,
      -2,     7,    -6,    55,    58,     8,     9,    10,    11,    73,
      33,    -7,     7,    71,    56,    -2,     8,     9,    10,    11,
      57,   -37,   -45,    12,    29,    30,    31,    32,    33,    29,
      30,    31,    32,    33,    12,    29,    30,    31,    32,    33,
      64,    29,    30,    31,    32,    33,   -38,    29,    30,    31,
      32,    33,    68,    72,    63,     0,     7,     0,     0,    40,
      20,     9,    10,    11,    29,    30,    31,    32,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12
};

static const yytype_int8 yycheck[] =
{
       0,     1,     7,    28,    47,     4,    22,    12,     0,    44,
      45,    46,    65,    13,    14,    22,    47,    22,    23,     4,
       4,    13,    14,    65,    29,    30,    31,    32,    33,    34,
       4,    28,    13,    14,    39,    47,     4,    48,    49,    50,
      65,     4,    64,    43,    66,    66,    66,    47,    48,    49,
      50,    43,    68,     4,    69,    47,    48,    49,    50,     4,
      46,    68,    43,    68,    28,    65,    47,    48,    49,    50,
      66,    69,    66,    65,    42,    43,    44,    45,    46,    42,
      43,    44,    45,    46,    65,    42,    43,    44,    45,    46,
      67,    42,    43,    44,    45,    46,    69,    42,    43,    44,
      45,    46,    22,    68,    58,    -1,    43,    -1,    -1,    66,
      47,    48,    49,    50,    42,    43,    44,    45,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    71,     4,     0,    13,    14,    43,    47,    48,
      49,    50,    65,    74,    75,    79,    80,    81,    47,    47,
      47,    81,    28,    65,    81,     4,     4,     4,     4,    42,
      43,    44,    45,    46,    28,    76,    81,    81,    85,    86,
      66,    81,    81,    81,    81,    81,    81,    65,     4,    66,
      81,    47,    82,    83,    84,     4,    28,    66,    69,    48,
      49,    50,    77,    84,    67,    72,    73,    74,    22,    78,
      74,    81,    68,     4
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    71,    71,    71,    72,    72,    72,    73,    73,
      74,    74,    74,    74,    76,    77,    75,    78,    78,    79,
      79,    80,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    82,    82,    82,    83,    83,    84,
      84,    84,    84,    85,    85,    85,    86,    86
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     0,     1,     2,     1,     2,
       2,     2,     2,     2,     0,     0,    11,     0,     3,     3,
       4,     2,     1,     1,     1,     1,     3,     3,     3,     3,
       2,     3,     3,     4,     0,     1,     3,     1,     3,     1,
       3,     3,     3,     0,     2,     3,     2,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
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

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
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
| yyreduce -- Do a reduction.  |
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 139 "noname.y" /* yacc.c:1661  */
    {
    eval((yyvsp[0].ast_node));
  }
#line 1533 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 4:
#line 142 "noname.y" /* yacc.c:1661  */
    { yyerrok; fprintf(stderr, "Error at %d:%d", (yylsp[-1]).first_column, (yylsp[-1]).last_column); }
#line 1539 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 5:
#line 146 "noname.y" /* yacc.c:1661  */
    { (yyval.stmt_list) = NULL; }
#line 1545 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 6:
#line 147 "noname.y" /* yacc.c:1661  */
    { (yyval.stmt_list) = new_stmt_list(context, (yyvsp[0].ast_node)); }
#line 1551 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 7:
#line 148 "noname.y" /* yacc.c:1661  */
    { (yyval.stmt_list) = new_stmt_list(context, (yyvsp[-1].stmt_list), (yyvsp[0].ast_node)); }
#line 1557 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 8:
#line 152 "noname.y" /* yacc.c:1661  */
    { (yyval.stmt_list) = new_stmt_list(context, (yyvsp[0].ast_node)); }
#line 1563 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 9:
#line 153 "noname.y" /* yacc.c:1661  */
    { (yyval.stmt_list) = new_stmt_list(context, (yyvsp[-1].stmt_list), (yyvsp[0].ast_node)); }
#line 1569 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 10:
#line 157 "noname.y" /* yacc.c:1661  */
    { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt - declaration]: ");
      }
      (yyval.ast_node) = (yyvsp[-1].ast_node);
    }
#line 1580 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 11:
#line 163 "noname.y" /* yacc.c:1661  */
    { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt - assignment]: ");
      }
      (yyval.ast_node) = (yyvsp[-1].exp_node);
    }
#line 1591 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 12:
#line 169 "noname.y" /* yacc.c:1661  */
    { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt - function_def]: ");
      }
      // $$ = $1;
    }
#line 1602 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 13:
#line 175 "noname.y" /* yacc.c:1661  */
    { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt exp]: ");
      }
      (yyval.ast_node) = (yyvsp[-1].exp_node);
    }
#line 1613 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 14:
#line 189 "noname.y" /* yacc.c:1661  */
    {

        fprintf(stderr, "\n[############## processing function_def BEFORE arg_list ##############]");
        (yyval.context) = new ASTContext(std::string((yyvsp[0].id_v)), context);
        context_stack.push((yyval.context));
        context = (yyval.context);
        
      }
#line 1626 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 15:
#line 196 "noname.y" /* yacc.c:1661  */
    {
        fprintf(stderr, "\n[############## processing function_def BEFORE exp_list ##############]");

      }
#line 1635 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 16:
#line 199 "noname.y" /* yacc.c:1661  */
    {
      // ASTContext newContext(context);

      if ((yyvsp[-6].arg_list) == NULL) {
        (yyvsp[-6].arg_list) = new_arg_list(context);
      } 

      if ((yyvsp[-2].stmt_list) == NULL) {
        (yyvsp[-2].stmt_list) = new_stmt_list(context);
      } 

      // $$ = new_function_def(*$<context>function_context, $ID, $arg_list, $stmt_list);
      (yyval.ast_node) = new_function_def(context, (yyvsp[-9].id_v), (yyvsp[-6].arg_list), (yyvsp[-2].stmt_list), (yyvsp[-1].exp_node));
      context_stack.pop();
      context = context_stack.top();
    }
#line 1656 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 17:
#line 218 "noname.y" /* yacc.c:1661  */
    { (yyval.exp_node) = NULL; }
#line 1662 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 18:
#line 219 "noname.y" /* yacc.c:1661  */
    { (yyval.exp_node) = (yyvsp[-1].exp_node); }
#line 1668 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 19:
#line 223 "noname.y" /* yacc.c:1661  */
    {
    // $$ = new AssignmentNode($1, $3);
    (yyval.exp_node) = new AssignmentNode(context, (yyvsp[-2].id_v), std::move((ExpNode*) (yyvsp[0].exp_node)));
  }
#line 1677 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 20:
#line 227 "noname.y" /* yacc.c:1661  */
    {
    (yyval.exp_node) = new AssignmentNode(context, (yyvsp[-2].id_v), std::move((ExpNode*) (yyvsp[0].exp_node)));
  }
#line 1685 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 21:
#line 233 "noname.y" /* yacc.c:1661  */
    {
    (yyval.ast_node) = new DeclarationNode(context, (yyvsp[0].id_v));
  }
#line 1693 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 22:
#line 239 "noname.y" /* yacc.c:1661  */
    {
    (yyval.exp_node) = new VarNode(context, (yyvsp[0].id_v));
  }
#line 1701 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 23:
#line 242 "noname.y" /* yacc.c:1661  */
    {
    (yyval.exp_node) = new StringNode(context, (yyvsp[0].id_v));
  }
#line 1709 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 24:
#line 245 "noname.y" /* yacc.c:1661  */
    {
    (yyval.exp_node) = new NumberNode(context, (yyvsp[0].long_v));
  }
#line 1717 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 25:
#line 248 "noname.y" /* yacc.c:1661  */
    {
    (yyval.exp_node) = new NumberNode(context, (yyvsp[0].double_v));
  }
#line 1725 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 26:
#line 251 "noname.y" /* yacc.c:1661  */
    {
      (yyval.exp_node) = new BinaryExpNode(context, '+', (yyvsp[-2].exp_node), (yyvsp[0].exp_node));
    }
#line 1733 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 27:
#line 254 "noname.y" /* yacc.c:1661  */
    {
      (yyval.exp_node) = new BinaryExpNode(context, '-', (yyvsp[-2].exp_node), (yyvsp[0].exp_node));
    }
#line 1741 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 28:
#line 257 "noname.y" /* yacc.c:1661  */
    {
      (yyval.exp_node) = new BinaryExpNode(context, '*', (yyvsp[-2].exp_node), (yyvsp[0].exp_node));
    }
#line 1749 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 29:
#line 260 "noname.y" /* yacc.c:1661  */
    {
      (yyval.exp_node) = new BinaryExpNode(context, '/', (yyvsp[-2].exp_node), (yyvsp[0].exp_node));
    }
#line 1757 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 30:
#line 263 "noname.y" /* yacc.c:1661  */
    {
      (yyval.exp_node) = new UnaryExpNode(context, '-', (yyvsp[0].exp_node));
    }
#line 1765 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 31:
#line 266 "noname.y" /* yacc.c:1661  */
    {
      (yyval.exp_node) = new BinaryExpNode(context, '^', (yyvsp[-2].exp_node), (yyvsp[0].exp_node));
    }
#line 1773 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 32:
#line 269 "noname.y" /* yacc.c:1661  */
    {
      (yyval.exp_node) = new BinaryExpNode(context, 0, (yyvsp[-1].exp_node), NULL);
    }
#line 1781 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 33:
#line 272 "noname.y" /* yacc.c:1661  */
    {

      if ((yyvsp[-1].exp_list) == NULL) {
        (yyvsp[-1].exp_list) = new_exp_list(context);
      } 
      (yyval.exp_node) = new_call_node(context, (yyvsp[-3].id_v), (yyvsp[-1].exp_list));
    }
#line 1793 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 34:
#line 282 "noname.y" /* yacc.c:1661  */
    { (yyval.arg_list) = NULL; }
#line 1799 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 35:
#line 283 "noname.y" /* yacc.c:1661  */
    {  //fprintf(stderr, "\n[arglist processing]"); 
                              (yyval.arg_list) = new_arg_list(context, NULL, (yyvsp[0].arg)); }
#line 1806 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 36:
#line 285 "noname.y" /* yacc.c:1661  */
    {  //fprintf(stderr, "\n[arglist processing]"); 
                              (yyval.arg_list) = new_arg_list(context, (yyvsp[-2].arg_list), (yyvsp[0].arg)); }
#line 1813 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 37:
#line 290 "noname.y" /* yacc.c:1661  */
    {  //fprintf(stderr, "\n[ne_arg_list processing]"); 
                              (yyval.arg_list) = new_arg_list(context, NULL, (yyvsp[0].arg)); }
#line 1820 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 38:
#line 292 "noname.y" /* yacc.c:1661  */
    {  //fprintf(stderr, "\n[ne_arg_list processing]"); 
                              (yyval.arg_list) = new_arg_list(context, (yyvsp[-2].arg_list), (yyvsp[0].arg)); }
#line 1827 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 39:
#line 297 "noname.y" /* yacc.c:1661  */
    { (yyval.arg) = new_arg(context, (yyvsp[0].id_v), NULL); }
#line 1833 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 40:
#line 298 "noname.y" /* yacc.c:1661  */
    { (yyval.arg) = new_arg(context, (yyvsp[-2].id_v), (yyvsp[0].double_v)); }
#line 1839 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 41:
#line 299 "noname.y" /* yacc.c:1661  */
    { (yyval.arg) = new_arg(context, (yyvsp[-2].id_v), (yyvsp[0].long_v)); }
#line 1845 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 42:
#line 300 "noname.y" /* yacc.c:1661  */
    { (yyval.arg) = new_arg(context, (yyvsp[-2].id_v), (yyvsp[0].id_v)); }
#line 1851 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 43:
#line 304 "noname.y" /* yacc.c:1661  */
    { (yyval.exp_list) = NULL; }
#line 1857 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 44:
#line 305 "noname.y" /* yacc.c:1661  */
    { (yyval.exp_list) = new_exp_list(context, (yyvsp[-1].exp_node)); }
#line 1863 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 45:
#line 306 "noname.y" /* yacc.c:1661  */
    { (yyval.exp_list) = new_exp_list(context, (yyvsp[-2].exp_list), (yyvsp[-1].exp_node)); }
#line 1869 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 46:
#line 310 "noname.y" /* yacc.c:1661  */
    { (yyval.exp_list) = new_exp_list(context, (yyvsp[-1].exp_node)); }
#line 1875 "noname.tab.c" /* yacc.c:1661  */
    break;

  case 47:
#line 311 "noname.y" /* yacc.c:1661  */
    { (yyval.exp_list) = new_exp_list(context, (yyvsp[-2].exp_list), (yyvsp[-1].exp_node)); }
#line 1881 "noname.tab.c" /* yacc.c:1661  */
    break;


#line 1885 "noname.tab.c" /* yacc.c:1661  */
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
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
                  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 314 "noname.y" /* yacc.c:1906  */
