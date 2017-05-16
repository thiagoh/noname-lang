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

ReturnExpNode* new_return_exp_node(ExpNode* exp_node) {
  ASTContext* exp_node_context = exp_node->getContext();
  ReturnExpNode* return_exp_node = new ReturnExpNode(exp_node_context, exp_node);
  return return_exp_node;
}

ReturnExpNode::ReturnExpNode(ASTContext* context, ExpNode* exp_node)
    : ExpNode(context, AST_NODE_TYPE_RETURN_NODE), exp_node(exp_node) {
  if (noname::debug >= 2) {
    fprintf(stdout, "\n[ReturnExpNode::ReturnExpNode() called]");
    fflush(stdout);
  }
}
ReturnExpNode::~ReturnExpNode() {
  delete exp_node;

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[ReturnExpNode::~ReturnExpNode()]");
    fflush(stdout);
  }
}

Value* ReturnExpNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }
std::vector<Value*> ReturnExpNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  std::vector<Value*> codegen;

  std::vector<Value*> exp_node_codegen_elements = exp_node->get_codegen_elements(error, bb);

  for (auto current_value : exp_node_codegen_elements) {
    if (isa<Constant>(current_value)) {
      // ((Constant*)current_value)->dropAllReferences();
      continue;
    }
    // if (isa<CallInst>(current_value)) {
    //   continue;
    // }
    codegen.push_back(current_value);
  }

  // codegen.insert(codegen.end(), exp_node_codegen_elements.begin(), exp_node_codegen_elements.end());
  Value* value_to_be_returned = exp_node_codegen_elements.back();
  ReturnInst* return_inst = nullptr;

  if (value_to_be_returned) {
    // createError(error, "No such elements from expression node of return");
    // return codegen;
    return_inst = ReturnInst::Create(TheContext, value_to_be_returned);
  } else {
    return_inst = ReturnInst::Create(TheContext);
  }

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[return_inst->dump()]");
    fflush(stdout);
    return_inst->dump();
  }

  codegen.push_back(return_inst);

  return codegen;
}
}