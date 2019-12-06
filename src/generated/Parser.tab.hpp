/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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
#line 11 "../../res/Grammar.y" /* yacc.c:1921  */

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

#line 180 "Parser.tab.hpp" /* yacc.c:1921  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_HPP_INCLUDED  */
