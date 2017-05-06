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

void* DeclarationAssignmentNode::eval() {
  NodeValue* node_value = getValue();

  getContext()->store(name, node_value);

  if (noname::debug >= 2) {
    fprintf(stdout, "\n############ stored %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
  }

  return node_value;
}
std::vector<std::unique_ptr<Value>> DeclarationAssignmentNode::codegen_elements(Error** error, llvm::BasicBlock* bb) {
  AllocaInst* untyped_poiter_alloca = declaration_codegen_util(this, bb);

  std::vector<std::unique_ptr<Value>> assign_codegen = assign_codegen_util(untyped_poiter_alloca, this, bb);
  std::vector<std::unique_ptr<Value>> codegen;

  codegen.push_back(std::unique_ptr<Value>(untyped_poiter_alloca));
  codegen.reserve(1 + assign_codegen.size());
  for (auto&& uptr : assign_codegen) {
    codegen.push_back(std::move(uptr));
  }

  return codegen;
}

Value* DeclarationAssignmentNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }
}