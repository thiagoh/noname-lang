%{
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
#include "noname-utils.h"
#include "noname-types.h"

using namespace llvm;
using namespace noname;

//# define YY_SYMBOL_PRINT(Title, Type, Value, Location) 

extern int yylex(void);
extern int yydebug;
extern void yyerror(const char *error_msg);

namespace noname {

  extern ASTContext* context;
  extern std::stack<ASTContext*> context_stack;

  extern int noname_read(char *buf, int *result, int max_size);
  extern void write_cursor();
  extern void division_by_zero(YYLTYPE &yylloc);
  extern void eval(ASTNode* ast_node);
}

%}

//////////////////////////////////////////////////
///////////* Bison declarations.  *///////////////
//////////////////////////////////////////////////

%union {
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
};

%{

  template<class Ret, class In>
  inline Ret _cast(void* v) {
    return (Ret) *(In*)v;
  }

%}

%token LINE_BREAK            "line_break"             
%token IMPORT                "#import"
%token STMT_SEP              "stmt_sep"           
%token LETTER                "letter"         
%token DIGIT                 "digit"         
%token DIGITS                "digits"         
%token DARROW                "darrow"
%token ELSE                  "else"
%token FALSE                 "false"
%token IF                    "if"
%token IN                    "in"
%token LET                   "let"
%token DEF                   "def"
%token LOOP                  "loop"
%token THEN                  "then"
%token WHILE                 "while"
%token BREAK                 "break"
%token CASE                  "case"
%token NEW                   "new"
%token NOT                   "not"
%token RETURN                "return"
%token TRUE                  "true"
%token NEWLINE               "newline"
%token NOTNEWLINE            "notnewline"
%token WHITESPACE            "whitespace"
%token LE                    "le"
%token ASSIGN                "assign"
%token NULLCH                "nullch"
%token BACKSLASH             "backslash"
%token STAR                  "star"
%token NOTSTAR               "notstar"
%token LEFTPAREN             "leftparen"
%token NOTLEFTPAREN          "notleftparen"
%token RIGHTPAREN            "rightparen"
%token NOTRIGHTPAREN         "notrightparen"
%token LINE_COMMENT          "line_comment"
%token START_COMMENT         "start_comment"
%token END_COMMENT           "end_comment"
%token QUOTES                "quotes"
%token ERROR                 "error"
%token '+'                    "+"
%token '-'                    "-"
%token '/'                    "/"
%token '*'                    "*"
%token '^'                    "^"

%token <id_v> ID                    "identifier"
%token <id_v> STR_CONST             "string_constant"
%token <double_v> DOUBLE            "double"
%token <long_v> LONG                "long"
%type  <ast_node> declaration       "declaration"
%type  <exp_node> assignment        "assignment"
%type  <exp_node> optional_ret_stmt "optional_ret_stmt"
%type  <exp_node> exp               "expression"
%type  <ast_node> function_def      "function_def"
%type  <stmt_list> stmt_list        "stmt_list"
%type  <stmt_list> ne_stmt_list     "ne_stmt_list"
// %type  <exp_list> exp_list          "exp_list"
// %type  <exp_list> ne_exp_list       "ne_exp_list"
%type  <exp_list> arg_exp_list      "arg_exp_list"
%type  <exp_list> ne_arg_exp_list   "ne_arg_exp_list"
%type  <arg_list> arg_list          "arg_list"
%type  <arg_list> ne_arg_list       "ne_arg_list"
%type  <arg> arg                    "arg"
%type  <ast_node> import            "import"
%type  <ast_node> stmt              "statement"

%left '-' '+'
%left '*' '/'
%right '^'        /* exponentiation */
%precedence NEG   /* negation--unary minus */

%start prog

%% 

//////////////////////////////////////////////////
///////////* The grammar follows. *///////////////
//////////////////////////////////////////////////

prog:
  %empty {
    write_cursor();
  }
  | prog stmt {
      eval($2);
      write_cursor();
    }
  | error STMT_SEP { 
    yyerrok; 
    fprintf(stderr, "Error at %d:%d", @1.first_column, @1.last_column); 
    write_cursor();
  }
;

stmt_list:
  %empty                   { $$ = NULL; }
  | stmt                   { $$ = new_stmt_list(context, $1); }
  | ne_stmt_list stmt      { $$ = new_stmt_list(context, $1, $2); }
;

ne_stmt_list:
  stmt                     { $$ = new_stmt_list(context, $1); }
  | ne_stmt_list stmt      { $$ = new_stmt_list(context, $1, $2); }
;

stmt:
  declaration STMT_SEP            { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt - declaration]: ");
      }
      $$ = $1;
    }
  | assignment STMT_SEP           { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt - assignment]: ");
      }
      $$ = $1;
    }
  | import STMT_SEP           { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt - import]: ");
      }
      $$ = $1;
    }
  | function_def optional_stmt_sep { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt - function_def]: ");
      }
      // $$ = $1;
    }
  | exp STMT_SEP                  { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt exp]: ");
      }
      $$ = $1;
    }
;

optional_stmt_sep:
  %empty
  | STMT_SEP
;
  // 
  // Handling multi level scope/context
  // gnu.org/software/bison/manual/html_node/Using-Mid_002dRule-Actions.html#Using-Mid_002dRule-Actions
  // 

function_def:
    DEF ID {

        if (yydebug >= 1) {
          fprintf(stdout, "\n[############## processing function_def BEFORE arg_list ##############]");
        }
        $<context>$ = new ASTContext(std::string($ID), context);
        context_stack.push($<context>$);
        context = $<context>$;
        
      }[function_context] '(' arg_list ')' {
        if (yydebug >= 1) {
          fprintf(stdout, "\n[############## processing function_def BEFORE stmt_list ##############]");
        }
      } '{' stmt_list optional_ret_stmt '}' {
      // ASTContext newContext(context);

      if ($arg_list == NULL) {
        $arg_list = new_arg_list(context);
      } 

      if ($stmt_list == NULL) {
        $stmt_list = new_stmt_list(context);
      } 

      // $$ = new_function_def(*$<context>function_context, $ID, $arg_list, $stmt_list);
      $$ = new_function_def(context, std::string($ID), $arg_list, $stmt_list, $optional_ret_stmt);
      context_stack.pop();
      context = context_stack.top();
    }
;

optional_ret_stmt:
  %empty                    { $$ = NULL; }
  | RETURN exp STMT_SEP     { $$ = $exp; }
;

import:
  IMPORT STR_CONST          { $$ = new_import(context, std::string($STR_CONST)); }
;

assignment:
  ID ASSIGN exp {
    // $$ = new AssignmentNode($1, $3);
    $$ = new AssignmentNode(context, std::string($ID), std::move((ExpNode*) $exp));
  }
  | LET ID ASSIGN exp {
    $$ = new DeclarationAssignmentNode(context, std::string($ID), std::move((ExpNode*) $exp));
  }
;

declaration:
  LET ID {
    $$ = new DeclarationNode(context, std::string($ID));
  }
;

exp:
  ID {
    $$ = new VarNode(context, std::string($ID));
  }
  | STR_CONST {
    $$ = new StringNode(context, std::string($STR_CONST));
  }
  | LONG {
    $$ = new NumberNode(context, $1);
  }
  | DOUBLE {
    $$ = new NumberNode(context, $1);
  }
  | exp '+' exp        {
      $$ = new BinaryExpNode(context, '+', $1, $3);
    }
  | exp '-' exp        {
      $$ = new BinaryExpNode(context, '-', $1, $3);
    }
  | exp '*' exp        {
      $$ = new BinaryExpNode(context, '*', $1, $3);
    }
  | exp '/' exp {
      $$ = new BinaryExpNode(context, '/', $1, $3);
    }
  | '-' exp  %prec NEG {
      $$ = new UnaryExpNode(context, '-', $2);
    }
  | exp '^' exp        {
      $$ = new BinaryExpNode(context, '^', $1, $3);
    }
  | '(' exp ')'        {
      $$ = new BinaryExpNode(context, 0, $2, NULL);
    }
  | ID '(' arg_exp_list ')'        {

      // fprintf(stderr, "\n[ID(arg_exp_list)]"); 

      if ($arg_exp_list == NULL) {
        // fprintf(stderr, "\n[$arg_exp_list is NULL]"); 
        $arg_exp_list = new_exp_list(context);
      } 
      // fprintf(stderr, "\n[new_call_node() %s]", $ID); 
      $$ = new_call_node(context, $ID, $arg_exp_list);
    }
  ;

arg_list:
  %empty                   { $$ = NULL; } 
  | arg                    {  $$ = new_arg_list(context, $1); }
  | ne_arg_list ',' arg    {  $$ = new_arg_list(context, $1, $3); }
;

ne_arg_list:
  arg                      {  $$ = new_arg_list(context, $1); }
  | ne_arg_list ',' arg    {  $$ = new_arg_list(context, $1, $3); }
;

arg:
  ID                      { $$ = new_arg(context, $1, NULL); }
  | ID ASSIGN DOUBLE      { $$ = new_arg(context, $1, $3); }
  | ID ASSIGN LONG        { $$ = new_arg(context, $1, $3); }
  | ID ASSIGN STR_CONST   { $$ = new_arg(context, $1, $3); }
;

arg_exp_list:
  %empty                         { $$ = NULL; }
  | exp                          { $$ = new_exp_list(context, $1); }
  | ne_arg_exp_list ',' exp      { $$ = new_exp_list(context, $1, $3); }
;

ne_arg_exp_list:
  exp                             { $$ = new_exp_list(context, $1); }
  | ne_arg_exp_list ',' exp       { $$ = new_exp_list(context, $1, $3); }
;

// exp_list:
//   %empty                     { $$ = NULL; }
//   | exp STMT_SEP             { $$ = new_exp_list(context, $1); }
//   | ne_exp_list exp STMT_SEP { $$ = new_exp_list(context, $1, $2); }
// ;

// ne_exp_list:
//   exp STMT_SEP               { $$ = new_exp_list(context, $1); }
//   | ne_exp_list exp STMT_SEP { $$ = new_exp_list(context, $1, $2); }
// ;

%%