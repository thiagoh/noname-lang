// #define NDEBUG
// #include "assert.h"

// #include "llvm/ADT/APFloat.h"
// #include "llvm/ADT/STLExtras.h"
// #include "llvm/IR/BasicBlock.h"
// #include "llvm/IR/Constants.h"
// #include "llvm/IR/DerivedTypes.h"
// #include "llvm/IR/Function.h"
// #include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/LLVMContext.h"
// #include "llvm/IR/LegacyPassManager.h"
// #include "llvm/IR/Module.h"
// #include "llvm/IR/Type.h"
// #include "llvm/IR/Verifier.h"
// #include "llvm/Support/TargetSelect.h"
// #include "llvm/Target/TargetMachine.h"
// #include "llvm/Transforms/Scalar.h"
// #include "llvm/Transforms/Scalar/GVN.h"

#include "lexer-utilities.h"
#include "noname-utils.h"
#include "noname-parse.h"
#include "noname-types.h"
#include "noname-jit.h"
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
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

using namespace llvm;
using namespace llvm::orc;
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

LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
std::unique_ptr<Module> TheModule;
std::unique_ptr<legacy::FunctionPassManager> TheFPM;
std::unique_ptr<NonameJIT> TheJIT;

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

//===----------------------------------------------------------------------===//
// Top-Level parsing and JIT Driver
//===----------------------------------------------------------------------===//

void InitializeModuleAndPassManager() {
  // Open a new module.
  TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
  TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());

  // Create a new pass manager attached to it.
  TheFPM = llvm::make_unique<legacy::FunctionPassManager>(TheModule.get());

  // Do simple "peephole" optimizations and bit-twiddling optzns.
  TheFPM->add(createInstructionCombiningPass());
  // Reassociate expressions.
  TheFPM->add(createReassociatePass());
  // Eliminate Common SubExpressions.
  TheFPM->add(createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  TheFPM->add(createCFGSimplificationPass());

  TheFPM->doInitialization();
}

void eval(ASTNode *node) {
  if (!node || isa<ErrorNode>(*node)) {
    logError((ErrorNode *)node);
    return;
  }

  if (yydebug >= 1) {
    fprintf(stdout, "\n[### eval: %s]\n", ASTNode::toString(node->getKind()).c_str());
  }

  if (yydebug >= 2) {
    fprintf(stderr, "\n[is_of_type<AssignmentNode>(*node) -> %s %s]\n", isa<AssignmentNode>(*node) ? "true" : "false",
            isa<DeclarationAssignmentNode>(*node) ? "true" : "false");
  }

  node->getProcessorStrategy()->process(node);
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

int yylex(void) {
  int token = noname_yylex();

  if (yydebug > 1) {
    if (token == LONG) {
      fprintf(stdout, "\n#TOKEN %d[%s] yytext -> %ld\n", token, map[token].c_str(), yylval.long_v);
    } else if (token == DOUBLE) {
      fprintf(stdout, "\n#TOKEN %d[%s] yytext -> %lf\n", token, map[token].c_str(), yylval.double_v);
    } else if (token == IDENTIFIER) {
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
  map[264] = "ELSE_TOK";
  map[265] = "FALSE";
  map[266] = "IF_TOK";
  map[267] = "IN";
  map[268] = "LET_TOK";
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
  map[297] = "IDENTIFIER";
  map[298] = "STR_CONST";
  map[299] = "DOUBLE";
  map[300] = "LONG";
  map[314] = "NEG";

  yydebug = 0;

  fprintf(stdout, "\n[MUST INCLUDE BASIC LIBRARIES]");

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  write_cursor();

  TheJIT = llvm::make_unique<NonameJIT>();

  InitializeModuleAndPassManager();

  return yyparse();
}