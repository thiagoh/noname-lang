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

#ifndef CHAR_BIT
#define CHAR_BIT __CHAR_BIT__
#endif

extern FILE *fin;

namespace noname {

int debug = 0;
LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
std::unique_ptr<Module> TheModule;
std::unique_ptr<legacy::FunctionPassManager> TheFPM;
std::unique_ptr<llvm::orc::NonameJIT> TheJIT;

bool initialized = false;
Type *VoidTy;
PointerType *PointerTy_32;
PointerType *PointerTy_64;
PointerType *PointerTy_16;
PointerType *PointerTy_8;
PointerType *PointerTy_Double;
PointerType *PointerTy_Float;
StructType *StructTy_struct_datatype_t;
PointerType *PointerTy_StructTy_struct_datatype_t;
Function *func__Znwm;

ConstantInt *const_int32_0;
ConstantInt *const_int32_1;
ConstantInt *const_int32_2;
ConstantInt *const_int32_3;
ConstantInt *const_int32_4;
ConstantInt *const_int32_5;
ConstantInt *const_int32_6;
ConstantInt *const_int32_7;
ConstantInt *const_int32_8;
ConstantInt *const_int32_9;

ConstantInt *const_int64_0;
ConstantInt *const_int64_1;
ConstantInt *const_int64_2;
ConstantInt *const_int64_3;
ConstantInt *const_int64_4;
ConstantInt *const_int64_5;
ConstantInt *const_int64_6;
ConstantInt *const_int64_7;
ConstantInt *const_int64_8;
ConstantInt *const_int64_9;

ProcessorStrategy *astNodeProcessorStrategy;
ProcessorStrategy *expNodeProcessorStrategy;
ProcessorStrategy *topLevelExpNodeProcessorStrategy;
ProcessorStrategy *functionDefNodeProcessorStrategy;
ProcessorStrategy *assignmentNodeProcessorStrategy;
ProcessorStrategy *callNodeProcessorStrategy;
ProcessorStrategy *importNodeProcessorStrategy;

void InitializeNonameEnvironment() {
  if (initialized) {
    return;
  }

  // TheContext TheContext;
  // IRBuilder<> Builder(TheContext);
  // std::unique_ptr<Module> TheModule;
  // std::unique_ptr<legacy::FunctionPassManager> TheFPM;
  // std::unique_ptr<NonameJIT> TheJIT;

  const_int32_0 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));
  const_int32_1 = ConstantInt::get(TheContext, APInt(32, StringRef("1"), 10));
  const_int32_2 = ConstantInt::get(TheContext, APInt(32, StringRef("2"), 10));
  const_int32_3 = ConstantInt::get(TheContext, APInt(32, StringRef("3"), 10));
  const_int32_4 = ConstantInt::get(TheContext, APInt(32, StringRef("4"), 10));
  const_int32_5 = ConstantInt::get(TheContext, APInt(32, StringRef("5"), 10));
  const_int32_6 = ConstantInt::get(TheContext, APInt(32, StringRef("6"), 10));
  const_int32_7 = ConstantInt::get(TheContext, APInt(32, StringRef("7"), 10));
  const_int32_8 = ConstantInt::get(TheContext, APInt(32, StringRef("8"), 10));
  const_int32_9 = ConstantInt::get(TheContext, APInt(32, StringRef("9"), 10));

  const_int64_0 = ConstantInt::get(TheContext, APInt(64, StringRef("0"), 10));
  const_int64_1 = ConstantInt::get(TheContext, APInt(64, StringRef("1"), 10));
  const_int64_2 = ConstantInt::get(TheContext, APInt(64, StringRef("2"), 10));
  const_int64_3 = ConstantInt::get(TheContext, APInt(64, StringRef("3"), 10));
  const_int64_4 = ConstantInt::get(TheContext, APInt(64, StringRef("4"), 10));
  const_int64_5 = ConstantInt::get(TheContext, APInt(64, StringRef("5"), 10));
  const_int64_6 = ConstantInt::get(TheContext, APInt(64, StringRef("6"), 10));
  const_int64_7 = ConstantInt::get(TheContext, APInt(64, StringRef("7"), 10));
  const_int64_8 = ConstantInt::get(TheContext, APInt(64, StringRef("8"), 10));
  const_int64_9 = ConstantInt::get(TheContext, APInt(64, StringRef("9"), 10));

  // Now we going to create JIT

  VoidTy = Type::getVoidTy(TheContext);
  PointerTy_32 = PointerType::get(IntegerType::get(TheContext, 32), 0);
  PointerTy_64 = PointerType::get(IntegerType::get(TheContext, 64), 0);
  PointerTy_16 = PointerType::get(IntegerType::get(TheContext, 16), 0);
  PointerTy_8 = PointerType::get(IntegerType::get(TheContext, 8), 0);
  PointerTy_Double = PointerType::get(Type::getDoubleTy(TheContext), 0);
  PointerTy_Float = PointerType::get(Type::getFloatTy(TheContext), 0);

  StructTy_struct_datatype_t = StructType::create(TheContext, "struct.datatype_t");
  std::vector<Type *> StructTy_struct_datatype_t_fields;
  StructTy_struct_datatype_t_fields.push_back(IntegerType::get(TheContext, 32));
  StructTy_struct_datatype_t_fields.push_back(PointerTy_8);
  if (StructTy_struct_datatype_t->isOpaque()) {
    StructTy_struct_datatype_t->setBody(StructTy_struct_datatype_t_fields, /*isPacked=*/false);
  }

  PointerTy_StructTy_struct_datatype_t = PointerType::get(StructTy_struct_datatype_t, 0);

  // ###################################
  // ############## _Znwm ##############
  // ###################################

  std::vector<Type *> FuncTy_Znwm_args;
  FuncTy_Znwm_args.push_back(IntegerType::get(TheContext, 64));
  FunctionType *FuncTy_Znwm = FunctionType::get(
      /*Result=*/PointerTy_8,
      /*Params=*/FuncTy_Znwm_args,
      /*isVarArg=*/false);

  func__Znwm = TheModule->getFunction("_Znwm");
  if (!func__Znwm) {
    func__Znwm = Function::Create(
        /*Type=*/FuncTy_Znwm,
        /*Linkage=*/GlobalValue::ExternalLinkage,
        /*Name=*/"_Znwm", TheModule.get());  // (external, no body)
    func__Znwm->setCallingConv(CallingConv::C);
  }
  {
    AttributeSet func__Znwm_PAL;
    SmallVector<AttributeSet, 4> Attrs;
    AttributeSet PAS;
    {
      AttrBuilder B;
      B.addAttribute(Attribute::NoAlias);
      PAS = AttributeSet::get(TheContext, 0U, B);
    }

    Attrs.push_back(PAS);
    {
      AttrBuilder B;
      PAS = AttributeSet::get(TheContext, ~0U, B);
    }

    Attrs.push_back(PAS);
    func__Znwm_PAL = AttributeSet::get(TheContext, Attrs);
    func__Znwm->setAttributes(func__Znwm_PAL);
  }

  // ###################################
  // ############## END ################
  // ###################################

  // initialization
  astNodeProcessorStrategy = new ASTNodeProcessorStrategy();
  expNodeProcessorStrategy = new ExpNodeProcessorStrategy();
  topLevelExpNodeProcessorStrategy = new TopLevelExpNodeProcessorStrategy();
  functionDefNodeProcessorStrategy = new FunctionDefNodeProcessorStrategy();
  assignmentNodeProcessorStrategy = new AssignmentNodeProcessorStrategy();
  callNodeProcessorStrategy = new CallExpNodeProcessorStrategy();
  importNodeProcessorStrategy = new ImportNodeProcessorStrategy();

  initialized = true;
}
void ReleaseNonameEnvironment() {
  TheJIT->release();

  llvm_shutdown();

  delete context;

  delete astNodeProcessorStrategy;
  delete expNodeProcessorStrategy;
  delete topLevelExpNodeProcessorStrategy;
  delete functionDefNodeProcessorStrategy;
  delete assignmentNodeProcessorStrategy;
  delete callNodeProcessorStrategy;
  delete importNodeProcessorStrategy;

  if (noname::debug >= 1) {
    fprintf(stderr, "\n[END OF PROGRAM]");
  }
}

/// LogError* - These are little helper functions for error handling.
void createError(Error &error, const char *str) {
  char msg[1024];
  sprintf(msg, "%s\n", str);
  error.what(str);
}
ASTNode *logError(const char *str) {
  char msg[1024];
  sprintf(msg, "%s\n", str);
  yyerror(msg);
  // abort();
  // fprintf(stdout, "Error: %s\n", str);
  return nullptr;
}

FunctionDefNode *logErrorF(const char *str) {
  logError(str);
  return nullptr;
}

AssignmentNode *logErrorV(const char *str) {
  logError(str);
  return nullptr;
}

ASTNode *logError(ErrorNode *error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

NodeValue *logErrorNV(ErrorNode *error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

llvm::Value *logErrorLLVM(const char *str) {
  logError(str);
  return nullptr;
}

llvm::Function *logErrorLLVMF(const char *str) {
  logError(str);
  return nullptr;
}

llvm::Value *logErrorLLVM(ErrorNode *error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

llvm::AllocaInst *logErrorLLVMA(ErrorNode *error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

llvm::Function *logErrorLLVMF(ErrorNode *error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

void print_node_value(FILE *file, NodeValue *node_value) {
  if (noname::debug >= 2) {
    if (!node_value) {
      fprintf(file, "\n##########[print_node_value] undef");
    } else if (node_value->getType() == TYPE_INT) {
      fprintf(file, "\n##########[print_node_value] %d", *(int *)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_LONG) {
      fprintf(file, "\n##########[print_node_value] %ld", *(long *)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_DOUBLE) {
      fprintf(file, "\n##########[print_node_value] %lf", *(double *)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_STRING) {
      fprintf(file, "\n##########[print_node_value] %s", (*(std::string *)node_value->getRawValue()).c_str());
    } else {
      fprintf(file == stdout ? stderr : file, "\n##########[print_node_value] [WARN] could not print type %d", node_value->getType());
    }
  } else {
    if (!node_value) {
      fprintf(file, "undef");
    } else if (node_value->getType() == TYPE_INT) {
      fprintf(file, "%d", *(int *)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_LONG) {
      fprintf(file, "%ld", *(long *)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_DOUBLE) {
      fprintf(file, "%lf", *(double *)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_STRING) {
      fprintf(file, "%s", (*(std::string *)node_value->getRawValue()).c_str());
    } else {
      fprintf(file == stdout ? stderr : file, "[WARN] could not print type %d", node_value->getType());
    }
  }
}
void print_node_value(NodeValue *node_value) { print_node_value(stdout, node_value); }

void *call_jit_symbol(llvm::Type *result_type, JITSymbol &jit_symbol) {
  void *result = nullptr;
  // http://llvm.org/docs/doxygen/html/classllvm_1_1Value.html#pub-types
  if (!result_type) {
    assert(result_type && "Result type is null");
  } else if (result_type == llvm::Type::getVoidTy(TheContext)) {
    ;
  } else if (result_type == llvm::Type::getDoubleTy(TheContext)) {
    double (*function_pointer)() = (double (*)())(intptr_t)jit_symbol.getAddress();
    result = new double(function_pointer());

  } else if (result_type == llvm::Type::getFloatTy(TheContext)) {
    float (*function_pointer)() = (float (*)())(intptr_t)jit_symbol.getAddress();
    result = new float(function_pointer());

  } else if (result_type == llvm::Type::getInt64Ty(TheContext)) {
    long (*function_pointer)() = (long (*)())(intptr_t)jit_symbol.getAddress();
    result = new long(function_pointer());

  } else if (result_type == llvm::Type::getInt32Ty(TheContext)) {
    int (*function_pointer)() = (int (*)())(intptr_t)jit_symbol.getAddress();
    result = new int(function_pointer());

  } else if (result_type == llvm::Type::getInt16Ty(TheContext)) {
    short (*function_pointer)() = (short (*)())(intptr_t)jit_symbol.getAddress();
    result = new short(function_pointer());

  } else if (result_type == llvm::Type::getInt8Ty(TheContext)) {
    char (*function_pointer)() = (char (*)())(intptr_t)jit_symbol.getAddress();
    result = new char(function_pointer());

  } else if (result_type == StructTy_struct_datatype_t) {
    datatype_t (*function_pointer)() = (datatype_t(*)())(intptr_t)jit_symbol.getAddress();

    datatype_t tmp = function_pointer();
    datatype_t *output_datatype = (datatype_t *)malloc(sizeof(struct datatype_t));

    output_datatype->v = 0;
    output_datatype->type = 0;

    if (tmp.v) {
      output_datatype->v = tmp.v;
    }

    output_datatype->type = tmp.type;
    result = output_datatype;

  } else if (result_type == PointerTy_StructTy_struct_datatype_t) {
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fprintf(stdout, "\nNOT IMPLEMENTED YET");
    fflush(stdout);
  }

  return result;
}
void print_jit_symbol_value(llvm::Type *result_type, void *result) { print_jit_symbol_value(stdout, result_type, result); }
void print_jit_symbol_value(FILE *file, llvm::Type *result_type, void *result) {
  // http://llvm.org/docs/doxygen/html/classllvm_1_1Value.html#pub-types
  if (!result_type) {
    assert(result_type && "Result type is null");
  } else {
    int result_noname_type = toNonameType(result_type);
    print_jit_symbol_value(file, result_noname_type, result);

    if (noname::debug >= 1) {
      result_type->dump();
    }
  }
}

void print_jit_symbol_value(int result_type, void *result) { print_jit_symbol_value(stdout, result_type, result); }
void print_jit_symbol_value(FILE *file, int result_type, void *result) {
  // http://llvm.org/docs/doxygen/html/classllvm_1_1Value.html#pub-types
  if (noname::debug >= 2) {
    if (result_type == TYPE_VOID) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] undef");
      fflush(file);

    } else if (result_type == TYPE_DOUBLE) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %lf", *(double *)result);
      fflush(file);

    } else if (result_type == TYPE_FLOAT) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %f", *(float *)result);
      fflush(file);

    } else if (result_type == TYPE_LONG) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %ld", *(long *)result);
      fflush(file);

    } else if (result_type == TYPE_INT) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %d", *(int *)result);
      fflush(file);

    } else if (result_type == TYPE_SHORT) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %hd", *(short *)result);
      fflush(file);

    } else if (result_type == TYPE_CHAR) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %c", *(char *)result);
      fflush(file);

    } else if (result_type == TYPE_DATATYPE) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %d", (*(datatype_t *)result).type);
      fflush(file);

    } else if (result_type == TYPE_VOID_POINTER) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %p", result);
      fflush(file);
    } else {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] No such type found: %d", result_type);
      fflush(file);
    }
  } else {
    if (result_type == TYPE_VOID) {
      fprintf(file, "undef");
      fflush(file);
    } else if (result_type == TYPE_DOUBLE) {
      fprintf(file, "%lf", *(double *)result);
      fflush(file);

    } else if (result_type == TYPE_FLOAT) {
      fprintf(file, "%f", *(float *)result);
      fflush(file);

    } else if (result_type == TYPE_LONG) {
      fprintf(file, "%ld", *(long *)result);
      fflush(file);

    } else if (result_type == TYPE_INT) {
      fprintf(file, "%d", *(int *)result);
      fflush(file);

    } else if (result_type == TYPE_SHORT) {
      fprintf(file, "%hd", *(short *)result);
      fflush(file);

    } else if (result_type == TYPE_CHAR) {
      fprintf(file, "%c", *(char *)result);
      fflush(file);

    } else if (result_type == TYPE_DATATYPE) {
      datatype_t datatype_result = *((datatype_t *)result);
      print_jit_symbol_value(file, datatype_result.type, datatype_result.v);

    } else if (result_type == TYPE_VOID_POINTER) {
      fprintf(file, "%p", result);
      fflush(file);
    } else {
      fprintf(file, "No such type found: %d", result_type);
      fflush(file);
    }
  }
}
void *call_and_print_jit_symbol_value(FILE *file, llvm::Type *result_type, JITSymbol &jit_symbol) {
  void *result = call_jit_symbol(result_type, jit_symbol);
  print_jit_symbol_value(file, result_type, result);
  return result;
}

void *call_and_print_jit_symbol_value(llvm::Type *result_type, JITSymbol &jit_symbol) {
  return call_and_print_jit_symbol_value(stdout, result_type, jit_symbol);
}

// ReturnNode* new_return(ASTContext* context, ExpNode* exp_node) {
//   ReturnNode* new_node = new ReturnNode(context, exp_node);
//   return new_node;
// }

stmtlist_t *new_stmt_list(ASTContext *context) {
  stmtlist_t *head_stmt_list = (stmtlist_t *)malloc(sizeof(struct stmtlist_t));

  if (!head_stmt_list) {
    yyerror("out of space");
    exit(0);
  }
  head_stmt_list->first = nullptr;
  head_stmt_list->last = nullptr;
  return head_stmt_list;
}

void release(stmtlist_t *stmtlist);
void release(stmtlist_node_t *stmtlist_node);
void release(explist_t *explist);
void release(explist_node_t *explist_node);
void release(arg_t *arg);
void release(arglist_t *arglist);
void release(arglist_node_t *arglist_node);

stmtlist_t *new_stmt_list(ASTContext *context, ASTNode *ast_node) {
  stmtlist_t *head_stmt_list = (stmtlist_t *)malloc(sizeof(struct stmtlist_t));
  stmtlist_node_t *new_node = (stmtlist_node_t *)malloc(sizeof(struct stmtlist_node_t));

  if (!head_stmt_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = ast_node;
  new_node->next = nullptr;
  head_stmt_list->first = new_node;
  head_stmt_list->last = new_node;
  return head_stmt_list;
}

stmtlist_t *new_stmt_list(ASTContext *context, stmtlist_t *head_stmt_list, ASTNode *ast_node) {
  stmtlist_node_t *new_node = (stmtlist_node_t *)malloc(sizeof(struct stmtlist_node_t));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = ast_node;
  new_node->next = nullptr;
  head_stmt_list->last->next = new_node;
  head_stmt_list->last = new_node;
  return head_stmt_list;
}

explist_t *new_exp_list(ASTContext *context) {
  explist_t *head_exp_list = (explist_t *)malloc(sizeof(struct explist_t));

  if (!head_exp_list) {
    yyerror("out of space");
    exit(0);
  }
  head_exp_list->first = nullptr;
  head_exp_list->last = nullptr;
  return head_exp_list;
}

explist_t *new_exp_list(ASTContext *context, ExpNode *exp_node) {
  explist_t *head_exp_list = (explist_t *)malloc(sizeof(struct explist_t));
  explist_node_t *new_node = (explist_node_t *)malloc(sizeof(struct explist_node_t));

  if (!head_exp_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = exp_node;
  new_node->next = nullptr;
  head_exp_list->first = new_node;
  head_exp_list->last = new_node;
  return head_exp_list;
}

explist_t *new_exp_list(ASTContext *context, explist_t *head_exp_list, ExpNode *exp_node) {
  explist_node_t *new_node = (explist_node_t *)malloc(sizeof(struct explist_node_t));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = exp_node;
  new_node->next = nullptr;
  head_exp_list->last->next = new_node;
  head_exp_list->last = new_node;
  return head_exp_list;
}

arglist_t *new_arg_list(ASTContext *context) {
  arglist_t *head_arg_list = (arglist_t *)malloc(sizeof(struct arglist_t));

  if (!head_arg_list) {
    yyerror("out of space");
    exit(0);
  }
  head_arg_list->first = nullptr;
  head_arg_list->last = nullptr;
  return head_arg_list;
}

arglist_t *new_arg_list(ASTContext *context, arg_t *arg) {
  arglist_t *head_arg_list = (arglist_t *)malloc(sizeof(struct arglist_t));
  arglist_node_t *new_node = (arglist_node_t *)malloc(sizeof(struct arglist_node_t));

  if (!head_arg_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->arg = arg;
  new_node->next = nullptr;
  head_arg_list->first = new_node;
  head_arg_list->last = new_node;
  return head_arg_list;
}
arglist_t *new_arg_list(ASTContext *context, arglist_t *head_arg_list, arg_t *arg) {
  arglist_node_t *new_node = (arglist_node_t *)malloc(sizeof(struct arglist_node_t));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->arg = arg;
  new_node->next = nullptr;
  head_arg_list->last->next = new_node;
  head_arg_list->last = new_node;
  return head_arg_list;
}

arg_t *create_new_arg(ASTContext *context, char *arg_name) {
  arg_t *new_arg = (arg_t *)malloc(sizeof(struct arg_t));

  if (!new_arg) {
    yyerror("out of space");
    exit(0);
  }
  new_arg->name = arg_name;
  new_arg->default_value = nullptr;
  return new_arg;
}

arg_t *new_arg(ASTContext *context, char *arg_name, ExpNode *default_value) {
  arg_t *new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = default_value;
  return new_arg;
}

arg_t *new_arg(ASTContext *context, char *arg_name, double default_value) {
  arg_t *new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new NumberExpNode(context, default_value);
  return new_arg;
}
arg_t *new_arg(ASTContext *context, char *arg_name, long default_value) {
  arg_t *new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new NumberExpNode(context, default_value);
  return new_arg;
}
arg_t *new_arg(ASTContext *context, char *arg_name, char *default_value) {
  arg_t *new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new StringExpNode(context, default_value);
  return new_arg;
}

void release(stmtlist_t *stmtlist) {
  if (stmtlist->first) {
    release(stmtlist->first);  // this will free all the pointers -> up to the last
  }
  free(stmtlist);
}
void release(stmtlist_node_t *stmtlist_node) {
  if (stmtlist_node->node) {
    // delete stmtlist_node->node;
  }
  if (stmtlist_node->next) {
    release(stmtlist_node->next);
  }
  free(stmtlist_node);
}

void release(explist_t *explist) {
  ;
  ;
  //TODO: IMPLEMENT ME
}
void release(explist_node_t *explist_node) {
  ;
  ;
  //TODO: IMPLEMENT ME
}
void release(arg_t *arg) {
  ;
  ;
  //TODO: IMPLEMENT ME
}
void release(arglist_t *arglist) {
  ;
  ;
  //TODO: IMPLEMENT ME
}
void release(arglist_node_t *arglist_node) {
  ;
  ;
  //TODO: IMPLEMENT ME
}

llvm::Type *toLLVLType(int type) {
  if (type == TYPE_DOUBLE) {
    return llvm::Type::getDoubleTy(TheContext);
  } else if (type == TYPE_FLOAT) {
    return llvm::Type::getFloatTy(TheContext);
  } else if (type == TYPE_LONG) {
    return llvm::Type::getInt64Ty(TheContext);
  } else if (type == TYPE_INT) {
    return llvm::Type::getInt32Ty(TheContext);
  } else if (type == TYPE_SHORT) {
    return llvm::Type::getInt16Ty(TheContext);
  } else if (type == TYPE_CHAR) {
    return llvm::Type::getInt8Ty(TheContext);
  } else if (type == TYPE_VOID) {
    return llvm::Type::getVoidTy(TheContext);
  } else if (type == TYPE_VOID_POINTER) {
    return llvm::Type::getInt8PtrTy(TheContext);
  } else if (type == TYPE_DATATYPE) {
    return StructTy_struct_datatype_t;
  } else if (type == TYPE_DATATYPE_POINTER) {
    return PointerTy_StructTy_struct_datatype_t;
  }

  return nullptr;
}
llvm::Type *toLLVMType(llvm::Value *value) {
  llvm::Type *type = nullptr;

  if (!value) {
    type = llvm::Type::getVoidTy(TheContext);
  }

  if (!type) {
    if (isa<ReturnInst>(value)) {
      value = ((ReturnInst *)value)->getReturnValue();
    }

    if (isa<Function>(value)) {
      Function *function = (Function *)value;
      if (!function) {
        logError("Could not find function");
        return nullptr;
      }
      type = function->getReturnType();
      if (!type) {
        logError("Function returning invalid type");
        return nullptr;
      }
    } else if (isa<CallInst>(value)) {
      CallInst *call_inst = (CallInst *)value;
      Function *function = call_inst->getCalledFunction();
      if (!function) {
        logError("Could not find function");
        return nullptr;
      }
      type = function->getReturnType();
      if (!type) {
        logError("Function returning invalid type");
        return nullptr;
      }
    } else if (isa<LoadInst>(value)) {
      // https://stackoverflow.com/questions/29211941/is-any-way-to-get-llvm-deference-pointer-values-raw-typei-e-pointer-type
      LoadInst *load_inst = (LoadInst *)value;
      // load_inst->getType()->dump();
      // Value *value = load_inst->getPointerOperand();
      // PointerType* PT = cast<PointerType>(PO->getType());
      type = load_inst->getType();
      if (!type) {
        logError("LoadInst invalid type");
        return nullptr;
      }
    } else {
      type = value->getType();
    }
  }

  if (type) {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[## toLLVMType: %d]", type->getTypeID());
      fflush(stdout);
      type->dump();
    }
  }

  return type;
}
int toNonameType(llvm::Value *value) {
  llvm::Type *type = toLLVMType(value);
  return toNonameType(type);
}

int toNonameType(llvm::Type *type) {
  if (llvm::Type::getDoubleTy(TheContext) == type) {
    return TYPE_DOUBLE;
  } else if (llvm::Type::getFloatTy(TheContext) == type) {
    return TYPE_FLOAT;
  } else if (llvm::Type::getInt64Ty(TheContext) == type) {
    return TYPE_LONG;
  } else if (llvm::Type::getInt32Ty(TheContext) == type) {
    return TYPE_INT;
  } else if (llvm::Type::getInt16Ty(TheContext) == type) {
    return TYPE_SHORT;
  } else if (llvm::Type::getInt8Ty(TheContext) == type) {
    return TYPE_CHAR;
  } else if (llvm::Type::getVoidTy(TheContext) == type) {
    return TYPE_VOID;
  } else if (llvm::Type::getInt8PtrTy(TheContext) == type) {
    return TYPE_VOID_POINTER;
  } else if (StructTy_struct_datatype_t == type) {
    return TYPE_DATATYPE;
  } else if (PointerTy_StructTy_struct_datatype_t == type) {
    return TYPE_DATATYPE_POINTER;
  }

  return 0;
}
VarExpNode *new_var_node(ASTContext *context, const std::string name) {
  VarExpNode *new_node = new VarExpNode(context, name);
  return new_node;
}

ImportNode *new_import(ASTContext *context, std::string filename) {
  fprintf(stderr, "\n[new_import %s]", filename.c_str());
  ImportNode *new_node = new ImportNode(context, filename);
  return new_node;
}

std::unique_ptr<NodeValue> StringExpNode::getValue() const {
  NodeValue *node = new NodeValue(value);
  return std::unique_ptr<NodeValue>(node);
}

std::unique_ptr<NodeValue> VarExpNode::getValue() const {
  NodeValue *node = getContext()->getVariable(name);

  if (!node) {
    fprintf(stdout, "\n\n############ could not find %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
  }

  return std::unique_ptr<NodeValue>(node);
}

bool both_of_type(int lhs_type, int rhs_type, int type) { return lhs_type == type && rhs_type == type; }

bool any_of_type(int lhs_type, int rhs_type, int type) { return (lhs_type == type || rhs_type == type); }

bool match_to_types(int lhs_type, int rhs_type, int type1, int type2) {
  return ((lhs_type == type1 && rhs_type == type2) || (lhs_type == type2 && rhs_type == type1));
}

bool both_of_type(NodeValue *lhs, NodeValue *rhs, int type) { return lhs && rhs && both_of_type(lhs->getType(), rhs->getType(), type); }

bool any_of_type(NodeValue *lhs, NodeValue *rhs, int type) { return lhs && rhs && any_of_type(lhs->getType(), rhs->getType(), type); }

bool match_to_types(NodeValue *lhs, NodeValue *rhs, int type1, int type2) {
  return lhs && rhs && match_to_types(lhs->getType(), rhs->getType(), type1, type2);
}

int get_adequate_result_type(NodeValue *lhs, NodeValue *rhs) {
  if (any_of_type(lhs, rhs, TYPE_DOUBLE)) {
    return TYPE_DOUBLE;
  }
  if (both_of_type(lhs, rhs, TYPE_FLOAT)) {
    return TYPE_FLOAT;
  }
  if (match_to_types(lhs, rhs, TYPE_FLOAT, TYPE_LONG)) {
    return TYPE_DOUBLE;
  }
  if (any_of_type(lhs, rhs, TYPE_FLOAT)) {
    return TYPE_FLOAT;
  }
  if (any_of_type(lhs, rhs, TYPE_LONG)) {
    return TYPE_LONG;
  }
  if (any_of_type(lhs, rhs, TYPE_INT)) {
    return TYPE_INT;
  }
  if (any_of_type(lhs, rhs, TYPE_SHORT)) {
    return TYPE_SHORT;
  }
  if (any_of_type(lhs, rhs, TYPE_CHAR)) {
    return TYPE_CHAR;
  }
  return 0;
}

int get_adequate_result_type(int lhs_type, int rhs_type) {
  if (any_of_type(lhs_type, rhs_type, TYPE_DOUBLE)) {
    return TYPE_DOUBLE;
  }
  if (both_of_type(lhs_type, rhs_type, TYPE_FLOAT)) {
    return TYPE_FLOAT;
  }
  if (match_to_types(lhs_type, rhs_type, TYPE_FLOAT, TYPE_LONG)) {
    return TYPE_DOUBLE;
  }
  if (any_of_type(lhs_type, rhs_type, TYPE_FLOAT)) {
    return TYPE_FLOAT;
  }
  if (any_of_type(lhs_type, rhs_type, TYPE_LONG)) {
    return TYPE_LONG;
  }
  if (any_of_type(lhs_type, rhs_type, TYPE_INT)) {
    return TYPE_INT;
  }
  if (any_of_type(lhs_type, rhs_type, TYPE_SHORT)) {
    return TYPE_SHORT;
  }
  if (any_of_type(lhs_type, rhs_type, TYPE_CHAR)) {
    return TYPE_CHAR;
  }
  return 0;
}

void *ASTNodeProcessorStrategy::process(ASTNode *node) {
  fprintf(stderr, "\n[NOT IMPLEMENTED void *ASTNodeProcessorStrategy::process(ASTNode *node)]");
  // std::unique_ptr<NodeValue> return_value(node->getValue());
  // print_node_value(stdout, return_value.get());
  return nullptr;
}
void *ExpNodeProcessorStrategy::process(ASTNode *node) {
  ExpNode *exp_node = (ExpNode *)node;
  std::unique_ptr<NodeValue> return_value(exp_node->getValue());
  print_node_value(stdout, return_value.get());
  return nullptr;
}

void *ImportNodeProcessorStrategy::process(ASTNode *node) {
  ImportNode *import_node = (ImportNode *)node;

  char *file_path = get_file_path(import_node->getFilename().c_str());
  char *const_file_path[] = {file_path};
  // const char *const_file_path = file_path;
  FILE *opened_file = fopen(*const_file_path, "r");

  if (opened_file != NULL) {
    if (is_file_already_imported(*const_file_path)) {
      if (noname::debug >= 3) {
        fprintf(stdout, "\nNOTICE: File '%s' already imported\n", file_path);
      }
    } else {
      imported_files.push_back(file_path);
      read_from_file_import = true;
      fin = opened_file;
    }

  } else {
    fprintf(stderr, "\nError: File '%s' could not be opened.\n", file_path);
  }

  free(file_path);
  return nullptr;
}

std::unique_ptr<NodeValue> NumberExpNode::getValue() const {
  NodeValue *node = nullptr;

  if (type == TYPE_DOUBLE) {
    node = new NodeValue(*(double *)value);
  } else if (type == TYPE_LONG) {
    node = new NodeValue(*(long *)value);
  } else if (type == TYPE_INT) {
    node = new NodeValue(*(int *)value);
  } else if (type == TYPE_FLOAT) {
    node = new NodeValue(*(float *)value);
  } else if (type == TYPE_SHORT) {
    node = new NodeValue(*(short *)value);
  } else if (type == TYPE_CHAR) {
    node = new NodeValue(*(char *)value);
  } else {
    std::string msg("No such type " + std::to_string(type) + " is implemented for NodeValue *NumberExpNode::getValue()");
    node = logErrorNV(new ErrorNode(getContext(), msg));
  }

  return std::unique_ptr<NodeValue>(node);
}

//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//
std::vector<Value *> NumberExpNode::codegen_elements(Error &error, llvm::BasicBlock *bb) const {
  std::vector<Value *> codegen;
  std::unique_ptr<NodeValue> node(getValue());

  if (!node) {
    createError(error, "Invalid or undefined NodeValue");
    return codegen;
  }

  ConstantInt *const_int32_1 = ConstantInt::get(TheContext, APInt(32, StringRef("1"), 10));
  ConstantInt *const_int32_0 = ConstantInt::get(TheContext, APInt(32, StringRef("0"), 10));
  ConstantInt *const_int32_type = ConstantInt::get(TheContext, APInt(32, type, true));

  // struct datatype_t
  AllocaInst *alloca_datatype = alloca_typed_var_codegen(TYPE_DATATYPE, bb);

  codegen.push_back(alloca_datatype);
  // codegen.push_back(alloca_value);

  // typed value
  Value *constant_value = node->constant_codegen(bb);

  if (!constant_value) {
    createError(error, "Invalid or undefined constant value");
    return codegen;
  }

  GetElementPtrInst *get_elem_ptr_v =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype, {const_int32_0, const_int32_1}, "v", bb);
  GetElementPtrInst *get_elem_ptr_type =
      GetElementPtrInst::Create(StructTy_struct_datatype_t, alloca_datatype, {const_int32_0, const_int32_0}, "type", bb);

  codegen.push_back(get_elem_ptr_v);
  codegen.push_back(get_elem_ptr_type);

  AllocaInst *alloca_value_typed = alloca_typed_var_codegen(type, bb);
  codegen.push_back(store_typed_var_codegen(type, constant_value, alloca_value_typed, bb));

  CastInst *cast_inst_untyped = new BitCastInst(alloca_value_typed, PointerTy_8, "cast_inst_untyped", bb);
  codegen.push_back(cast_inst_untyped);

  StoreInst *store_ptr_v = store_typed_var_codegen(TYPE_VOID_POINTER, cast_inst_untyped, get_elem_ptr_v, bb);
  codegen.push_back(store_ptr_v);

  StoreInst *store_ptr_type = store_typed_var_codegen(TYPE_INT, const_int32_type, get_elem_ptr_type, bb);
  codegen.push_back(store_ptr_type);

  LoadInst *load_inst_type = load_inst_codegen(TYPE_DATATYPE, alloca_datatype, bb);
  codegen.push_back(load_inst_type);

  return codegen;
}
Value *NumberExpNode::codegen(llvm::BasicBlock *bb) {
  Value *value = codegen_elements_retlast(this, bb);
  if (!value) {
    return logErrorLLVM("Could not resolve constant expression");
  }
  return value;
}
Value *StringExpNode::codegen(llvm::BasicBlock *bb) {
  std::unique_ptr<NodeValue> node(getValue());

  if (!node) {
    return logErrorLLVM("Could not resolve string expression");
  }

  return node->constant_codegen(bb);
}
std::vector<Value *> StringExpNode::codegen_elements(Error &error, llvm::BasicBlock *bb) const {
  createError(error, "NOT IMPLEMENTED - std::vector<Value*> StringExpNode::codegen_elements");
  return std::vector<Value *>();
}
Value *VarExpNode::codegen(llvm::BasicBlock *bb) {
  //
  // TODO: FIXME: create a cache for this
  // this method should NOT look for NodeValue* then convert it to Value
  // this method should create a cache so that the code would generated once
  //

  NodeValue *node = getContext()->getVariable(name);

  if (!node) {
    char msg[1024];
    sprintf(msg, "\n\n############ could not find %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
    return logErrorLLVM(msg);
  }

  return node->constant_codegen(bb);
}
void prepare(Error &error, VarExpNode_Data_t &data, std::vector<Value *> &codegen, const VarExpNode *const node, llvm::BasicBlock *bb) {
  // prepare variable to receive return
  data.alloca_datatype = push_back_ret(codegen, alloca_typed_var_codegen(TYPE_DATATYPE, "_main", bb));

  if (!data.alloca_datatype) {
    createError(error, "AllocaInst could not be created");
    return;
  }

  data.get_elem_ptr_v = push_back_ret(codegen, get_element_ptr_v_codegen(data.alloca_datatype, "_main", bb));
  data.get_elem_ptr_type = push_back_ret(codegen, get_element_ptr_type_codegen(data.alloca_datatype, "_main", bb));

  AllocaInst *var_alloca_datatype;

  data.var_value = node->getContext()->getValue(node->getName());
  if (!data.var_value) {
    createError(error, "Value could not be found");
    return;
  }

  Type *llvm_type = node->getContext()->getValueType(node->getName());
  if (!llvm_type) {
    createError(error, "Value type could not be found");
    return;
  }

  data.var_type = toNonameType(llvm_type);
  GetElementPtrInst *var_get_elem_ptr_v;
  GetElementPtrInst *var_get_elem_ptr_type;
}
std::vector<Value *> VarExpNode::codegen_elements(Error &error, llvm::BasicBlock *bb) const {
  std::vector<Value *> codegen;

  VarExpNode_Data_t data;
  prepare(error, data, codegen, this, bb);

  StoreInst *store_inst = store_typed_var_codegen(TYPE_DATATYPE, data.var_value, data.alloca_datatype, bb);
  if (!store_inst) {
    createError(error, "StoreInst could not be created");
    return codegen;
  }

  codegen.push_back(store_inst);

  LoadInst *load_inst = load_inst_codegen(TYPE_DATATYPE, data.alloca_datatype, bb);
  if (!load_inst) {
    createError(error, "LoadInst could not be created");
    return codegen;
  }

  codegen.push_back(load_inst);

  return codegen;
}
}
