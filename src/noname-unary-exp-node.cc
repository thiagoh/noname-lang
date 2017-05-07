#include "noname-utils.h"
#include "noname-types.h"
#include "noname-jit.h"
#include <limits.h>
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

namespace noname {

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
extern std::unique_ptr<NonameJIT> TheJIT;

NodeValue* UnaryExpNode::getValue() {
  NodeValue* rhs_value = rhs->getValue();
  fprintf(stdout, "\n\n############ IMPLEMENT ME: UnaryExpNode::getValue ###########\n\n");
  return rhs_value;
}

Value* UnaryExpNode::codegen(llvm::BasicBlock* bb) {
  Value* R = rhs->codegen();
  fprintf(stdout, "\n\n############ IMPLEMENT ME: UnaryExpNode::codegen ###########\n\n");
  return R;
}
std::vector<Value*> UnaryExpNode::codegen_elements(Error** error, llvm::BasicBlock* bb) {
  logError(
      "NOT IMPLEMENTED - std::vector<Value*> "
      "UnaryExpNode::codegen_elements");
  return std::vector<Value*>();
}
}