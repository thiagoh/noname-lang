/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
    IMPORT = 259,
    STMT_SEP = 260,
    LETTER = 261,
    DIGIT = 262,
    DIGITS = 263,
    DARROW = 264,
    ELSE = 265,
    FALSE = 266,
    IF = 267,
    IN = 268,
    LET = 269,
    DEF = 270,
    LOOP = 271,
    THEN = 272,
    WHILE = 273,
    BREAK = 274,
    CASE = 275,
    NEW = 276,
    NOT = 277,
    RETURN = 278,
    TRUE = 279,
    NEWLINE = 280,
    NOTNEWLINE = 281,
    WHITESPACE = 282,
    LE = 283,
    ASSIGN = 284,
    NULLCH = 285,
    BACKSLASH = 286,
    STAR = 287,
    NOTSTAR = 288,
    LEFTPAREN = 289,
    NOTLEFTPAREN = 290,
    RIGHTPAREN = 291,
    NOTRIGHTPAREN = 292,
    LINE_COMMENT = 293,
    START_COMMENT = 294,
    END_COMMENT = 295,
    QUOTES = 296,
    ERROR = 297,
    ID = 298,
    STR_CONST = 299,
    DOUBLE = 300,
    LONG = 301,
    NEG = 316
  };
#endif
/* Tokens.  */
#define LINE_BREAK 258
#define IMPORT 259
#define STMT_SEP 260
#define LETTER 261
#define DIGIT 262
#define DIGITS 263
#define DARROW 264
#define ELSE 265
#define FALSE 266
#define IF 267
#define IN 268
#define LET 269
#define DEF 270
#define LOOP 271
#define THEN 272
#define WHILE 273
#define BREAK 274
#define CASE 275
#define NEW 276
#define NOT 277
#define RETURN 278
#define TRUE 279
#define NEWLINE 280
#define NOTNEWLINE 281
#define WHITESPACE 282
#define LE 283
#define ASSIGN 284
#define NULLCH 285
#define BACKSLASH 286
#define STAR 287
#define NOTSTAR 288
#define LEFTPAREN 289
#define NOTLEFTPAREN 290
#define RIGHTPAREN 291
#define NOTRIGHTPAREN 292
#define LINE_COMMENT 293
#define START_COMMENT 294
#define END_COMMENT 295
#define QUOTES 296
#define ERROR 297
#define ID 298
#define STR_CONST 299
#define DOUBLE 300
#define LONG 301
#define NEG 316

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 48 "noname.y" /* yacc.c:1915  */

  char* id_v;
  double double_v;
  long long_v;

  noname::ASTContext* context;
  noname::ASTNode* ast_node;
  noname::ExpNode* exp_node;
  noname::stmtlist* stmt_list;
  noname::explist* exp_list;
  noname::arglist* arg_list;
  noname::arg* arg;
  char* error_msg;

#line 163 "noname.tab.h" /* yacc.c:1915  */
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
