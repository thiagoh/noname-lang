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
PointerType *PointerTy_1;
PointerType *PointerTy_2;
PointerType *PointerTy_3;
PointerType *PointerTy_4;
PointerType *PointerTy_5;
PointerType *PointerTy_6;
PointerType *PointerTy_7;

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

  // Now we going to create JIT

  PointerTy_1 = PointerType::get(IntegerType::get(TheContext, 32), 0);
  PointerTy_2 = PointerType::get(IntegerType::get(TheContext, 64), 0);
  PointerTy_3 = PointerType::get(IntegerType::get(TheContext, 16), 0);
  PointerTy_4 = PointerType::get(IntegerType::get(TheContext, 8), 0);
  PointerTy_5 = PointerType::get(Type::getDoubleTy(TheContext), 0);
  PointerTy_6 = PointerType::get(Type::getFloatTy(TheContext), 0);
  PointerTy_7 = PointerType::get(PointerTy_4, 0);

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

/// LogError* - These are little helper functions for error handling.
Error *createError(const char *str) {
  char msg[1024];
  sprintf(msg, "%s\n", str);
  return new Error(msg);
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
      fprintf(file == stdout ? stderr : file, "\n##########[print_node_value] [WARN] could not print type %d",
              node_value->getType());
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
  }

  return result;
}

void print_jit_symbol_value(FILE *file, llvm::Type *result_type, void *result) {
  // http://llvm.org/docs/doxygen/html/classllvm_1_1Value.html#pub-types
  if (noname::debug >= 2) {
    if (!result_type) {
      assert(result_type && "Result type is null");
    } else if (result_type == llvm::Type::getVoidTy(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] undef");
    } else if (result_type == llvm::Type::getDoubleTy(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %lf", *(double *)result);

    } else if (result_type == llvm::Type::getFloatTy(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %f", *(float *)result);

    } else if (result_type == llvm::Type::getInt64Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %ld", *(long *)result);

    } else if (result_type == llvm::Type::getInt32Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %d", *(int *)result);

    } else if (result_type == llvm::Type::getInt16Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %hd", *(short *)result);

    } else if (result_type == llvm::Type::getInt8Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %c", *(char *)result);
    }
  } else {
    if (!result_type) {
      assert(result_type && "Result type is null");
    } else if (result_type == llvm::Type::getVoidTy(TheContext)) {
      fprintf(file, "undef");
    } else if (result_type == llvm::Type::getDoubleTy(TheContext)) {
      fprintf(file, "%lf", *(double *)result);

    } else if (result_type == llvm::Type::getFloatTy(TheContext)) {
      fprintf(file, "%f", *(float *)result);

    } else if (result_type == llvm::Type::getInt64Ty(TheContext)) {
      fprintf(file, "%ld", *(long *)result);

    } else if (result_type == llvm::Type::getInt32Ty(TheContext)) {
      fprintf(file, "%d", *(int *)result);

    } else if (result_type == llvm::Type::getInt16Ty(TheContext)) {
      fprintf(file, "%hd", *(short *)result);

    } else if (result_type == llvm::Type::getInt8Ty(TheContext)) {
      fprintf(file, "%c", *(char *)result);
    } else {
      fprintf(file, "No such type found");
      result_type->dump();
    }
  }
}
void print_jit_symbol_value(llvm::Type *result_type, void *result) {
  print_jit_symbol_value(stdout, result_type, result);
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

void release(explist_t *explist) {}
void release(explist_node_t *explist_node) {}
void release(arg_t *arg) {}
void release(arglist_t *arglist) {}
void release(arglist_node_t *arglist_node) {}

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
  }

  return nullptr;
}
llvm::Type *toLLVMType(llvm::Value *value) {
  llvm::Type *type = nullptr;

  if (!value) {
    type = llvm::Type::getVoidTy(TheContext);
  } else if (isa<Function>(value)) {
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
  } else {
    type = value->getType();
    if (noname::debug >= 1) {
      fprintf(stderr, "\n[## toLLVMType: %d]", type->getTypeID());
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

std::unique_ptr<NodeValue> StringExpNode::getValue() {
  NodeValue *node = new NodeValue(value);
  return std::unique_ptr<NodeValue>(node);
}

std::unique_ptr<NodeValue> VarExpNode::getValue() {
  NodeValue *node = getContext()->getVariable(name);

  if (!node) {
    fprintf(stdout, "\n\n############ could not find %s on context %s \n\n", name.c_str(),
            getContext()->getName().c_str());
  }

  return std::unique_ptr<NodeValue>(node);
}

bool both_of_type(int lhs_type, int rhs_type, int type) { return lhs_type == type && rhs_type == type; }

bool any_of_type(int lhs_type, int rhs_type, int type) { return (lhs_type == type || rhs_type == type); }

bool match_to_types(int lhs_type, int rhs_type, int type1, int type2) {
  return ((lhs_type == type1 && rhs_type == type2) || (lhs_type == type2 && rhs_type == type1));
}

bool both_of_type(NodeValue *lhs, NodeValue *rhs, int type) {
  return lhs && rhs && both_of_type(lhs->getType(), rhs->getType(), type);
}

bool any_of_type(NodeValue *lhs, NodeValue *rhs, int type) {
  return lhs && rhs && any_of_type(lhs->getType(), rhs->getType(), type);
}

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
  return -1;
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
  return -1;
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

std::unique_ptr<NodeValue> NumberExpNode::getValue() {
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
    std::string error_msg("No such type " + std::to_string(type) +
                          " is implemented for NodeValue *NumberExpNode::getValue()");
    node = logErrorNV(new ErrorNode(getContext(), error_msg));
  }

  return std::unique_ptr<NodeValue>(node);
}

//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//
std::vector<Value *> NumberExpNode::codegen_elements(Error **error, llvm::BasicBlock *bb) {
  std::vector<Value *> codegen;
  std::unique_ptr<NodeValue> node(getValue());

  if (!node) {
    *error = createError("Invalid or undefined NodeValue");
    return codegen;
  }

  Value *constant_value = node->constant_codegen(bb);

  if (!constant_value) {
    *error = createError("Invalid or undefined constant value");
    return codegen;
  }

  codegen.push_back(constant_value);
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
std::vector<Value *> StringExpNode::codegen_elements(Error **error, llvm::BasicBlock *bb) {
  *error = createError("NOT IMPLEMENTED - std::vector<Value*> StringExpNode::codegen_elements");
  return std::vector<Value *>();
}
Value *VarExpNode::codegen(llvm::BasicBlock *bb) {
  //
  // TODO FIXME: create a cache for this
  // this method should NOT look for NodeValue* then convert it to Value
  // this method should create a cache so that the code would generated once
  //

  NodeValue *node = getContext()->getVariable(name);

  if (!node) {
    char msg[1024];
    sprintf(msg, "\n\n############ could not find %s on context %s \n\n", name.c_str(),
            getContext()->getName().c_str());
    return logErrorLLVM(msg);
  }

  return node->constant_codegen(bb);
}
std::vector<Value *> VarExpNode::codegen_elements(Error **error, llvm::BasicBlock *bb) {
  *error = createError("NOT IMPLEMENTED - std::vector<Value*> VarExpNode::codegen_elements");
  return std::vector<Value *>();
}
}
