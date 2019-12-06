/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 6 "../../res/Grammar.y" /* yacc.c:337  */

#include "Tree.h"
#include "Lexer.h"

#line 75 "Parser.tab.cpp" /* yacc.c:337  */
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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "Parser.tab.hpp".  */
#ifndef YY_YY_PARSER_TAB_HPP_INCLUDED
# define YY_YY_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    VAR = 258,
    IS = 259,
    TYPE = 260,
    IDENTIFIER = 261,
    INTEGER = 262,
    REAL = 263,
    BOOLEAN = 264,
    RECORD = 265,
    END = 266,
    ARRAY = 267,
    NEW_LINE = 268,
    WHILE = 269,
    LOOP = 270,
    FOR = 271,
    IN = 272,
    REVERSE = 273,
    IF = 274,
    THEN = 275,
    ELSE = 276,
    ROUTINE = 277,
    RETURN = 278,
    COLON = 279,
    OPEN_PAREN = 280,
    CLOSE_PAREN = 281,
    OPEN_BRACKET = 282,
    CLOSE_BRACKET = 283,
    DOT = 284,
    ELLIPSIS = 285,
    COMMA = 286,
    OP_ASSIGNMENT = 287,
    OP_AND = 288,
    OP_OR = 289,
    OP_XOR = 290,
    OP_EQUAL = 291,
    OP_NOT_EQUAL = 292,
    OP_LESS = 293,
    OP_GREATER = 294,
    OP_LESS_OR_EQUAL = 295,
    OP_GREATER_OR_EQUAL = 296,
    OP_MULTIPLY = 297,
    OP_DIVIDE = 298,
    OP_MODULE = 299,
    OP_PLUS = 300,
    OP_MINUS = 301,
    INTEGER_LITERAL = 302,
    REAL_LITERAL = 303,
    BOOLEAN_LITERAL = 304
  };
#endif
/* Tokens.  */
#define VAR 258
#define IS 259
#define TYPE 260
#define IDENTIFIER 261
#define INTEGER 262
#define REAL 263
#define BOOLEAN 264
#define RECORD 265
#define END 266
#define ARRAY 267
#define NEW_LINE 268
#define WHILE 269
#define LOOP 270
#define FOR 271
#define IN 272
#define REVERSE 273
#define IF 274
#define THEN 275
#define ELSE 276
#define ROUTINE 277
#define RETURN 278
#define COLON 279
#define OPEN_PAREN 280
#define CLOSE_PAREN 281
#define OPEN_BRACKET 282
#define CLOSE_BRACKET 283
#define DOT 284
#define ELLIPSIS 285
#define COMMA 286
#define OP_ASSIGNMENT 287
#define OP_AND 288
#define OP_OR 289
#define OP_XOR 290
#define OP_EQUAL 291
#define OP_NOT_EQUAL 292
#define OP_LESS 293
#define OP_GREATER 294
#define OP_LESS_OR_EQUAL 295
#define OP_GREATER_OR_EQUAL 296
#define OP_MULTIPLY 297
#define OP_DIVIDE 298
#define OP_MODULE 299
#define OP_PLUS 300
#define OP_MINUS 301
#define INTEGER_LITERAL 302
#define REAL_LITERAL 303
#define BOOLEAN_LITERAL 304

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 11 "../../res/Grammar.y" /* yacc.c:352  */

	char *stringValue;
	int intValue;
	float realValue;
	bool booleanValue;

	AstNode *astNode;
	Program *program;
	Declaration *declaration;
	VariableDeclaration *variableDeclaration;
	VariableDeclarationList *variableDeclarationList;
	ParameterDeclaration *parameterDeclaration;
	ParameterList *parameterList;
	TypeDefinition *typeDefinition;
	Statement *statement;
	ArgumentList *argumentList;
	Body *body;
	Range *range;
	Expression *expression;
	Primary *primary;
	ModifiablePrimary *modifiablePrimary;
	CallablePrimary *callablePrimary;

#line 240 "Parser.tab.cpp" /* yacc.c:352  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_HPP_INCLUDED  */



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
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
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
#  define YYSIZE_T unsigned
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

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
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
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   261

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  87
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  170

#define YYUNDEFTOK  2
#define YYMAXUTOK   304

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
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
      45,    46,    47,    48,    49
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    72,    72,    77,    78,    80,    82,    86,    88,    92,
      94,    96,   100,   102,   106,   110,   112,   114,   118,   120,
     122,   126,   128,   132,   136,   140,   144,   148,   150,   152,
     156,   159,   169,   178,   179,   183,   185,   189,   193,   195,
     199,   203,   205,   209,   211,   216,   217,   221,   223,   228,
     233,   234,   236,   238,   242,   244,   246,   248,   252,   254,
     256,   258,   260,   262,   264,   268,   270,   272,   274,   278,
     280,   282,   286,   288,   292,   294,   296,   298,   300,   302,
     304,   306,   308,   312,   314,   316,   318,   322
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "VAR", "IS", "TYPE", "IDENTIFIER",
  "INTEGER", "REAL", "BOOLEAN", "RECORD", "END", "ARRAY", "NEW_LINE",
  "WHILE", "LOOP", "FOR", "IN", "REVERSE", "IF", "THEN", "ELSE", "ROUTINE",
  "RETURN", "COLON", "OPEN_PAREN", "CLOSE_PAREN", "OPEN_BRACKET",
  "CLOSE_BRACKET", "DOT", "ELLIPSIS", "COMMA", "OP_ASSIGNMENT", "OP_AND",
  "OP_OR", "OP_XOR", "OP_EQUAL", "OP_NOT_EQUAL", "OP_LESS", "OP_GREATER",
  "OP_LESS_OR_EQUAL", "OP_GREATER_OR_EQUAL", "OP_MULTIPLY", "OP_DIVIDE",
  "OP_MODULE", "OP_PLUS", "OP_MINUS", "INTEGER_LITERAL", "REAL_LITERAL",
  "BOOLEAN_LITERAL", "$accept", "NewLine", "Program", "SimpleDeclaration",
  "VariableDeclaration", "VariableDeclarationList", "TypeDeclaration",
  "Type", "PrimitiveType", "UserType", "RecordType", "ArrayType",
  "Statement", "ExpressionStatement", "ReturnStatement", "ArgumentList",
  "NonEmptyArgumentList", "WhileLoop", "ForLoop", "Range", "IfStatement",
  "RoutineDeclaration", "Parameters", "NonEmptyParameters",
  "ParameterDeclaration", "Body", "Expression", "Relation", "Simple",
  "Factor", "Summand", "Primary", "ModifiablePrimary", "CallablePrimary", YY_NULLPTR
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304
};
# endif

#define YYPACT_NINF -114

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-114)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -114,    12,  -114,     2,     8,  -114,    26,  -114,  -114,  -114,
       5,  -114,    15,    20,    59,  -114,    24,   227,   227,    29,
      35,    24,    34,    50,  -114,  -114,  -114,    -6,  -114,   180,
      44,  -114,  -114,    62,  -114,  -114,  -114,  -114,  -114,     5,
      32,     7,  -114,  -114,  -114,  -114,  -114,    27,    66,    74,
    -114,    24,   102,    22,  -114,  -114,  -114,  -114,    24,    24,
      24,  -114,    24,    24,    24,    24,    24,    24,    24,    24,
      24,    24,    24,    24,   111,    24,    24,  -114,   115,    18,
      29,    84,  -114,  -114,  -114,  -114,  -114,   184,   184,   184,
     184,   184,   184,    44,    44,    44,  -114,  -114,   103,   101,
     219,  -114,    10,   197,    -6,  -114,  -114,   227,  -114,   110,
    -114,    24,  -114,  -114,   227,  -114,   117,   137,   102,   219,
    -114,  -114,    24,   136,    24,    24,  -114,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,    42,     5,  -114,  -114,   179,   139,
      91,   219,    24,  -114,   132,  -114,    -2,  -114,    -6,  -114,
     147,    24,   134,   216,    72,  -114,  -114,   142,  -114,    24,
    -114,  -114,  -114,   162,   219,   177,   192,  -114,  -114,  -114
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     1,     0,     0,     2,     0,     4,     5,     7,
       0,     6,     0,     0,     0,     8,     0,     0,     0,    45,
      83,     0,     0,     0,    74,    77,    80,     0,    54,    58,
      65,    69,    72,    81,    82,    17,    18,    19,    20,     0,
       0,     0,    15,    16,    22,    21,    14,     0,     0,    46,
      47,     0,     0,     0,    76,    79,    75,    78,     0,     0,
       0,    11,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,     0,     0,     0,     9,     0,     0,
       0,     0,    84,    73,    55,    56,    57,    59,    60,    61,
      62,    63,    64,    66,    67,    68,    70,    71,     0,    34,
      35,    12,     0,     0,     0,    49,    50,     0,    48,    85,
      87,     0,    23,    13,     0,    10,     0,     0,     0,    36,
      24,    43,     0,     0,     0,     0,    51,    52,    53,    25,
      26,    27,    28,    29,     0,     0,    50,    86,     0,     0,
       0,    32,     0,    31,     0,    50,     0,    50,     0,    44,
       0,     0,     0,     0,     0,    30,    37,     0,    50,     0,
      41,    50,    50,     0,    40,     0,     0,    38,    42,    39
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -114,    -1,  -114,   153,   -69,  -114,  -114,    -8,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,    11,
    -114,  -114,  -114,  -114,    79,   -82,   -15,   198,   178,   191,
      30,  -114,   -50,  -113
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   126,     1,   127,     9,   102,    10,    41,    42,    43,
      44,    45,   128,   129,   130,    98,    99,   131,   132,   152,
     133,    11,    48,    49,    50,   116,   153,    28,    29,    30,
      31,    32,    33,    34
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
       7,    27,    82,   135,    20,   101,    53,     5,    12,    15,
      46,    76,     2,     3,    13,     3,   151,     4,     5,    16,
       5,   112,   106,    21,    18,     5,    61,    58,    59,    60,
      20,   135,    14,   113,     6,    47,    81,   135,    74,    17,
      77,   135,   107,    22,    23,    24,    25,    26,    83,    21,
     135,    78,   135,   135,   144,    58,    59,    60,   100,    75,
     103,   104,    51,   150,    52,   154,   134,    73,   137,    22,
      23,    24,    25,    26,   142,     3,   163,     4,    20,   165,
     166,    54,    55,   160,    19,     5,   122,    73,   123,    71,
      72,   124,    79,   161,   134,   125,   119,    56,    57,   117,
     134,    96,    97,   115,   134,    80,   120,   138,    20,   140,
     141,   147,   109,   134,     3,   134,   134,    58,    59,    60,
       3,   105,     4,    20,    58,    59,    60,   148,   121,   110,
       5,   122,   111,   123,   143,     3,   124,     4,    20,   118,
     125,   136,   139,   149,   164,     5,   122,   155,   123,   158,
       3,   124,     4,    20,     8,   125,   146,   162,   156,   108,
       5,   122,   157,   123,     0,     3,   124,     4,    20,     0,
     125,     0,     0,   167,     0,     5,   122,     0,   123,     0,
       3,   124,     4,    20,     0,   125,     0,     0,   168,     0,
       5,   122,     0,   123,   145,     3,   124,     4,    20,     0,
     125,     0,     0,   169,     0,     5,   122,     0,   123,     0,
       0,   124,    58,    59,    60,   125,    62,    63,    64,    65,
      66,    67,    68,    69,    70,   114,    68,    69,    70,     0,
      58,    59,    60,    35,    36,    37,    38,    39,     0,    40,
      87,    88,    89,    90,    91,    92,   159,     0,     0,    58,
      59,    60,    58,    59,    60,     0,    84,    85,    86,    93,
      94,    95
};

static const yytype_int16 yycheck[] =
{
       1,    16,    52,   116,     6,    74,    21,    13,     6,    10,
      18,     4,     0,     3,     6,     3,    18,     5,    13,     4,
      13,    11,     4,    25,     4,    13,    27,    33,    34,    35,
       6,   144,     6,   102,    22,     6,    51,   150,    39,    24,
      41,   154,    24,    45,    46,    47,    48,    49,    26,    25,
     163,    24,   165,   166,   136,    33,    34,    35,    73,    27,
      75,    76,    27,   145,    29,   147,   116,    25,   118,    45,
      46,    47,    48,    49,    32,     3,   158,     5,     6,   161,
     162,    47,    48,    11,    25,    13,    14,    25,    16,    45,
      46,    19,    26,    21,   144,    23,   111,    47,    48,   107,
     150,    71,    72,   104,   154,    31,   114,   122,     6,   124,
     125,    20,    28,   163,     3,   165,   166,    33,    34,    35,
       3,     6,     5,     6,    33,    34,    35,   142,    11,    26,
      13,    14,    31,    16,   135,     3,    19,     5,     6,    29,
      23,     4,     6,    11,   159,    13,    14,   148,    16,    15,
       3,    19,     5,     6,     1,    23,    17,    15,    11,    80,
      13,    14,   151,    16,    -1,     3,    19,     5,     6,    -1,
      23,    -1,    -1,    11,    -1,    13,    14,    -1,    16,    -1,
       3,    19,     5,     6,    -1,    23,    -1,    -1,    11,    -1,
      13,    14,    -1,    16,    15,     3,    19,     5,     6,    -1,
      23,    -1,    -1,    11,    -1,    13,    14,    -1,    16,    -1,
      -1,    19,    33,    34,    35,    23,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    28,    42,    43,    44,    -1,
      33,    34,    35,     6,     7,     8,     9,    10,    -1,    12,
      62,    63,    64,    65,    66,    67,    30,    -1,    -1,    33,
      34,    35,    33,    34,    35,    -1,    58,    59,    60,    68,
      69,    70
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    52,     0,     3,     5,    13,    22,    51,    53,    54,
      56,    71,     6,     6,     6,    51,     4,    24,     4,    25,
       6,    25,    45,    46,    47,    48,    49,    76,    77,    78,
      79,    80,    81,    82,    83,     6,     7,     8,     9,    10,
      12,    57,    58,    59,    60,    61,    57,     6,    72,    73,
      74,    27,    29,    76,    47,    48,    47,    48,    33,    34,
      35,    51,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    25,    51,    27,     4,    51,    24,    26,
      31,    76,    82,    26,    77,    77,    77,    78,    78,    78,
      78,    78,    78,    79,    79,    79,    80,    80,    65,    66,
      76,    54,    55,    76,    76,     6,     4,    24,    74,    28,
      26,    31,    11,    54,    28,    51,    75,    57,    29,    76,
      57,    11,    14,    16,    19,    23,    51,    53,    62,    63,
      64,    67,    68,    70,    82,    83,     4,    82,    76,     6,
      76,    76,    32,    51,    75,    15,    17,    20,    76,    11,
      75,    18,    69,    76,    75,    51,    11,    69,    15,    30,
      11,    21,    15,    75,    76,    75,    75,    11,    11,    11
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    51,    52,    52,    52,    52,    53,    53,    54,
      54,    54,    55,    55,    56,    57,    57,    57,    58,    58,
      58,    59,    59,    60,    61,    62,    62,    62,    62,    62,
      63,    63,    64,    65,    65,    66,    66,    67,    68,    68,
      69,    70,    70,    71,    71,    72,    72,    73,    73,    74,
      75,    75,    75,    75,    76,    76,    76,    76,    77,    77,
      77,    77,    77,    77,    77,    78,    78,    78,    78,    79,
      79,    79,    80,    80,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    82,    82,    82,    82,    83
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     1,     2,     5,
       7,     5,     1,     2,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     5,     1,     1,     1,     1,     1,
       4,     2,     2,     0,     1,     1,     3,     5,     7,     8,
       3,     5,     7,     8,    10,     0,     1,     1,     3,     3,
       0,     2,     2,     2,     1,     3,     3,     3,     1,     3,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     1,
       3,     3,     1,     3,     1,     2,     2,     1,     2,     2,
       1,     1,     1,     1,     3,     4,     6,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
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
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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
            else
              goto append;

          append:
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

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
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
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
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
    default: /* Avoid compiler warnings. */
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
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
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

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
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
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 77 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.program) = new Program();			}
#line 1473 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 4:
#line 79 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.program) = (yyvsp[-1].program);				}
#line 1479 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 5:
#line 81 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.program) = (yyvsp[-1].program)->addSimpleDeclaration((yyvsp[0].declaration));	}
#line 1485 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 6:
#line 83 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.program) = (yyvsp[-1].program)->addRoutineDeclaration((yyvsp[0].declaration));	}
#line 1491 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 7:
#line 87 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.declaration) = (yyvsp[0].variableDeclaration);				}
#line 1497 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 8:
#line 89 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.declaration) = (yyvsp[-1].declaration);				}
#line 1503 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 9:
#line 93 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.variableDeclaration) = new VariableDeclaration((yyvsp[-3].stringValue),(yyvsp[-1].typeDefinition),nullptr); 	}
#line 1509 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 10:
#line 95 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.variableDeclaration) = new VariableDeclaration((yyvsp[-5].stringValue),(yyvsp[-3].typeDefinition),(yyvsp[-1].expression)); 	}
#line 1515 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 11:
#line 97 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.variableDeclaration) = new VariableDeclaration((yyvsp[-3].stringValue),nullptr,(yyvsp[-1].expression)); 	}
#line 1521 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 12:
#line 101 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.variableDeclarationList) = new VariableDeclarationList((yyvsp[0].variableDeclaration));		}
#line 1527 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 13:
#line 103 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.variableDeclarationList) = (yyvsp[-1].variableDeclarationList)->addVariableDeclaration((yyvsp[0].variableDeclaration));		}
#line 1533 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 14:
#line 107 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.declaration) = new TypeDeclaration((yyvsp[-2].stringValue),(yyvsp[0].typeDefinition));		}
#line 1539 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 15:
#line 111 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = (yyvsp[0].typeDefinition);					}
#line 1545 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 16:
#line 113 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = (yyvsp[0].typeDefinition);					}
#line 1551 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 17:
#line 115 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = new NamedTypeDefinition((yyvsp[0].stringValue));		}
#line 1557 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 18:
#line 119 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = new NamedTypeDefinition(INTEGER);	}
#line 1563 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 19:
#line 121 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = new NamedTypeDefinition(REAL);		}
#line 1569 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 20:
#line 123 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = new NamedTypeDefinition(BOOLEAN);	}
#line 1575 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 21:
#line 127 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = (yyvsp[0].typeDefinition);					}
#line 1581 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 22:
#line 129 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = (yyvsp[0].typeDefinition);					}
#line 1587 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 23:
#line 133 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = new RecordTypeDefinition((yyvsp[-1].variableDeclarationList));		}
#line 1593 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 24:
#line 137 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.typeDefinition) = new ArrayTypeDefinition((yyvsp[-2].expression),(yyvsp[0].typeDefinition));		}
#line 1599 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 25:
#line 141 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.astNode) = (yyvsp[0].statement);	}
#line 1605 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 26:
#line 145 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.astNode) = (yyvsp[0].statement);      }
#line 1611 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 27:
#line 149 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.astNode) = (yyvsp[0].statement); 	}
#line 1617 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 28:
#line 151 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.astNode) = (yyvsp[0].statement); 	}
#line 1623 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 29:
#line 153 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.astNode) = (yyvsp[0].statement); 	}
#line 1629 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 30:
#line 157 "../../res/Grammar.y" /* yacc.c:1652  */
    { auto binExpr = new BinaryExpression(OP_ASSIGNMENT, (yyvsp[-3].modifiablePrimary), (yyvsp[-1].expression));
	  (yyval.statement) = new ExpressionStatement(binExpr); }
#line 1636 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 31:
#line 160 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.statement) = new ExpressionStatement((yyvsp[-1].callablePrimary));	}
#line 1642 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 32:
#line 170 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.statement) = new ReturnStatement((yyvsp[0].expression));		}
#line 1648 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 33:
#line 178 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.argumentList) = new ArgumentList(); 		}
#line 1654 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 34:
#line 180 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.argumentList) = (yyvsp[0].argumentList);				}
#line 1660 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 35:
#line 184 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.argumentList) = new ArgumentList((yyvsp[0].expression)); 		}
#line 1666 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 36:
#line 186 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.argumentList) = (yyvsp[-2].argumentList)->addArgument((yyvsp[0].expression)); 		}
#line 1672 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 37:
#line 190 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.statement) = new WhileLoopStatement((yyvsp[-3].expression),(yyvsp[-1].body));	 }
#line 1678 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 38:
#line 194 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.statement) = new ForLoopStatement((yyvsp[-5].stringValue),(yyvsp[-3].range),false,(yyvsp[-1].body));	}
#line 1684 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 39:
#line 196 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.statement) = new ForLoopStatement((yyvsp[-6].stringValue),(yyvsp[-3].range),true, (yyvsp[-1].body)); 	}
#line 1690 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 40:
#line 200 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.range) = new Range((yyvsp[-2].expression),(yyvsp[0].expression)); 		}
#line 1696 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 41:
#line 204 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.statement) = new IfStatement((yyvsp[-3].expression),(yyvsp[-1].body),nullptr);	}
#line 1702 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 42:
#line 206 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.statement) = new IfStatement((yyvsp[-5].expression),(yyvsp[-3].body),(yyvsp[-1].body));	}
#line 1708 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 43:
#line 210 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.declaration) = new RoutineDeclaration((yyvsp[-6].stringValue),(yyvsp[-4].parameterList), nullptr, (yyvsp[-1].body));	}
#line 1714 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 44:
#line 212 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.declaration) = new RoutineDeclaration((yyvsp[-8].stringValue), (yyvsp[-6].parameterList), (yyvsp[-3].typeDefinition), (yyvsp[-1].body));}
#line 1720 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 45:
#line 216 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.parameterList) = new ParameterList(); 		}
#line 1726 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 46:
#line 218 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.parameterList) = (yyvsp[0].parameterList);				}
#line 1732 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 47:
#line 222 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.parameterList) = new ParameterList((yyvsp[0].parameterDeclaration)); 		}
#line 1738 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 48:
#line 224 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.parameterList) = (yyvsp[-2].parameterList)->addParameter((yyvsp[0].parameterDeclaration));		}
#line 1744 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 49:
#line 229 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.parameterDeclaration) = new ParameterDeclaration((yyvsp[-2].stringValue),(yyvsp[0].stringValue));	}
#line 1750 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 50:
#line 233 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.body) = new Body();			}
#line 1756 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 51:
#line 235 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.body) = (yyvsp[-1].body); 				}
#line 1762 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 52:
#line 237 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.body) = (yyvsp[-1].body)->addNode((yyvsp[0].declaration)); 		}
#line 1768 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 53:
#line 239 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.body) = (yyvsp[-1].body)->addNode((yyvsp[0].astNode)); 		}
#line 1774 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 54:
#line 243 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = (yyvsp[0].expression);					}
#line 1780 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 55:
#line 245 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_AND,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1786 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 56:
#line 247 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_OR,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1792 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 57:
#line 249 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_XOR,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1798 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 58:
#line 253 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = (yyvsp[0].expression);						}
#line 1804 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 59:
#line 255 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_EQUAL,(yyvsp[-2].expression),(yyvsp[0].expression));		}
#line 1810 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 60:
#line 257 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_NOT_EQUAL,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1816 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 61:
#line 259 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_LESS,(yyvsp[-2].expression),(yyvsp[0].expression));		}
#line 1822 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 62:
#line 261 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_GREATER,(yyvsp[-2].expression),(yyvsp[0].expression));		}
#line 1828 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 63:
#line 263 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_LESS_OR_EQUAL,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1834 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 64:
#line 265 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_GREATER_OR_EQUAL,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1840 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 65:
#line 269 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = (yyvsp[0].expression);					}
#line 1846 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 66:
#line 271 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_MULTIPLY,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1852 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 67:
#line 273 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_DIVIDE,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1858 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 68:
#line 275 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_MODULE,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1864 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 69:
#line 279 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = (yyvsp[0].expression);					}
#line 1870 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 70:
#line 281 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_PLUS,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1876 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 71:
#line 283 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = new BinaryExpression(OP_MINUS,(yyvsp[-2].expression),(yyvsp[0].expression));	}
#line 1882 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 72:
#line 287 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = (yyvsp[0].primary);				}
#line 1888 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 73:
#line 289 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.expression) = (yyvsp[-1].expression); 				}
#line 1894 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 74:
#line 293 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = new ConstPrimary((yyvsp[0].intValue));		}
#line 1900 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 75:
#line 295 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = new ConstPrimary(0-((yyvsp[0].intValue))); 	}
#line 1906 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 76:
#line 297 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = new ConstPrimary(0+((yyvsp[0].intValue)));	}
#line 1912 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 77:
#line 299 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = new ConstPrimary((yyvsp[0].realValue));		}
#line 1918 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 78:
#line 301 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = new ConstPrimary(0.0f-((yyvsp[0].realValue))); 	}
#line 1924 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 79:
#line 303 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = new ConstPrimary(0.0f+((yyvsp[0].realValue)));	}
#line 1930 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 80:
#line 305 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = new ConstPrimary((yyvsp[0].booleanValue));		}
#line 1936 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 81:
#line 307 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = (yyvsp[0].modifiablePrimary);				}
#line 1942 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 82:
#line 309 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.primary) = (yyvsp[0].callablePrimary);				}
#line 1948 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 83:
#line 313 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.modifiablePrimary) = new ModifiablePrimary((yyvsp[0].stringValue));	}
#line 1954 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 84:
#line 315 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.modifiablePrimary) = new ModifiablePrimary((yyvsp[-2].stringValue),(yyvsp[0].modifiablePrimary));	}
#line 1960 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 85:
#line 317 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.modifiablePrimary) = new ModifiablePrimary((yyvsp[-3].stringValue),(yyvsp[-1].expression));	}
#line 1966 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 86:
#line 319 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.modifiablePrimary) = new ModifiablePrimary((yyvsp[-5].stringValue),(yyvsp[-3].expression),(yyvsp[0].modifiablePrimary));	}
#line 1972 "Parser.tab.cpp" /* yacc.c:1652  */
    break;

  case 87:
#line 323 "../../res/Grammar.y" /* yacc.c:1652  */
    { (yyval.callablePrimary) = new CallablePrimary((yyvsp[-3].modifiablePrimary),(yyvsp[-1].argumentList));	}
#line 1978 "Parser.tab.cpp" /* yacc.c:1652  */
    break;


#line 1982 "Parser.tab.cpp" /* yacc.c:1652  */
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
                      yytoken, &yylval);
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


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
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
#line 325 "../../res/Grammar.y" /* yacc.c:1918  */
