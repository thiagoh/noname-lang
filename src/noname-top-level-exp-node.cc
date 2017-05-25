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
    fprintf(stdout, "\n[TopLevelExpNode::TopLevelExpNode() called]");
    fflush(stdout);
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

ASTNode* create_anonymous_function_def_node(ASTContext* context, ExpNode* exp_node) {
  ReturnExpNode* return_exp_node = nullptr;

  if (isa<ReturnExpNode>(exp_node)) {
    return_exp_node = (ReturnExpNode*)exp_node;
  } else if (!exp_node) {
    return new ErrorNode(context, "No such expression node defined for anonymous function");
  } else {
    return_exp_node = new_return_exp_node(exp_node);
  }

  const std::string annon_name = "__anon_expr";
  std::unique_ptr<arglist_t> arg_list(new_arg_list(context));
  std::unique_ptr<stmtlist_t> stmt_list(new_stmt_list(context, return_exp_node));

  return new_function_def(context, annon_name, arg_list.get(), stmt_list.get());
}
ASTNode* new_top_level_exp_node(ExpNode* exp_node) {
  CreateNewModuleAndInitialize();

  if (simple_version) {
    return new TopLevelExpNode(nullptr, nullptr, nullptr, nullptr);
  }

  ASTContext* top_level_context = exp_node->getContext();
  CallExpNode* called_function_call_exp_node = (CallExpNode*)exp_node;
  ASTNode* function_def_node = create_anonymous_function_def_node(top_level_context, called_function_call_exp_node);

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
std::vector<Value*> TopLevelExpNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  std::vector<Value*> codegen;

  if (simple_version) {
    llvm::Function* called_function = nullptr;
    {
      std::vector<llvm::Type*> called_function_args_types(0, Type::getVoidTy(TheContext));
      FunctionType* called_function_type =
          FunctionType::get(Type::getInt64Ty(TheContext), called_function_args_types, false);
      called_function = Function::Create(called_function_type, Function::ExternalLinkage, "fun");
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

    ConstantInt* const_int32_14 = ConstantInt::get(TheContext, APInt(32, StringRef("1"), 10));
    ConstantInt* const_int32_15 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));
    ConstantInt* const_int64_16 = ConstantInt::get(TheContext, APInt(64, StringRef("16"), 10));
    ConstantInt* const_int32_17 = ConstantInt::get(TheContext, APInt(32, StringRef("8"), 10));
    ConstantInt* const_int1_18 = ConstantInt::get(TheContext, APInt(1, StringRef("0"), 10));
    ConstantInt* const_int64_19 = ConstantInt::get(TheContext, APInt(64, StringRef("786"), 10));
    ConstantInt* const_int32_20 = ConstantInt::get(TheContext, APInt(32, StringRef("999"), 10));
    ConstantInt* const_int32_21 = ConstantInt::get(TheContext, APInt(32, StringRef("333"), 10));
    ConstantInt* const_int32_22 = ConstantInt::get(TheContext, APInt(32, StringRef("3333333"), 10));

    // Block entry (label_entry_34)
    AllocaInst* ptr_my_integer_var = new AllocaInst(IntegerType::get(TheContext, 64), "my_integer_var");
    ptr_my_integer_var->setAlignment(8);
    ptr_my_integer_var->dump();
    AllocaInst* ptr_my_datatype = new AllocaInst(StructTy_struct_datatype_t, "my_datatype");
    ptr_my_datatype->setAlignment(8);
    ptr_my_datatype->dump();
    StoreInst* void_35 = new StoreInst(const_int64_19, ptr_my_integer_var, false);
    void_35->setAlignment(8);
    void_35->dump();
    GetElementPtrInst* ptr_type = GetElementPtrInst::Create(StructTy_struct_datatype_t, ptr_my_datatype,
                                                            {const_int32_15, const_int32_15}, "type");
    ptr_type->dump();
    StoreInst* void_36 = new StoreInst(const_int32_20, ptr_type, false);
    void_36->setAlignment(8);
    void_36->dump();
    CastInst* ptr_37 = new BitCastInst(ptr_my_integer_var, PointerTy_8, "");
    ptr_37->dump();
    GetElementPtrInst* ptr_v =
        GetElementPtrInst::Create(StructTy_struct_datatype_t, ptr_my_datatype, {const_int32_15, const_int32_14}, "v");
    ptr_v->dump();
    StoreInst* void_38 = new StoreInst(ptr_37, ptr_v, false);
    void_38->setAlignment(8);
    void_38->dump();

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
    createError(error, "Could not resolve top level expression");
    return codegen;
  }

  codegen.push_back(anonymous_function);

  if (noname::debug >= 1) {
    fprintf(stdout, "\n[TopLevelExpNode::codegen_elements anonymous_function->dump()]\n");
    fflush(stdout);
    anonymous_function->dump();
  }

  // std::vector<Value*> call_exp_node_codegen_elements = call_exp_node->get_codegen_elements(error);
  // if (error.code()) {
  //   logError(error.what().c_str());
  //   return codegen;
  // }

  // for (auto current_value : call_exp_node_codegen_elements) {
  //   codegen.push_back(current_value);
  //   if (noname::debug >= 1) {
  //     fprintf(stdout, "\n[TopLevelExpNode::codegen_elements current_value->dump()]\n");
  //     fflush(stdout);
  //     current_value->dump();
  //   }
  // }

  return codegen;
}
Value* TopLevelExpNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }

void* TopLevelExpNodeProcessorStrategy::process(ASTNode* node) {
  TopLevelExpNode* top_level_exp_node = (TopLevelExpNode*)node;

  // http://llvm.org/docs/doxygen/html/classllvm_1_1ExecutionEngine.html#a97bbf524ee03354bb73dce9614b0e959

  Error error;
  std::vector<Value*> elements(top_level_exp_node->get_codegen_elements(error));

  if (error.code()) {
    return logErrorLLVM(error.what().c_str());
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

    llvm::Type* result_type = nullptr;

    // getContext()->getValueType()

    result_type = toLLVMType(elements.back());
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