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

AssignmentNode* new_declaration_node(ASTContext* context, const std::string name) {
  AssignmentNode* new_node = new AssignmentNode(context, name, NULL);

  NodeValue* temp_node = context->getVariable(name);
  if (temp_node) {
    return logErrorV("\nVariable already exists in this context!");
  }

  context->storeVariable(name, temp_node);  // temp_node is null. It doesn't matter, it's only a declaration

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[new_assignment %s]", context->getName().c_str());
  }
  return new_node;
}

std::vector<Value*> DeclarationNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  AllocaInst* alloca_inst = declaration_codegen_util(this, bb);

  std::vector<Value*> codegen;
  codegen.push_back(alloca_inst);

  return codegen;
}
Value* DeclarationNode::codegen(llvm::BasicBlock* bb) { return declaration_codegen_util(this, bb); }
void* DeclarationNode::eval() {
  NodeValue* node_value = nullptr;
  getContext()->storeVariable(name, node_value);
  return nullptr;
}
}