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

Value* BinaryExpNode::CreatePow(Value* LHS, Value* RHS, const char* name = "call_pow_tmp") const {
  Function* pow_function = TheModule->getFunction(pow_function_name);

  if (!pow_function) {
    return logErrorLLVM("Unknown function referenced");
  }

  std::vector<Value*> args_values;
  args_values.push_back(LHS);
  args_values.push_back(RHS);

  // return Builder.CreateCall(pow_function, args_values, name);
  logError("pow function not implemented");
  return nullptr;
}

template <typename T, typename E>
E push_back_ret(std::vector<T>& vec, E item) {
  vec.push_back(item);
  return item;
};

GetElementPtrInst* get_element_ptr_type_codegen(llvm::AllocaInst* alloca_inst, const std::string& sufix = "",
                                                llvm::BasicBlock* bb = nullptr) {
  ConstantInt* const_int32_0 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));

  GetElementPtrInst* get_elem_ptr =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_inst, {const_int32_0, const_int32_0}, "get_element_ptr_type", bb);
  get_elem_ptr->setName(get_elem_ptr->getName() + sufix);
  return get_elem_ptr;
}

GetElementPtrInst* get_element_ptr_v_codegen(llvm::AllocaInst* alloca_inst, const std::string& sufix = "", llvm::BasicBlock* bb = nullptr) {
  ConstantInt* const_int32_0 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));
  ConstantInt* const_int32_1 = ConstantInt::get(TheContext, APInt(32, StringRef("1"), 10));

  GetElementPtrInst* get_elem_ptr =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_inst, {const_int32_0, const_int32_1}, "get_element_ptr_v", bb);
  get_elem_ptr->setName(get_elem_ptr->getName() + sufix);
  return get_elem_ptr;
}

typedef struct BinaryExpNode_Data_t {
  // prepare variable to receive return
  AllocaInst* alloca_datatype;
  GetElementPtrInst* get_elem_ptr_v;
  GetElementPtrInst* get_elem_ptr_type;

  // prepare variable to receive LHS
  AllocaInst* alloca_datatype_larg;
  GetElementPtrInst* get_elem_ptr_larg_v;
  GetElementPtrInst* get_elem_ptr_larg_type;

  // prepare variable to receive RHS
  AllocaInst* alloca_datatype_rarg;
  GetElementPtrInst* get_elem_ptr_rarg_v;
  GetElementPtrInst* get_elem_ptr_rarg_type;
} BinaryExpNode_Data_t;

void prepare(Error& error, BinaryExpNode_Data_t& data, std::vector<Value*>& codegen, llvm::BasicBlock* bb) {
  // prepare variable to receive return
  AllocaInst* alloca_datatype = push_back_ret(codegen, alloca_typed_var_codegen(TYPE_DATATYPE, "_main", bb));
  GetElementPtrInst* get_elem_ptr_v = push_back_ret(codegen, get_element_ptr_v_codegen(alloca_datatype, "_main", bb));
  GetElementPtrInst* get_elem_ptr_type = push_back_ret(codegen, get_element_ptr_type_codegen(alloca_datatype, "_main", bb));

  // prepare variable to receive LHS
  AllocaInst* alloca_datatype_larg = push_back_ret(codegen, alloca_typed_var_codegen(TYPE_DATATYPE, "_larg", bb));
  GetElementPtrInst* get_elem_ptr_larg_v = push_back_ret(codegen, get_element_ptr_v_codegen(alloca_datatype_larg, "_larg", bb));
  GetElementPtrInst* get_elem_ptr_larg_type = push_back_ret(codegen, get_element_ptr_type_codegen(alloca_datatype_larg, "_larg", bb));

  // prepare variable to receive RHS

  AllocaInst* alloca_datatype_rarg = push_back_ret(codegen, alloca_typed_var_codegen(TYPE_DATATYPE, "_rarg", bb));
  GetElementPtrInst* get_elem_ptr_rarg_v = push_back_ret(codegen, get_element_ptr_v_codegen(alloca_datatype_rarg, "_rarg", bb));
  GetElementPtrInst* get_elem_ptr_rarg_type = push_back_ret(codegen, get_element_ptr_type_codegen(alloca_datatype_rarg, "_rarg", bb));
}

std::vector<Value*> BinaryExpNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  std::vector<Value*> codegen;

  ConstantInt* const_int32_1 = ConstantInt::get(TheContext, APInt(32, StringRef("1"), 10));
  ConstantInt* const_int32_0 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));
  ConstantInt* const_int32_double = ConstantInt::get(TheContext, APInt(32, TYPE_DOUBLE, true));
  ConstantInt* const_int32_long = ConstantInt::get(TheContext, APInt(32, TYPE_LONG, true));

  BinaryExpNode_Data_t data;
  prepare(error, data, codegen, bb);

  Function* function = bb->getParent();

  BasicBlock* label_if_then_double = push_back_ret(codegen, BasicBlock::Create(TheContext, "if_then_double", function, 0));
  BasicBlock* label_else_if = push_back_ret(codegen, BasicBlock::Create(TheContext, "if_else", function, 0));
  BasicBlock* label_else_if_then_long = push_back_ret(codegen, BasicBlock::Create(TheContext, "if_then_long", function, 0));
  BasicBlock* label_if_default = push_back_ret(codegen, BasicBlock::Create(TheContext, "label_if_default", function, 0));
  BasicBlock* label_if_end = push_back_ret(codegen, BasicBlock::Create(TheContext, "if_end", function, 0));

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

  // fprintf(stdout, "\nlhs_codegen_elements:\n");
  // fflush(stdout);
  for (auto current_value : lhs_codegen_elements) {
    if (!current_value->getType()->isVoidTy()) {
      current_value->setName(current_value->getName() + "_LHS");
    }
    current_value->dump();
    if (isa<Instruction>(current_value)) {
      // bb->getInstList().push_back((Instruction*)current_value);
    }
    codegen.push_back(current_value);
  }

  // fprintf(stdout, "\n\nlhs_codegen_elements:\n");
  // fflush(stdout);
  for (auto current_value : rhs_codegen_elements) {
    if (!current_value->getType()->isVoidTy()) {
      current_value->setName(current_value->getName() + "_RHS");
    }

    current_value->dump();
    if (isa<Instruction>(current_value)) {
      // bb->getInstList().push_back((Instruction*)current_value);
    }
    codegen.push_back(current_value);
  }

  // fprintf(stdout, "\n\n");
  // fflush(stdout);

  Value* LHS = lhs_codegen_elements.back();
  Value* RHS = rhs_codegen_elements.back();

  if (!LHS || !RHS) {
    logError("LHS or RHS are undefined");
    return codegen;
  }

  push_back_ret(codegen, store_typed_var_codegen(TYPE_DATATYPE, LHS, data.alloca_datatype_larg, bb));
  push_back_ret(codegen, store_typed_var_codegen(TYPE_DATATYPE, RHS, data.alloca_datatype_rarg, bb));

  LoadInst* lload_inst_type = push_back_ret(codegen, load_inst_codegen(TYPE_INT, data.get_elem_ptr_larg_type, bb));
  LoadInst* rload_inst_type = push_back_ret(codegen, load_inst_codegen(TYPE_INT, data.get_elem_ptr_rarg_type, bb));

  // codegen.push_back(store_typed_var_codegen(TYPE_VOID_POINTER, const_int32_1, get_elem_ptr_larg_v, bb));
  // codegen.push_back(store_typed_var_codegen(TYPE_VOID_POINTER, const_int32_1, get_elem_ptr_rarg_v, bb));

  LoadInst* lload_inst_v = push_back_ret(codegen, load_inst_codegen(TYPE_VOID_POINTER, data.get_elem_ptr_larg_v, bb));
  LoadInst* rload_inst_v = push_back_ret(codegen, load_inst_codegen(TYPE_VOID_POINTER, data.get_elem_ptr_rarg_v, bb));

  CastInst* lcast_inst_long_v = push_back_ret(codegen, new BitCastInst(lload_inst_v, PointerTy_64, "lcast_inst_long_v", bb));
  CastInst* rcast_inst_long_v = push_back_ret(codegen, new BitCastInst(rload_inst_v, PointerTy_64, "rcast_inst_long_v", bb));
  LoadInst* lload_inst_long_v = push_back_ret(codegen, load_inst_codegen(TYPE_VOID_POINTER, lcast_inst_long_v, bb));
  LoadInst* rload_inst_long_v = push_back_ret(codegen, load_inst_codegen(TYPE_VOID_POINTER, rcast_inst_long_v, bb));

  CastInst* lcast_inst_double_v = push_back_ret(codegen, new BitCastInst(lload_inst_v, PointerTy_Double, "lcast_inst_double_v", bb));
  CastInst* rcast_inst_double_v = push_back_ret(codegen, new BitCastInst(rload_inst_v, PointerTy_Double, "rcast_inst_double_v", bb));
  LoadInst* lload_inst_double_v = push_back_ret(codegen, load_inst_codegen(TYPE_VOID_POINTER, lcast_inst_double_v, bb));
  LoadInst* rload_inst_double_v = push_back_ret(codegen, load_inst_codegen(TYPE_VOID_POINTER, rcast_inst_double_v, bb));

  //http://llvm.org/docs/doxygen/html/IRBuilder_8h_source.html#l01428

  CmpInst* cond_equal_double = new ICmpInst(*bb, ICmpInst::ICMP_EQ, lload_inst_type, const_int32_double, "cond_equal_double");
  codegen.push_back(BranchInst::Create(label_if_then_double, label_else_if, cond_equal_double, bb));

  CmpInst* cond_equal_long = new ICmpInst(*label_else_if, ICmpInst::ICMP_EQ, lload_inst_type, const_int32_long, "cond_equal_long");
  codegen.push_back(BranchInst::Create(label_else_if_then_long, label_if_default, cond_equal_long, label_else_if));

  AllocaInst* alloca_datatype_double_v = push_back_ret(codegen, alloca_typed_var_codegen(TYPE_DOUBLE, label_if_then_double));
  AllocaInst* alloca_datatype_long_v = push_back_ret(codegen, alloca_typed_var_codegen(TYPE_LONG, label_else_if_then_long));

  Value* binary_op_double = nullptr;
  Value* binary_op_long = nullptr;

  if (op == '+') {
    binary_op_double = BinaryOperator::Create(Instruction::FAdd, lload_inst_double_v, rload_inst_double_v, "add", label_if_then_double);
    binary_op_long = BinaryOperator::Create(Instruction::Add, lload_inst_long_v, rload_inst_long_v, "add", label_else_if_then_long);
  } else if (op == '-') {
    binary_op_double = BinaryOperator::Create(Instruction::FSub, lload_inst_double_v, rload_inst_double_v, "sub", label_if_then_double);
    binary_op_long = BinaryOperator::Create(Instruction::Sub, lload_inst_long_v, rload_inst_long_v, "sub", label_else_if_then_long);
  } else if (op == '*') {
    binary_op_double = BinaryOperator::Create(Instruction::FMul, lload_inst_double_v, rload_inst_double_v, "mul", label_if_then_double);
    binary_op_long = BinaryOperator::Create(Instruction::Mul, lload_inst_long_v, rload_inst_long_v, "mul", label_else_if_then_long);
  } else if (op == '/') {
    binary_op_double = BinaryOperator::Create(Instruction::FDiv, lload_inst_double_v, rload_inst_double_v, "div", label_if_then_double);
    binary_op_long = BinaryOperator::Create(Instruction::SDiv, lload_inst_long_v, rload_inst_long_v, "div", label_else_if_then_long);
  } else if (op == '^') {
    logError("^ NOT IMPLEMENTED YET");
    // result = CreatePow(LHS, RHS);
  }

  push_back_ret(codegen, store_typed_var_codegen(TYPE_INT, const_int32_double, data.get_elem_ptr_type, label_if_then_double));
  push_back_ret(codegen, store_typed_var_codegen(TYPE_INT, const_int32_long, data.get_elem_ptr_type, label_else_if_then_long));

  // #################################
  // #################################
  // #################################

  push_back_ret(codegen, store_typed_var_codegen(TYPE_DOUBLE, binary_op_double, alloca_datatype_double_v, label_if_then_double));
  push_back_ret(codegen, store_typed_var_codegen(TYPE_LONG, binary_op_long, alloca_datatype_long_v, label_else_if_then_long));

  CastInst* cast_inst_double_v = push_back_ret(codegen, new BitCastInst(alloca_datatype_double_v, PointerTy_8, "", label_if_then_double));
  CastInst* cast_inst_long_v = push_back_ret(codegen, new BitCastInst(alloca_datatype_long_v, PointerTy_8, "", label_else_if_then_long));

  // LoadInst* load_inst_double_v = load_inst_codegen(TYPE_VOID_POINTER, cast_inst_double_v, label_if_then_double);
  // LoadInst* load_inst_long_v = load_inst_codegen(TYPE_VOID_POINTER, cast_inst_long_v, label_else_if_then_long);
  // codegen.push_back(load_inst_double_v);
  // codegen.push_back(load_inst_long_v);

  push_back_ret(codegen, store_typed_var_codegen(TYPE_VOID_POINTER, cast_inst_double_v, data.get_elem_ptr_v, label_if_then_double));
  push_back_ret(codegen, store_typed_var_codegen(TYPE_VOID_POINTER, cast_inst_long_v, data.get_elem_ptr_v, label_else_if_then_long));

  //////////////////////////////////////
  //////////////////////////////////////
  //////////////////////////////////////

  ConstantInt* const_int32_777 = ConstantInt::get(TheContext, APInt(32, 777, true));

  long* heap_value = new long(888777);
  Constant* default_constant_address = ConstantInt::get(Type::getInt64Ty(TheContext), (uint64_t)heap_value);
  Value* constant_value = ConstantExpr::getIntToPtr(default_constant_address, PointerType::getUnqual(IntegerType::get(TheContext, 8)));

  if (!constant_value) {
    createError(error, "Invalid or undefined constant value");
    return codegen;
  }

  push_back_ret(codegen, store_typed_var_codegen(TYPE_VOID_POINTER, constant_value, data.get_elem_ptr_v, label_if_default));
  push_back_ret(codegen, store_typed_var_codegen(TYPE_INT, const_int32_777, data.get_elem_ptr_type, label_if_default));

  //////////////////////////////////////
  //////////////////////////////////////
  //////////////////////////////////////

  BranchInst::Create(label_if_end, label_if_default);
  BranchInst::Create(label_if_end, label_if_then_double);
  BranchInst::Create(label_if_end, label_else_if_then_long);

  codegen.push_back(label_if_end);

  LoadInst* load_inst_type = push_back_ret(codegen, load_inst_codegen(TYPE_DATATYPE, data.alloca_datatype, label_if_end));

  CastInst* cast_inst_alloca_datatype =
      push_back_ret(codegen, new BitCastInst(data.alloca_datatype, PointerTy_StructTy_struct_datatype_t, "", label_if_end));
  LoadInst* load_inst_alloca_datatype =
      push_back_ret(codegen, load_inst_codegen(TYPE_VOID_POINTER, cast_inst_alloca_datatype, label_if_end));

  return codegen;
}
/*
std::vector<Value*> BinaryExpNode::codegen_elements(Error& error, llvm::BasicBlock* bb) const {
  std::vector<Value*> codegen;

  ConstantInt* const_int32_1 = ConstantInt::get(TheContext, APInt(32, StringRef("1"), 10));
  ConstantInt* const_int32_0 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));
  ConstantInt* const_int32_double = ConstantInt::get(TheContext, APInt(32, TYPE_DOUBLE, true));
  ConstantInt* const_int32_long = ConstantInt::get(TheContext, APInt(32, TYPE_LONG, true));

  if (false) {
    ConstantInt* const_int32_type = ConstantInt::get(TheContext, APInt(32, TYPE_LONG, true));

    // struct datatype_t
    AllocaInst* alloca_datatype = alloca_typed_var_codegen(TYPE_DATATYPE, bb);
    // AllocaInst *alloca_value = alloca_typed_var_codegen(type, bb);

    codegen.push_back(alloca_datatype);
    // codegen.push_back(alloca_value);

    // typed value
    // Value* constant_value = node->constant_codegen(bb);

    long* heap_value = new long(888777);
    Constant* beginConstAddress = ConstantInt::get(Type::getInt64Ty(TheContext), (uint64_t)heap_value);
    Value* constant_value =
        ConstantExpr::getIntToPtr(beginConstAddress, PointerType::getUnqual(IntegerType::get(TheContext, 8)));

    if (!constant_value) {
      createError(error, "Invalid or undefined constant value");
      return codegen;
    }

    GetElementPtrInst* get_elem_ptr_v =
        GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype, {const_int32_0, const_int32_1}, "v", bb);
    GetElementPtrInst* get_elem_ptr_type = GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype,
                                                                     {const_int32_0, const_int32_0}, "type", bb);

    codegen.push_back(get_elem_ptr_v);
    codegen.push_back(get_elem_ptr_type);

    StoreInst* store_ptr_v = store_typed_var_codegen(TYPE_VOID_POINTER, constant_value, get_elem_ptr_v, bb);
    codegen.push_back(store_ptr_v);

    StoreInst* store_ptr_type = store_typed_var_codegen(TYPE_INT, const_int32_type, get_elem_ptr_type, bb);
    codegen.push_back(store_ptr_type);

    LoadInst* load_inst_type = load_inst_codegen(TYPE_DATATYPE, alloca_datatype, bb);
    codegen.push_back(load_inst_type);

    return codegen;
  }

  // prepare variable to receive return
  AllocaInst* alloca_datatype = alloca_typed_var_codegen(TYPE_DATATYPE, bb);
  alloca_datatype->setName(alloca_datatype->getName() + "_main");
  GetElementPtrInst* get_elem_ptr_v =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype, {const_int32_0, const_int32_1}, "v", bb);
  GetElementPtrInst* get_elem_ptr_type = GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype,
                                                                   {const_int32_0, const_int32_0}, "type", bb);
  get_elem_ptr_v->setName(get_elem_ptr_v->getName() + "_larg");
  get_elem_ptr_type->setName(get_elem_ptr_type->getName() + "_larg");
  codegen.push_back(alloca_datatype);
  codegen.push_back(get_elem_ptr_v);
  codegen.push_back(get_elem_ptr_type);

  // prepare variable to receive LHS
  AllocaInst* alloca_datatype_larg = alloca_typed_var_codegen(TYPE_DATATYPE, bb);
  alloca_datatype_larg->setName(alloca_datatype_larg->getName() + "_arg_l");
  GetElementPtrInst* get_elem_ptr_larg_v = GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype_larg,
                                                                     {const_int32_0, const_int32_1}, "v", bb);
  GetElementPtrInst* get_elem_ptr_larg_type = GetElementPtrInst::Create(
      StructTy_struct_datatype_t, alloca_datatype_larg, {const_int32_0, const_int32_0}, "type", bb);
  get_elem_ptr_larg_v->setName(get_elem_ptr_larg_v->getName() + "_larg");
  get_elem_ptr_larg_type->setName(get_elem_ptr_larg_type->getName() + "_larg");
  codegen.push_back(alloca_datatype_larg);
  codegen.push_back(get_elem_ptr_larg_v);
  codegen.push_back(get_elem_ptr_larg_type);

  // prepare variable to receive RHS
  AllocaInst* alloca_datatype_rarg = alloca_typed_var_codegen(TYPE_DATATYPE, bb);
  GetElementPtrInst* get_elem_ptr_rarg_v = GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype_rarg,
                                                                     {const_int32_0, const_int32_1}, "v", bb);
  GetElementPtrInst* get_elem_ptr_rarg_type = GetElementPtrInst::Create(
      StructTy_struct_datatype_t, alloca_datatype_rarg, {const_int32_0, const_int32_0}, "type", bb);
  get_elem_ptr_rarg_v->setName(get_elem_ptr_rarg_v->getName() + "_rarg");
  get_elem_ptr_rarg_type->setName(get_elem_ptr_rarg_type->getName() + "_rarg");
  codegen.push_back(alloca_datatype_rarg);
  codegen.push_back(get_elem_ptr_rarg_v);
  codegen.push_back(get_elem_ptr_rarg_type);

  Function* function = bb->getParent();

  BasicBlock* label_if_then_double = BasicBlock::Create(TheContext, "if_then_double", function, 0);
  BasicBlock* label_else_if = BasicBlock::Create(TheContext, "if_else", function, 0);
  BasicBlock* label_else_if_then_long = BasicBlock::Create(TheContext, "if_then_long", function, 0);
  BasicBlock* label_if_default = BasicBlock::Create(TheContext, "label_if_default", function, 0);
  BasicBlock* label_if_end = BasicBlock::Create(TheContext, "if_end", function, 0);

  codegen.push_back(bb);
  codegen.push_back(label_if_then_double);
  codegen.push_back(label_else_if);
  codegen.push_back(label_else_if_then_long);
  codegen.push_back(label_if_default);
  codegen.push_back(label_if_end);

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

  // fprintf(stdout, "\nlhs_codegen_elements:\n");
  // fflush(stdout);
  for (auto current_value : lhs_codegen_elements) {
    if (!current_value->getType()->isVoidTy()) {
      current_value->setName(current_value->getName() + "_LHS");
    }
    current_value->dump();
    if (isa<Instruction>(current_value)) {
      // bb->getInstList().push_back((Instruction*)current_value);
    }
    codegen.push_back(current_value);
  }

  // fprintf(stdout, "\n\nlhs_codegen_elements:\n");
  // fflush(stdout);
  for (auto current_value : rhs_codegen_elements) {
    if (!current_value->getType()->isVoidTy()) {
      current_value->setName(current_value->getName() + "_RHS");
    }

    current_value->dump();
    if (isa<Instruction>(current_value)) {
      // bb->getInstList().push_back((Instruction*)current_value);
    }
    codegen.push_back(current_value);
  }

  // fprintf(stdout, "\n\n");
  // fflush(stdout);

  Value* LHS = lhs_codegen_elements.back();
  Value* RHS = rhs_codegen_elements.back();

  if (!LHS || !RHS) {
    logError("LHS or RHS are undefined");
    return codegen;
  }

  codegen.push_back(store_typed_var_codegen(TYPE_DATATYPE, LHS, alloca_datatype_larg, bb));
  codegen.push_back(store_typed_var_codegen(TYPE_DATATYPE, RHS, alloca_datatype_rarg, bb));

  LoadInst* lload_inst_type = load_inst_codegen(TYPE_INT, get_elem_ptr_larg_type, bb);
  LoadInst* rload_inst_type = load_inst_codegen(TYPE_INT, get_elem_ptr_rarg_type, bb);
  codegen.push_back(lload_inst_type);
  codegen.push_back(rload_inst_type);

  // codegen.push_back(store_typed_var_codegen(TYPE_VOID_POINTER, const_int32_1, get_elem_ptr_larg_v, bb));
  // codegen.push_back(store_typed_var_codegen(TYPE_VOID_POINTER, const_int32_1, get_elem_ptr_rarg_v, bb));

  LoadInst* lload_inst_v = load_inst_codegen(TYPE_VOID_POINTER, get_elem_ptr_larg_v, bb);
  LoadInst* rload_inst_v = load_inst_codegen(TYPE_VOID_POINTER, get_elem_ptr_rarg_v, bb);
  codegen.push_back(lload_inst_v);
  codegen.push_back(rload_inst_v);

  CastInst* lcast_inst_long_v = new BitCastInst(lload_inst_v, PointerTy_64, "lcast_inst_long_v", bb);
  CastInst* rcast_inst_long_v = new BitCastInst(rload_inst_v, PointerTy_64, "rcast_inst_long_v", bb);
  LoadInst* lload_inst_long_v = load_inst_codegen(TYPE_VOID_POINTER, lcast_inst_long_v, bb);
  LoadInst* rload_inst_long_v = load_inst_codegen(TYPE_VOID_POINTER, rcast_inst_long_v, bb);

  codegen.push_back(lcast_inst_long_v);
  codegen.push_back(rcast_inst_long_v);
  codegen.push_back(lload_inst_long_v);
  codegen.push_back(rload_inst_long_v);

  CastInst* lcast_inst_double_v = new BitCastInst(lload_inst_v, PointerTy_Double, "lcast_inst_double_v", bb);
  CastInst* rcast_inst_double_v = new BitCastInst(rload_inst_v, PointerTy_Double, "rcast_inst_double_v", bb);
  LoadInst* lload_inst_double_v = load_inst_codegen(TYPE_VOID_POINTER, lcast_inst_double_v, bb);
  LoadInst* rload_inst_double_v = load_inst_codegen(TYPE_VOID_POINTER, rcast_inst_double_v, bb);

  codegen.push_back(lcast_inst_double_v);
  codegen.push_back(rcast_inst_double_v);
  codegen.push_back(lload_inst_double_v);
  codegen.push_back(rload_inst_double_v);

  //http://llvm.org/docs/doxygen/html/IRBuilder_8h_source.html#l01428

  CmpInst* cond_equal_double =
      new ICmpInst(*bb, ICmpInst::ICMP_EQ, lload_inst_type, const_int32_double, "cond_equal_double");
  codegen.push_back(BranchInst::Create(label_if_then_double, label_else_if, cond_equal_double, bb));

  CmpInst* cond_equal_long =
      new ICmpInst(*label_else_if, ICmpInst::ICMP_EQ, lload_inst_type, const_int32_long, "cond_equal_long");

  codegen.push_back(BranchInst::Create(label_else_if_then_long, label_if_default, cond_equal_long, label_else_if));

  AllocaInst* alloca_datatype_double_v = alloca_typed_var_codegen(TYPE_DOUBLE, label_if_then_double);
  AllocaInst* alloca_datatype_long_v = alloca_typed_var_codegen(TYPE_LONG, label_else_if_then_long);

  codegen.push_back(alloca_datatype_double_v);
  codegen.push_back(alloca_datatype_long_v);

  Value* binary_op_double = nullptr;
  Value* binary_op_long = nullptr;

  if (op == '+') {
    binary_op_double = BinaryOperator::Create(Instruction::FAdd, lload_inst_double_v, rload_inst_double_v, "add",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::Add, lload_inst_long_v, rload_inst_long_v, "add", label_else_if_then_long);
  } else if (op == '-') {
    binary_op_double = BinaryOperator::Create(Instruction::FSub, lload_inst_double_v, rload_inst_double_v, "sub",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::Sub, lload_inst_long_v, rload_inst_long_v, "sub", label_else_if_then_long);
  } else if (op == '*') {
    binary_op_double = BinaryOperator::Create(Instruction::FMul, lload_inst_double_v, rload_inst_double_v, "mul",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::Mul, lload_inst_long_v, rload_inst_long_v, "mul", label_else_if_then_long);
  } else if (op == '/') {
    binary_op_double = BinaryOperator::Create(Instruction::FDiv, lload_inst_double_v, rload_inst_double_v, "div",
                                              label_if_then_double);
    binary_op_long =
        BinaryOperator::Create(Instruction::SDiv, lload_inst_long_v, rload_inst_long_v, "div", label_else_if_then_long);
  } else if (op == '^') {
    logError("^ NOT IMPLEMENTED YET");
    // result = CreatePow(LHS, RHS);
  }

  codegen.push_back(store_typed_var_codegen(TYPE_INT, const_int32_double, get_elem_ptr_type, label_if_then_double));
  codegen.push_back(store_typed_var_codegen(TYPE_INT, const_int32_long, get_elem_ptr_type, label_else_if_then_long));

  // #################################
  // #################################
  // #################################

  codegen.push_back(
      store_typed_var_codegen(TYPE_DOUBLE, binary_op_double, alloca_datatype_double_v, label_if_then_double));
  codegen.push_back(
      store_typed_var_codegen(TYPE_LONG, binary_op_long, alloca_datatype_long_v, label_else_if_then_long));

  CastInst* cast_inst_double_v = new BitCastInst(alloca_datatype_double_v, PointerTy_8, "", label_if_then_double);
  CastInst* cast_inst_long_v = new BitCastInst(alloca_datatype_long_v, PointerTy_8, "", label_else_if_then_long);
  codegen.push_back(cast_inst_double_v);
  codegen.push_back(cast_inst_long_v);

  // LoadInst* load_inst_double_v = load_inst_codegen(TYPE_VOID_POINTER, cast_inst_double_v, label_if_then_double);
  // LoadInst* load_inst_long_v = load_inst_codegen(TYPE_VOID_POINTER, cast_inst_long_v, label_else_if_then_long);
  // codegen.push_back(load_inst_double_v);
  // codegen.push_back(load_inst_long_v);

  codegen.push_back(
      store_typed_var_codegen(TYPE_VOID_POINTER, cast_inst_double_v, get_elem_ptr_v, label_if_then_double));
  codegen.push_back(
      store_typed_var_codegen(TYPE_VOID_POINTER, cast_inst_long_v, get_elem_ptr_v, label_else_if_then_long));

  //////////////////////////////////////
  //////////////////////////////////////
  //////////////////////////////////////

  ConstantInt* const_int32_777 = ConstantInt::get(TheContext, APInt(32, 777, true));

  long* heap_value = new long(888777);
  Constant* default_constant_address = ConstantInt::get(Type::getInt64Ty(TheContext), (uint64_t)heap_value);
  Value* constant_value =
      ConstantExpr::getIntToPtr(default_constant_address, PointerType::getUnqual(IntegerType::get(TheContext, 8)));

  if (!constant_value) {
    createError(error, "Invalid or undefined constant value");
    return codegen;
  }

  StoreInst* store_ptr_v = store_typed_var_codegen(TYPE_VOID_POINTER, constant_value, get_elem_ptr_v, label_if_default);
  codegen.push_back(store_ptr_v);

  StoreInst* store_ptr_type = store_typed_var_codegen(TYPE_INT, const_int32_777, get_elem_ptr_type, label_if_default);
  codegen.push_back(store_ptr_type);

  //////////////////////////////////////
  //////////////////////////////////////
  //////////////////////////////////////

  BranchInst::Create(label_if_end, label_if_default);
  BranchInst::Create(label_if_end, label_if_then_double);
  BranchInst::Create(label_if_end, label_else_if_then_long);

  codegen.push_back(label_if_end);

  LoadInst* load_inst_type = load_inst_codegen(TYPE_DATATYPE, alloca_datatype, label_if_end);
  codegen.push_back(load_inst_type);

  CastInst* cast_inst_alloca_datatype =
      new BitCastInst(alloca_datatype, PointerTy_StructTy_struct_datatype_t, "", label_if_end);
  LoadInst* load_inst_alloca_datatype = load_inst_codegen(TYPE_VOID_POINTER, cast_inst_alloca_datatype, label_if_end);

  codegen.push_back(cast_inst_alloca_datatype);
  codegen.push_back(load_inst_alloca_datatype);

  return codegen;
}
*/

Value* BinaryExpNode::codegen(llvm::BasicBlock* bb) { return codegen_elements_retlast(this, bb); }
}