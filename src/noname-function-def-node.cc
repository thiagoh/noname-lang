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

FunctionSignature::~FunctionSignature() {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionSignature::~FunctionSignature() for %s]", getName().c_str());
    fflush(stdout);
  }
}

Function* FunctionSignature::codegen() {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionSignature::codegen for %s]", getName().c_str());
    fflush(stdout);
  }

  // std::vector<llvm::Type*> function_args_types(args_defs.size(), Type::getVoidTy(TheContext));
  std::vector<llvm::Type*> function_args_types(args_defs.size(), StructTy_struct_datatype_t);
  // std::vector<llvm::Type*> function_args_types;
  // for (unsigned int i = 0; i < args_defs.size(); i++) {
  //   function_args_types.push_back(IntegerType::get(TheContext, 32));
  //   function_args_types.push_back(PointerTy_8);
  // }

  FunctionType* function_type = FunctionType::get(return_type, function_args_types, false);

  Function* function = Function::Create(function_type, Function::ExternalLinkage, name);
  function->setCallingConv(CallingConv::C);

  // Set names for all arguments.
  double index = 0.0;
  for (auto& function_arg : function->args()) {
    // function_arg.setName(args_defs[floor(index / 2.0)]->name);
    function_arg.setName(args_defs[index++]->name);
  }

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionSignature %s created unlinked from modules]", getName().c_str());
    fflush(stdout);
    function->dump();
  }

  return function;
}

ASTNode* new_function_def(ASTContext* context, const std::string name, arglist_t* arg_list, stmtlist_t* stmt_list) {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[new_function_def for funtcion '%s']", name.c_str());
    fflush(stdout);
  }

  FunctionDefNode* function_new_node = new FunctionDefNode(context, name, arg_list, stmt_list);

  ASTNode* check_result = function_new_node->check();

  if (check_result && isa<ErrorNode>(*check_result)) {
    return check_result;
  }

  context->storeFunctionSignature(name, new FunctionSignature(*function_new_node->getFunctionSignature()));

  return function_new_node;
}

FunctionSignature* FunctionDefNode::createFunctionSignature(Error& error, const std::string& name,
                                                            std::vector<FunctionArgument*> args_defs) {
  llvm::Type* return_type = nullptr;
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[Figuring out the return type of %s]", name.c_str());
    fflush(stdout);
  }
  // Value* return_value = nullptr;
  // if (auto& return_node = getReturnNode()) {
  //   std::vector<Value*> return_node_codegen_elements = return_node->get_codegen_elements(error);

  //   if (error.code()) {
  //     logError(error.what().c_str());
  //     return nullptr;
  //   }

  //   for (auto current_value : return_node_codegen_elements) {
  //     if (isa<ReturnInst>(current_value)) {
  //       return_value = current_value;
  //     }
  //   }

  //   if (!return_value) {
  //     logError("Return value is null or undefined");
  //     return nullptr;
  //   }
  // }
  // return_type = toLLVMType(return_value);
  return_type = StructTy_struct_datatype_t;

  return new FunctionSignature(name, args_defs, return_type);
}

FunctionDefNode::FunctionDefNode(ASTContext* context, const std::string& name, std::vector<FunctionArgument*> args_defs,
                                 std::vector<std::unique_ptr<ASTNode>> body_nodes)
    : ASTNode(context, AST_NODE_TYPE_DEF_FUNCTION), body_nodes(std::move(body_nodes)), function_signature(nullptr) {
  Error error;
  function_signature = createFunctionSignature(error, name, args_defs);

  if (error.code()) {
    logError(error.what().c_str());
    return;
  }

  if (!function_signature) {
    logError("FunctionDefNode could not be defined");
    return;
  }
}

FunctionDefNode::FunctionDefNode(ASTContext* context, const std::string& name, arglist_t* head_arg_list,
                                 stmtlist_t* head_stmt_list)
    : ASTNode(context, AST_NODE_TYPE_DEF_FUNCTION),
      body_nodes(std::vector<std::unique_ptr<ASTNode>>()),
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

  Error error;
  function_signature = createFunctionSignature(error, name, args_defs);

  if (error.code()) {
    logError(error.what().c_str());
    return;
  }

  if (!function_signature) {
    logError("FunctionDefNode could not be defined");
    return;
  }
}
FunctionDefNode::~FunctionDefNode() {
  delete function_signature;
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[FunctionDefNode::~FunctionDefNode() for %s]", getName().c_str());
    fflush(stdout);
  }
}

ASTNode* FunctionDefNode::check() const {
  ASTContext* context = getContext();

  if (!function_signature) {
    char msg[2048];
    snprintf(msg, 2048, "Function signature is not set");
    return new LogicErrorNode(context, msg);
  }

  Function* function = TheModule->getFunction(getName());

  if (function) {
    char msg[2048];
    snprintf(msg, 2048, "Function '%s' already exists in this context. %s", getName().c_str(),
             context->getName().c_str());
    return new LogicErrorNode(context, msg);
  }

  std::vector<std::unique_ptr<ASTNode>>::const_iterator it_body_nodes = body_nodes.begin();

  for (; it_body_nodes != body_nodes.end();) {
    const std::unique_ptr<ASTNode>& bodyNode = *it_body_nodes++;

    if (isa<ImportNode>(*bodyNode.get())) {
      return new InvalidStatement(context, "Cannot import inside function definition");
    }

    if (noname::debug >= 2) {
      fprintf(stdout, "\n[## evaluating body: ASTNode of type %s]\n",
              ASTNode::toString(bodyNode.get()->getKind()).c_str());
      fflush(stdout);
    }
  }

  return nullptr;
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
llvm::ReturnInst* FunctionDefNode::getLLVMReturnInst(Value* return_value) const {
  ReturnInst* return_inst = nullptr;
  // Finish off the function by creating the ReturnInst
  if (!return_value) {
    if (noname::debug >= 2) {
      fprintf(stdout, "\n[## no return_value nullptr]\n");
      fflush(stdout);
    }
    return_inst = ReturnInst::Create(TheContext);

  } else {
    if (noname::debug >= 2) {
      fprintf(stdout, "\n[## return_value]\n");
      fflush(stdout);
      return_value->dump();
    }

    return_inst = ReturnInst::Create(TheContext, return_value);
  }

  return return_inst;
}
Value* FunctionDefNode::codegen(BasicBlock* bb) {
  // fprintf(stdout, "\n[## codegen of %s ]", name.c_str());
  // fflush(stdout);

  auto& return_node = getReturnNode();
  Function* function = getFunctionDefinition();

  if (!function) {
    fprintf(stdout, "\nError: function %s not defined", getName().c_str());
    fflush(stdout);
    return nullptr;
  }

  // Define function inside Module
  TheModule->getFunctionList().push_back(function);

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[Function %s declared inside Module %s]", getName().c_str(), TheModule->getName().str().c_str());
    fflush(stdout);
    function->dump();
  }

  // Create a new basic block to start insertion into.
  BasicBlock* function_bb = BasicBlock::Create(TheContext, "entry", function);

  ASTContext* function_def_node_context = getContext();
  std::vector<FunctionArgument*>& signature_args = getFunctionArguments();
  std::vector<FunctionArgument*>::iterator it_signature_args = signature_args.begin();

  llvm::Function::arg_iterator it_function_args = function->arg_begin();
  while (it_function_args != function->arg_end()) {
    FunctionArgument* signature_arg = *it_signature_args++;

    // NodeValue* arg_node_value = NULL;

    // if (signature_arg->default_value) {
    //   arg_node_value = std::move(signature_arg->default_value->getValue().get());
    // }

    // function_def_node_context->storeVariable(signature_arg->name, arg_node_value);

    /** 
      * Do we really need to create this AllocaInst?
      */
    // AllocaInst* alloca_inst = alloca_typed_var_codegen(TYPE_VOID_POINTER);
    // if (!alloca_inst) {
    //   fprintf(stdout, "\n[## Argument could not be allocated]");
    //   fflush(stdout);
    //   return nullptr;
    // }
    // function_def_node_context->storeAllocaInst(signature_arg->name, alloca_inst);
    // function_bb->getInstList().push_back(alloca_inst);

    // Argument* function_arg_type = (Argument*)it_function_args++;
    // Argument* function_arg_value = (Argument*)it_function_args++;
    // function_def_node_context->storeValue(signature_arg->name + "_type", function_arg_type);
    // function_def_node_context->storeValue(signature_arg->name + "_value", function_arg_value);

    Argument* function_arg = (Argument*)it_function_args++;
    function_def_node_context->storeValue(signature_arg->name, function_arg);
  }

  std::vector<std::unique_ptr<ASTNode>>& body_nodes = getBodyNodes();
  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes = body_nodes.begin();

  ReturnInst* return_inst = nullptr;
  while (it_body_nodes != body_nodes.end()) {
    std::unique_ptr<ASTNode>& body_node = *it_body_nodes++;

    if (noname::debug >= 1) {
      fprintf(stdout, "\n[## codegen of body statement (type %s)]", ASTNode::toString(body_node->getKind()).c_str());
      fflush(stdout);
    }

    Error error;
    std::vector<Value*> body_node_codegen_elements = body_node->get_codegen_elements(error, function_bb);

    if (error.code()) {
      return logErrorLLVM(error.what().c_str());
    }

    for (auto current_value : body_node_codegen_elements) {
      Instruction* instruction_codegen_value = (Instruction*)current_value;

      if (noname::debug >= 1) {
        instruction_codegen_value->dump();
      }

      // function_bb->getInstList().push_back(instruction_codegen_value);

      if (isa<ReturnInst>(instruction_codegen_value)) {
        return_inst = (ReturnInst*)instruction_codegen_value;
      }
    }
  }

  if (!return_inst) {
    Value* return_value = nullptr;
    if (return_node) {
      return_value = return_node->codegen();
    }

    return_inst = getLLVMReturnInst(return_value);
  }

  if (!return_inst) {
    function->eraseFromParent();
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[Function %s was erased from Module %s]", getName().c_str(),
              TheModule->getName().str().c_str());
      fflush(stdout);
    }
    return nullptr;
  }

  // function_bb->getInstList().push_back(return_inst);

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