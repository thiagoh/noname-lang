// #define NDEBUG
// #include "assert.h"
#include "lexer-utilities.h"
#include "noname-utils.h"
#include "noname-parse.h"
#include "noname-types.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <vector>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace llvm;
using namespace noname;

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif

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
FILE *fin = stdin;                /* we read from this file */

namespace noname {

ASTContext *context;
std::vector<std::string> imported_files;
std::stack<ASTContext *> context_stack;
std::map<int, std::string> map;
bool read_from_file_import = false;

void fatal_error(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(YY_EXIT_FAILURE);
}

bool is_file_already_imported(const std::string &file_path) {
  return std::find(imported_files.begin(), imported_files.end(), file_path) != imported_files.end();
}

bool is_file_already_imported(const char *file_path) { return is_file_already_imported(std::string(file_path)); }
char *get_current_dir() {
  size_t size;
  char *buf;
  char *ptr;

  long path_max = pathconf(".", _PC_PATH_MAX);
  if (path_max == -1) {
    size = 1024;
  } else if (path_max > 10240) {
    size = 10240;
  } else {
    size = path_max;
  }

  for (buf = ptr = NULL; ptr == NULL; size *= 2) {
    if ((buf = (char *)realloc(buf, size * sizeof(char))) == NULL) {
      fprintf(stderr, "\n[out of space]");
      // yyerror("out of space");
      // exit(0);
    }

    ptr = getcwd(buf, size);
    if (ptr == NULL && errno != ERANGE) {
      fprintf(stderr, "\n[out of range]");
      // yyerror("out of space");
      // exit(0);
    }
  }
  return buf;
}

char *concat_strs(const char *format, const char *s1, const char *s2, int size) {
  char *buf = new char[size];
  int bytes_pottentially_written = snprintf(buf, size, format, s1, s2);

  if (bytes_pottentially_written > size) {
    free(buf);
    return concat_strs(format, s1, s2, bytes_pottentially_written);
  }

  return buf;
}

char *get_file_path(const char *filename) {
  char *dirname = get_current_dir();

  if (dirname == NULL) {
    logError("Error when trying to get the directory name");  // TODO inform the filename to the user
    return nullptr;
  }

  int buf_size = strlen(filename) + strlen(dirname) + 2;
  char *file_path = concat_strs("%s/%s", dirname, filename, buf_size);
  free(dirname);
  return file_path;
}

int noname_read(char *buf, int *result, int max_size) {
  if (fin == NULL) {
    fatal_error("Input stream is invalid");
  }

  int cur_char = '*';
  int n = 0;
  for (; n < max_size && (cur_char = getc(fin)) != EOF; ++n) {
    buf[n] = (char)cur_char;
    if (read_from_file_import) {
      // NOP
    } else {
      if (cur_char == '\n') {
        ++n;
        break;
      }
    }
  }
  if (cur_char == EOF) {
    // fprintf(stderr, "\n[EOF] %s", buf);
    if (feof(fin) && read_from_file_import) {
      // fprintf(stderr, "\n[read_from_file_import %d]", *result);
      fclose(fin);  // close the file
      read_from_file_import = false;
      fin = stdin;
    } else if (ferror(fin)) {
      fatal_error("Input stream scanner failed");
    }
  }

  *result = n;
  // fprintf(stderr, "\n[noname_read %d]", max_size);
  return 0;
}

void *ExpNodeProcessorStrategy::process(ASTNode *node) {
  ;
  fprintf(stderr, "\n[############# ExpNodeProcessorStrategy]");

  NodeValue *return_value = (NodeValue *)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
void *AssignmentNodeProcessorStrategy::process(ASTNode *node) {
  ;
  fprintf(stderr, "\n[############# AssignmentNodeProcessorStrategy]");

  NodeValue *return_value = (NodeValue *)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
void *CallExpNodeProcessorStrategy::process(ASTNode *node) {
  ;
  fprintf(stderr, "\n[############# CallExpNodeProcessorStrategy]");
  CallExpNode *call_exp_node = (CallExpNode *)node;
  FunctionDefNode *function_def_node = context->getFunction(call_exp_node->getCallee());

  if (function_def_node) {
    if (yydebug >= 2) {
      fprintf(stdout, "\nThe called function was: '%s'\n", function_def_node->getName().c_str());
    }

    NodeValue *return_value = (NodeValue *)call_exp_node->eval();
    print_node_value(stdout, return_value);

  } else {
    fprintf(stderr, "\nError: The function %s was not found int the context\n", call_exp_node->getCallee().c_str());
  }
  return nullptr;
}
void *ImportNodeProcessorStrategy::process(ASTNode *node) {
  ;
  fprintf(stderr, "\n[############# ImportNodeProcessorStrategy]");

  ImportNode *import_node = (ImportNode *)node;

  char *file_path = get_file_path(import_node->getFilename().c_str());
  char *const_file_path[] = {file_path};
  // const char *const_file_path = file_path;
  FILE *opened_file = fopen(*const_file_path, "r");

  if (opened_file != NULL) {
    if (is_file_already_imported(*const_file_path)) {
      if (yydebug >= 3) {
        fprintf(stdout, "\nNOTICE: File '%s' already imported\n", file_path);
      }
    } else {
      imported_files.push_back(file_path);
      read_from_file_import = true;
      fin = opened_file;
    }

  } else {
    fprintf(stderr, "\nError: File '%s' could not be opened.\n", file_path);
  }

  free(file_path);
  return nullptr;
}

ProcessorStrategy *expNodeProcessorStrategy = new ExpNodeProcessorStrategy();
ProcessorStrategy *assignmentNodeProcessorStrategy = new AssignmentNodeProcessorStrategy();
ProcessorStrategy *callNodeProcessorStrategy = new CallExpNodeProcessorStrategy();
ProcessorStrategy *importNodeProcessorStrategy = new ImportNodeProcessorStrategy();

// void process_node(ExpNode &exp_node) {
//   NodeValue *return_value = (NodeValue *)exp_node->eval();
//   print_node_value(stdout, return_value);
// }
// void process_node(AssignmentNode &assignment_node) {
//   NodeValue *return_value = (NodeValue *)assignment_node->eval();
//   print_node_value(stdout, return_value);
// }
// void process_node(CallExpNode &call_exp_node) {
//   FunctionDefNode *function_def_node = context->getFunction(call_exp_node->getCallee());

//   if (function_def_node) {
//     if (yydebug >= 2) {
//       fprintf(stdout, "\nThe called function was: '%s'\n", function_def_node->getName().c_str());
//     }

//     NodeValue *return_value = (NodeValue *)call_exp_node->eval();
//     print_node_value(stdout, return_value);

//   } else {
//     fprintf(stderr, "\nError: The function %s was not found int the context\n", call_exp_node->getCallee().c_str());
//   }
// }
// void process_node(ImportNode &import_node) {
//   char *file_path = get_file_path(import_node->getFilename().c_str());
//   char *const_file_path[] = {file_path};
//   // const char *const_file_path = file_path;
//   FILE *opened_file = fopen(*const_file_path, "r");

//   if (opened_file != NULL) {
//     if (is_file_already_imported(*const_file_path)) {
//       if (yydebug >= 3) {
//         fprintf(stdout, "\nNOTICE: File '%s' already imported\n", file_path);
//       }
//     } else {
//       imported_files.push_back(file_path);
//       read_from_file_import = true;
//       fin = opened_file;
//     }

//   } else {
//     fprintf(stderr, "\nError: File '%s' could not be opened.\n", file_path);
//   }

//   free(file_path);
// }

void eval(ASTNode *node) {
  if (!node || is_of_type<ErrorNode>(*node)) {
    logError((ErrorNode *)node);
    return;
  }

  if (yydebug >= 1) {
    fprintf(stdout, "\n[### eval: %d]\n", node->getType());
  }

  if (yydebug >= 2) {
    // fprintf(stderr, "\n[is_of_type<ExpNode>(*node) -> %s]\n", is_of_type<ExpNode>(*node) ? "true" : "false");
    fprintf(stderr, "\n[is_of_type<AssignmentNode>(*node) -> %s %s]\n",
            is_of_type<AssignmentNode>(*node) ? "true" : "false",
            is_of_type<DeclarationAssignmentNode>(*node) ? "true" : "false");
  }

  node->getProcessorStrategy()->process(node);

  // if (is_of_type<ImportNode>(*node)) {
  //   ImportNode *import_node = (ImportNode *)node;
  //   process_import_node(import_node);

  // } else if (is_of_type<AssignmentNode>(*node)) {
  //   AssignmentNode *assignment_node = (AssignmentNode *)node;
  //   process_assignment_node(assignment_node);

  // } else if (is_of_type<CallExpNode>(*node)) {
  //   CallExpNode *call_exp_node = (CallExpNode *)node;
  //   process_call_exp_node(call_exp_node);

  // } else if (is_of_type<ExpNode>(*node)) {  // this has some bug. it's returning false when node is a VarNode
  //   ExpNode *exp_node = (ExpNode *)node;
  //   process_exp_node(exp_node);
  // }
}

// void assert_equals(int i1, int i2) {
//   fprintf(stdout, "v1 '%d' / v2 '%d' %s \n", i1, i2, i1 == i2 ? "equal" : "not equal");
//   assert(i1 == i2);
// }

void write_cursor() {
  // writes the cursor so the user can input code
  fprintf(stdout, "\n>");
}

void division_by_zero(YYLTYPE &yylloc) {
  fprintf(stderr, "Error: %d:%d - %d:%d. Division by zero", yylloc.first_line, yylloc.first_column, yylloc.last_line,
          yylloc.last_column);
}
}

// #ifndef YY_INPUT
// #define YY_INPUT(buf,result,max_size) \
// 	if ( YY_CURRENT_BUFFER_LVALUE->yy_is_interactive ) \
// 		{ \
// 		int c = '*'; \
// 		int n; \
// 		for ( n = 0; n < max_size && \
// 			     (c = getc( noname_yyin )) != EOF && c != '\n'; ++n ) \
// 			buf[n] = (char) c; \
// 		if ( c == '\n' ) \
// 			buf[n++] = (char) c; \
// 		if ( c == EOF && ferror( noname_yyin ) ) \
// 			YY_FATAL_ERROR( "input in flex scanner failed" ); \
// 		result = n; \
// 		} \
// 	else \
// 		{ \
// 		errno=0; \
// 		while ( (result = (int) fread(buf, 1, (yy_size_t) max_size, noname_yyin)) == 0 && ferror(noname_yyin)) \
// 			{ \
// 			if( errno != EINTR) \
// 				{ \
// 				YY_FATAL_ERROR( "input in flex scanner failed" ); \
// 				break; \
// 				} \
// 			errno=0; \
// 			clearerr(noname_yyin); \
// 			} \
// 		}\
// \

// #endif

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

void yyerror(char const *s) { fprintf(stdout, "\nERROR: %s\n", s); }

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

  yydebug = 2;

  write_cursor();

  // fprintf(
  //     stdin,
  //     "def inner_func() {  def f2() {    return 2;  }  def f7() {    return 7;  }  return f2() * f3();}inner_func();
  //     ");

  return yyparse();
}