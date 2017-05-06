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

NodeValue* CallExpNode::getValue() {
  ASTContext* call_exp_context = getContext();
  FunctionDefNode* function_node = call_exp_context->getFunction(getCallee());

  if (!function_node) {
    fprintf(stderr,
            "\n\nThe called function was: '%s' BUT it wan not found on the "
            "context\n",
            getCallee().c_str());
    return nullptr;
  }

  // ASTContext* function_context = function_node->getContext();
  // ASTContext* temp_context = new ASTContext("temp_context", call_exp_context,
  // function_context);
  // function_node->setContext(temp_context);

  ExpNode* returnNode = function_node->getReturnNode();
  std::vector<std::unique_ptr<ExpNode>>* value_args = &getArgs();
  std::vector<std::unique_ptr<ExpNode>>::iterator it_value_args =
      value_args->begin();
  std::vector<std::unique_ptr<arg_t>>* signature_args =
      &function_node->getArgs();
  std::vector<std::unique_ptr<arg_t>>::iterator it_signature_args =
      signature_args->begin();
  std::vector<std::unique_ptr<ASTNode>>* body_nodes =
      &function_node->getBodyNodes();
  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes =
      body_nodes->begin();

  for (; it_signature_args != signature_args->end() ||
         it_value_args != value_args->end();) {
    std::unique_ptr<ExpNode>& value_arg = *it_value_args;
    std::unique_ptr<arg_t>& signature_arg = *it_signature_args;

    call_exp_context->store(signature_arg->name, value_arg->getValue());

    ++it_signature_args;
    ++it_value_args;
  }

  for (; it_body_nodes != body_nodes->end();) {
    std::unique_ptr<ASTNode>& body_node = *it_body_nodes;

    body_node->eval();

    if (noname::debug >= 1) {
      fprintf(stdout, "\n[## evaluating body: ASTNode of type %s]\n",
              ASTNode::toString(body_node->getKind()).c_str());
    }
    ++it_body_nodes;
  }

  if (returnNode) {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[## evaluating return]\n");
    }
    return returnNode->getValue();
  } else {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[## no return given]\n");
    }
  }

  // delete temp_context;
  // function_node->setContext(function_context);

  return nullptr;
}

Value* CallExpNode::codegen(llvm::BasicBlock* bb) {
  ASTContext* call_exp_context = getContext();

  FunctionDefNode* function_def_node =
      call_exp_context->getFunction(getCallee());

  if (!function_def_node) {
    fprintf(stderr,
            "\nError: the called function was: '%s' BUT it wan not found on "
            "the context\n",
            getCallee().c_str());
    return nullptr;
  }

  // return function_def_node->codegen();

  // Look up the name in the global module table.
  Function* function = function_def_node->getFunctionDefinition();
  if (!function) {
    return logErrorLLVM("Unknown function referenced");
  }

  std::vector<std::unique_ptr<ExpNode>>& value_args = getArgs();

  // If argument mismatch error.
  if (function->arg_size() != value_args.size()) {
    return logErrorLLVM("Incorrect # arguments passed");
  }

  std::vector<std::unique_ptr<ExpNode>>::iterator it_value_args =
      value_args.begin();
  std::vector<std::unique_ptr<arg_t>>& signature_args =
      function_def_node->getArgs();
  std::vector<std::unique_ptr<arg_t>>::iterator it_signature_args =
      signature_args.begin();
  std::vector<llvm::Value*> args_value;

  for (; it_signature_args != signature_args.end() ||
         it_value_args != value_args.end();) {
    std::unique_ptr<ExpNode>& value_arg = *it_value_args;
    std::unique_ptr<arg_t>& signature_arg = *it_signature_args;

    call_exp_context->store(signature_arg->name, value_arg->getValue());

    args_value.push_back(value_arg->codegen());
    if (!args_value.back()) {
      return nullptr;
    }

    ++it_signature_args;
    ++it_value_args;
  }

  llvm::CallInst* call_inst = nullptr;

  if (function->getReturnType() == llvm::Type::getVoidTy(TheContext)) {
    // Cannot assign a name to void values!
    // call_inst = Builder.CreateCall(function, args_value);
    call_inst = CallInst::Create(function, args_value);
    call_inst->setTailCall(false);
  } else {
    // call_inst = Builder.CreateCall(function, args_value, "__call_exp");
    call_inst = CallInst::Create(function, args_value, "__call_exp");
  }

  call_inst->setCallingConv(CallingConv::C);

  if (noname::debug >= 1) {
    call_inst->dump();
  }

  return call_inst;
}

//----------------------------------------------//
//----------- Processor Strategy ---------------//
//----------------------------------------------//

void* CallExpNodeProcessorStrategy::process(ASTNode* node) {
  CallExpNode* call_exp_node = (CallExpNode*)node;
  FunctionDefNode* function_def_node =
      node->getContext()->getFunction(call_exp_node->getCallee());

  if (function_def_node) {
    if (noname::debug >= 2) {
      fprintf(stdout, "\nThe called function was: '%s'\n",
              function_def_node->getName().c_str());
    }

    NodeValue* return_value = (NodeValue*)call_exp_node->eval();
    print_node_value(stdout, return_value);

  } else {
    fprintf(stderr, "\nError: The function %s was not found int the context\n",
            call_exp_node->getCallee().c_str());
  }
  return nullptr;
}

}