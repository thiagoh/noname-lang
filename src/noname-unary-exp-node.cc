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
  fprintf(
      stdout,
      "\n\n############ IMPLEMENT ME: UnaryExpNode::getValue ###########\n\n");
  return rhs_value;
}

Value* UnaryExpNode::codegen(llvm::BasicBlock* bb) {
  NodeValue* rhs_node_value = rhs->getValue();
  Value* R = rhs_node_value->codegen();
  fprintf(
      stdout,
      "\n\n############ IMPLEMENT ME: UnaryExpNode::codegen ###########\n\n");
  return R;
}
std::vector<std::unique_ptr<Value>> UnaryExpNode::codegen_elements(
    BasicBlock* bb) {
  logError(
      "NOT IMPLEMENTED - std::vector<std::unique_ptr<Value>> "
      "UnaryExpNode::codegen_elements");
}
}