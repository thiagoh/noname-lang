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

void CallExpNode::initializeArgs(explist_t* head_exp_list) {
  if (head_exp_list) {
    explist_node_t* explist_node_t = head_exp_list->first;
    do {
      if (explist_node_t && explist_node_t->node) {
        args.push_back(std::unique_ptr<ExpNode>(std::move(explist_node_t->node)));
        explist_node_t = explist_node_t->next;
      }
    } while (explist_node_t);
    // TODO: free all the expressions not just the head_exp_list one
    // free(head_exp_list);
  }
}

CallExpNode::CallExpNode(ASTContext* context, const std::string& callee, explist_t* head_exp_list)
    : ExpNode(context, AST_NODE_TYPE_CALL_EXP),
      callee(callee),
      called_function(nullptr),
      args(std::vector<std::unique_ptr<ExpNode>>()) {
  initializeArgs(head_exp_list);
}

CallExpNode::CallExpNode(ASTContext* context, llvm::Function* called_function, explist_t* head_exp_list)
    : ExpNode(context, AST_NODE_TYPE_CALL_EXP),
      callee(""),
      called_function(called_function),
      args(std::vector<std::unique_ptr<ExpNode>>()) {
  initializeArgs(head_exp_list);
  if (!called_function) {
    logError("Function is not defined");
    return;
  }
  callee = called_function->getName().str();
}
CallExpNode::~CallExpNode() {
  if (noname::debug >= 1) {
    fprintf(stderr, "\n[CallExpNode::~CallExpNode() called]");
  }
}

CallExpNode* new_call_node(ASTContext* context, const std::string name, explist_t* arg_exp_list) {
  CallExpNode* new_node = new CallExpNode(context, name, arg_exp_list);
  return new_node;
}
CallExpNode* new_call_node(ASTContext* context, Function* function, explist_t* arg_exp_list) {
  CallExpNode* new_node = new CallExpNode(context, function, arg_exp_list);
  return new_node;
}
llvm::Function* CallExpNode::getCalledFunction(Error** error) {
    ASTContext* call_exp_context = getContext();
  llvm::Function* function = TheModule->getFunction(getCallee());

  if (!function) {
    FunctionSignature* function_signature = call_exp_context->getFunctionSignature(getCallee());

    if (!function_signature) {
      char msg[1024];
      sprintf(msg, "Could not find function signature '%s' referenced", getCallee().c_str());
      *error = createError(msg);
      return nullptr;
    }

    function = function_signature->codegen();
  }

  return function;
}

std::vector<Value*> CallExpNode::codegen_elements(Error** error, llvm::BasicBlock* bb) {
  std::vector<Value*> codegen;
  ASTContext* call_exp_context = getContext();

  if (!called_function) {
    called_function = getCalledFunction(error);
  }

  if (!called_function) {
    if (!error) {  // error may be already set
      char msg[1024];
      sprintf(msg, "Unknown function '%s' referenced", getCallee().c_str());
      *error = createError(msg);
    }
    return codegen;
  }

  const std::vector<std::unique_ptr<ExpNode>>& value_args = getArgs();

  // If argument mismatch error.
  if (called_function->arg_size() != (size_t)value_args.size()) {
    char msg[1024];
    sprintf(msg, "Incorrect # arguments passed for function '%s'", getCallee().c_str());
    *error = createError(msg);
    return codegen;
  }

  iterator_range<Argument*> signature_args = called_function->args();
  llvm::Argument* it_signature_args = signature_args.begin();

  std::vector<std::unique_ptr<ExpNode>>::const_iterator it_value_args = value_args.begin();
  std::vector<llvm::Value*> args_value;

  for (; it_signature_args != signature_args.end() || it_value_args != value_args.end();) {
    const std::unique_ptr<ExpNode>& value_arg = *it_value_args;
    const Argument* signature_arg = it_signature_args;

    call_exp_context->store(signature_arg->getName().str(), std::move(value_arg->getValue().get()));

    args_value.push_back(value_arg->codegen());
    if (!args_value.back()) {
      char msg[1024];
      sprintf(msg, "Invalid or undefined argument for function '%s'", getCallee().c_str());
      *error = createError(msg);
      return codegen;
    }

    ++it_signature_args;
    ++it_value_args;
  }

  llvm::CallInst* call_inst = nullptr;

  if (called_function->getReturnType() == llvm::Type::getVoidTy(TheContext)) {
    // Cannot assign a name to void values!
    // call_inst = Builder.CreateCall(called_function, args_value);
    call_inst = CallInst::Create(called_function, args_value);
    call_inst->setTailCall(false);
  } else {
    // call_inst = Builder.CreateCall(called_function, args_value, "__call_exp");
    call_inst = CallInst::Create(called_function, args_value, "__call_exp");
  }

  call_inst->setCallingConv(CallingConv::C);

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[call_inst->dump]");
    call_inst->dump();
  }

  codegen.push_back(call_inst);
  return codegen;
}
Value* CallExpNode::codegen(llvm::BasicBlock* bb) {
  ;
  return codegen_elements_retlast(this, bb);
}

//----------------------------------------------//
//----------- Processor Strategy ---------------//
//----------------------------------------------//

void* CallExpNodeProcessorStrategy::process(ASTNode* node) {
  CallExpNode* call_exp_node = (CallExpNode*)node;
  Function* function = TheModule->getFunction(call_exp_node->getCallee());

  if (function) {
    if (noname::debug >= 2) {
      fprintf(stdout, "\n[The called function was: '%s']", call_exp_node->getCallee().c_str());
    }

    std::unique_ptr<NodeValue> return_value(call_exp_node->getValue());
    print_node_value(stdout, return_value.get());

  } else {
    char msg[1024];
    sprintf(msg, "The function %s was not found int the context", call_exp_node->getCallee().c_str());
    return logError(msg);
  }

  return nullptr;
}
}