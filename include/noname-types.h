#ifndef _NONAME_TYPES_H
#define _NONAME_TYPES_H

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
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "noname-utils.h"
#include "lexer-utilities.h"
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
#include <stack>
#include <vector>

extern void yyerror(const char* error_msg);

using namespace llvm;
// using namespace llvm::orc;

namespace noname {

/* Token type.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
enum yytokentype {
  TYPE_VOID = 32,
  TYPE_VOID_POINTER = 33,
  TYPE_CHAR = 34,
  TYPE_SHORT = 35,
  TYPE_INT = 36,
  TYPE_FLOAT = 37,
  TYPE_LONG = 38,
  TYPE_DOUBLE = 39,
  TYPE_STRING = 40,
};
#endif

// #define TYPE_CHAR 32
// #define TYPE_SHORT 33
// #define TYPE_INT 34
// #define TYPE_FLOAT 35
// #define TYPE_LONG 36
// #define TYPE_DOUBLE 37
// #define TYPE_STRING 38

class Error;
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
class FunctionDefNode;
class DeclarationNode;
class AssignmentNode;
class DeclarationAssignmentNode;

class ProcessorStrategy;
class ASTNodeProcessorStrategy;
class ExpNodeProcessorStrategy;
class TopLevelExpNodeProcessorStrategy;
class FunctionDefNodeProcessorStrategy;
class AssignmentNodeProcessorStrategy;
class CallExpNodeProcessorStrategy;
class ImportNodeProcessorStrategy;

extern ProcessorStrategy* astNodeProcessorStrategy;
extern ProcessorStrategy* expNodeProcessorStrategy;
extern ProcessorStrategy* topLevelExpNodeProcessorStrategy;
extern ProcessorStrategy* functionDefNodeProcessorStrategy;
extern ProcessorStrategy* assignmentNodeProcessorStrategy;
extern ProcessorStrategy* callNodeProcessorStrategy;
extern ProcessorStrategy* importNodeProcessorStrategy;

extern PointerType* PointerTy_1;
extern PointerType* PointerTy_2;
extern PointerType* PointerTy_3;
extern PointerType* PointerTy_4;
extern PointerType* PointerTy_5;
extern PointerType* PointerTy_6;
extern PointerType* PointerTy_7;

extern int debug;
extern ASTContext* context;
extern std::vector<std::string> imported_files;
extern std::stack<ASTContext*> context_stack;
extern std::map<int, std::string> map;
extern bool read_from_file_import;

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
// extern std::unique_ptr<llvm::orc::NonameJIT> TheJIT;

/* list of statements */
typedef struct stmtlist_node_t {
  ASTNode* node;
  stmtlist_node_t* next;
} stmtlist_node_t;
typedef struct stmtlist_t {
  stmtlist_node_t* first;
  stmtlist_node_t* last;
} stmtlist_t;
/* list of expressions */
struct explist_node_t {
  ExpNode* node;
  struct explist_node_t* next;
};
struct explist_t {
  struct explist_node_t* first;
  struct explist_node_t* last;
};
/* list of args, for an argument list */
struct arg_t {
  char* name;
  ExpNode* default_value;
};
struct arglist_node_t {
  struct arg_t* arg;
  struct arglist_node_t* next;
};
struct arglist_t {
  struct arglist_node_t* first;
  struct arglist_node_t* last;
};

typedef struct explist_t explist_t;
typedef struct explist_node_t explist_node_t;
typedef struct arglist_t arglist_t;
typedef struct arglist_node_t arglist_node_t;
typedef struct stmtlist_t stmtlist_t;
typedef struct stmtlist_node_t stmtlist_node_t;
typedef struct arg_t arg_t;

ASTNode* logError(const char* str);
FunctionDefNode* logErrorF(const char* str);
AssignmentNode* logErrorV(const char* str);
ASTNode* logError(ErrorNode* error_node);
NodeValue* logErrorNV(ErrorNode* error_node);

/// LogError* - These are little helper functions for error handling.
Error* createError(const char* str);
ASTNode* logError(const char* str);
FunctionDefNode* logErrorF(const char* str);
AssignmentNode* logErrorV(const char* str);
ASTNode* logError(ErrorNode* error_node);
NodeValue* logErrorNV(ErrorNode* error_node);
llvm::Value* logErrorLLVM(const char* str);
llvm::Function* logErrorLLVMF(const char* str);
llvm::Value* logErrorLLVM(ErrorNode* error_node);
llvm::AllocaInst* logErrorLLVMA(ErrorNode* error_node);
llvm::Function* logErrorLLVMF(ErrorNode* error_node);

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

void InitializeNonameEnvironment();
void ReleaseNonameEnvironment();
ImportNode* new_import(ASTContext* context, std::string filename);
ASTNode* new_top_level_exp_node(ExpNode* node);
VarExpNode* new_var_node(ASTContext* context, const std::string name);
AssignmentNode* new_assignment_node(ASTContext* context, const std::string name, ExpNode* node);
AssignmentNode* new_declaration_node(ASTContext* context, const std::string name);
CallExpNode* new_call_node(ASTContext* context, const std::string name, explist_t* arg_exp_list = nullptr);
CallExpNode* new_call_node(ASTContext* context, FunctionDefNode* function_def_node, explist_t* arg_exp_list = nullptr);
ASTNode* new_function_def(ASTContext* context, const std::string name, arglist_t* arg_list, stmtlist_t* stmt_list,
                          ExpNode* returnNode);

// Codegen functions
Value* codegen_elements_retlast(ASTNode* node, llvm::BasicBlock* bb = nullptr);
llvm::AllocaInst* declaration_codegen_util(ASTNode* node, llvm::BasicBlock* bb = nullptr);
std::vector<Value*> assign_codegen_util(llvm::AllocaInst* untyped_poiter_alloca, AssignmentNode* assignment,
                                        llvm::BasicBlock* bb = nullptr);

bool both_of_type(NodeValue* lhs, NodeValue* rhs, int type);
bool any_of_type(NodeValue* lhs, NodeValue* rhs, int type);
bool match_to_types(NodeValue* lhs, NodeValue* rhs, int type1, int type2);
int get_adequate_result_type(NodeValue* lhs, NodeValue* rhs);
int get_adequate_result_type(int lhs_type, int rhs_type);

void release(explist_t* explist);
void release(explist_node_t* explist_node);
void release(stmtlist_t* stmtlist);
void release(stmtlist_node_t* stmtlist_node);
void release(arg_t* arg);
void release(arglist_t* arglist);
void release(arglist_node_t* arglist_node);

llvm::Type* toLLVLType(int type);
llvm::Type* toLLVMType(llvm::Value* return_value);
int toNonameType(llvm::Value* value);
int toNonameType(llvm::Type* type);

extern void InitializeModuleAndPassManager();

class Error {
 private:
  std::string _what;

 public:
  Error(const std::string& what) : _what(what) {}
  std::string what() { return _what; }
};

class ASTNode {
 public:
#ifndef AST_NODE_TYPE
#define AST_NODE_TYPE
  enum ASTNodeKind {
    AST_NODE_TYPE_AST_NODE,

    AST_NODE_TYPE_ERROR_NODE,
    AST_NODE_TYPE_ERROR_NODE_LAST,

    AST_NODE_TYPE_EXP_NODE,  // begin of EXP_NODE
    AST_NODE_TYPE_NUMBER,
    AST_NODE_TYPE_VARIABLE,
    AST_NODE_TYPE_STRING,
    AST_NODE_TYPE_UNARY_EXP,
    AST_NODE_TYPE_BINARY,
    AST_NODE_TYPE_CALL_EXP,
    AST_NODE_TYPE_TOP_LEVEL_EXP_NODE,

    AST_NODE_TYPE_ASSIGNMENT,  // begin of ASSIGNMENT
    AST_NODE_TYPE_DECLARATION_ASSIGNMENT,
    AST_NODE_TYPE_ASSIGNMENT_LAST,  // end of ASSIGNMENT

    AST_NODE_TYPE_EXP_NODE_LAST,  // end of EXP_NODE

    AST_NODE_TYPE_DECLARATION,
    AST_NODE_TYPE_DEF_FUNCTION,
    AST_NODE_TYPE_IMPORT,

    AST_NODE_TYPE_AST_NODE_LAST,
  };
#endif

 private:
  ASTContext* context;
  const ASTNodeKind kind;

 public:
  ASTNode(ASTContext* context) : context(context), kind(AST_NODE_TYPE_AST_NODE) {}
  ASTNode(ASTContext* context, ASTNodeKind kind) : context(context), kind(kind) {}
  virtual ~ASTNode() = default;
  ASTNodeKind getKind() const { return kind; }
  virtual void* eval() { return nullptr; };
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) { return codegen_elements_retlast(this, bb); };
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) {
    return std::vector<Value*>();
  };

  virtual ASTNode* check() { return this; };
  virtual ProcessorStrategy* getProcessorStrategy() { return astNodeProcessorStrategy; };

  ASTContext* getContext() const { return context; };

  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_AST_NODE && S->getKind() <= AST_NODE_TYPE_AST_NODE_LAST;
  }

  static std::string toString(ASTNode::ASTNodeKind kind) {
    std::string s;

#define AST_NODE_KIND_PROCESS_VAL(p) \
  case (p):                          \
    s = #p;                          \
    break;

    switch (kind) {
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_AST_NODE)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_AST_NODE_LAST)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_ERROR_NODE)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_ERROR_NODE_LAST)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_EXP_NODE)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_TOP_LEVEL_EXP_NODE)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_EXP_NODE_LAST)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_NUMBER)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_VARIABLE)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_STRING)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_UNARY_EXP)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_BINARY)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_CALL_EXP)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_DEF_FUNCTION)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_ASSIGNMENT)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_DECLARATION)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_DECLARATION_ASSIGNMENT)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_ASSIGNMENT_LAST)
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_IMPORT)
    }

#undef AST_NODE_KIND_PROCESS_VAL

    return s;
  };

  // virtual int getType() const { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_AST_NODE; };
};

class ErrorNode : public ASTNode {
 private:
  std::string _what;

 public:
  ErrorNode(ASTContext* context, const std::string& what) : ASTNode(context, AST_NODE_TYPE_ERROR_NODE), _what(what) {}
  std::string what() { return _what; }

  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_ERROR_NODE && S->getKind() <= AST_NODE_TYPE_ERROR_NODE_LAST;
  }
};

class LogicErrorNode : public ErrorNode {
 public:
  LogicErrorNode(ASTContext* context, const std::string& what) : ErrorNode(context, what) {}
};

class InvalidStatement : public LogicErrorNode {
 public:
  InvalidStatement(ASTContext* context) : LogicErrorNode(context, "Invalid statement inside current scope") {}
  InvalidStatement(ASTContext* context, const std::string& what) : LogicErrorNode(context, what) {}
};

class ASTContext {
 private:
  std::string name;
  ASTContext* parent;

  std::map<std::string, FunctionDefNode*> mFunctions;
  std::map<std::string, FunctionDefNode*>::iterator itFunctions;

  std::map<std::string, NodeValue*> mVariables;
  std::map<std::string, NodeValue*>::iterator itVariables;

  std::map<std::string, AllocaInst*> mAllocaInst;
  std::map<std::string, AllocaInst*>::iterator itAllocaInst;

 public:
  ASTContext(const std::string& name) : name(name), parent(NULL) {}
  ASTContext(const std::string& name, ASTContext* parent) : name(name), parent(parent) {}
  ASTContext(const ASTContext& copy)
      : name(copy.name),
        parent(copy.parent),
        mFunctions(copy.mFunctions),
        mVariables(copy.mVariables),
        mAllocaInst(copy.mAllocaInst) {}
  ASTContext(const ASTContext& copy, ASTContext* parent)
      : name(copy.name),
        parent(parent),
        mFunctions(copy.mFunctions),
        mVariables(copy.mVariables),
        mAllocaInst(copy.mAllocaInst) {}
  ASTContext(const std::string& name, const ASTContext& copy, ASTContext* parent)
      : name(name),
        parent(parent),
        mFunctions(copy.mFunctions),
        mVariables(copy.mVariables),
        mAllocaInst(copy.mAllocaInst) {}
  virtual ~ASTContext() = default;
  ASTContext& operator=(const ASTContext& copy) {
    name = copy.name;
    parent = copy.parent;
    mFunctions = copy.mFunctions;
    mVariables = copy.mVariables;
    mAllocaInst = copy.mAllocaInst;
    return *this;
  }
  std::string& getName() { return name; }
  ASTContext* getParent() { return parent; }

  // Functions
  FunctionDefNode* getFunction(const std::string& name);
  bool storeFunction(const std::string name, FunctionDefNode* function_nomde);
  bool store(const std::string name, FunctionDefNode* function_nomde);
  bool removeFunction(const std::string name);

  // Variables
  NodeValue* getVariableShallow(const std::string& name);
  NodeValue* getVariable(const std::string& name);
  bool storeVariable(const std::string name, NodeValue* node_value);
  bool store(const std::string name, NodeValue* node_value);
  bool removeVariable(const std::string name);
  NodeValue* updateVariable(const std::string name, NodeValue* node_value);
  NodeValue* update(const std::string name, NodeValue* node_value);

  // AllocaInst
  llvm::AllocaInst* getAllocaInst(const std::string& name);
  bool storeAllocaInst(const std::string name, llvm::AllocaInst* alloca_inst);
  bool store(const std::string name, llvm::AllocaInst* alloca_inst);
  bool removeAllocaInst(const std::string name);
  llvm::AllocaInst* updateAllocaInst(const std::string name, llvm::AllocaInst* alloca_inst);
  llvm::AllocaInst* update(const std::string name, llvm::AllocaInst* alloca_inst);
};

class NodeValue {
  int type;
  void* value;

 public:
  NodeValue(const std::string& value);
  NodeValue(int value);
  NodeValue(double value);
  NodeValue(long value);
  virtual ~NodeValue();

  int getType() { return type; }
  void* getRawValue() { return value; }
  Value* constant_codegen(llvm::BasicBlock* bb = nullptr);
  void* getValue(int as_type);
};

class ExpNode : public ASTNode {
 public:
  ExpNode(ASTContext* context) : ASTNode(context, AST_NODE_TYPE_EXP_NODE) {}
  ExpNode(ASTContext* context, ASTNodeKind kind) : ASTNode(context, kind) {}
  virtual ~ExpNode() = default;

  void* eval() override {
    getValue();
    return nullptr;
  };

  virtual std::unique_ptr<NodeValue> getValue() = 0;

  ProcessorStrategy* getProcessorStrategy() override { return expNodeProcessorStrategy; };

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_EXP_NODE; };

  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_EXP_NODE && S->getKind() <= AST_NODE_TYPE_EXP_NODE_LAST;
  }
};

class NumberExpNode : public ExpNode {
 private:
  void* value;
  int type;

 public:
  NumberExpNode(ASTContext* context, double val) : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_DOUBLE) {
    value = new double;
    memcpy(value, &val, sizeof(double));
  };
  NumberExpNode(ASTContext* context, float val) : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_FLOAT) {
    value = new float;
    memcpy(value, &val, sizeof(float));
  };
  NumberExpNode(ASTContext* context, long val) : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_LONG) {
    value = new long;
    memcpy(value, &val, sizeof(long));
  };
  NumberExpNode(ASTContext* context, int val) : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_INT) {
    value = new int;
    memcpy(value, &val, sizeof(int));
  };
  NumberExpNode(ASTContext* context, short val) : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_SHORT) {
    value = new short;
    memcpy(value, &val, sizeof(short));
  };
  NumberExpNode(ASTContext* context, char val) : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_CHAR) {
    value = new char;
    memcpy(value, &val, sizeof(char));
  };

  // virtual void* eval() override;
  virtual std::unique_ptr<NodeValue> getValue() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_NUMBER; };

  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_NUMBER; }
};

class StringExpNode : public ExpNode {
 private:
  std::string value;

 public:
  StringExpNode(ASTContext* context, const std::string& value) : ExpNode(context, AST_NODE_TYPE_STRING), value(value){};
  StringExpNode(ASTContext* context, const char* value)
      : ExpNode(context, AST_NODE_TYPE_STRING), value(std::string(value)){};

  // virtual void* eval() override;
  virtual std::unique_ptr<NodeValue> getValue() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_STRING; };

  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_STRING; }
};

class VarExpNode : public ExpNode {
 private:
  std::string name;

 public:
  VarExpNode(ASTContext* context, const std::string& name) : ExpNode(context, AST_NODE_TYPE_VARIABLE), name(name) {}
  const std::string& getName() const { return name; }

  // virtual void* eval() override;
  virtual std::unique_ptr<NodeValue> getValue() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_VARIABLE; };

  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_VARIABLE; }
};

class UnaryExpNode : public ExpNode {
 private:
  char op;
  std::unique_ptr<ExpNode> rhs;

 public:
  UnaryExpNode(ASTContext* context, char op, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, AST_NODE_TYPE_UNARY_EXP), op(op), rhs(std::move(rhs)) {}
  UnaryExpNode(ASTContext* context, char op, ExpNode* rhs)
      : ExpNode(context, AST_NODE_TYPE_UNARY_EXP), op(op), rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  // virtual void* eval() override;
  virtual std::unique_ptr<NodeValue> getValue() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_UNARY_EXP; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_UNARY_EXP; }
};

class BinaryExpNode : public ExpNode {
 private:
  char op;
  std::unique_ptr<ExpNode> lhs;
  std::unique_ptr<ExpNode> rhs;

 public:
  BinaryExpNode(ASTContext* context, char op, std::unique_ptr<ExpNode> lhs, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, AST_NODE_TYPE_BINARY), op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  BinaryExpNode(ASTContext* context, char op, ExpNode* lhs, ExpNode* rhs)
      : ExpNode(context, AST_NODE_TYPE_BINARY),
        op(op),
        lhs(std::unique_ptr<ExpNode>(std::move(lhs))),
        rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  // virtual void* eval() override;
  virtual std::unique_ptr<NodeValue> getValue() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_BINARY; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_BINARY; }

 private:
  Value* CreatePow(Value* L, Value* R, const char* name);
};

// ImportNode - Node class for file import
class ImportNode : public ASTNode {
 private:
  std::string filename;

 public:
  ImportNode(ASTContext* context, const std::string& filename)
      : ASTNode(context, AST_NODE_TYPE_IMPORT), filename(filename) {}

  // virtual void* eval() override;
  ProcessorStrategy* getProcessorStrategy() override { return importNodeProcessorStrategy; };
  const std::string& getFilename() const { return filename; }

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_IMPORT; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_IMPORT; }
};
ASTNode* createAnnonymousFunctionDefNode(ASTContext* context, ExpNode* exp_node);

// FunctionDefNode - Node class for function definition.
class FunctionDefNode : public ASTNode {
 private:
  std::string name;
  std::vector<std::unique_ptr<arg_t>> args;
  std::vector<std::unique_ptr<ASTNode>> bodyNodes;
  ExpNode* return_node;
  llvm::Type* returnLLVMType;

 public:
  FunctionDefNode(ASTContext* context, const std::string& name, std::vector<std::unique_ptr<arg_t>>& args,
                  std::vector<std::unique_ptr<ASTNode>>& body_nodes, ExpNode* return_node);
  FunctionDefNode(ASTContext* context, const std::string& name, arglist_t* head_arg_list, stmtlist_t* head_stmt_list,
                  ExpNode* return_node);
  virtual ~FunctionDefNode();

  // virtual void* eval() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;

  ASTNode* check() override;
  const std::string& getName() const { return name; }
  const std::vector<std::unique_ptr<arg_t>>& getArgs() const { return args; }
  const std::vector<std::unique_ptr<ASTNode>>& getBodyNodes() const { return bodyNodes; }
  ExpNode* getReturnNode() { return return_node; }
  llvm::Type* getReturnLLVMType();

  Function* getFunctionDefinition(Value* return_value = nullptr);
  ProcessorStrategy* getProcessorStrategy() override { return functionDefNodeProcessorStrategy; };

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_DEF_FUNCTION; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_DEF_FUNCTION; }

 private:
  llvm::ReturnInst* getLLVMReturnInst(Value* return_value);
  llvm::Type* getLLVMReturnInstType(llvm::Value* return_value);
};

class TopLevelExpNode : public ExpNode {
 private:
  ExpNode* exp_node;
  CallExpNode* call_exp_node;
  FunctionDefNode* anonymous_def_node;

 public:
  TopLevelExpNode(ASTContext* context, ExpNode* exp_node, CallExpNode* call_exp_node,
                  FunctionDefNode* anonymous_def_node);
  virtual ~TopLevelExpNode();

  void* eval() override { return exp_node->eval(); };
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  virtual std::unique_ptr<NodeValue> getValue() override { return exp_node->getValue(); };
  void* release();
  llvm::Type* getReturnLLVMType() { return anonymous_def_node->getReturnLLVMType(); }
  ProcessorStrategy* getProcessorStrategy() override { return topLevelExpNodeProcessorStrategy; };

  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_TOP_LEVEL_EXP_NODE; }
};

/// CallExpNode - Expression class for function calls.
class CallExpNode : public ExpNode {
 private:
  std::string callee;
  FunctionDefNode* called_function;
  std::vector<std::unique_ptr<ExpNode>> args;

 public:
  CallExpNode(ASTContext* context, const std::string& callee, explist_t* head_exp_list = nullptr);
  CallExpNode(ASTContext* context, FunctionDefNode* called_function, explist_t* head_exp_list = nullptr);
  virtual ~CallExpNode() override;
  // virtual void* eval() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  virtual std::unique_ptr<NodeValue> getValue() override;
  virtual ProcessorStrategy* getProcessorStrategy() override { return callNodeProcessorStrategy; };

  const std::string& getCallee() const { return callee; }
  FunctionDefNode* getCalledFunction();
  const std::vector<std::unique_ptr<ExpNode>>& getArgs() const { return args; }

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_CALL_EXP; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_CALL_EXP; }

 private:
  void initializeArgs(explist_t* head_exp_list);
};

class AssignmentNode : public ExpNode {
 protected:
  std::string name;
  std::unique_ptr<ExpNode> rhs;

 public:
  AssignmentNode(ASTContext* context, const std::string& name, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, AST_NODE_TYPE_ASSIGNMENT), name(name), rhs(std::move(rhs)) {}
  AssignmentNode(ASTContext* context, const std::string& name, ExpNode* rhs)
      : ExpNode(context, AST_NODE_TYPE_ASSIGNMENT), name(name), rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  AssignmentNode(ASTContext* context, ASTNodeKind kind, const std::string& name, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, kind), name(name), rhs(std::move(rhs)) {}
  AssignmentNode(ASTContext* context, ASTNodeKind kind, const std::string& name, ExpNode* rhs)
      : ExpNode(context, kind), name(name), rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  virtual void* eval() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  virtual std::unique_ptr<NodeValue> getValue() override;
  virtual ProcessorStrategy* getProcessorStrategy() override { return assignmentNodeProcessorStrategy; };
  const std::string& getName() const { return name; }
  const std::unique_ptr<ExpNode>& getRHS() const { return rhs; }

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_ASSIGNMENT; };
  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_ASSIGNMENT && S->getKind() <= AST_NODE_TYPE_ASSIGNMENT_LAST;
  }
};

class DeclarationAssignmentNode : public AssignmentNode {
 public:
  DeclarationAssignmentNode(ASTContext* context, const std::string& name, std::unique_ptr<ExpNode> rhs)
      : AssignmentNode(context, AST_NODE_TYPE_DECLARATION_ASSIGNMENT, name, std::move(rhs)) {}
  DeclarationAssignmentNode(ASTContext* context, const std::string& name, ExpNode* rhs)
      : AssignmentNode(context, AST_NODE_TYPE_DECLARATION_ASSIGNMENT, name, std::move(rhs)) {}

  virtual void* eval() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;
  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_DECLARATION_ASSIGNMENT; };

  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_DECLARATION_ASSIGNMENT; }
};

class DeclarationNode : public ASTNode {
 private:
  std::string name;

 public:
  DeclarationNode(ASTContext* context, const std::string& name)
      : ASTNode(context, AST_NODE_TYPE_DECLARATION), name(name) {}

  virtual void* eval() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error** error, llvm::BasicBlock* bb = nullptr) override;

  const std::string& getName() const { return name; }

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_DECLARATION; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_DECLARATION; }
};

class ProcessorStrategy {
 public:
  virtual ~ProcessorStrategy() = default;
  virtual void* process(ASTNode* node) = 0;
};
class ASTNodeProcessorStrategy : public ProcessorStrategy {
 public:
  void* process(ASTNode* node) override;
};
class ExpNodeProcessorStrategy : public ProcessorStrategy {
 public:
  void* process(ASTNode* node) override;
};
class TopLevelExpNodeProcessorStrategy : public ProcessorStrategy {
 public:
  void* process(ASTNode* node) override;
};
class FunctionDefNodeProcessorStrategy : public ProcessorStrategy {
 public:
  void* process(ASTNode* node) override;
};
class AssignmentNodeProcessorStrategy : public ProcessorStrategy {
 public:
  void* process(ASTNode* node) override;
};
class CallExpNodeProcessorStrategy : public ProcessorStrategy {
 public:
  void* process(ASTNode* node) override;
};
class ImportNodeProcessorStrategy : public ProcessorStrategy {
 public:
  void* process(ASTNode* node) override;
};

// class ReturnNode : public ExpNode {
//  private:
//   ExpNode* rhs;

//  public:
//   ReturnNode(ASTContext* context, ExpNode* rhs) : ExpNode(context),
//   rhs(std::move(rhs)) {}

//   virtual std::unique_ptr<NodeValue> getValue() override { return 0; }

// int getType() const override { return getClassType(); };
//   static int getClassType() { return AST_NODE_TYPE_ASSIGNMENT; };
// };
// class DeclarationNode : public ASTNode {
//  private:
//   std::string name;

//  public:
//   DeclarationNode(ASTContext* context, const std::string& name) :
//   ASTNode(context), name(name) {}

// int getType() const override { return getClassType(); };
//   static int getClassType() { return AST_NODE_TYPE_DECLARATION; };
// };
}
#endif