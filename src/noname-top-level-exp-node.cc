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

ASTNode* new_top_level_exp_node(ExpNode* exp_node) {
  auto* context = exp_node->getContext();

  ASTNode* anonymous_def_node =
      createAnnonymousFunctionDefNode(context, exp_node);

  if (anonymous_def_node && isa<ErrorNode>(*anonymous_def_node)) {
    return anonymous_def_node;
  }

  TopLevelExpNode* new_node = new TopLevelExpNode(
      context, exp_node, (FunctionDefNode*)anonymous_def_node);

  return new_node;
}
std::vector<std::unique_ptr<Value>> DeclarationNode::codegen_elements(
    llvm::BasicBlock* bb) {
  return declaration_codegen(this, bb);
}
Value* TopLevelExpNode::codegen(llvm::BasicBlock* bb) {
  if (!anonymous_def_node) {
    fprintf(stderr, "\n\n############ could not resolve top level expression");
    return nullptr;
  }
  return anonymous_def_node->codegen();
}
void* TopLevelExpNode::release() {
  if (anonymous_def_node) {
    getContext()->removeFunction(anonymous_def_node->getName());
    delete anonymous_def_node;
  }

  return nullptr;
}
}