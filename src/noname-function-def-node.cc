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

FunctionSignature::FunctionSignature(const std::string& name, std::vector<FunctionArgument*> args_defs,
                                     llvm::Type* return_type)
    : name(name), args_defs(args_defs), return_type(return_type) {}
FunctionSignature::FunctionSignature(const FunctionSignature& copy)
    : name(copy.name), args_defs(copy.args_defs), return_type(copy.return_type) {}

Function* FunctionSignature::codegen() {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionSignature::codegen for %s]", getName().c_str());
    fflush(stdout);
  }

  std::vector<llvm::Type*> function_args_types(args_defs.size(), Type::getVoidTy(TheContext));
  FunctionType* function_type = FunctionType::get(return_type, function_args_types, false);

  Function* function = Function::Create(function_type, Function::ExternalLinkage, name);
  function->setCallingConv(CallingConv::C);

  // Set names for all arguments.
  int index = 0;
  for (auto& function_arg : function->args()) {
    function_arg.setName(args_defs[index++]->name);
  }

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionSignature %s created unlinked from modules]", getName().c_str());
    fflush(stdout);
    function->dump();
  }

  return function;
}

ASTNode* new_function_def(ASTContext* context, const std::string name, arglist_t* arg_list, stmtlist_t* stmt_list,
                          ExpNode* return_node) {
  FunctionDefNode* function_new_node = new FunctionDefNode(context, name, arg_list, stmt_list, return_node);

  ASTNode* check_result = function_new_node->check();

  if (check_result && isa<ErrorNode>(*check_result)) {
    return check_result;
  }

  context->store(name, new FunctionSignature(*function_new_node->getFunctionSignature()));

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[new_function_def %s]", context->getName().c_str());
    fflush(stdout);
  }

  return function_new_node;
}

FunctionSignature* FunctionDefNode::createFunctionSignature(const std::string& name,
                                                            std::vector<FunctionArgument*> args_defs) {
  llvm::Type* return_type = nullptr;
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[Figuring out the return type of %s]", name.c_str());
    fflush(stdout);
  }
  Value* return_value = nullptr;
  if (return_node) {
    return_value = return_node->codegen();
    if (!return_value) {
      logError("Return value is null or undefined");
      return nullptr;
    }
  }
  return_type = toLLVMType(return_value);

  return new FunctionSignature(name, args_defs, return_type);
}

FunctionDefNode::FunctionDefNode(ASTContext* context, const std::string& name, std::vector<FunctionArgument*> args_defs,
                                 std::vector<std::unique_ptr<ASTNode>> body_nodes, ExpNode* return_node)
    : ASTNode(context, AST_NODE_TYPE_DEF_FUNCTION),
      body_nodes(std::move(body_nodes)),
      return_node(std::move(return_node)),
      function_signature(nullptr) {
  function_signature = createFunctionSignature(name, args_defs);
  if (!function_signature) {
    logError("FunctionDefNode could not be defined");
    return;
  }
}

FunctionDefNode::FunctionDefNode(ASTContext* context, const std::string& name, arglist_t* head_arg_list,
                                 stmtlist_t* head_stmt_list, ExpNode* return_node)
    : ASTNode(context, AST_NODE_TYPE_DEF_FUNCTION),
      body_nodes(std::vector<std::unique_ptr<ASTNode>>()),
      return_node(std::move(return_node)),
      function_signature(nullptr) {
  std::vector<FunctionArgument*> args_defs;

  arglist_node_t* arglist_node = head_arg_list->first;
  stmtlist_node_t* stmtlist_node = head_stmt_list->first;
  do {
    if (arglist_node && arglist_node->arg) {
      FunctionArgument* function_argument =
          new FunctionArgument(std::string(arglist_node->arg->name), VoidTy, arglist_node->arg->default_value);

      args_defs.push_back(function_argument);

      arglist_node = arglist_node->next;
    }
  } while (arglist_node);

  do {
    if (stmtlist_node && stmtlist_node->node) {
      body_nodes.push_back(std::unique_ptr<ASTNode>(stmtlist_node->node));
      stmtlist_node = stmtlist_node->next;
    }
  } while (stmtlist_node);

  function_signature = createFunctionSignature(name, args_defs);
  if (!function_signature) {
    logError("FunctionDefNode could not be defined");
    return;
  }
}
FunctionDefNode::~FunctionDefNode() {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionDefNode::~FunctionDefNode() for %s]", getName().c_str());
    fflush(stdout);
  }
}

ASTNode* FunctionDefNode::check() {
  ASTContext* context = getContext();
  Function* function = TheModule->getFunction(getName());

  if (function) {
    char msg[2048];
    snprintf(msg, 2048, "Function '%s' already exists in this context. %s", getName().c_str(),
             context->getName().c_str());
    return new LogicErrorNode(context, msg);
  }

  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes = body_nodes.begin();

  for (; it_body_nodes != body_nodes.end();) {
    std::unique_ptr<ASTNode>& bodyNode = *it_body_nodes;

    if (isa<ImportNode>(*bodyNode.get())) {
      return new InvalidStatement(context, "Cannot import inside function definition");
    }

    if (noname::debug >= 2) {
      fprintf(stdout, "\n[## evaluating body: ASTNode of type %s]\n",
              ASTNode::toString(bodyNode.get()->getKind()).c_str());
      fflush(stdout);
    }
    ++it_body_nodes;
  }

  if (return_node && isa<ImportNode>(*return_node)) {
    return new InvalidStatement(context, "Cannot import inside function definition");
  }

  return this;
}

Function* FunctionDefNode::getFunctionDefinition() {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionDefNode::getFunctionDefinition for %s]", getName().c_str());
    fflush(stdout);
  }

  // First, see if the function has already been added to the current module.
  Function* function = TheModule->getFunction(getName());

  if (function) {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[Function %s found]", getName().c_str());
      fflush(stdout);
    }
    return function;
  }

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[Function %s NOT found]", getName().c_str());
    fflush(stdout);
  }

  function = function_signature->codegen();

  return function;
}
llvm::ReturnInst* FunctionDefNode::getLLVMReturnInst(Value* return_value) {
  ReturnInst* return_inst = nullptr;
  // Finish off the function by creating the ReturnInst
  if (!return_value) {
    if (noname::debug >= 2) {
      fprintf(stdout, "\n[## no return_value nullptr]\n");
      fflush(stdout);
    }
    // Builder.CreateRetVoid();
    return_inst = ReturnInst::Create(TheContext);

  } else {
    if (noname::debug >= 2) {
      fprintf(stdout, "\n[## return_value]\n");
      fflush(stdout);
      return_value->dump();
    }

    // Builder.CreateRet(return_value);
    return_inst = ReturnInst::Create(TheContext, return_value);
  }

  return return_inst;
}
Value* FunctionDefNode::codegen(BasicBlock* bb) {
  // fprintf(stdout, "\n[## codegen of %s ]", name.c_str());
  // fflush(stdout);

  ExpNode* return_node = getReturnNode();
  Value* return_value = nullptr;
  if (return_node) {
    return_value = return_node->codegen();
  }
  Function* function = getFunctionDefinition();
  ReturnInst* return_inst = getLLVMReturnInst(return_value);

  if (!function) {
    fprintf(stdout, "\nError: function %s not defined", getName().c_str());
    fflush(stdout);
    return nullptr;
  }

  // Define function inside Module
  TheModule->getFunctionList().push_back(function);

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[Function %s declared inside Module %s]", getName().c_str(),
            TheModule->getName().str().c_str());
    fflush(stdout);
    function->dump();
  }

  // Create a new basic block to start insertion into.
  BasicBlock* function_bb = BasicBlock::Create(TheContext, "entry", function);
  // Builder.SetInsertPoint(function_bb);

  ASTContext* function_def_node_context = getContext();
  std::vector<FunctionArgument*>& signature_args = getFunctionArguments();
  std::vector<FunctionArgument*>::iterator it_signature_args = signature_args.begin();

  while (it_signature_args != signature_args.end()) {
    FunctionArgument* signature_arg = *it_signature_args++;
    NodeValue* arg_node_value = NULL;

    if (signature_arg->default_value) {
      arg_node_value = std::move(signature_arg->default_value->getValue().get());
    }

    function_def_node_context->store(signature_arg->name, arg_node_value);
  }

  std::vector<std::unique_ptr<ASTNode>>& body_nodes = getBodyNodes();
  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes = body_nodes.begin();
  while (it_body_nodes != body_nodes.end()) {
    std::unique_ptr<ASTNode>& body_node = *it_body_nodes++;

    Instruction* body_codegen_value = (Instruction*)body_node->codegen();
    function_bb->getInstList().push_back(body_codegen_value);

    if (noname::debug >= 1) {
      fprintf(stdout, "\n[## codegen of body statement (type %s)]", ASTNode::toString(body_node->getKind()).c_str());
      fflush(stdout);
      body_codegen_value->dump();
    }
  }

  if (return_value) {
    if (isa<CallInst>(return_value)) {
      function_bb->getInstList().push_back((Instruction*)return_value);
    }
  }

  if (!return_inst) {
    function->eraseFromParent();
    return nullptr;
  }

  function_bb->getInstList().push_back(return_inst);

  // Validate the generated code, checking for consistency.
  verifyFunction(*function);

  // Run the optimizer on the function.
  TheFPM->run(*function);

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[Function %s defined inside Module %s]", getName().c_str(), TheModule->getName().str().c_str());
    fflush(stdout);
    function->dump();
  }

  return function;
}

//----------------------------------------------//
//----------- Processor Strategy ---------------//
//----------------------------------------------//

void* FunctionDefNodeProcessorStrategy::process(ASTNode* node) {
  FunctionDefNode* function_def_node = (FunctionDefNode*)node;
  Function* function = (Function*)function_def_node->codegen();

  if (!function) {
    fprintf(stdout, "\nFunction could not be defined");
    fflush(stdout);
    return nullptr;
  }

  if (function) {
    if (noname::debug >= 1) {
      fprintf(stdout, "\nRead function definition:");
      fflush(stdout);
      function->dump();
    }
    if (false) {
      TheJIT->writeToFile(TheModule.get());
      TheJIT->addModule(std::move(TheModule));
      InitializeModuleAndPassManager();
    }
  }

  // std::unique_ptr<NodeValue> return_value(function_def_node->getValue());
  // print_node_value(stdout, return_value.get());

  return nullptr;
}
}