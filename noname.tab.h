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
#line 43 "noname.y" /* yacc.c:1915  */

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

#line 161 "noname.tab.h" /* yacc.c:1915  */
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
