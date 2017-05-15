#ifndef _NONAME_UTILS_H
#define _NONAME_UTILS_H

#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include <stdio.h>
#include <algorithm>
#include <memory>
#include <cctype>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <typeinfo>
#include <type_traits>

using namespace llvm;

namespace noname {

// template <typename Base, typename Derived>
// struct is_of_type_impl {
//   static inline bool doit(const Derived& derived) {
//     /**
//       * Inspiration sources
//       * http://en.cppreference.com/w/cpp/types/is_base_of
//       * https://repl.it/H4pc/1
//       *
//       * this will not work std::is_base_of<Base, Derived>::value
//       * because the Base and Derived types must be complete at this point
//       *
//       * If both Base and Derived are non-union class types, and they are not the same type
//       * (ignoring cv-qualification), Derived shall be a complete type; otherwise the behavior is undefined.
//       * EXPLANATION: http://en.cppreference.com/w/cpp/language/type#Incomplete_type
//       */
//     return derived.getType() == Base::getClassType() || std::is_base_of<Base, Derived>::value;
//   }
// };
// template <class Base, class Derived>
// inline bool is_of_type(const Derived& derived) {
//   return is_of_type_impl<Base, Derived>::doit(derived);
// }

class ASTNode;
class ASTContext;
class ErrorNode;
class LogicErrorNode;
class ExpNode;
class TopLevelExpNode;
class NodeValue;
class ImportNode;
class BinaryExpNode;
class VarExpNode;
class CallExpNode;
class FunctionSignature;
class FunctionDefNode;
class DeclarationNode;
class AssignmentNode;
class DeclarationAssignmentNode;

/* list of statements */
struct stmtlist_node_t;
struct stmtlist_t;
/* list of expressions */
struct explist_node_t;
struct explist_t;
/* list of args, for an argument list */
struct arg_t;
struct arglist_node_t;
struct arglist_t;

// class ProcessorStrategy;
// class ASTNodeProcessorStrategy;
// class ExpNodeProcessorStrategy;
// class TopLevelExpNodeProcessorStrategy;
// class FunctionDefNodeProcessorStrategy;
// class AssignmentNodeProcessorStrategy;
// class CallExpNodeProcessorStrategy;
// class ImportNodeProcessorStrategy;

template <typename To, typename From>
struct cast_impl {
  static inline To doit(const From& from) { return (To)from; }
};

template <class To, class From>
inline To cast(const From& from) {
  return cast_impl<To, From>::doit(from);
}

bool is_file_already_imported(const std::string& file_path);
bool is_file_already_imported(const char* file_path);
char* get_current_dir();
char* concat_strs(const char* format, const char* s1, const char* s2, int size);
char* get_file_path(const char* filename);
int noname_read(char* buf, int* result, int max_size);

void print_node_value(NodeValue* nodeValue);
void print_node_value(FILE* file, NodeValue* nodeValue);

void* call_jit_symbol(llvm::Type* result_type, llvm::orc::JITSymbol& jit_symbol);
void print_jit_symbol_value(FILE* file, llvm::Type* result_type, void* result);
void print_jit_symbol_value(llvm::Type* result_type, void* result);
void* call_and_print_jit_symbol_value(FILE* file, llvm::Type* result_type, llvm::orc::JITSymbol& jit_symbol);
void* call_and_print_jit_symbol_value(llvm::Type* result_type, llvm::orc::JITSymbol& jit_symbol);

stmtlist_t* new_stmt_list(ASTContext* context);
stmtlist_t* new_stmt_list(ASTContext* context, ASTNode* node);
stmtlist_t* new_stmt_list(ASTContext* context, stmtlist_t* head_exp_list, ASTNode* node);

explist_t* new_exp_list(ASTContext* context);
explist_t* new_exp_list(ASTContext* context, ExpNode* node);
explist_t* new_exp_list(ASTContext* context, explist_t* head_exp_list, ExpNode* node);
arg_t* new_arg(ASTContext* context, char* arg, ExpNode* defaultValue);
arg_t* new_arg(ASTContext* context, char* arg, double defaultValue);
arg_t* new_arg(ASTContext* context, char* arg, long defaultValue);
arg_t* new_arg(ASTContext* context, char* arg, char* defaultValue);
arglist_t* new_arg_list(ASTContext* context);
arglist_t* new_arg_list(ASTContext* context, arg_t* arg);
arglist_t* new_arg_list(ASTContext* context, arglist_t* head_arg_list, arg_t* arg);

void release(explist_t* explist);
void release(explist_node_t* explist_node);
void release(stmtlist_t* stmtlist);
void release(stmtlist_node_t* stmtlist_node);
void release(arg_t* arg);
void release(arglist_t* arglist);
void release(arglist_node_t* arglist_node);

ImportNode* new_import(ASTContext* context, std::string filename);
ASTNode* new_top_level_exp_node(ExpNode* node);
VarExpNode* new_var_node(ASTContext* context, const std::string name);
AssignmentNode* new_assignment_node(ASTContext* context, const std::string name, ExpNode* node);
AssignmentNode* new_declaration_node(ASTContext* context, const std::string name);
CallExpNode* new_call_node(ASTContext* context, const std::string name, explist_t* arg_exp_list = nullptr);
CallExpNode* new_call_node(ASTContext* context, Function* function, explist_t* arg_exp_list = nullptr);
ASTNode* new_function_def(ASTContext* context, const std::string name, arglist_t* arg_list, stmtlist_t* stmt_list,
                          ExpNode* returnNode);

// Codegen functions
Value* codegen_elements_retlast(ASTNode* node, llvm::BasicBlock* bb = nullptr);
llvm::AllocaInst* declaration_codegen_util(const ASTNode* node, llvm::BasicBlock* bb = nullptr);
std::vector<Value*> assign_codegen_util(llvm::AllocaInst* untyped_poiter_alloca, const AssignmentNode* assignment,
                                        llvm::BasicBlock* bb = nullptr);

bool both_of_type(NodeValue* lhs, NodeValue* rhs, int type);
bool any_of_type(NodeValue* lhs, NodeValue* rhs, int type);
bool match_to_types(NodeValue* lhs, NodeValue* rhs, int type1, int type2);
int get_adequate_result_type(NodeValue* lhs, NodeValue* rhs);
int get_adequate_result_type(int lhs_type, int rhs_type);

llvm::Type* toLLVLType(int type);
llvm::Type* toLLVMType(llvm::Value* return_value);
int toNonameType(llvm::Value* value);
int toNonameType(llvm::Type* type);
}

#endif