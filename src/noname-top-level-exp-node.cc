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

bool simple_version = false;

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

TopLevelExpNode::~TopLevelExpNode() {
  delete exp_node;
  delete call_exp_node;

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[TopLevelExpNode::~TopLevelExpNode() for __anon_expr]");
    fflush(stdout);
  }
}

ASTNode* create_anonymous_function_def_node(ASTContext* context, ExpNode* return_node) {
  const std::string annon_name = "__anon_expr";
  std::unique_ptr<arglist_t> arg_list(new_arg_list(context));
  std::unique_ptr<stmtlist_t> stmt_list(new_stmt_list(context));

  return new_function_def(context, annon_name, arg_list.get(), stmt_list.get(), return_node);
}

ASTNode* new_top_level_exp_node(ExpNode* exp_node) {
  CreateNewModuleAndInitialize();

  if (simple_version) {
    return new TopLevelExpNode(nullptr, nullptr, nullptr, nullptr);
  }

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

  ASTNode* function_def_node = create_anonymous_function_def_node(top_level_context, called_function_exp_node);

  if (function_def_node && isa<ErrorNode>(*function_def_node)) {
    return function_def_node;
  }

  Function* anonymous_function = (Function*)function_def_node->codegen();

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
std::vector<Value*> TopLevelExpNode::codegen_elements(Error** error, llvm::BasicBlock* bb) const {
  std::vector<Value*> codegen;

  if (simple_version) {
    llvm::Function* called_function = nullptr;
    {
      std::vector<llvm::Type*> called_function_args_types(0, Type::getVoidTy(TheContext));
      FunctionType* called_function_type =
          FunctionType::get(Type::getInt64Ty(TheContext), called_function_args_types, false);
      called_function = Function::Create(called_function_type, Function::ExternalLinkage, "f");
      called_function->dump();
    }

    llvm::CallInst* call_inst = nullptr;
    {
      std::vector<llvm::Value*> args_value;
      call_inst = CallInst::Create(called_function, args_value);
      call_inst->setTailCall(false);
      call_inst->setCallingConv(CallingConv::C);
      call_inst->dump();

      call_inst->dropAllReferences();  //this line makes everything work

      call_inst = CallInst::Create(called_function, args_value);
      call_inst->setTailCall(false);
      call_inst->setCallingConv(CallingConv::C);
      call_inst->dump();
    }

    llvm::ReturnInst* return_inst = ReturnInst::Create(TheContext, call_inst);
    llvm::Type* return_type = toLLVMType(call_inst);

    llvm::Function* function = nullptr;

    {
      std::vector<llvm::Type*> function_args_types(0, Type::getVoidTy(TheContext));
      FunctionType* function_type = FunctionType::get(return_type, function_args_types, false);
      function = Function::Create(function_type, Function::ExternalLinkage, "__anon_expr");
      function->setCallingConv(CallingConv::C);
      function->dump();
    }

    BasicBlock* function_bb = BasicBlock::Create(TheContext, "entry", function);

    // Define function inside Module
    TheModule->getFunctionList().push_back(function);
    // Define the called function inside Module
    TheModule->getFunctionList().push_back(called_function);

    function_bb->getInstList().push_back(call_inst);
    function_bb->getInstList().push_back(return_inst);

    // // Validate the generated code, checking for consistency.
    verifyFunction(*function);

    // // Run the optimizer on the function.
    TheFPM->run(*function);
    codegen.push_back(function);
    return codegen;
  }

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
Value* TopLevelExpNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }

void* TopLevelExpNodeProcessorStrategy::process(ASTNode* node) {
  TopLevelExpNode* top_level_exp_node = (TopLevelExpNode*)node;

  // http://llvm.org/docs/doxygen/html/classllvm_1_1ExecutionEngine.html#a97bbf524ee03354bb73dce9614b0e959

  Error* error = nullptr;

  std::vector<Value*> elements(top_level_exp_node->get_codegen_elements(&error));

  if (error) {
    return logErrorLLVM(error->what().c_str());
  }

  if (elements.size() <= 0) {
    fprintf(stderr, "\nTop level expression could not be evaluated");
  } else {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[read top level expression]");
      fprintf(stdout, "\n[print module '%s']", TheModule->getName().str().c_str());
      fflush(stdout);
      TheModule->dump();
    }

    llvm::Type* result_type = toLLVMType(elements.back());
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