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
                                 Function* anonymous_function)
    : ExpNode(context, AST_NODE_TYPE_TOP_LEVEL_EXP_NODE),
      exp_node(exp_node),
      call_exp_node(call_exp_node),
      anonymous_function(anonymous_function) {
  if (noname::debug >= 2) {
    fprintf(stderr, "\n[TopLevelExpNode::TopLevelExpNode() called]");
  }
}

TopLevelExpNode::~TopLevelExpNode() {}

ASTNode* createAnnonymousFunctionDefNode(ASTContext* context, ExpNode* return_node) {
  const std::string annon_name = "__anon_expr";
  arglist_t* arg_list = new_arg_list(context);
  stmtlist_t* stmt_list = new_stmt_list(context);

  auto* function_def_node = new_function_def(context, annon_name, arg_list, stmt_list, return_node);

  release(arg_list);
  release(stmt_list);

  return function_def_node;
}

ASTNode* new_top_level_exp_node(ExpNode* exp_node) {
  CreateNewModuleAndInitialize();

  ASTContext* top_level_context = exp_node->getContext();
  CallExpNode* called_function_exp_node = (CallExpNode*)exp_node;

  /*
  Error* error = nullptr;
  Function* called_function_declaration = called_function_exp_node->getCalledFunction(&error);

  if (error) {
    // TODO
    return new ErrorNode(top_level_context, "Called function could not be declared");
  }

  if (!called_function_declaration) {
    return new ErrorNode(top_level_context, "Called function could not be declared");
  }

  TheModule->getFunctionList().push_back(called_function_declaration);
  */

  std::unique_ptr<ASTNode> function_ptr(createAnnonymousFunctionDefNode(top_level_context, exp_node));

  if (function_ptr && isa<ErrorNode>(*function_ptr.get())) {
    return function_ptr.release();
  }

  Function* anonymous_function = (Function*)((FunctionDefNode*)function_ptr.get())->codegen();

  if (!anonymous_function) {
    return new ErrorNode(top_level_context, "Function could not be defined");
  }

  ASTNode* call_exp_node = new_call_node(top_level_context, anonymous_function);

  if (call_exp_node && isa<ErrorNode>(*call_exp_node)) {
    return call_exp_node;
  }

  TopLevelExpNode* top_level_exp_node =
      new TopLevelExpNode(top_level_context, exp_node, (CallExpNode*)call_exp_node, anonymous_function);

  return top_level_exp_node;
}
std::vector<Value*> TopLevelExpNode::codegen_elements(Error** error, llvm::BasicBlock* bb) {
  std::vector<Value*> codegen;

  if (!anonymous_function) {
    *error = createError("Could not resolve top level expression");
    return codegen;
  }

  codegen.push_back(anonymous_function);

  // Instruction* call_inst = (Instruction*)call_exp_node->codegen();
  // if (call_inst) {
  //   codegen.push_back(call_inst);
  // }

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
      TheModule->dump();
    }

    llvm::Type* result_type = toLLVMType(codegen.back());
    assert(result_type && "Result type is null");

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

void* TopLevelExpNode::release() { return nullptr; }
}