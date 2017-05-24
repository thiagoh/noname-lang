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
  Error error;
  std::vector<Value*> elements(node->get_codegen_elements(error, bb));

  if (error.code()) {
    return logErrorLLVM(error.what().c_str());
  }

  llvm::Value* last = nullptr;
  for (Value* ptr : elements) {
    last = ptr;
    if (bb) {
      if (isa<llvm::Instruction>(last)) {
        bb->getInstList().push_back((llvm::Instruction*)last);
      }
    }
  }

  return last;
}

AllocaInst* declaration_codegen_util(const ASTNode* node, llvm::BasicBlock* bb) {
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

  AllocaInst* untyped_poiter_alloca = new AllocaInst(PointerTy_8, alloca_name);
  untyped_poiter_alloca->setAlignment(8);

  return untyped_poiter_alloca;
}

AllocaInst* alloca_typed_var_codegen(int type, llvm::BasicBlock* bb) {
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

  } else if (type == TYPE_VOID_POINTER) {
    typed_pointer_alloca = new AllocaInst(PointerTy_8, "alloca_char_v");
    typed_pointer_alloca->setAlignment(8);

  } else if (type == TYPE_DATATYPE) {
    typed_pointer_alloca = new AllocaInst(StructTy_struct_datatype, "alloca_datatype_v");
    typed_pointer_alloca->setAlignment(8);
  }

  if (bb && typed_pointer_alloca) {
    bb->getInstList().push_back(typed_pointer_alloca);
  }

  return typed_pointer_alloca;
}

StoreInst* store_typed_var_codegen(int type, llvm::Value* value, llvm::Value* ptr, llvm::BasicBlock* bb) {
  StoreInst* store_inst = new StoreInst(value, ptr, false);

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

  } else if (type == TYPE_VOID_POINTER) {
    store_inst->setAlignment(8);

  } else if (type == TYPE_DATATYPE) {
    store_inst->setAlignment(8);
  }

  if (bb && store_inst) {
    bb->getInstList().push_back(store_inst);
  }

  return store_inst;
}

LoadInst* load_inst_codegen(int type, llvm::AllocaInst* alloca_inst, llvm::BasicBlock* bb) {
  LoadInst* load_inst = new LoadInst(alloca_inst, "load_inst", false);

  if (type == TYPE_DOUBLE) {
    load_inst->setAlignment(8);

  } else if (type == TYPE_FLOAT) {
    load_inst->setAlignment(4);

  } else if (type == TYPE_LONG) {
    load_inst->setAlignment(8);

  } else if (type == TYPE_INT) {
    load_inst->setAlignment(4);

  } else if (type == TYPE_SHORT) {
    load_inst->setAlignment(2);

  } else if (type == TYPE_CHAR) {
    load_inst->setAlignment(1);

  } else if (type == TYPE_VOID_POINTER) {
    load_inst->setAlignment(8);

  } else if (type == TYPE_DATATYPE) {
    load_inst->setAlignment(8);
  }

  if (bb && load_inst) {
    bb->getInstList().push_back(load_inst);
  }

  return load_inst;
}

StoreInst* store_untyped_var_codegen(int type, CastInst* cast_inst_from, AllocaInst* alloca_inst_to,
                                     llvm::BasicBlock* bb) {
  StoreInst* store_inst = new StoreInst(cast_inst_from, alloca_inst_to, false);

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

  } else if (type == TYPE_VOID_POINTER) {
    store_inst->setAlignment(8);

  } else if (type == TYPE_DATATYPE) {
    store_inst->setAlignment(8);
  }

  if (bb && store_inst) {
    bb->getInstList().push_back(store_inst);
  }

  return store_inst;
}

CastInst* cast_codegen(AllocaInst* alloca_inst_from, llvm::BasicBlock* bb) {
  CastInst* casted_inst = new BitCastInst(alloca_inst_from, PointerTy_8, "cast_inst_");

  return casted_inst;
}

std::vector<Value*> assign_codegen_util(AllocaInst* untyped_poiter_alloca, Value* value, llvm::BasicBlock* bb) {
  /**
    * Instructions for this method can be found at:docs/declare-and-assign.cc
    */

  int rhs_type = toNonameType(value);

  // create the actual Constant value
  AllocaInst* alloca_inst = alloca_typed_var_codegen(rhs_type, bb);

  // store the Constant into the allocated "typed" variable
  StoreInst* store_inst_typed_var = store_typed_var_codegen(rhs_type, value, bb);

  // Cast the the "typed" variable to the "untyped" variable
  CastInst* cast_inst = cast_codegen(alloca_inst, bb);

  // Store the address of the
  StoreInst* store_inst_untyped_var = store_untyped_var_codegen(rhs_type, cast_inst, alloca_inst, bb);

  std::vector<Value*> codegen;
  codegen.push_back(value);
  codegen.push_back(alloca_inst);
  codegen.push_back(store_inst_typed_var);
  codegen.push_back(store_inst_untyped_var);

  return codegen;
}
}