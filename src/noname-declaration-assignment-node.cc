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
  std::unique_ptr<NodeValue> node_value = getValue();

  getContext()->storeVariable(name, std::move(node_value.get()));

  if (noname::debug >= 2) {
    fprintf(stdout, "\n############ stored %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
  }

  return nullptr;
}
std::vector<Value*> DeclarationAssignmentNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  AllocaInst* untyped_poiter_alloca = declaration_codegen_util(this, bb);

  const std::unique_ptr<ExpNode>& rhs = getRHS();
  Value* value = rhs->codegen();
  getContext()->storeValue(getName(), value);

  std::vector<Value*> assign_codegen = assign_codegen_util(untyped_poiter_alloca, value, bb);
  std::vector<Value*> codegen;

  codegen.push_back(untyped_poiter_alloca);
  codegen.reserve(1 + assign_codegen.size());
  for (auto& ptr : assign_codegen) {
    codegen.push_back(std::move(ptr));
  }

  return codegen;
}

Value* DeclarationAssignmentNode::codegen(llvm::BasicBlock* bb) {
  ;
  return codegen_elements_retlast(this, bb);
}
}