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

AssignmentNode* new_assignment_node(ASTContext* context, const std::string name, ExpNode* exp) {
  AssignmentNode* new_node = new AssignmentNode(context, name, exp);
  return new_node;
}

std::unique_ptr<NodeValue> AssignmentNode::getValue() const { return rhs->getValue(); }
void* AssignmentNode::eval() {
  std::unique_ptr<NodeValue> node_value = getValue();

  getContext()->update(name, std::move(node_value.get()));

  if (debug >= 2) {
    fprintf(stdout, "\n############ updated %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
  }

  return nullptr;
}
std::vector<Value*> AssignmentNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  std::vector<Value*> codegen;
  AllocaInst* alloca_inst = getContext()->getAllocaInst(getName());

  if (!alloca_inst) {
    createError(error, "AllocaInst is invalid or undefined");
    return codegen;
  }

  std::vector<Value*> assign_codegen = assign_codegen_util(alloca_inst, this, bb);

  codegen.push_back(alloca_inst);
  for (auto& ptr : assign_codegen) {
    codegen.push_back(std::move(ptr));
  }

  return codegen;
}
Value* AssignmentNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }

//----------------------------------------------//
//----------- Processor Strategy ---------------//
//----------------------------------------------//

void* AssignmentNodeProcessorStrategy::process(ASTNode* node) {
  NodeValue* return_value = (NodeValue*)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
}