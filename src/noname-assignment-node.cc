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

AssignmentNode* new_assignment_node(ASTContext* context, const std::string name,
                                    ExpNode* exp) {
  AssignmentNode* new_node = new AssignmentNode(context, name, exp);
  return new_node;
}

NodeValue* AssignmentNode::getValue() { return rhs->getValue(); }
void* AssignmentNode::eval() {
  NodeValue* node_value = getValue();

  getContext()->update(name, node_value);

  if (debug >= 2) {
    fprintf(stdout, "\n############ updated %s on context %s \n\n",
            name.c_str(), getContext()->getName().c_str());
  }

  return node_value;
}
std::vector<std::unique_ptr<Value>> AssignmentNode::codegen_elements(
    llvm::BasicBlock* bb) {
  std::vector<std::unique_ptr<Value>> codegen;
  AllocaInst* alloca_inst = getContext()->getAllocaInst(getName());

  if (!alloca_inst) {
    return codegen;
  }

  std::vector<std::unique_ptr<Value>> assign_codegen =
      assign_codegen_util(alloca_inst, this, bb);

  codegen.push_back(std::unique_ptr<Value>(alloca_inst));
  for (auto&& uptr : assign_codegen) {
    codegen.push_back(std::move(uptr));
  }

  return codegen;
}
Value* AssignmentNode::codegen(llvm::BasicBlock* bb) {
  return codegen_elements_retlast(this, bb);
}

//----------------------------------------------//
//----------- Processor Strategy ---------------//
//----------------------------------------------//

void* AssignmentNodeProcessorStrategy::process(ASTNode* node) {
  NodeValue* return_value = (NodeValue*)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
}