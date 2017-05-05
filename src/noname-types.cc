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

extern FILE* fin;

namespace noname {

int debug = 0;
LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
std::unique_ptr<Module> TheModule;
std::unique_ptr<legacy::FunctionPassManager> TheFPM;
std::unique_ptr<llvm::orc::NonameJIT> TheJIT;

bool initialized = false;
PointerType* PointerTy_1;
PointerType* PointerTy_2;
PointerType* PointerTy_3;
PointerType* PointerTy_4;
PointerType* PointerTy_5;
PointerType* PointerTy_6;
PointerType* PointerTy_7;

std::string pow_function_name("_noname_function_pow");

void InitializeNonameEnvironment() {
  if (initialized) {
    return;
  }

  // LLVMContext TheContext;
  // IRBuilder<> Builder(TheContext);
  // std::unique_ptr<Module> TheModule;
  // std::unique_ptr<legacy::FunctionPassManager> TheFPM;
  // std::unique_ptr<NonameJIT> TheJIT;

  PointerTy_1 = PointerType::get(IntegerType::get(TheContext, 32), 0);
  PointerTy_2 = PointerType::get(IntegerType::get(TheContext, 64), 0);
  PointerTy_3 = PointerType::get(IntegerType::get(TheContext, 16), 0);
  PointerTy_4 = PointerType::get(IntegerType::get(TheContext, 8), 0);
  PointerTy_5 = PointerType::get(Type::getDoubleTy(TheContext), 0);
  PointerTy_6 = PointerType::get(Type::getFloatTy(TheContext), 0);
  PointerTy_7 = PointerType::get(PointerTy_4, 0);

  initialized = true;
}

/// LogError* - These are little helper functions for error handling.
ASTNode* logError(const char* str) {
  char msg[1024];
  sprintf(msg, "%s\n", str);
  yyerror(msg);
  // abort();
  // fprintf(stdout, "Error: %s\n", str);
  return nullptr;
}

FunctionDefNode* logErrorF(const char* str) {
  logError(str);
  return nullptr;
}

AssignmentNode* logErrorV(const char* str) {
  logError(str);
  return nullptr;
}

ASTNode* logError(ErrorNode* error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

NodeValue* logErrorNV(ErrorNode* error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

llvm::Value* logErrorLLVM(const char* str) {
  logError(str);
  return nullptr;
}

llvm::Function* logErrorLLVMF(const char* str) {
  logError(str);
  return nullptr;
}

llvm::Value* logErrorLLVM(ErrorNode* error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

llvm::Function* logErrorLLVMF(ErrorNode* error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

void print_node_value(FILE* file, NodeValue* node_value) {
  if (yydebug >= 2) {
    if (!node_value) {
      fprintf(file, "\n##########[print_node_value] undef");
    } else if (node_value->getType() == TYPE_INT) {
      fprintf(file, "\n##########[print_node_value] %d",
              *(int*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_LONG) {
      fprintf(file, "\n##########[print_node_value] %ld",
              *(long*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_DOUBLE) {
      fprintf(file, "\n##########[print_node_value] %lf",
              *(double*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_STRING) {
      fprintf(file, "\n##########[print_node_value] %s",
              (*(std::string*)node_value->getRawValue()).c_str());
    } else {
      fprintf(file == stdout ? stderr : file,
              "\n##########[print_node_value] [WARN] could not print type %d",
              node_value->getType());
    }
  } else {
    if (!node_value) {
      fprintf(file, "undef");
    } else if (node_value->getType() == TYPE_INT) {
      fprintf(file, "%d", *(int*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_LONG) {
      fprintf(file, "%ld", *(long*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_DOUBLE) {
      fprintf(file, "%lf", *(double*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_STRING) {
      fprintf(file, "%s", (*(std::string*)node_value->getRawValue()).c_str());
    } else {
      fprintf(file == stdout ? stderr : file, "[WARN] could not print type %d",
              node_value->getType());
    }
  }
}
void print_node_value(NodeValue* node_value) {
  print_node_value(stdout, node_value);
}

void* call_jit_symbol(LLVMContext& TheContext, llvm::Type* result_type,
                      JITSymbol& jit_symbol) {
  void* result = nullptr;
  // http://llvm.org/docs/doxygen/html/classllvm_1_1Value.html#pub-types
  if (!result_type) {
    assert(result_type && "Result type is null");
  } else if (result_type == llvm::Type::getVoidTy(TheContext)) {
    ;
  } else if (result_type == llvm::Type::getDoubleTy(TheContext)) {
    double (*function_pointer)() =
        (double (*)())(intptr_t)jit_symbol.getAddress();
    result = new double(function_pointer());

  } else if (result_type == llvm::Type::getFloatTy(TheContext)) {
    float (*function_pointer)() =
        (float (*)())(intptr_t)jit_symbol.getAddress();
    result = new float(function_pointer());

  } else if (result_type == llvm::Type::getInt64Ty(TheContext)) {
    long (*function_pointer)() = (long (*)())(intptr_t)jit_symbol.getAddress();
    result = new long(function_pointer());

  } else if (result_type == llvm::Type::getInt32Ty(TheContext)) {
    int (*function_pointer)() = (int (*)())(intptr_t)jit_symbol.getAddress();
    result = new int(function_pointer());

  } else if (result_type == llvm::Type::getInt16Ty(TheContext)) {
    short (*function_pointer)() =
        (short (*)())(intptr_t)jit_symbol.getAddress();
    result = new short(function_pointer());

  } else if (result_type == llvm::Type::getInt8Ty(TheContext)) {
    char (*function_pointer)() = (char (*)())(intptr_t)jit_symbol.getAddress();
    result = new char(function_pointer());
  }

  return result;
}

void print_jit_symbol_value(FILE* file, LLVMContext& TheContext,
                            llvm::Type* result_type, void* result) {
  // http://llvm.org/docs/doxygen/html/classllvm_1_1Value.html#pub-types
  if (yydebug >= 2) {
    if (!result_type) {
      assert(result_type && "Result type is null");
    } else if (result_type == llvm::Type::getVoidTy(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] undef");
    } else if (result_type == llvm::Type::getDoubleTy(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %lf",
              *(double*)result);

    } else if (result_type == llvm::Type::getFloatTy(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %f",
              *(float*)result);

    } else if (result_type == llvm::Type::getInt64Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %ld",
              *(long*)result);

    } else if (result_type == llvm::Type::getInt32Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %d",
              *(int*)result);

    } else if (result_type == llvm::Type::getInt16Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %hd",
              *(short*)result);

    } else if (result_type == llvm::Type::getInt8Ty(TheContext)) {
      fprintf(file, "\n###########[call_and_print_jit_symbol_value] %c",
              *(char*)result);
    }
  } else {
    if (!result_type) {
      assert(result_type && "Result type is null");
    } else if (result_type == llvm::Type::getVoidTy(TheContext)) {
      fprintf(file, "undef");
    } else if (result_type == llvm::Type::getDoubleTy(TheContext)) {
      fprintf(file, "%lf", *(double*)result);

    } else if (result_type == llvm::Type::getFloatTy(TheContext)) {
      fprintf(file, "%f", *(float*)result);

    } else if (result_type == llvm::Type::getInt64Ty(TheContext)) {
      fprintf(file, "%ld", *(long*)result);

    } else if (result_type == llvm::Type::getInt32Ty(TheContext)) {
      fprintf(file, "%d", *(int*)result);

    } else if (result_type == llvm::Type::getInt16Ty(TheContext)) {
      fprintf(file, "%hd", *(short*)result);

    } else if (result_type == llvm::Type::getInt8Ty(TheContext)) {
      fprintf(file, "%c", *(char*)result);
    } else {
      fprintf(file, "no such type found");
    }
  }
}
void print_jit_symbol_value(LLVMContext& TheContext, llvm::Type* result_type,
                            void* result) {
  print_jit_symbol_value(stdout, TheContext, result_type, result);
}
void* call_and_print_jit_symbol_value(FILE* file, LLVMContext& TheContext,
                                      llvm::Type* result_type,
                                      JITSymbol& jit_symbol) {
  void* result = call_jit_symbol(TheContext, result_type, jit_symbol);
  print_jit_symbol_value(file, TheContext, result_type, result);
  return result;
}

void* call_and_print_jit_symbol_value(LLVMContext& TheContext,
                                      llvm::Type* result_type,
                                      JITSymbol& jit_symbol) {
  return call_and_print_jit_symbol_value(stdout, TheContext, result_type,
                                         jit_symbol);
}

// ReturnNode* new_return(ASTContext* context, ExpNode* exp_node) {
//   ReturnNode* new_node = new ReturnNode(context, exp_node);
//   return new_node;
// }

stmtlist_t* new_stmt_list(ASTContext* context) {
  stmtlist_t* head_stmt_list = (stmtlist_t*)malloc(sizeof(struct stmtlist_t));

  if (!head_stmt_list) {
    yyerror("out of space");
    exit(0);
  }
  head_stmt_list->first = nullptr;
  head_stmt_list->last = nullptr;
  return head_stmt_list;
}

void release(stmtlist_t* stmtlist);
void release(stmtlist_node_t* stmtlist_node);
void release(explist_t* explist);
void release(explist_node_t* explist_node);
void release(arg_t* arg);
void release(arglist_t* arglist);
void release(arglist_node_t* arglist_node);

void release(stmtlist_t* stmtlist) {
  if (stmtlist->first) {
    release(
        stmtlist->first);  // this will free all the pointers -> up to the last
  }
  free(stmtlist);
}
void release(stmtlist_node_t* stmtlist_node) {
  if (stmtlist_node->node) {
    // delete stmtlist_node->node;
  }
  if (stmtlist_node->next) {
    release(stmtlist_node->next);
  }
  free(stmtlist_node);
}

void release(explist_t* explist) {}
void release(explist_node_t* explist_node) {}
void release(arg_t* arg) {}
void release(arglist_t* arglist) {}
void release(arglist_node_t* arglist_node) {}

stmtlist_t* new_stmt_list(ASTContext* context, ASTNode* ast_node) {
  stmtlist_t* head_stmt_list = (stmtlist_t*)malloc(sizeof(struct stmtlist_t));
  stmtlist_node_t* new_node =
      (stmtlist_node_t*)malloc(sizeof(struct stmtlist_node_t));

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

stmtlist_t* new_stmt_list(ASTContext* context, stmtlist_t* head_stmt_list,
                          ASTNode* ast_node) {
  stmtlist_node_t* new_node =
      (stmtlist_node_t*)malloc(sizeof(struct stmtlist_node_t));

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

explist_t* new_exp_list(ASTContext* context) {
  explist_t* head_exp_list = (explist_t*)malloc(sizeof(struct explist_t));

  if (!head_exp_list) {
    yyerror("out of space");
    exit(0);
  }
  head_exp_list->first = nullptr;
  head_exp_list->last = nullptr;
  return head_exp_list;
}

explist_t* new_exp_list(ASTContext* context, ExpNode* exp_node) {
  explist_t* head_exp_list = (explist_t*)malloc(sizeof(struct explist_t));
  explist_node_t* new_node =
      (explist_node_t*)malloc(sizeof(struct explist_node_t));

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

explist_t* new_exp_list(ASTContext* context, explist_t* head_exp_list,
                        ExpNode* exp_node) {
  explist_node_t* new_node =
      (explist_node_t*)malloc(sizeof(struct explist_node_t));

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

arglist_t* new_arg_list(ASTContext* context) {
  arglist_t* head_arg_list = (arglist_t*)malloc(sizeof(struct arglist_t));

  if (!head_arg_list) {
    yyerror("out of space");
    exit(0);
  }
  head_arg_list->first = nullptr;
  head_arg_list->last = nullptr;
  return head_arg_list;
}

arglist_t* new_arg_list(ASTContext* context, arg_t* arg) {
  arglist_t* head_arg_list = (arglist_t*)malloc(sizeof(struct arglist_t));
  arglist_node_t* new_node =
      (arglist_node_t*)malloc(sizeof(struct arglist_node_t));

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
arglist_t* new_arg_list(ASTContext* context, arglist_t* head_arg_list,
                        arg_t* arg) {
  arglist_node_t* new_node =
      (arglist_node_t*)malloc(sizeof(struct arglist_node_t));

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

arg_t* create_new_arg(ASTContext* context, char* arg_name) {
  arg_t* new_arg = (arg_t*)malloc(sizeof(struct arg_t));

  if (!new_arg) {
    yyerror("out of space");
    exit(0);
  }
  new_arg->name = arg_name;
  new_arg->default_value = nullptr;
  return new_arg;
}

arg_t* new_arg(ASTContext* context, char* arg_name, ExpNode* default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = default_value;
  return new_arg;
}

arg_t* new_arg(ASTContext* context, char* arg_name, double default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new NumberExpNode(context, default_value);
  return new_arg;
}
arg_t* new_arg(ASTContext* context, char* arg_name, long default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new NumberExpNode(context, default_value);
  return new_arg;
}
arg_t* new_arg(ASTContext* context, char* arg_name, char* default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new StringExpNode(context, default_value);
  return new_arg;
}

llvm::Type* toLLVLType(llvm::LLVMContext& LLVMContext, int type) {
  if (type == TYPE_DOUBLE) {
    return Type::getDoubleTy(LLVMContext);
  } else if (type == TYPE_FLOAT) {
    return Type::getFloatTy(LLVMContext);
  } else if (type == TYPE_LONG) {
    return Type::getInt64Ty(LLVMContext);
  } else if (type == TYPE_INT) {
    return Type::getInt32Ty(LLVMContext);
  } else if (type == TYPE_SHORT) {
    return Type::getInt16Ty(LLVMContext);
  } else if (type == TYPE_CHAR) {
    return Type::getInt8Ty(LLVMContext);
  }

  return nullptr;
}

VarExpNode* new_var_node(ASTContext* context, const std::string name) {
  VarExpNode* new_node = new VarExpNode(context, name);
  return new_node;
}
ASTNode* new_top_level_exp_node(ExpNode* exp_node) {
  auto* context = exp_node->getContext();

  ASTNode* anonymous_def_node =
      createAnnonymousFunctionDefNode(context, exp_node);

  if (anonymous_def_node && isa<ErrorNode>(*anonymous_def_node)) {
    return anonymous_def_node;
  }

  TopLevelExpNode* new_node = new TopLevelExpNode(
      context, exp_node, (FunctionDefNode*)anonymous_def_node);

  return new_node;
}
AssignmentNode* new_assignment_node(ASTContext* context, const std::string name,
                                    ExpNode* exp) {
  AssignmentNode* new_node = new AssignmentNode(context, name, exp);
  return new_node;
}
CallExpNode* new_call_node(ASTContext* context, const std::string name,
                           explist_t* exp_list) {
  CallExpNode* new_node = new CallExpNode(context, name, exp_list);
  return new_node;
}
AssignmentNode* new_declaration_node(ASTContext* context,
                                     const std::string name) {
  AssignmentNode* new_node = new AssignmentNode(context, name, NULL);

  NodeValue* temp_node = context->getVariable(name);
  if (temp_node) {
    return logErrorV("\nVariable already exists in this context!");
  }

  context->store(name, temp_node);  // temp_node is null. It doesn't matter,
                                    // it's only a declaration

  if (yydebug >= 1) {
    fprintf(stdout, "\n[new_assignment %s]", context->getName().c_str());
  }
  return new_node;
}

ImportNode* new_import(ASTContext* context, std::string filename) {
  fprintf(stderr, "\n[new_import %s]", filename.c_str());
  ImportNode* new_node = new ImportNode(context, filename);

  return new_node;
}

ASTNode* new_function_def(ASTContext* context, const std::string name,
                          arglist_t* arg_list, stmtlist_t* stmt_list,
                          ExpNode* return_node) {
  FunctionDefNode* new_node =
      new FunctionDefNode(context, name, arg_list, stmt_list, return_node);

  ASTNode* check_result = new_node->check();

  if (check_result && isa<ErrorNode>(*check_result)) {
    return check_result;
  }

  context->store(name, new_node);
  if (yydebug >= 1) {
    fprintf(stdout, "\n[new_function_def %s]", context->getName().c_str());
  }

  return new_node;
}

NodeValue* NumberExpNode::getValue() {
  NodeValue* node = nullptr;

  if (type == TYPE_DOUBLE) {
    node = new NodeValue(*(double*)value);
  } else if (type == TYPE_LONG) {
    node = new NodeValue(*(long*)value);
  } else if (type == TYPE_INT) {
    node = new NodeValue(*(int*)value);
  } else if (type == TYPE_FLOAT) {
    node = new NodeValue(*(float*)value);
  } else if (type == TYPE_SHORT) {
    node = new NodeValue(*(short*)value);
  } else if (type == TYPE_CHAR) {
    node = new NodeValue(*(char*)value);
  }

  return node;
}

NodeValue* StringExpNode::getValue() {
  NodeValue* node = new NodeValue(value);
  return node;
}

// UNNECESSARY
// void* VarExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* VarExpNode::getValue() {
  NodeValue* node = getContext()->getVariable(name);

  if (!node) {
    fprintf(stdout, "\n\n############ could not find %s on context %s \n\n",
            name.c_str(), getContext()->getName().c_str());
  }

  return node;
}

// UNNECESSARY
// void* UnaryExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* UnaryExpNode::getValue() {
  NodeValue* rhs_value = rhs->getValue();
  fprintf(
      stdout,
      "\n\n############ IMPLEMENT ME: UnaryExpNode::getValue ###########\n\n");
  return rhs_value;
}

bool both_of_type(NodeValue* lhs, NodeValue* rhs, int type) {
  return lhs && rhs && lhs->getType() == type && rhs->getType() == type;
}

bool any_of_type(NodeValue* lhs, NodeValue* rhs, int type) {
  return lhs && rhs && (lhs->getType() == type || rhs->getType() == type);
}

bool match_to_types(NodeValue* lhs, NodeValue* rhs, int type1, int type2) {
  return lhs && rhs && ((lhs->getType() == type1 && rhs->getType() == type2) ||
                        (lhs->getType() == type2 && rhs->getType() == type1));
}

int get_adequate_result_type(NodeValue* lhs, NodeValue* rhs) {
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

// UNNECESSARY
// void* BinaryExpNode::eval() { NodeValue* node_value = getValue(); }
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

void* DeclarationNode::eval() {
  NodeValue* node_value = nullptr;

  getContext()->store(name, node_value);

  return node_value;
}

void* AssignmentNode::eval() {
  NodeValue* node_value = getValue();

  getContext()->update(name, node_value);

  if (yydebug >= 2) {
    fprintf(stdout, "\n\n############ updated %s on context %s \n\n",
            name.c_str(), getContext()->getName().c_str());
  }

  return node_value;
}

void* DeclarationAssignmentNode::eval() {
  NodeValue* node_value = getValue();

  getContext()->store(name, node_value);

  if (yydebug >= 2) {
    fprintf(stdout, "\n\n############ stored %s on context %s \n\n",
            name.c_str(), getContext()->getName().c_str());
  }

  return node_value;
}

NodeValue* AssignmentNode::getValue() { return rhs->getValue(); }

// UNNECESSARY
// void* CallExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* CallExpNode::getValue() {
  ASTContext* call_exp_context = getContext();
  FunctionDefNode* function_node = call_exp_context->getFunction(getCallee());

  if (!function_node) {
    fprintf(stderr,
            "\n\nThe called function was: '%s' BUT it wan not found on the "
            "context\n",
            getCallee().c_str());
    return nullptr;
  }

  // ASTContext* function_context = function_node->getContext();
  // ASTContext* temp_context = new ASTContext("temp_context", call_exp_context,
  // function_context);
  // function_node->setContext(temp_context);

  ExpNode* returnNode = function_node->getReturnNode();
  std::vector<std::unique_ptr<ExpNode>>* value_args = &getArgs();
  std::vector<std::unique_ptr<ExpNode>>::iterator it_value_args =
      value_args->begin();
  std::vector<std::unique_ptr<arg_t>>* signature_args =
      &function_node->getArgs();
  std::vector<std::unique_ptr<arg_t>>::iterator it_signature_args =
      signature_args->begin();
  std::vector<std::unique_ptr<ASTNode>>* body_nodes =
      &function_node->getBodyNodes();
  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes =
      body_nodes->begin();

  for (; it_signature_args != signature_args->end() ||
         it_value_args != value_args->end();) {
    std::unique_ptr<ExpNode>& value_arg = *it_value_args;
    std::unique_ptr<arg_t>& signature_arg = *it_signature_args;

    call_exp_context->store(signature_arg->name, value_arg->getValue());

    ++it_signature_args;
    ++it_value_args;
  }

  for (; it_body_nodes != body_nodes->end();) {
    std::unique_ptr<ASTNode>& body_node = *it_body_nodes;

    body_node->eval();

    if (yydebug >= 1) {
      fprintf(stdout, "\n[## evaluating body: ASTNode of type %s]\n",
              ASTNode::toString(body_node->getKind()).c_str());
    }
    ++it_body_nodes;
  }

  if (returnNode) {
    if (yydebug >= 1) {
      fprintf(stdout, "\n[## evaluating return]\n");
    }
    return returnNode->getValue();
  } else {
    if (yydebug >= 1) {
      fprintf(stdout, "\n[## no return given]\n");
    }
  }

  // delete temp_context;
  // function_node->setContext(function_context);

  return nullptr;
}

void* ASTNodeProcessorStrategy::process(ASTNode* node) {
  NodeValue* return_value = (NodeValue*)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
void* ExpNodeProcessorStrategy::process(ASTNode* node) {
  ExpNode* exp_node = (ExpNode*)node;
  NodeValue* return_value = (NodeValue*)exp_node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}

void* TopLevelExpNodeProcessorStrategy::process(ASTNode* node) {
  TopLevelExpNode* top_level_exp_node = (TopLevelExpNode*)node;
  // NodeValue* return_value = (NodeValue*)top_level_exp_node->eval();
  // print_node_value(stdout, return_value);

  auto* top_level_node_ir = top_level_exp_node->codegen();

  if (!top_level_node_ir) {
    fprintf(stderr, "\nTop level expression could not be evaluated");
  } else {
    if (yydebug >= 1) {
      fprintf(stderr, "\n[read top level expression]");
      top_level_node_ir->dump();
    }

    // JIT the module containing the anonymous expression, keeping a handle so
    // we can free it later.
    TheJIT->writeToFile(TheModule.get());
    auto module_handle = TheJIT->addModule(std::move(TheModule));
    InitializeModuleAndPassManager();

    // Search the JIT for the __anon_expr symbol.
    auto ExprSymbol = TheJIT->findSymbol("__anon_expr");
    assert(ExprSymbol && "Function not found");

    llvm::Type* result_type = top_level_exp_node->getReturnLLVMType(TheContext);
    assert(result_type && "Result type is null");

    // result_type->print(dbgs(), true);

    // fprintf(stderr, "\n[type: %d]", result_type->getTypeID());

    call_and_print_jit_symbol_value(stdout, TheContext, result_type,
                                    ExprSymbol);

    // Delete the anonymous expression module from the JIT.
    TheJIT->removeModule(module_handle);
  }

  top_level_exp_node->release();

  return nullptr;
}
Value* TopLevelExpNode::codegen() {
  if (!anonymous_def_node) {
    fprintf(stderr, "\n\n############ could not resolve top level expression");
    return nullptr;
  }
  return anonymous_def_node->codegen();
}
Value* CallExpNode::codegen() {
  ASTContext* call_exp_context = getContext();

  FunctionDefNode* function_def_node =
      call_exp_context->getFunction(getCallee());

  if (!function_def_node) {
    fprintf(stderr,
            "\nError: the called function was: '%s' BUT it wan not found on "
            "the context\n",
            getCallee().c_str());
    return nullptr;
  }

  // return function_def_node->codegen();

  // Look up the name in the global module table.
  Function* function = function_def_node->getFunctionDefinition();
  if (!function) {
    return logErrorLLVM("Unknown function referenced");
  }

  std::vector<std::unique_ptr<ExpNode>>& value_args = getArgs();

  // If argument mismatch error.
  if (function->arg_size() != value_args.size()) {
    return logErrorLLVM("Incorrect # arguments passed");
  }

  std::vector<std::unique_ptr<ExpNode>>::iterator it_value_args =
      value_args.begin();
  std::vector<std::unique_ptr<arg_t>>& signature_args =
      function_def_node->getArgs();
  std::vector<std::unique_ptr<arg_t>>::iterator it_signature_args =
      signature_args.begin();
  std::vector<llvm::Value*> args_value;

  for (; it_signature_args != signature_args.end() ||
         it_value_args != value_args.end();) {
    std::unique_ptr<ExpNode>& value_arg = *it_value_args;
    std::unique_ptr<arg_t>& signature_arg = *it_signature_args;

    call_exp_context->store(signature_arg->name, value_arg->getValue());

    args_value.push_back(value_arg->codegen());
    if (!args_value.back()) {
      return nullptr;
    }

    ++it_signature_args;
    ++it_value_args;
  }

  llvm::CallInst* call_inst = nullptr;

  if (function->getReturnType() == llvm::Type::getVoidTy(TheContext)) {
    // Cannot assign a name to void values!
    // call_inst = Builder.CreateCall(function, args_value);
    call_inst = CallInst::Create(function, args_value);
    call_inst->setTailCall(false);
  } else {
    // call_inst = Builder.CreateCall(function, args_value, "__call_exp");
    call_inst = CallInst::Create(function, args_value, "__call_exp");
  }

  call_inst->setCallingConv(CallingConv::C);

  if (yydebug >= 1) {
    call_inst->dump();
  }

  return call_inst;
}
void* TopLevelExpNode::release() {
  if (anonymous_def_node) {
    getContext()->removeFunction(anonymous_def_node->getName());
    delete anonymous_def_node;
  }

  return nullptr;
}

void* FunctionDefNodeProcessorStrategy::process(ASTNode* node) {
  FunctionDefNode* function_def_node = (FunctionDefNode*)node;
  auto* function_ir = function_def_node->codegen();

  if (!function_ir) {
    fprintf(stderr, "\nFunction could not be defined");
  }
  if (function_ir) {
    if (yydebug >= 1) {
      fprintf(stderr, "\nRead function definition:");
      function_ir->dump();
    }
    TheJIT->writeToFile(TheModule.get());
    TheJIT->addModule(std::move(TheModule));
    InitializeModuleAndPassManager();
  }

  NodeValue* return_value = (NodeValue*)function_def_node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
void* AssignmentNodeProcessorStrategy::process(ASTNode* node) {
  NodeValue* return_value = (NodeValue*)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
void* CallExpNodeProcessorStrategy::process(ASTNode* node) {
  CallExpNode* call_exp_node = (CallExpNode*)node;
  FunctionDefNode* function_def_node =
      node->getContext()->getFunction(call_exp_node->getCallee());

  if (function_def_node) {
    if (yydebug >= 2) {
      fprintf(stdout, "\nThe called function was: '%s'\n",
              function_def_node->getName().c_str());
    }

    NodeValue* return_value = (NodeValue*)call_exp_node->eval();
    print_node_value(stdout, return_value);

  } else {
    fprintf(stderr, "\nError: The function %s was not found int the context\n",
            call_exp_node->getCallee().c_str());
  }
  return nullptr;
}

void* ImportNodeProcessorStrategy::process(ASTNode* node) {
  ImportNode* import_node = (ImportNode*)node;

  char* file_path = get_file_path(import_node->getFilename().c_str());
  char* const_file_path[] = {file_path};
  // const char *const_file_path = file_path;
  FILE* opened_file = fopen(*const_file_path, "r");

  if (opened_file != NULL) {
    if (is_file_already_imported(*const_file_path)) {
      if (yydebug >= 3) {
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

// initialization
ProcessorStrategy* astNodeProcessorStrategy = new ASTNodeProcessorStrategy();
ProcessorStrategy* expNodeProcessorStrategy = new ExpNodeProcessorStrategy();
ProcessorStrategy* topLevelExpNodeProcessorStrategy =
    new TopLevelExpNodeProcessorStrategy();
ProcessorStrategy* functionDefNodeProcessorStrategy =
    new FunctionDefNodeProcessorStrategy();
ProcessorStrategy* assignmentNodeProcessorStrategy =
    new AssignmentNodeProcessorStrategy();
ProcessorStrategy* callNodeProcessorStrategy =
    new CallExpNodeProcessorStrategy();
ProcessorStrategy* importNodeProcessorStrategy =
    new ImportNodeProcessorStrategy();

//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//

Value* NodeValue::codegen() {
  Value* llvm_value = nullptr;

  if (type == TYPE_DOUBLE) {
    llvm_value = ConstantFP::get(TheContext, APFloat(*(double*)value));
  } else if (type == TYPE_FLOAT) {
    llvm_value = ConstantFP::get(TheContext, APFloat(*(float*)value));
  } else if (type == TYPE_LONG) {
    APInt v(CHAR_BIT * sizeof(long), *(long*)value, true);
    llvm_value = ConstantInt::get(TheContext, v);
  } else if (type == TYPE_INT) {
    // APInt (unsigned numBits, uint64_t val, bool isSigned=false)
    APInt v(CHAR_BIT * sizeof(int), *(int*)value, true);
    llvm_value = ConstantInt::get(TheContext, v);
  } else if (type == TYPE_SHORT) {
    APInt v(CHAR_BIT * sizeof(short), *(short*)value, true);
    llvm_value = ConstantInt::get(TheContext, v);
  } else if (type == TYPE_CHAR) {
    APInt v(CHAR_BIT * sizeof(char), *(char*)value, true);
    llvm_value = ConstantInt::get(TheContext, v);
  }

  return llvm_value;
}

Value* NumberExpNode::codegen() {
  NodeValue* node = this->getValue();

  if (!node) {
    fprintf(stdout, "\n\n############ could not resolve number expression");
    return nullptr;
  }

  return node->codegen();
}
Value* StringExpNode::codegen() {
  NodeValue* node = this->getValue();

  if (!node) {
    fprintf(stdout, "\n\n############ could not resolve string expression");
    return nullptr;
  }

  return node->codegen();
}
Value* AssignmentNode::codegen() {
  fprintf(stderr, "\n[Value* AssignmentNode::codegen() - NOT IMPLEMENTED]");
  return nullptr;
}
Value* DeclarationAssignmentNode::codegen() {
  fprintf(stderr,
          "\n[Value* DeclarationAssignmentNode::codegen() - NOT IMPLEMENTED]");
  return nullptr;
}

Value* DeclarationNode::codegen() {
  std::string alloca_name = "alloca_" + name;

  AllocaInst* alloca_inst = new AllocaInst(PointerTy_4, alloca_name);
  alloca_inst->setAlignment(8);

  return alloca_inst;
}
Value* VarExpNode::codegen() {
  //
  // TODO FIXME: create a cache for this
  // this method should NOT look for NodeValue* then convert it to Value
  // this method should create a cache so that the code would generated once
  //

  NodeValue* node = getContext()->getVariable(name);

  if (!node) {
    fprintf(stdout, "\n\n############ could not find %s on context %s \n\n",
            name.c_str(), getContext()->getName().c_str());
    return nullptr;
  }

  return node->codegen();
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
  fprintf(stderr, "\nError: pow function not implemented");
  return nullptr;
}

Value* BinaryExpNode::codegen() {
  NodeValue* lhs_node_value = lhs->getValue();
  NodeValue* rhs_node_value = rhs->getValue();

  int result_type = get_adequate_result_type(lhs_node_value, rhs_node_value);
  void* lhs_value = lhs_node_value->getValue(result_type);
  void* rhs_value = rhs_node_value->getValue(result_type);
  Value* result = nullptr;

  Value* L = lhs_node_value->codegen();
  Value* R = rhs_node_value->codegen();

  if (!L || !R) {
    return result;
  }

  ////////////////////////////////
  ////////////////////////////////
  ////////////////////////////////
  ////////////////////////////////

  /*
  how to insert into correct point
  http://llvm.org/docs/doxygen/html/Instruction_8cpp_source.html#l00023
  Instruction::Instruction(Type *ty, unsigned it, Use *Ops, unsigned NumOps,
                          Instruction *InsertBefore)
   : User(ty, Value::InstructionVal + it, Ops, NumOps), Parent(nullptr) {

   // If requested, insert this instruction into a basic block...
   if (InsertBefore) {
     BasicBlock *BB = InsertBefore->getParent();
     assert(BB && "Instruction to insert before is not in a basic block!");
     BB->getInstList().insert(InsertBefore->getIterator(), this);
   }
 }

 Instruction::Instruction(Type *ty, unsigned it, Use *Ops, unsigned NumOps,
                          BasicBlock *InsertAtEnd)
   : User(ty, Value::InstructionVal + it, Ops, NumOps), Parent(nullptr) {

   // append this instruction into the basic block
   assert(InsertAtEnd && "Basic block to append to may not be NULL!");
   InsertAtEnd->getInstList().push_back(this);
 }


  */

  ////////////////////////////////
  ////////////////////////////////
  ////////////////////////////////
  ////////////////////////////////

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
    int typed_lhs_value = *(int*)lhs_value;
    int typed_rhs_value = *(int*)rhs_value;

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

  return result;
}

Value* UnaryExpNode::codegen() {
  NodeValue* rhs_node_value = rhs->getValue();
  Value* R = rhs_node_value->codegen();
  fprintf(
      stdout,
      "\n\n############ IMPLEMENT ME: UnaryExpNode::codegen ###########\n\n");
  return R;
}
}
