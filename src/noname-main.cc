// #define NDEBUG
// #include "assert.h"

#include "llvm/Support/CommandLine.h"
#include "llvm-c/BitWriter.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
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
int mod_id = 1;
const char *curr_filename = "<stdin>";  // this name is arbitrary
FILE *fin = stdin;                      /* we read from this file */

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
    logError("Error when trying to get the directory name");  // TODO inform the
                                                              // filename to the
                                                              // user
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

//===----------------------------------------------------------------------===//
// Top-Level parsing and JIT Driver
//===----------------------------------------------------------------------===//

void CreateNewModuleAndInitialize() {
  if (TheModule) {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[print module '%s']", TheModule->getName().str().c_str());
      fflush(stdout);
      TheModule->dump();
    }

    TheJIT->writeToFile(TheModule.get());
    TheJIT->addModule(std::move(TheModule));
    InitializeModuleAndPassManager();
  }
}

void InitializeModuleAndPassManager() {
  // if (TheModule) {
  //   fprintf(stdout, "\n[module write]");
  //   raw_fd_ostream os(output_filedescriptor, false, false);
  //   llvm::WriteBitcodeToFile(TheModule.get(), os);
  // }

  // Open a new module.
  std::string module_name("my-jit-module-");
  module_name += std::to_string(mod_id++);
  TheModule = llvm::make_unique<Module>(module_name, TheContext);
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

ASTNode *pre_process(ASTNode *node) {
  if (!node) {
    return node;
  }

  if (isa<CallExpNode>(*node)) {
    return new_top_level_exp_node((CallExpNode *)node);
  }

  return node;
}

void eval(ASTNode *node) {
  if (!node || isa<ErrorNode>(*node)) {
    logError((ErrorNode *)node);
    return;
  }

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[### eval: %s]\n", ASTNode::toString(node->getKind()).c_str());
  }

  if (noname::debug >= 2) {
    fprintf(stderr, "\n[is_of_type<AssignmentNode>(*node) -> %s %s]\n", isa<AssignmentNode>(*node) ? "true" : "false",
            isa<DeclarationAssignmentNode>(*node) ? "true" : "false");
  }

  node->getProcessorStrategy()->process(node);
}

// void assert_equals(int i1, int i2) {
//   fprintf(stdout, "v1 '%d' / v2 '%d' %s \n", i1, i2, i1 == i2 ? "equal" :
//   "not equal");
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
    if (token == LONG_TOK) {
      fprintf(stdout, "\n#TOKEN %d[%s] yytext -> %ld\n", token, map[token].c_str(), yylval.long_v);
    } else if (token == DOUBLE_TOK) {
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

void exit_hook() {
  TheJIT->release();
  // def f() { return 32122; }; f();
  llvm_shutdown();
  fprintf(stderr, "\nEND");
}

int main(int argc, char **argv) {
  // http://llvm.org/docs/CommandLine.html#the-cl-getregisteredoptions-function

  // // ManagedStatic<CommandLineParser> GlobalParser;
  // // cl::GlobalParser;
  // StringMap<llvm::cl::Option *> &opts = llvm::cl::getRegisteredOptions();
  // opts["debug"]->removeArgument();

  // cl::opt<bool> force_arg("f", cl::desc("Enable binary output on
  // terminals"));
  cl::opt<int> debug_arg1("app-debug", cl::desc("Enable application debug 1-4"));
  cl::opt<int> debug_arg2("d", cl::desc("Enable application debug 1-4"));
  cl::opt<int> yydebug_arg("yydebug", cl::desc("Enable yydebug (lexer and parser)"));
  cl::opt<bool> quiet_arg1("quiet", cl::desc("Don't print informational messages"));
  cl::opt<bool> quiet_arg2("q", cl::desc("Don't print informational messages"));
  cl::opt<bool> quiet_arg3("no-verbose", cl::desc("Don't print informational messages"), cl::Hidden);

  cl::ParseCommandLineOptions(argc, argv,
                              " CommandLine compiler example\n\n"
                              "  This program blah blah blah...\n");

  int token;
  bool quiet = quiet_arg1 || quiet_arg2 || quiet_arg3;

  yydebug = yydebug_arg;
  noname::debug = std::max((int)debug_arg1, (int)debug_arg2);

  if (atexit(exit_hook) != 0) {
    logError("Cannot set exit function\n");
    exit(EXIT_FAILURE);
  }

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
  map[267] = "IN_TOK";
  map[268] = "LET_TOK";
  map[269] = "DEF_TOK";
  map[270] = "LOOP_TOK";
  map[271] = "THEN_TOK";
  map[272] = "WHILE";
  map[273] = "BREAK_TOK";
  map[274] = "CASE_TOK";
  map[275] = "NEW_TOK";
  map[276] = "NOT_TOK";
  map[277] = "RETURN";
  map[278] = "TRUE";
  map[279] = "NEWLINE";
  map[280] = "NOTNEWLINE";
  map[281] = "WHITESPACE";
  map[282] = "LE_TOK";
  map[283] = "ASSIGN";
  map[284] = "NULLCH";
  map[285] = "BACKSLASH";
  map[286] = "STAR_TOK";
  map[287] = "NOTSTAR";
  map[288] = "LEFTPAREN_TOK";
  map[289] = "NOTLEFTPAREN_TOK";
  map[290] = "RIGHTPAREN";
  map[291] = "NOTRIGHTPAREN";
  map[292] = "LINE_COMMENT";
  map[293] = "START_COMMENT";
  map[294] = "END_COMMENT";
  map[295] = "QUOTES";
  map[296] = "ERROR_TOK";
  map[297] = "IDENTIFIER";
  map[298] = "STR_CONST";
  map[299] = "DOUBLE_TOK";
  map[300] = "LONG_TOK";
  map[314] = "NEG_TOK";

  fprintf(stdout, "\n[MUST INCLUDE BASIC LIBRARIES]\n");

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  write_cursor();

  TheJIT = llvm::make_unique<NonameJIT>();

  InitializeModuleAndPassManager();

  noname::InitializeNonameEnvironment();

  /**
    * 
    * How to load LLVM bitcode file from an ifstream?
    * 
    * http://stackoverflow.com/questions/22239801/how-to-load-llvm-bitcode-file-from-an-ifstream
    */

  int parse_output = yyparse();

  /*
    
    def fun(a,b) { return a; };

    def fun2(a,b) { return a + b; };
    fun2(32.2,4);
    
    def f() { return 32122; };
    def f1() { return 111; };
    def f2() { return 222; };
    def f3() { return 333; };
    def f4() { return 444; };
    def f5() { return 555; };
    def f6() { return 666; };
    def f7() { return 777; };
    def f8() { return 888; };
    f8();
    f();
    f();
    f();
    f();
    f();
    f();
    f();
    f();
    f();
    f();

    ENDWhile deleting: i64 %
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Use still stuck around after Def is destroyed:  ret i64 32122
    Assertion failed: (use_empty() && "Uses remain when a value is destroyed!"), function ~Value, file /Users/thiagoh/dev/llvm-src/llvm-3.9/lib/IR/Value.cpp, line 85.
   */

  noname::ReleaseNonameEnvironment();

  return parse_output;
}