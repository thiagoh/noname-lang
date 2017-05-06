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

NodeValue* BinaryExpNode::getValue() {
  NodeValue* lhs_node_value = lhs->getValue();
  NodeValue* rhs_node_value = rhs->getValue();
  int result_type = get_adequate_result_type(lhs_node_value, rhs_node_value);
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

  return result;
}

Value* BinaryExpNode::CreatePow(Value* L, Value* R,
                                const char* name = "call_pow_tmp") {
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

std::vector<std::unique_ptr<Value>> BinaryExpNode::codegen_elements(
    llvm::BasicBlock* bb) {
  std::vector<std::unique_ptr<Value>> codegen;
  Value* result = nullptr;
  Value* L = lhs->codegen();
  Value* R = rhs->codegen();

  if (!L || !R) {
    logError("L or R are undefined");
    return codegen;
  }

  int lhs_type = fromLLVMValueToType(L);
  int rhs_type = fromLLVMValueToType(R);
  int result_type = get_adequate_result_type(lhs_type, rhs_type);

  /*
  how to insert into correct point
  http://llvm.org/docs/doxygen/html/Instruction_8cpp_source.html#l00023
  Instruction::Instruction(Type *ty, unsigned it, Use *Ops, unsigned NumOps,
    Instruction *InsertBefore)
    : User(ty, Value::InstructionVal + it, Ops, NumOps), Parent(nullptr) {

   // If requested, insert this instruction into a basic block...
   if (InsertBefore) {
     llvm::BasicBlock *BB = InsertBefore->getParent();
     assert(BB && "Instruction to insert before is not in a basic block!");
     BB->getInstList().insert(InsertBefore->getIterator(), this);
   }
  }
  Instruction::Instruction(Type *ty, unsigned it, Use *Ops, unsigned NumOps,
    llvm::BasicBlock *InsertAtEnd)
    : User(ty, Value::InstructionVal + it, Ops, NumOps), Parent(nullptr) {

    // append this instruction into the basic block
    assert(InsertAtEnd && "Basic block to append to may not be NULL!");
    InsertAtEnd->getInstList().push_back(this);
  }
  */

  if (result_type == TYPE_DOUBLE || result_type == TYPE_FLOAT) {
    if (op == '+') {
      // result = Builder.CreateFAdd(L, R, "addtmp");
      result = BinaryOperator::Create(Instruction::FAdd, L, R, "add");
    } else if (op == '-') {
      // result = Builder.CreateFSub(L, R, "addtmp");
      result = BinaryOperator::Create(Instruction::FSub, L, R, "sub");
    } else if (op == '*') {
      // result = Builder.CreateFMul(L, R, "multmp");
      result = BinaryOperator::Create(Instruction::FMul, L, R, "mul");
    } else if (op == '/') {
      // result = Builder.CreateFDiv(L, R, "divtmp");
      result = BinaryOperator::Create(Instruction::FDiv, L, R, "div");
    } else if (op == '^') {
      result = CreatePow(L, R);
    }

  } else if (result_type == TYPE_LONG || result_type == TYPE_INT ||
             result_type == TYPE_SHORT || result_type == TYPE_CHAR) {
    if (op == '+') {
      // result = Builder.CreateAdd(L, R, "addtmp");
      result = BinaryOperator::Create(Instruction::Add, L, R, "add");
    } else if (op == '-') {
      // result = Builder.CreateSub(L, R, "addtmp");
      result = BinaryOperator::Create(Instruction::Sub, L, R, "sub");
    } else if (op == '*') {
      // result = Builder.CreateMul(L, R, "multmp");
      result = BinaryOperator::Create(Instruction::Mul, L, R, "mul");
    } else if (op == '/') {
      // result = Builder.CreateSDiv(L, R, "divtmp");
      result = BinaryOperator::Create(Instruction::SDiv, L, R, "div");

      // cast from int to double
      // Builder.CreateSIToFP(result, Type * DestTy, const Twine& Name = "")

    } else if (op == '^') {
      result = CreatePow(L, R);
    }

    // } else if (result_type == TYPE_STRING) {
    //   std::string typed_lhs_value = *(std::string*)lhs_value;
    //   std::string typed_rhs_value = *(std::string*)rhs_value;
    //   if (op == '+') {
    //     result = new NodeValue(typed_lhs_value + typed_rhs_value);
    //   }
  }

  codegen.push_back(std::unique_ptr<Value>(result));
  return codegen;
}

Value* BinaryExpNode::codegen(llvm::BasicBlock* bb) {
  return codegen_elements_retlast(this, bb);
}
}