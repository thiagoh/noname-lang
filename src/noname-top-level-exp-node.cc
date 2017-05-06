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

ASTNode* createAnnonymousFunctionDefNode(ASTContext* context, ExpNode* exp_node) {
  const std::string annon_name = "__anon_expr";
  arglist_t* arg_list = new_arg_list(context);
  stmtlist_t* stmt_list = new_stmt_list(context);
  ExpNode* return_node = exp_node;

  auto* function_def_node = new_function_def(context, annon_name, arg_list, stmt_list, return_node);

  release(arg_list);
  release(stmt_list);

  return function_def_node;
}

ASTNode* new_top_level_exp_node(ExpNode* exp_node) {
  auto* context = exp_node->getContext();

  ASTNode* anonymous_def_node = createAnnonymousFunctionDefNode(context, exp_node);

  if (anonymous_def_node && isa<ErrorNode>(*anonymous_def_node)) {
    return anonymous_def_node;
  }

  TopLevelExpNode* new_node = new TopLevelExpNode(context, exp_node, (FunctionDefNode*)anonymous_def_node);

  return new_node;
}
std::vector<std::unique_ptr<Value>> TopLevelExpNode::codegen_elements(Error** error, llvm::BasicBlock* bb) {
  std::vector<std::unique_ptr<Value>> codegen;

  if (!anonymous_def_node) {
    *error = createError("\n\n############ could not resolve top level expression");
    return codegen;
  }

  Value* value = anonymous_def_node->codegen();

  if (!isa<CallInst>(value)) {
    *error = createError("TopLevelExpNode should be a CallInst");
    return codegen;
  }

  codegen.push_back(std::unique_ptr<Value>((Instruction*)value));
  return codegen;
}
Value* TopLevelExpNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }
void* TopLevelExpNode::release() {
  if (anonymous_def_node) {
    getContext()->removeFunction(anonymous_def_node->getName());
    delete anonymous_def_node;
  }

  return nullptr;
}
}