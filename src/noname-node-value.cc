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

NodeValue::NodeValue(const std::string& value) : type(TYPE_STRING), value(0) {
  this->value = new std::string(value);
}
NodeValue::NodeValue(int value) : type(TYPE_INT), value(0) {
  this->value = new int;
  memcpy(this->value, &value, sizeof(int));
}
NodeValue::NodeValue(double value) : type(TYPE_DOUBLE), value(0) {
  this->value = new double;
  memcpy(this->value, &value, sizeof(double));
}
NodeValue::NodeValue(long value) : type(TYPE_LONG), value(0) {
  this->value = new long;
  memcpy(this->value, &value, sizeof(long));
}

Value* constant_codegen_util(int type, void* value, llvm::BasicBlock* bb = nullptr) {
  Value* codegen = nullptr;

  if (type == TYPE_DOUBLE) {
    APFloat ap_value(*(double*)value);
    codegen = ConstantFP::get(TheContext, ap_value);
  } else if (type == TYPE_FLOAT) {
    APFloat ap_value(*(float*)value);
    codegen = ConstantFP::get(TheContext, ap_value);
  } else if (type == TYPE_LONG) {
    APInt ap_value(CHAR_BIT * sizeof(long), *(long*)value, true);
    codegen = ConstantInt::get(TheContext, ap_value);
  } else if (type == TYPE_INT) {
    // APInt (unsigned numBits, uint64_t val, bool isSigned=false)
    APInt ap_value(CHAR_BIT * sizeof(int), *(int*)value, true);
    codegen = ConstantInt::get(TheContext, ap_value);
  } else if (type == TYPE_SHORT) {
    APInt ap_value(CHAR_BIT * sizeof(short), *(short*)value, true);
    codegen = ConstantInt::get(TheContext, ap_value);
  } else if (type == TYPE_CHAR) {
    APInt ap_value(CHAR_BIT * sizeof(char), *(char*)value, true);
    codegen = ConstantInt::get(TheContext, ap_value);
  }

  return codegen;
}
Value* NodeValue::constant_codegen(llvm::BasicBlock* bb) {
  return constant_codegen_util(type, value, bb);
}

void* NodeValue::getValue(int as_type) {
  // string
  if (as_type == TYPE_STRING && type != TYPE_STRING) {
    return nullptr;
  }
  // double
  if (as_type == TYPE_DOUBLE) {
    if (type == TYPE_DOUBLE) {
      return value;
    }
    double* doublep_res = new double;
    if (type == TYPE_FLOAT) {
      double double_res = (double)*(float*)value;
      *doublep_res = double_res;
    } else if (type == TYPE_LONG) {
      double double_res = (double)*(long*)value;
      *doublep_res = double_res;
    } else if (type == TYPE_INT) {
      double double_res = (double)*(int*)value;
      *doublep_res = double_res;
    } else if (type == TYPE_SHORT) {
      double double_res = (double)*(short*)value;
      *doublep_res = double_res;
    } else if (type == TYPE_CHAR) {
      double double_res = (double)*(char*)value;
      *doublep_res = double_res;
    }
    return doublep_res;
  }
  // float
  if (as_type == TYPE_FLOAT) {
    if (type == TYPE_FLOAT) {
      return value;
    }
    float* floatp_res = new float;
    if (type == TYPE_DOUBLE) {
      float float_res = (float)*(double*)value;
      *floatp_res = float_res;
    } else if (type == TYPE_LONG) {
      float float_res = (float)*(long*)value;
      *floatp_res = float_res;
    } else if (type == TYPE_INT) {
      float float_res = (float)*(int*)value;
      *floatp_res = float_res;
    } else if (type == TYPE_SHORT) {
      float float_res = (float)*(short*)value;
      *floatp_res = float_res;
    } else if (type == TYPE_CHAR) {
      float float_res = (float)*(char*)value;
      *floatp_res = float_res;
    }
    return floatp_res;
  }
  // long
  if (as_type == TYPE_LONG) {
    if (type == TYPE_LONG) {
      return value;
    }
    long* longp_res = new long;
    if (type == TYPE_DOUBLE) {
      long long_res = (long)*(double*)value;
      *longp_res = long_res;
    } else if (type == TYPE_FLOAT) {
      long long_res = (long)*(float*)value;
      *longp_res = long_res;
    } else if (type == TYPE_INT) {
      long long_res = (long)*(int*)value;
      *longp_res = long_res;
    } else if (type == TYPE_SHORT) {
      long long_res = (long)*(short*)value;
      *longp_res = long_res;
    } else if (type == TYPE_CHAR) {
      long long_res = (long)*(char*)value;
      *longp_res = long_res;
    }
    return longp_res;
  }
  // int
  if (as_type == TYPE_INT) {
    if (type == TYPE_INT) {
      return value;
    }
    int* intp_res = new int;
    if (type == TYPE_DOUBLE) {
      int int_res = (int)*(double*)value;
      *intp_res = int_res;
    } else if (type == TYPE_FLOAT) {
      int int_res = (int)*(float*)value;
      *intp_res = int_res;
    } else if (type == TYPE_LONG) {
      int int_res = (int)*(long*)value;
      *intp_res = int_res;
    } else if (type == TYPE_SHORT) {
      int int_res = (int)*(short*)value;
      *intp_res = int_res;
    } else if (type == TYPE_CHAR) {
      int int_res = (int)*(char*)value;
      *intp_res = int_res;
    }
    return intp_res;
  }
  // short
  if (as_type == TYPE_SHORT) {
    if (type == TYPE_SHORT) {
      return value;
    }
    short* shortp_res = new short;
    if (type == TYPE_DOUBLE) {
      short short_res = (short)*(double*)value;
      *shortp_res = short_res;
    } else if (type == TYPE_FLOAT) {
      short short_res = (short)*(float*)value;
      *shortp_res = short_res;
    } else if (type == TYPE_LONG) {
      short short_res = (short)*(long*)value;
      *shortp_res = short_res;
    } else if (type == TYPE_INT) {
      short short_res = (short)*(int*)value;
      *shortp_res = short_res;
    } else if (type == TYPE_CHAR) {
      short short_res = (short)*(char*)value;
      *shortp_res = short_res;
    }
    return shortp_res;
  }
  // char
  if (as_type == TYPE_CHAR) {
    if (type == TYPE_CHAR) {
      return value;
    }
    char* charp_res = new char;
    if (type == TYPE_DOUBLE) {
      char char_res = (char)*(double*)value;
      *charp_res = char_res;
    } else if (type == TYPE_FLOAT) {
      char char_res = (char)*(float*)value;
      *charp_res = char_res;
    } else if (type == TYPE_LONG) {
      char char_res = (char)*(long*)value;
      *charp_res = char_res;
    } else if (type == TYPE_INT) {
      char char_res = (char)*(int*)value;
      *charp_res = char_res;
    } else if (type == TYPE_SHORT) {
      char char_res = (char)*(short*)value;
      *charp_res = char_res;
    }
    return charp_res;
  }
  return value;
}
}