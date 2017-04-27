// #define NDEBUG
// #include "assert.h"
#include "lexer-utilities.h"
#include "noname-utils.h"
#include "noname-parse.h"
#include "noname-types.h"
#include <map>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace llvm;
using namespace noname;

std::map<int, std::string> map;
ASTContext *context;
std::stack<ASTContext *> context_stack;
extern YYSTYPE yylval;
extern int noname_yylex(void);
extern int yyparse();
void yyerror(char const *s);

//
//  The lexer keeps this global variable up to date with the line number
//  of the current line read from the input.
//
int curr_lineno = 1;
char *curr_filename = "<stdin>";  // this name is arbitrary
// FILE *fin; // This is the file pointer from which the lexer reads its input.

int yylex(void) {
  int token = noname_yylex();

  if (yydebug > 1) {
    if (token == LONG) {
      fprintf(stdout, "\n#TOKEN %d[%s] yytext -> %ld\n", token, map[token].c_str(), yylval.long_v);
    } else if (token == DOUBLE) {
      fprintf(stdout, "\n#TOKEN %d[%s] yytext -> %lf\n", token, map[token].c_str(), yylval.double_v);
    } else if (token == ID) {
      fprintf(stdout, "\n#TOKEN %d[%s] yytext -> %s\n", token, map[token].c_str(), yylval.id_v);
    } else {
      fprintf(stdout, "\n#TOKEN %d[%s] yytext -> %c\n", token, map[token].c_str(), (char)token);
    }
  }

  return token;
}

void division_by_zero(YYLTYPE &yylloc) {
  fprintf(stdout, "SEVERE ERROR %d:%d - %d:%d. Division by zero", yylloc.first_line, yylloc.first_column,
          yylloc.last_line, yylloc.last_column);
}

void yyerror(char const *s) { fprintf(stdout, "\nERROR: %s\n", s); }

void eval(ASTNode *node) {
  if (!node) {
    return;
  }

  if (yydebug >= 1) {
    fprintf(stdout, "\n[### eval: %d]\n", node->getType());
  }

  if (yydebug >= 2) {
    fprintf(stdout, "\n[is_of_type<ExpNode>(*node) -> %s]\n", is_of_type<ExpNode>(*node) ? "true" : "false");
  }

  if (is_of_type<AssignmentNode>(*node)) {
    AssignmentNode *assignment = (AssignmentNode *)node;
    NodeValue *return_value = (NodeValue *)assignment->eval();

    if (return_value) {
      print_node_value(stderr, *return_value);
    }

  } else if (is_of_type<CallExprNode>(*node)) {
    CallExprNode *callExp = (CallExprNode *)node;

    FunctionDefNode *functionNode = context->getFunction(callExp->getCallee());

    if (functionNode) {
      if (yydebug >= 2) {
        fprintf(stdout, "\nThe called function was: '%s'\n", functionNode->getName().c_str());
      }
      NodeValue *return_value = (NodeValue *)callExp->eval();

      if (return_value) {
        print_node_value(stderr, *return_value);
      }

    } else {
      if (yydebug >= 2) {
        fprintf(stdout, "\nThe called function was: '%s' BUT it wan not found on the context\n",
                callExp->getCallee().c_str());
      }
    }

  } else if (is_of_type<ExpNode>(*node)) {
    ExpNode *exp = (ExpNode *)node;

    NodeValue *return_value = (NodeValue *)exp->eval();
    if (return_value) {
      print_node_value(stderr, *return_value);
    }
  }
}

// void assert_equals(int i1, int i2) {
//   fprintf(stdout, "v1 '%d' / v2 '%d' %s \n", i1, i2, i1 == i2 ? "equal" : "not equal");
//   assert(i1 == i2);
// }

void write_cursor() {
  // writes the cursor so the user can input code
  fprintf(stdout, "\n>");
}

int main(int argc, char **argv) {
  int token;

  context = new ASTContext("root");
  context_stack.push(context);

  // assert_equals(cast<int>(17.3), (int)17.3);
  // assert_equals(cast<double>(333), (double)333);
  // assert_equals(cast<float>(1000.0), (float)1000.0);
  // exit(0);

  map[258] = "LINE_BREAK";
  map[259] = "STMT_SEP";
  map[260] = "LETTER";
  map[261] = "DIGIT";
  map[262] = "DIGITS";
  map[263] = "DARROW";
  map[264] = "ELSE";
  map[265] = "FALSE";
  map[266] = "IF";
  map[267] = "IN";
  map[268] = "LET";
  map[269] = "DEF";
  map[270] = "LOOP";
  map[271] = "THEN";
  map[272] = "WHILE";
  map[273] = "BREAK";
  map[274] = "CASE";
  map[275] = "NEW";
  map[276] = "NOT";
  map[277] = "RETURN";
  map[278] = "TRUE";
  map[279] = "NEWLINE";
  map[280] = "NOTNEWLINE";
  map[281] = "WHITESPACE";
  map[282] = "LE";
  map[283] = "ASSIGN";
  map[284] = "NULLCH";
  map[285] = "BACKSLASH";
  map[286] = "STAR";
  map[287] = "NOTSTAR";
  map[288] = "LEFTPAREN";
  map[289] = "NOTLEFTPAREN";
  map[290] = "RIGHTPAREN";
  map[291] = "NOTRIGHTPAREN";
  map[292] = "LINE_COMMENT";
  map[293] = "START_COMMENT";
  map[294] = "END_COMMENT";
  map[295] = "QUOTES";
  map[296] = "ERROR";
  map[297] = "ID";
  map[298] = "STR_CONST";
  map[299] = "DOUBLE";
  map[300] = "LONG";
  map[314] = "NEG";

  yydebug = 0;

  write_cursor();

  return yyparse();
}