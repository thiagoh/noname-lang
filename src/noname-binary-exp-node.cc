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

std::string pow_function_name("_noname_function_pow");

std::unique_ptr<NodeValue> BinaryExpNode::getValue() const {
  std::unique_ptr<NodeValue> lhs_node_value = lhs->getValue();
  std::unique_ptr<NodeValue> rhs_node_value = rhs->getValue();
  int result_type = get_adequate_result_type(lhs_node_value.get(), rhs_node_value.get());
  void* lhs_value = lhs_node_value->getValue(result_type);
  void* rhs_value = rhs_node_value->getValue(result_type);
  NodeValue* result = nullptr;
  if (result_type == TYPE_DOUBLE) {
    double typed_lhs_value = *(double*)lhs_value;
    double typed_rhs_value = *(double*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue(pow(typed_lhs_value, typed_rhs_value));
    }
  } else if (result_type == TYPE_FLOAT) {
    float typed_lhs_value = *(float*)lhs_value;
    float typed_rhs_value = *(float*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue(pow(typed_lhs_value, typed_rhs_value));
    }
  } else if (result_type == TYPE_LONG) {
    long typed_lhs_value = *(long*)lhs_value;
    long typed_rhs_value = *(long*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((long)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_INT) {
    int typed_lhs_value = *(int*)lhs_value;
    int typed_rhs_value = *(int*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((int)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_SHORT) {
    short typed_lhs_value = *(short*)lhs_value;
    short typed_rhs_value = *(short*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((short)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_CHAR) {
    char typed_lhs_value = *(char*)lhs_value;
    char typed_rhs_value = *(char*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((char)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_STRING) {
    std::string typed_lhs_value = *(std::string*)lhs_value;
    std::string typed_rhs_value = *(std::string*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    }
  }

  return std::unique_ptr<NodeValue>(std::move(result));
}

Value* BinaryExpNode::CreatePow(Value* L, Value* R, const char* name = "call_pow_tmp") const {
  Function* pow_function = TheModule->getFunction(pow_function_name);

  if (!pow_function) {
    return logErrorLLVM("Unknown function referenced");
  }

  std::vector<Value*> args_values;
  args_values.push_back(L);
  args_values.push_back(R);

  // return Builder.CreateCall(pow_function, args_values, name);
  logError("pow function not implemented");
  return nullptr;
}

std::vector<Value*> BinaryExpNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  std::vector<Value*> codegen;

  std::vector<Value*> lhs_codegen_elements(lhs->get_codegen_elements(error, bb));

  if (error.code()) {
    logError(error.what().c_str());
    return codegen;
  }

  std::vector<Value*> rhs_codegen_elements(rhs->get_codegen_elements(error, bb));
  if (error.code()) {
    logError(error.what().c_str());
    return codegen;
  }

  fprintf(stdout, "\nlhs_codegen_elements:\n");
  fflush(stdout);
  for (auto current_value : lhs_codegen_elements) {
    current_value->dump();
    codegen.push_back(current_value);
  }

  fprintf(stdout, "\n\nlhs_codegen_elements:\n");
  fflush(stdout);
  for (auto current_value : rhs_codegen_elements) {
    current_value->dump();
    codegen.push_back(current_value);
  }

  fprintf(stdout, "\n\n");
  fflush(stdout);

  Value* L = lhs_codegen_elements.back();
  Value* R = rhs_codegen_elements.back();

  if (!L || !R) {
    logError("L or R are undefined");
    return codegen;
  }

  AllocaInst* lptr_datatype = (AllocaInst*)((LoadInst*)L)->getPointerOperand();
  AllocaInst* rptr_datatype = (AllocaInst*)((LoadInst*)R)->getPointerOperand();

  ConstantInt* const_int32_1 = ConstantInt::get(TheContext, APInt(32, StringRef("1"), 10));
  ConstantInt* const_int32_0 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));
  ConstantInt* const_int32_double = ConstantInt::get(TheContext, APInt(32, TYPE_DOUBLE, true));
  ConstantInt* const_int32_long = ConstantInt::get(TheContext, APInt(32, TYPE_LONG, true));

  GetElementPtrInst* lptr_type =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, lptr_datatype, {const_int32_0, const_int32_0}, "type", bb);
  GetElementPtrInst* rptr_type =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, rptr_datatype, {const_int32_0, const_int32_0}, "type", bb);

  GetElementPtrInst* lptr_v =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, lptr_datatype, {const_int32_0, const_int32_1}, "v", bb);
  GetElementPtrInst* rptr_v =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, rptr_datatype, {const_int32_0, const_int32_1}, "v", bb);

  codegen.push_back(lptr_type);
  codegen.push_back(rptr_type);
  codegen.push_back(lptr_v);
  codegen.push_back(rptr_v);

  LoadInst* lload_inst_type = load_inst_codegen(TYPE_INT, lptr_type, bb);
  LoadInst* rload_inst_type = load_inst_codegen(TYPE_INT, rptr_type, bb);

  codegen.push_back(lload_inst_type);
  codegen.push_back(rload_inst_type);

  LoadInst* lload_inst_v = load_inst_codegen(TYPE_VOID_POINTER, lptr_v, bb);
  LoadInst* rload_inst_v = load_inst_codegen(TYPE_VOID_POINTER, rptr_v, bb);

  codegen.push_back(lload_inst_v);
  codegen.push_back(rload_inst_v);

  CastInst* lcast_inst_long_v = new BitCastInst(lload_inst_v, PointerTy_64, "lcast_inst_long_v", bb);
  CastInst* rcast_inst_long_v = new BitCastInst(rload_inst_v, PointerTy_64, "rcast_inst_long_v", bb);
  LoadInst* lload_inst_long_v = load_inst_codegen(TYPE_LONG, lcast_inst_long_v, bb);
  LoadInst* rload_inst_long_v = load_inst_codegen(TYPE_LONG, rcast_inst_long_v, bb);

  codegen.push_back(lcast_inst_long_v);
  codegen.push_back(rcast_inst_long_v);
  codegen.push_back(lload_inst_long_v);
  codegen.push_back(rload_inst_long_v);

  CastInst* lcast_inst_double_v = new BitCastInst(lload_inst_v, PointerTy_Double, "lcast_inst_double_v", bb);
  CastInst* rcast_inst_double_v = new BitCastInst(rload_inst_v, PointerTy_Double, "rcast_inst_double_v", bb);
  LoadInst* lload_inst_double_v = load_inst_codegen(TYPE_DOUBLE, lcast_inst_double_v, bb);
  LoadInst* rload_inst_double_v = load_inst_codegen(TYPE_DOUBLE, rcast_inst_double_v, bb);

  codegen.push_back(lcast_inst_double_v);
  codegen.push_back(rcast_inst_double_v);
  codegen.push_back(lload_inst_double_v);
  codegen.push_back(rload_inst_double_v);

  //http://llvm.org/docs/doxygen/html/IRBuilder_8h_source.html#l01428

  Function* function = bb->getParent();

  BasicBlock* label_if_then_double = BasicBlock::Create(TheContext, "if.then.double", function);
  BasicBlock* label_if_else = BasicBlock::Create(TheContext, "if.else", function);
  BasicBlock* label_if_then_long = BasicBlock::Create(TheContext, "if.then.long", function);
  BasicBlock* label_if_end = BasicBlock::Create(TheContext, "if.end", function);

  CmpInst* cond_equal_double = new ICmpInst(*bb, ICmpInst::ICMP_EQ, lload_inst_type,
                                            ConstantInt::get(TheContext, APInt(32, TYPE_DOUBLE)), "cond_equal_double");
  CmpInst* cond_equal_long = new ICmpInst(*label_if_else, ICmpInst::ICMP_EQ, lload_inst_type,
                                          ConstantInt::get(TheContext, APInt(32, TYPE_LONG)), "cond_equal_long");


  L->dump();
  R->dump();

  L->getType()->dump();
  R->getType()->dump();

  lload_inst_type->dump();
  rload_inst_type->dump();

  lload_inst_v->dump();
  rload_inst_v->dump();

  AllocaInst* alloca_datatype = alloca_typed_var_codegen(TYPE_DATATYPE, bb);
  GetElementPtrInst* get_elem_ptr_v =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype, {const_int32_0, const_int32_1}, "v", bb);
  GetElementPtrInst* get_elem_ptr_type = GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype,
                                                                   {const_int32_0, const_int32_0}, "type", bb);

  codegen.push_back(alloca_datatype);
  codegen.push_back(get_elem_ptr_v);
  codegen.push_back(get_elem_ptr_type);
  
  codegen.push_back(BranchInst::Create(label_if_then_double, label_if_else, cond_equal_double, bb));
  codegen.push_back(BranchInst::Create(label_if_then_long, label_if_end, cond_equal_long, label_if_else));

  AllocaInst* alloca_datatype_double_v = alloca_typed_var_codegen(TYPE_DOUBLE, label_if_then_double);
  AllocaInst* alloca_datatype_long_v = alloca_typed_var_codegen(TYPE_LONG, label_if_then_long);

  codegen.push_back(alloca_datatype_double_v);
  codegen.push_back(alloca_datatype_long_v);

  BinaryOperator* binary_op_double = nullptr;
  BinaryOperator* binary_op_long = nullptr;

  if (op == '+') {
    binary_op_double = BinaryOperator::Create(Instruction::FAdd, lload_inst_double_v, rload_inst_double_v, "add",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::Add, lload_inst_long_v, rload_inst_long_v, "add", label_if_then_long);
  } else if (op == '-') {
    binary_op_double = BinaryOperator::Create(Instruction::FSub, lload_inst_double_v, rload_inst_double_v, "sub",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::Sub, lload_inst_long_v, rload_inst_long_v, "sub", label_if_then_long);
  } else if (op == '*') {
    binary_op_double = BinaryOperator::Create(Instruction::FMul, lload_inst_double_v, rload_inst_double_v, "mul",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::Mul, lload_inst_long_v, rload_inst_long_v, "mul", label_if_then_long);
  } else if (op == '/') {
    binary_op_double = BinaryOperator::Create(Instruction::FDiv, lload_inst_double_v, rload_inst_double_v, "div",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::SDiv, lload_inst_long_v, rload_inst_long_v, "div", label_if_then_long);
  } else if (op == '^') {
    logError("^ NOT IMPLEMENTED YET");
    // result = CreatePow(L, R);
  }

  codegen.push_back(store_typed_var_codegen(TYPE_DOUBLE, const_int32_double, get_elem_ptr_type, label_if_then_double));
  codegen.push_back(store_typed_var_codegen(TYPE_LONG, const_int32_long, get_elem_ptr_type, label_if_then_long));

  StoreInst* store_inst_double_v =
      store_typed_var_codegen(TYPE_DOUBLE, binary_op_double, alloca_datatype_double_v, label_if_then_double);
  StoreInst* store_inst_long_v =
      store_typed_var_codegen(TYPE_LONG, binary_op_long, alloca_datatype_long_v, label_if_then_long);

  codegen.push_back(store_inst_double_v);
  codegen.push_back(store_inst_long_v);

  CastInst* cast_inst_double_v = new BitCastInst(alloca_datatype_double_v, PointerTy_8, "", label_if_then_double);
  CastInst* cast_inst_long_v = new BitCastInst(alloca_datatype_long_v, PointerTy_8, "", label_if_then_long);

  codegen.push_back(cast_inst_double_v);
  codegen.push_back(cast_inst_long_v);

  codegen.push_back(store_typed_var_codegen(TYPE_DOUBLE, cast_inst_double_v, get_elem_ptr_v, label_if_then_double));
  codegen.push_back(store_typed_var_codegen(TYPE_LONG, cast_inst_long_v, get_elem_ptr_v, label_if_then_long));

  BranchInst::Create(label_if_end, label_if_then_double);
  BranchInst::Create(label_if_end, label_if_then_long);

  codegen.push_back(label_if_end);

  CastInst* cast_inst_alloca_datatype =
      new BitCastInst(alloca_datatype, PointerTy_StructTy_struct_datatype_t, "", label_if_end);
  LoadInst* load_inst_alloca_datatype = load_inst_codegen(TYPE_VOID_POINTER, cast_inst_alloca_datatype, label_if_end);

  codegen.push_back(cast_inst_alloca_datatype);
  codegen.push_back(load_inst_alloca_datatype);

  return codegen;
}

Value* BinaryExpNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }
}