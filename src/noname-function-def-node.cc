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

FunctionDefNode::FunctionDefNode(
    ASTContext* context, const std::string& name,
    std::vector<std::unique_ptr<arg_t>>& args,
    std::vector<std::unique_ptr<ASTNode>>& body_nodes, ExpNode* returnNode)
    : ASTNode(context, AST_NODE_TYPE_DEF_FUNCTION),
      name(name),
      args(std::move(args)),
      bodyNodes(std::move(body_nodes)),
      returnNode(std::move(returnNode)) {
  ;
}

FunctionDefNode::FunctionDefNode(ASTContext* context, const std::string& name,
                                 arglist_t* head_arg_list,
                                 stmtlist_t* head_stmt_list,
                                 ExpNode* returnNode)
    : ASTNode(context, AST_NODE_TYPE_DEF_FUNCTION),
      name(name),
      args(std::vector<std::unique_ptr<arg_t>>()),
      bodyNodes(std::vector<std::unique_ptr<ASTNode>>()),
      returnNode(std::move(returnNode)) {
  arglist_node_t* arglist_node = head_arg_list->first;
  stmtlist_node_t* stmtlist_node = head_stmt_list->first;
  do {
    if (arglist_node && arglist_node->arg) {
      args.push_back(std::unique_ptr<arg_t>(arglist_node->arg));
      arglist_node = arglist_node->next;
    }
  } while (arglist_node);

  do {
    if (stmtlist_node && stmtlist_node->node) {
      bodyNodes.push_back(std::unique_ptr<ASTNode>(stmtlist_node->node));
      stmtlist_node = stmtlist_node->next;
    }
  } while (stmtlist_node);
}

ASTNode* FunctionDefNode::check() {
  ASTContext* context = getContext();

  FunctionDefNode* function_node = context->getFunction(name);
  if (function_node) {
    char error_message[2048];
    snprintf(error_message, 2048,
             "Function '%s' already exists in this context. %s", name.c_str(),
             context->getName().c_str());
    return new LogicErrorNode(context, error_message);
  }

  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes =
      bodyNodes.begin();

  for (; it_body_nodes != bodyNodes.end();) {
    std::unique_ptr<ASTNode>& bodyNode = *it_body_nodes;

    if (isa<ImportNode>(*bodyNode.get())) {
      return new InvalidStatement(context,
                                  "Cannot import inside function definition");
    }

    if (yydebug >= 2) {
      fprintf(stdout, "\n[## evaluating body: ASTNode of type %s]\n",
              ASTNode::toString(bodyNode.get()->getKind()).c_str());
    }
    ++it_body_nodes;
  }

  if (returnNode && isa<ImportNode>(*returnNode)) {
    return new InvalidStatement(context,
                                "Cannot import inside function definition");
  }

  return this;
}

llvm::Type* FunctionDefNode::getReturnLLVMType(LLVMContext& TheContext) {
  if (!returnLLVMType) {
    fprintf(stderr, "Function %s has no return type set", name.c_str());
    exit(1);
    return llvm::Type::getVoidTy(TheContext);
  }

  return returnLLVMType;
}

Function* FunctionDefNode::getFunctionDefinition(Value* return_value) {
  // First, see if the function has already been added to the current module.
  Function* function = TheModule->getFunction(name);

  if (function) {
    return function;
  }

  llvm::Type* return_type = nullptr;
  if (!return_value) {
    ExpNode* return_node = getReturnNode();
    if (return_node) {
      return_value = return_node->codegen();
      ReturnInst* return_inst = getLLVMReturnInst(return_value);
      if (!return_inst) {
        return logErrorLLVMF("ReturnInst is null");
      }
    }
  }
  return_type = getLLVMReturnInstType(return_value);

  // Make the function type:  double(double,double) etc.
  std::vector<llvm::Type*> arg_types(args.size(), Type::getVoidTy(TheContext));

  FunctionType* function_type =
      FunctionType::get(return_type, arg_types, false);

  function = Function::Create(function_type, Function::ExternalLinkage, name,
                              TheModule.get());
  function->setCallingConv(CallingConv::C);

  // Set names for all arguments.
  int index = 0;
  for (auto& function_arg : function->args()) {
    function_arg.setName(args[index++]->name);
  }

  return function;
}
ASTNode* createAnnonymousFunctionDefNode(ASTContext* context,
                                         ExpNode* exp_node) {
  const std::string annon_name = "__anon_expr";
  arglist_t* arg_list = new_arg_list(context);
  stmtlist_t* stmt_list = new_stmt_list(context);
  ExpNode* return_node = exp_node;

  auto* function_def_node =
      new_function_def(context, annon_name, arg_list, stmt_list, return_node);

  release(arg_list);
  release(stmt_list);

  return function_def_node;
}
llvm::Type* FunctionDefNode::getLLVMReturnInstType(llvm::Value* return_value) {
  llvm::Type* type = nullptr;

  if (!return_value) {
    type = llvm::Type::getVoidTy(TheContext);
  } else if (isa<CallInst>(return_value)) {
    CallInst* call_inst = (CallInst*)return_value;
    Function* function = call_inst->getCalledFunction();
    if (!function) {
      logError("Could not find function");
      return nullptr;
    }
    type = function->getReturnType();
    if (!type) {
      logError("Function returning invalid type");
      return nullptr;
    }
  } else {
    type = return_value->getType();
  }
  this->returnLLVMType = type;

  return type;
}
llvm::ReturnInst* FunctionDefNode::getLLVMReturnInst(Value* return_value) {
  ReturnInst* return_inst = nullptr;
  // Finish off the function by creating the ReturnInst
  if (!return_value) {
    if (yydebug >= 1) {
      fprintf(stderr, "\n[## no return_value nullptr]\n");
    }
    // Builder.CreateRetVoid();
    return_inst = ReturnInst::Create(TheContext);

  } else {
    if (yydebug >= 1) {
      return_value->dump();
      return_value->print(dbgs(), true);
    }

    // Builder.CreateRet(return_value);
    return_inst = ReturnInst::Create(TheContext, return_value);
  }

  return return_inst;
}
Value* FunctionDefNode::codegen() {
  // fprintf(stderr, "\n[## codegen of %s ]", name.c_str());

  ExpNode* return_node = getReturnNode();
  Value* return_value = nullptr;
  if (return_node) {
    return_value = return_node->codegen();
  }
  ReturnInst* return_inst = getLLVMReturnInst(return_value);
  Function* function = getFunctionDefinition(return_value);

  if (!function) {
    fprintf(stderr, "\nError: function %s not defined", name.c_str());
    return nullptr;
  }

  // Create a new basic block to start insertion into.
  BasicBlock* bb = BasicBlock::Create(TheContext, "entry", function);
  // Builder.SetInsertPoint(bb);

  ASTContext* function_def_node_context = getContext();
  std::vector<std::unique_ptr<arg_t>>* signature_args = &getArgs();
  std::vector<std::unique_ptr<arg_t>>::iterator it_signature_args =
      signature_args->begin();
  std::vector<std::unique_ptr<ASTNode>>* body_nodes = &getBodyNodes();
  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes =
      body_nodes->begin();

  while (it_signature_args != signature_args->end()) {
    std::unique_ptr<arg_t>& signature_arg = *it_signature_args++;

    NodeValue* arg_node_value = NULL;

    if (signature_arg->default_value) {
      arg_node_value = signature_arg->default_value->getValue();
    }

    function_def_node_context->store(signature_arg->name, arg_node_value);
  }

  while (it_body_nodes != body_nodes->end()) {
    std::unique_ptr<ASTNode>& body_node = *it_body_nodes++;

    if (yydebug >= 1) {
      fprintf(stderr, "\n[## codegen of body statement (type %s)]",
              ASTNode::toString(body_node->getKind()).c_str());
    }

    Instruction* body_codegen_value = (Instruction*)body_node->codegen();
    bb->getInstList().push_back(body_codegen_value);

    if (yydebug >= 1) {
      body_codegen_value->dump();
    }
  }

  if (isa<CallInst>(return_value)) {
    bb->getInstList().push_back((Instruction*)return_value);
  }

  if (yydebug >= 1) {
    fprintf(stderr, "\n[## adding return inst]");
  }

  if (!return_inst) {
    function->eraseFromParent();
    return nullptr;
  }

  bb->getInstList().push_back(return_inst);

  // Validate the generated code, checking for consistency.
  verifyFunction(*function);

  // Run the optimizer on the function.
  TheFPM->run(*function);

  if (yydebug >= 1) {
    function->dump();
  }

  return function;
}
}