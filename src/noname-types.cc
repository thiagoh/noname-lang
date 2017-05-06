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
  if (noname::debug >= 2) {
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
  if (noname::debug >= 2) {
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

CallExpNode* new_call_node(ASTContext* context, const std::string name,
                           explist_t* exp_list) {
  CallExpNode* new_node = new CallExpNode(context, name, exp_list);
  return new_node;
}

ImportNode* new_import(ASTContext* context, std::string filename) {
  fprintf(stderr, "\n[new_import %s]", filename.c_str());
  ImportNode* new_node = new ImportNode(context, filename);

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
    if (noname::debug >= 1) {
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
    if (noname::debug >= 2) {
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


Value* NumberExpNode::codegen(llvm::BasicBlock* bb) {
  NodeValue* node = this->getValue();

  if (!node) {
    fprintf(stdout, "\n\n############ could not resolve number expression");
    return nullptr;
  }

  return node->codegen();
}
Value* StringExpNode::codegen(llvm::BasicBlock* bb) {
  NodeValue* node = this->getValue();

  if (!node) {
    logError("Could not resolve string expression");
    return nullptr;
  }

  return node->codegen();
}
Value* codegen_elements_retlast(ASTNode* node, llvm::BasicBlock* bb = nullptr) { 

  std::vector<std::unique_ptr<Value>> codegen_elements(node->codegen_elements());
  Value* last=nullptr;
  if (bb) {
    for (std::unique_ptr<Value>& ptr: codegen_elements) {
      last = ptr.get();
      bb->getInstList().push_back(std::move(ptr));
    }
  }
  
  return last;
}

Value* VarExpNode::codegen(llvm::BasicBlock* bb) {
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

Value* UnaryExpNode::codegen(llvm::BasicBlock* bb) {
  NodeValue* rhs_node_value = rhs->getValue();
  Value* R = rhs_node_value->codegen();
  fprintf(
      stdout,
      "\n\n############ IMPLEMENT ME: UnaryExpNode::codegen ###########\n\n");
  return R;
}
}
