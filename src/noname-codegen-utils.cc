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

Value* codegen_elements_retlast(ASTNode* node, llvm::BasicBlock* bb = nullptr) {
  std::vector<std::unique_ptr<Value>> codegen_elements(
      node->codegen_elements());
  Value* last = nullptr;
  if (bb) {
    for (std::unique_ptr<Value>& ptr : codegen_elements) {
      last = ptr.get();
      bb->getInstList().push_back(std::move(ptr));
    }
  }

  return last;
}

std::vector<std::unique_ptr<Value>> declaration_codegen_util(ASTNode* node, llvm::BasicBlock* bb = nullptr) {
  std::string alloca_name = "decl_alloca_";

  if (isa<DeclarationNode>(node)) {
    DeclarationNode* declaration = (DeclarationNode*)node;
    alloca_name += declaration.getName();

  } else if (isa<DeclarationAssignmentNode>(node)) {
    DeclarationAssignmentNode* declaration = (DeclarationAssignmentNode*)node;
    alloca_name += declaration.getName();
  }

  AllocaInst* alloca_inst = new AllocaInst(PointerTy_4, alloca_name);
  alloca_inst->setAlignment(8);

  std::vector<std::unique_ptr<Value>> codegen(std::vector());
  codegen.push_back(alloca_inst);
  return codegen;
}
std::vector<std::unique_ptr<Value>> assign_codegen_util(Assignment* assignment, llvm::BasicBlock* bb = nullptr) {

  std::vector<std::unique_ptr<Value>> codegen(std::vector());
  
  /**
    * Instructions for this method can be found at:docs/declare-and-assign.cc
    */

  std::string voidp_alloca_name = "voidp_alloca_";
  voidp_alloca_name += assignment->getName();
  std::unique_ptr<ExpNode>rhs = assignment->geRHS();

  /**
  * user level untyped variable
  * declare the "untyped"  variable that will ultimately point to the value
  */
  AllocaInst* voidp_alloca = new AllocaInst(PointerTy_4, voidp_alloca_name);
  voidp_alloca->setAlignment(8);
  codegen.push_back(std::unique_ptr<Value>(voidp_alloca));
  
  // create the actual Constant value
  // ConstantInt* value_const = ConstantInt::get(mod->getContext(), APInt(32, StringRef("100"), 10));
  std::vector<std::unique_ptr<Value>> value_codegen_elements = assignment->getRHS()->codegen_elements();
  codegen.insert(codegen.end(), value_codegen_elements.begin(), value_codegen_elements.end());

  // alocate the "typed" variable that will handle the Constant value
  AllocaInst* typed_pointer_alloca = new AllocaInst(IntegerType::get(mod->getContext(), 32), "int_v");
  typed_pointer_alloca->setAlignment(4);
  codegen.push_back(std::unique_ptr<Value>(typed_pointer_alloca));

  // store the Constant into the allocated "typed" variable
  StoreInst* void_34 = new StoreInst(value_const, typed_pointer_alloca, false);
  void_34->setAlignment(4);
  codegen.push_back(std::unique_ptr<Value>(void_34));

  // Cast the the "typed" variable to the "untyped" variable
  CastInst* casted_inst = new BitCastInst(typed_pointer_alloca, PointerTy_4, "");
  codegen.push_back(std::unique_ptr<Value>(casted_inst));

  // Store the address of the 
  StoreInst* voidp_store = new StoreInst(casted_inst, voidp_alloca, false);
  voidp_store->setAlignment(8);
  codegen.push_back(std::unique_ptr<Value>(voidp_store));
  
  return codegen;
}

}