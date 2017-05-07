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

Value* codegen_elements_retlast(ASTNode* node, llvm::BasicBlock* bb) {
  Error* error = nullptr;
  std::vector<std::unique_ptr<Value>> codegen_elements(node->codegen_elements(&error));

  if (error) {
    return logErrorLLVM(error->what().c_str());
  }

  llvm::Value* last = nullptr;
  for (std::unique_ptr<Value>& ptr : codegen_elements) {
    last = ptr.release();
    if (bb) {
      if (isa<llvm::Instruction>(last)) {
        bb->getInstList().push_back((llvm::Instruction*)last);
      }
    }
  }

  return last;
}

AllocaInst* declaration_codegen_util(ASTNode* node, llvm::BasicBlock* bb) {
  std::string alloca_name = "untyped_poiter_alloca_";
  /**
   * user level untyped variable
   * declare the "untyped"  variable that will ultimately point to the value
   */

  if (isa<DeclarationNode>(node)) {
    DeclarationNode* declaration = (DeclarationNode*)node;
    alloca_name += declaration->getName();

  } else if (isa<DeclarationAssignmentNode>(node)) {
    DeclarationAssignmentNode* declaration = (DeclarationAssignmentNode*)node;
    alloca_name += declaration->getName();
  }

  AllocaInst* untyped_poiter_alloca = new AllocaInst(PointerTy_4, alloca_name);
  untyped_poiter_alloca->setAlignment(8);

  return untyped_poiter_alloca;
}

AllocaInst* alloca_typed_var_codegen(int type, llvm::BasicBlock* bb = nullptr) {
  AllocaInst* typed_pointer_alloca = nullptr;

  if (type == TYPE_DOUBLE) {
    typed_pointer_alloca = new AllocaInst(Type::getDoubleTy(TheContext), "alloca_double_v");
    typed_pointer_alloca->setAlignment(8);

  } else if (type == TYPE_FLOAT) {
    typed_pointer_alloca = new AllocaInst(Type::getFloatTy(TheContext), "alloca_float_v");
    typed_pointer_alloca->setAlignment(4);

  } else if (type == TYPE_LONG) {
    typed_pointer_alloca = new AllocaInst(IntegerType::get(TheContext, 64), "alloca_long_v");
    typed_pointer_alloca->setAlignment(8);

  } else if (type == TYPE_INT) {
    typed_pointer_alloca = new AllocaInst(IntegerType::get(TheContext, 32), "alloca_int_v");
    typed_pointer_alloca->setAlignment(4);

  } else if (type == TYPE_SHORT) {
    typed_pointer_alloca = new AllocaInst(IntegerType::get(TheContext, 16), "alloca_short_v");
    typed_pointer_alloca->setAlignment(2);

  } else if (type == TYPE_CHAR) {
    typed_pointer_alloca = new AllocaInst(IntegerType::get(TheContext, 8), "alloca_char_v");
    typed_pointer_alloca->setAlignment(1);
  }

  return typed_pointer_alloca;
}

StoreInst* store_typed_var_codegen(int type, Value* value, llvm::BasicBlock* bb = nullptr) {
  StoreInst* store_inst = new StoreInst(value, value, false, bb);

  if (type == TYPE_DOUBLE) {
    store_inst->setAlignment(8);

  } else if (type == TYPE_FLOAT) {
    store_inst->setAlignment(4);

  } else if (type == TYPE_LONG) {
    store_inst->setAlignment(8);

  } else if (type == TYPE_INT) {
    store_inst->setAlignment(4);

  } else if (type == TYPE_SHORT) {
    store_inst->setAlignment(2);

  } else if (type == TYPE_CHAR) {
    store_inst->setAlignment(1);
  }

  return store_inst;
}

StoreInst* store_untyped_var_codegen(int type, CastInst* cast_inst_from, AllocaInst* alloca_inst_to,
                                     llvm::BasicBlock* bb = nullptr) {
  StoreInst* store_inst = new StoreInst(cast_inst_from, alloca_inst_to, false, bb);

  if (type == TYPE_DOUBLE) {
    store_inst->setAlignment(8);

  } else if (type == TYPE_FLOAT) {
    store_inst->setAlignment(4);

  } else if (type == TYPE_LONG) {
    store_inst->setAlignment(8);

  } else if (type == TYPE_INT) {
    store_inst->setAlignment(4);

  } else if (type == TYPE_SHORT) {
    store_inst->setAlignment(2);

  } else if (type == TYPE_CHAR) {
    store_inst->setAlignment(1);
  }

  return store_inst;
}

CastInst* cast_codegen(int type, AllocaInst* alloca_inst_from, llvm::BasicBlock* bb = nullptr) {
  CastInst* casted_inst = new BitCastInst(alloca_inst_from, PointerTy_4, "cast_inst_");

  return casted_inst;
}

std::vector<std::unique_ptr<Value>> assign_codegen_util(AllocaInst* untyped_poiter_alloca, AssignmentNode* assignment,
                                                        llvm::BasicBlock* bb) {
  /**
    * Instructions for this method can be found at:docs/declare-and-assign.cc
    */

  const std::unique_ptr<ExpNode>& rhs = assignment->getRHS();

  // std::vector<std::unique_ptr<Value>> value_codegen_elements =
  // rhs->codegen_elements();
  // codegen.reserve(value_codegen_elements.size());
  // // codegen.insert(codegen.end(), value_codegen_elements.begin(),
  // value_codegen_elements.end());
  // for (auto && uptr : value_codegen_elements) {
  //   codegen.push_back(std::move(uptr));
  // }

  // create the actual Constant value
  //  -->  ConstantInt* value_const = ConstantInt::get(TheContext, APInt(32,
  //  StringRef("100"), 10));
  Value* value_codegen = rhs->codegen();
  int rhs_type = toNonameType(value_codegen);

  // alocate the "typed" variable that will handle the Constant value
  //  -->  AllocaInst* typed_pointer_alloca = new
  //  AllocaInst(IntegerType::get(TheContext, 32), "int_v");
  //  -->  typed_pointer_alloca->setAlignment(4);
  AllocaInst* alloca_inst = alloca_typed_var_codegen(rhs_type, bb);

  // store the Constant into the allocated "typed" variable
  //  -->  StoreInst* void_34 = new StoreInst(value_const, typed_pointer_alloca, false);
  //  -->  void_34->setAlignment(4);
  StoreInst* store_inst_typed_var = store_typed_var_codegen(rhs_type, value_codegen, bb);

  // Cast the the "typed" variable to the "untyped" variable
  //  -->  CastInst* casted_inst = new BitCastInst(typed_pointer_alloca, PointerTy_4, "");
  //  -->  codegen.push_back(std::unique_ptr<Value>(casted_inst));
  CastInst* cast_inst = cast_codegen(rhs_type, alloca_inst, bb);

  // Store the address of the
  //  -->  StoreInst* voidp_store = new StoreInst(casted_inst, untyped_poiter_alloca, false);
  //  -->  voidp_store->setAlignment(8);
  StoreInst* store_inst_untyped_var = store_untyped_var_codegen(rhs_type, cast_inst, alloca_inst, bb);

  std::vector<std::unique_ptr<Value>> codegen;
  codegen.push_back(std::unique_ptr<Value>(value_codegen));
  codegen.push_back(std::unique_ptr<Value>(alloca_inst));
  codegen.push_back(std::unique_ptr<Value>(store_inst_typed_var));
  codegen.push_back(std::unique_ptr<Value>(store_inst_untyped_var));

  return codegen;
}
}