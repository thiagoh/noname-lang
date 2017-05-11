#include "noname-jit.h"
#include "noname-types.h"
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;
using namespace llvm::orc;

namespace llvm {
namespace orc {
// extern LLVMContext TheContext;
// extern IRBuilder<> Builder;
// extern std::unique_ptr<Module> TheModule;
// extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;

// TODO FIXME
void NonameJIT::writeToFile(const Module *mod) {
  // if (true) return;

  std::string sname = "bc-output/";
  sname += mod->getName().str();
  sname += ".bc";
  const char *output_filename = sname.c_str();
  FILE *output_file = fopen(output_filename, "w");
  int output_filedescriptor = fileno(output_file);

  raw_fd_ostream os(output_filedescriptor, true, true);
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[module write]");
  }
  llvm::WriteBitcodeToFile(mod, os);
  // fflush(output_file);
  // fclose(output_file);
  // close(output_filedescriptor);
}

// TODO FIXME
void NonameJIT::writeToFile() {
  // raw_fd_ostream os(output_filedescriptor, false, false);
  for (auto &mod : Modules) {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[module write]");
    }
    // llvm::WriteBitcodeToFile(mod, os);
    writeToFile(mod);
  }
}

// TODO FIXME
void NonameJIT::release() {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[NonameJIT::release()]");
  }

  Modules.clear();
}
}
}
