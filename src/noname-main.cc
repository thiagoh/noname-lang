#include "lexer-utilities.h"
#include "noname-parse.h"
#include "noname-types.h"
#include <map>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string>

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
      fprintf(stderr, "\n#TOKEN %d[%s] yytext -> %ld\n", token, map[token].c_str(), yylval.long_v);
    } else if (token == DOUBLE) {
      fprintf(stderr, "\n#TOKEN %d[%s] yytext -> %lf\n", token, map[token].c_str(), yylval.double_v);
    } else if (token == ID) {
      fprintf(stderr, "\n#TOKEN %d[%s] yytext -> %s\n", token, map[token].c_str(), yylval.id_v);
    } else {
      fprintf(stderr, "\n#TOKEN %d[%s] yytext -> %c\n", token, map[token].c_str(), (char)token);
    }
  }

  return token;
}

void division_by_zero(YYLTYPE &yylloc) {
  fprintf(stderr, "SEVERE ERROR %d:%d - %d:%d. Division by zero", yylloc.first_line, yylloc.first_column,
          yylloc.last_line, yylloc.last_column);
}

void yyerror(char const *s) { fprintf(stderr, "\nERROR: %s\n", s); }

void eval(ASTNode *node) {
  if (!node) {
    return;
  }

  fprintf(stderr, "\neval: ASTNode %s %d", is_of_type<ASTNode>(*node) ? "true" : "false", node->getType());
  // fprintf(stderr, "\neval: NumberNode %s %d\n", is_of_type<NumberNode>(*node) ? "true" : "false", node->getType());
  // fprintf(stderr, "\neval: BinaryExpNode %s %d\n", is_of_type<BinaryExpNode>(*node) ? "true" : "false",
  // node->getType());
  // fprintf(stderr, "\neval: CallExprNode %s %d\n", is_of_type<CallExprNode>(*node) ? "true" : "false",
  // node->getType());

  if (is_of_type<CallExprNode>(*node)) {
    CallExprNode *callExp = (CallExprNode *)node;

    FunctionDefNode *functionNode = context->getFunction(callExp->getCallee());

    if (functionNode) {
      fprintf(stderr, "\nThe called function was: '%s'\n", functionNode->getName().c_str());
      NodeValue *return_value = callExp->getValue();

      if (return_value) {
        print_node_value(stderr, *return_value);
      }

    } else {
      fprintf(stderr, "\nThe called function was: '%s' BUT it wan not found on the context\n",
              callExp->getCallee().c_str());
    }
  }
}

int main(int argc, char **argv) {
  int token;

  context = new ASTContext("root");
  context_stack.push(context);

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
  map[277] = "TRUE";
  map[278] = "NEWLINE";
  map[279] = "NOTNEWLINE";
  map[280] = "WHITESPACE";
  map[281] = "LE";
  map[282] = "ASSIGN";
  map[283] = "NULLCH";
  map[284] = "BACKSLASH";
  map[285] = "STAR";
  map[286] = "NOTSTAR";
  map[287] = "LEFTPAREN";
  map[288] = "NOTLEFTPAREN";
  map[289] = "RIGHTPAREN";
  map[290] = "NOTRIGHTPAREN";
  map[291] = "LINE_COMMENT";
  map[292] = "START_COMMENT";
  map[293] = "END_COMMENT";
  map[294] = "QUOTES";
  map[295] = "ERROR";
  map[296] = "ID";
  map[297] = "STR_CONST";
  map[298] = "DOUBLE";
  map[299] = "LONG";
  map[312] = "NEG";

  yydebug = 2;

  return yyparse();
}