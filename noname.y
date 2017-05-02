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
  noname::stmtlist_t* stmtlist;
  noname::explist_t* explist;
  noname::arglist_t* arglist;
  noname::arg_t* arg;
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
%token ELSE_TOK                  "else"
%token FALSE                 "false"
%token IF_TOK                    "if"
%token IN_TOK                    "in"
%token LET_TOK                   "let"
%token DEF_TOK                   "def"
%token LOOP_TOK                  "loop"
%token THEN_TOK                  "then"
%token WHILE                 "while"
%token BREAK_TOK                 "break"
%token CASE_TOK                  "case"
%token NEW_TOK                   "new"
%token NOT_TOK                   "not"
%token RETURN                "return"
%token TRUE                  "true"
%token NEWLINE               "newline"
%token NOTNEWLINE            "notnewline"
%token WHITESPACE            "whitespace"
%token LE_TOK                    "le"
%token ASSIGN                "assign"
%token NULLCH                "nullch"
%token BACKSLASH             "backslash"
%token STAR_TOK                  "star"
%token NOTSTAR               "notstar"
%token LEFTPAREN_TOK             "leftparen"
%token NOTLEFTPAREN_TOK          "notleftparen"
%token RIGHTPAREN            "rightparen"
%token NOTRIGHTPAREN         "notrightparen"
%token LINE_COMMENT          "line_comment"
%token START_COMMENT         "start_comment"
%token END_COMMENT           "end_comment"
%token QUOTES                "quotes"
%token ERROR_TOK                 "error"
%token '+'                    "+"
%token '-'                    "-"
%token '/'                    "/"
%token '*'                    "*"
%token '^'                    "^"

%token <id_v> IDENTIFIER                    "identifier"
%token <id_v> STR_CONST             "string_constant"
%token <double_v> DOUBLE_TOK            "double"
%token <long_v> LONG_TOK                "long"
%type  <ast_node> declaration       "declaration"
%type  <exp_node> assignment        "assignment"
%type  <exp_node> optional_ret_stmt "optional_ret_stmt"
%type  <exp_node> exp               "expression"
%type  <ast_node> function_def      "function_def"
%type  <stmtlist> stmtlist        "stmtlist"
%type  <stmtlist> ne_stmt_list     "ne_stmt_list"
// %type  <explist> explist          "explist"
// %type  <explist> ne_exp_list       "ne_exp_list"
%type  <explist> arg_exp_list      "arg_exp_list"
%type  <explist> ne_arg_exp_list   "ne_arg_exp_list"
%type  <arglist> arglist          "arglist"
%type  <arglist> ne_arg_list       "ne_arg_list"
%type  <arg> arg                    "arg"
%type  <ast_node> import            "import"
%type  <ast_node> stmt              "statement"

%left '-' '+'
%left '*' '/'
%right '^'        /* exponentiation */
%precedence NEG_TOK   /* negation--unary minus */

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

stmtlist:
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
      $$ = $1;
    }
  | exp STMT_SEP                  { 
      if (yydebug) {
        fprintf(stderr, "\n[stmt exp]: ");
      }
      $$ = new_top_level_exp_node($1);
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
    DEF_TOK IDENTIFIER {

        if (yydebug >= 1) {
          fprintf(stdout, "\n[############## processing function_def BEFORE arglist ##############]");
        }
        $<context>$ = new ASTContext(std::string($IDENTIFIER), context);
        context_stack.push($<context>$);
        context = $<context>$;
        
      }[function_context] '(' arglist ')' {
        if (yydebug >= 1) {
          fprintf(stdout, "\n[############## processing function_def BEFORE stmtlist ##############]");
        }
      } '{' stmtlist optional_ret_stmt '}' {
      // ASTContext newContext(context);

      if ($arglist == NULL) {
        $arglist = new_arg_list(context);
      } 

      if ($stmtlist == NULL) {
        $stmtlist = new_stmt_list(context);
      } 

      // $$ = new_function_def(*$<context>function_context, $IDENTIFIER, $arglist, $stmtlist);
      $$ = new_function_def(context, std::string($IDENTIFIER), $arglist, $stmtlist, $optional_ret_stmt);
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
  IDENTIFIER ASSIGN exp {
    // $$ = new AssignmentNode($1, $3);
    $$ = new AssignmentNode(context, std::string($IDENTIFIER), std::move((ExpNode*) $exp));
  }
  | LET_TOK IDENTIFIER ASSIGN exp {
    $$ = new DeclarationAssignmentNode(context, std::string($IDENTIFIER), std::move((ExpNode*) $exp));
  }
;

declaration:
  LET_TOK IDENTIFIER {
    $$ = new DeclarationNode(context, std::string($IDENTIFIER));
  }
;

exp:
  IDENTIFIER {
    $$ = new VarExpNode(context, std::string($IDENTIFIER));
  }
  | STR_CONST {
    $$ = new StringExpNode(context, std::string($STR_CONST));
  }
  | LONG_TOK {
    $$ = new NumberExpNode(context, $1);
  }
  | DOUBLE_TOK {
    $$ = new NumberExpNode(context, $1);
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
  | '-' exp  %prec NEG_TOK {
      $$ = new UnaryExpNode(context, '-', $2);
    }
  | exp '^' exp        {
      $$ = new BinaryExpNode(context, '^', $1, $3);
    }
  | '(' exp ')'        {
      $$ = new BinaryExpNode(context, 0, $2, NULL);
    }
  | IDENTIFIER '(' arg_exp_list ')'        {

      // fprintf(stderr, "\n[IDENTIFIER(arg_exp_list)]"); 

      if ($arg_exp_list == NULL) {
        // fprintf(stderr, "\n[$arg_exp_list is NULL]"); 
        $arg_exp_list = new_exp_list(context);
      } 
      // fprintf(stderr, "\n[new_call_node() %s]", $IDENTIFIER); 
      $$ = new_call_node(context, $IDENTIFIER, $arg_exp_list);
    }
  ;

arglist:
  %empty                   { $$ = NULL; } 
  | arg                    {  $$ = new_arg_list(context, $1); }
  | ne_arg_list ',' arg    {  $$ = new_arg_list(context, $1, $3); }
;

ne_arg_list:
  arg                      {  $$ = new_arg_list(context, $1); }
  | ne_arg_list ',' arg    {  $$ = new_arg_list(context, $1, $3); }
;

arg:
  IDENTIFIER                      { $$ = new_arg(context, $1, NULL); }
  | IDENTIFIER ASSIGN DOUBLE_TOK      { $$ = new_arg(context, $1, $3); }
  | IDENTIFIER ASSIGN LONG_TOK        { $$ = new_arg(context, $1, $3); }
  | IDENTIFIER ASSIGN STR_CONST   { $$ = new_arg(context, $1, $3); }
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

// explist:
//   %empty                     { $$ = NULL; }
//   | exp STMT_SEP             { $$ = new_exp_list(context, $1); }
//   | ne_exp_list exp STMT_SEP { $$ = new_exp_list(context, $1, $2); }
// ;

// ne_exp_list:
//   exp STMT_SEP               { $$ = new_exp_list(context, $1); }
//   | ne_exp_list exp STMT_SEP { $$ = new_exp_list(context, $1, $2); }
// ;

%%