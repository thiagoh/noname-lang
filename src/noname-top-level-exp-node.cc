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

TopLevelExpNode::TopLevelExpNode(ASTContext* context, ExpNode* exp_node, CallExpNode* call_exp_node,
                                 FunctionDefNode* anonymous_def_node)
    : ExpNode(context, AST_NODE_TYPE_TOP_LEVEL_EXP_NODE),
      exp_node(exp_node),
      call_exp_node(call_exp_node),
      anonymous_def_node(anonymous_def_node) {}

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
  ASTContext* context = exp_node->getContext();

  ASTNode* anonymous_def_node = createAnnonymousFunctionDefNode(context, exp_node);

  if (anonymous_def_node && isa<ErrorNode>(*anonymous_def_node)) {
    return anonymous_def_node;
  }

  ASTNode* call_exp_node = new_call_node(context, (FunctionDefNode*)anonymous_def_node);

  if (call_exp_node && isa<ErrorNode>(*call_exp_node)) {
    return call_exp_node;
  }

  TopLevelExpNode* new_node =
      new TopLevelExpNode(context, exp_node, (CallExpNode*)call_exp_node, (FunctionDefNode*)anonymous_def_node);

  return new_node;
}
std::vector<Value*> TopLevelExpNode::codegen_elements(Error** error, llvm::BasicBlock* bb) {
  std::vector<Value*> codegen;

  if (!anonymous_def_node) {
    *error = createError("Could not resolve top level expression");
    return codegen;
  }

  Instruction* function_def = (Instruction*)anonymous_def_node->codegen();
  codegen.push_back(function_def);

  // Instruction* call_inst = (Instruction*)call_exp_node->codegen();
  // codegen.push_back(call_inst);
  return codegen;
}
Value* TopLevelExpNode::codegen(llvm::BasicBlock* bb) {
  Value* value = codegen_elements_retlast(this, bb);
  return value;
}

void* TopLevelExpNodeProcessorStrategy::process(ASTNode* node) {
  TopLevelExpNode* top_level_exp_node = (TopLevelExpNode*)node;

  // http://llvm.org/docs/doxygen/html/classllvm_1_1ExecutionEngine.html#a97bbf524ee03354bb73dce9614b0e959

  Error* error = nullptr;

  std::vector<Value*> codegen(top_level_exp_node->codegen_elements(&error));

  if (error) {
    return logErrorLLVM(error->what().c_str());
  }

  if (codegen.size() <= 0) {
    fprintf(stderr, "\nTop level expression could not be evaluated");
  } else {
    if (noname::debug >= 1) {
      fprintf(stderr, "\n[read top level expression]");
    }

    llvm::Type* result_type = toLLVMType(codegen.back());
    assert(result_type && "Result type is null");

    if (noname::debug >= 1) {
      TheModule->dump();
    }

    // JIT the module containing the anonymous expression, keeping a handle so
    // we can free it later.
    TheJIT->writeToFile(TheModule.get());
    auto module_handle = TheJIT->addModule(std::move(TheModule));
    InitializeModuleAndPassManager();

    // Search the JIT for the __anon_expr symbol.
    auto ExprSymbol = TheJIT->findSymbol("__anon_expr");
    assert(ExprSymbol && "Function not found");

    call_and_print_jit_symbol_value(stdout, result_type, ExprSymbol);

    // Delete the anonymous expression module from the JIT.
    TheJIT->removeModule(module_handle);
  }

  top_level_exp_node->release();

  return nullptr;
}

void* TopLevelExpNode::release() {
  if (anonymous_def_node) {
    getContext()->removeFunction(anonymous_def_node->getName());
    delete anonymous_def_node;
  }

  return nullptr;
}
}