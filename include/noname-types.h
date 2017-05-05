#ifndef _NONAME_TYPES_H
#define _NONAME_TYPES_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
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
  TYPE_CHAR = 32,
  TYPE_SHORT = 33,
  TYPE_INT = 34,
  TYPE_FLOAT = 35,
  TYPE_LONG = 36,
  TYPE_DOUBLE = 37,
  TYPE_STRING = 38,
};
#endif

#define TYPE_CHAR 32
#define TYPE_SHORT 33
#define TYPE_INT 34
#define TYPE_FLOAT 35
#define TYPE_LONG 36
#define TYPE_DOUBLE 37
#define TYPE_STRING 38

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

extern int debug;
extern ASTContext* context;
extern std::vector<std::string> imported_files;
extern std::stack<ASTContext*> context_stack;
extern std::map<int, std::string> map;
extern bool read_from_file_import;

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
ASTNode* logError(const char* str);
FunctionDefNode* logErrorF(const char* str);
AssignmentNode* logErrorV(const char* str);
ASTNode* logError(ErrorNode* error_node);
NodeValue* logErrorNV(ErrorNode* error_node);
llvm::Value* logErrorLLVM(const char* str);
llvm::Function* logErrorLLVMF(const char* str);
llvm::Value* logErrorLLVM(ErrorNode* error_node);
llvm::Function* logErrorLLVMF(ErrorNode* error_node);

bool is_file_already_imported(const std::string& file_path);
bool is_file_already_imported(const char* file_path);
char* get_current_dir();
char* concat_strs(const char* format, const char* s1, const char* s2, int size);
char* get_file_path(const char* filename);
int noname_read(char* buf, int* result, int max_size);

void print_node_value(NodeValue* nodeValue);
void print_node_value(FILE* file, NodeValue* nodeValue);
stmtlist_t* new_stmt_list(ASTContext* context);
stmtlist_t* new_stmt_list(ASTContext* context, ASTNode* node);
stmtlist_t* new_stmt_list(ASTContext* context, stmtlist_t* head_exp_list,
                          ASTNode* node);

explist_t* new_exp_list(ASTContext* context);
explist_t* new_exp_list(ASTContext* context, ExpNode* node);
explist_t* new_exp_list(ASTContext* context, explist_t* head_exp_list,
                        ExpNode* node);
arg_t* new_arg(ASTContext* context, char* arg, ExpNode* defaultValue);
arg_t* new_arg(ASTContext* context, char* arg, double defaultValue);
arg_t* new_arg(ASTContext* context, char* arg, long defaultValue);
arg_t* new_arg(ASTContext* context, char* arg, char* defaultValue);
arglist_t* new_arg_list(ASTContext* context);
arglist_t* new_arg_list(ASTContext* context, arg_t* arg);
arglist_t* new_arg_list(ASTContext* context, arglist_t* head_arg_list,
                        arg_t* arg);

ImportNode* new_import(ASTContext* context, std::string filename);
ASTNode* new_top_level_exp_node(ExpNode* node);
VarExpNode* new_var_node(ASTContext* context, const std::string name);
AssignmentNode* new_assignment_node(ASTContext* context, const std::string name,
                                    ExpNode* node);
AssignmentNode* new_declaration_node(ASTContext* context,
                                     const std::string name);
CallExpNode* new_call_node(ASTContext* context, const std::string name,
                           explist_t* exp_list);
ASTNode* new_function_def(ASTContext* context, const std::string name,
                          arglist_t* arg_list, stmtlist_t* stmt_list,
                          ExpNode* returnNode);

void release(explist_t* explist);
void release(explist_node_t* explist_node);
void release(stmtlist_t* stmtlist);
void release(stmtlist_node_t* stmtlist_node);
void release(arg_t* arg);
void release(arglist_t* arglist);
void release(arglist_node_t* arglist_node);

llvm::Type* toLLVLType(llvm::LLVMContext& LLVMContext, int type);

extern void InitializeModuleAndPassManager();

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
  ASTNode(ASTContext* context)
      : context(context), kind(AST_NODE_TYPE_AST_NODE) {}
  ASTNode(ASTContext* context, ASTNodeKind kind)
      : context(context), kind(kind) {}
  virtual ~ASTNode() = default;
  ASTNodeKind getKind() const { return kind; }
  virtual Value* codegen() { return nullptr; };

  virtual ASTNode* check() { return this; };
  virtual void* eval() { return nullptr; };
  virtual ProcessorStrategy* getProcessorStrategy() {
    return astNodeProcessorStrategy;
  };

  ASTContext* getContext() const { return context; };

  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_AST_NODE &&
           S->getKind() <= AST_NODE_TYPE_AST_NODE_LAST;
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
  ErrorNode(ASTContext* context, const std::string& what)
      : ASTNode(context, AST_NODE_TYPE_ERROR_NODE), _what(what) {}
  std::string what() { return _what; }

  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_ERROR_NODE &&
           S->getKind() <= AST_NODE_TYPE_ERROR_NODE_LAST;
  }
};

class LogicErrorNode : public ErrorNode {
 public:
  LogicErrorNode(ASTContext* context, const std::string& what)
      : ErrorNode(context, what) {}
};

class InvalidStatement : public LogicErrorNode {
 public:
  InvalidStatement(ASTContext* context)
      : LogicErrorNode(context, "Invalid statement inside current scope") {}
  InvalidStatement(ASTContext* context, const std::string& what)
      : LogicErrorNode(context, what) {}
};

class ASTContext {
 private:
  std::string name;
  ASTContext* parent;
  std::map<std::string, FunctionDefNode*> mFunctions;
  std::map<std::string, FunctionDefNode*>::iterator itFunctions;
  std::map<std::string, NodeValue*> mVariables;
  std::map<std::string, NodeValue*>::iterator itVariables;

 public:
  ASTContext(const std::string& name) : name(name), parent(NULL) {}
  ASTContext(const std::string& name, ASTContext* parent)
      : name(name), parent(parent) {}
  ASTContext(const ASTContext& copy)
      : name(copy.name),
        parent(copy.parent),
        mFunctions(copy.mFunctions),
        mVariables(copy.mVariables) {}
  ASTContext(const ASTContext& copy, ASTContext* parent)
      : name(copy.name),
        parent(parent),
        mFunctions(copy.mFunctions),
        mVariables(copy.mVariables) {}
  ASTContext(const std::string& name, const ASTContext& copy,
             ASTContext* parent)
      : name(name),
        parent(parent),
        mFunctions(copy.mFunctions),
        mVariables(copy.mVariables) {}
  virtual ~ASTContext() = default;
  ASTContext& operator=(const ASTContext& copy) {
    name = copy.name;
    parent = copy.parent;
    mFunctions = copy.mFunctions;
    mVariables = copy.mVariables;
    return *this;
  }
  std::string& getName() { return name; }
  ASTContext* getParent() { return parent; }
  // Functions
  FunctionDefNode* getFunction(const std::string& name);
  bool storeFunction(const std::string name, FunctionDefNode* function_nomde);
  bool store(const std::string name, FunctionDefNode* function_nomde);
  // Variables
  NodeValue* getVariableShallow(const std::string& name);
  NodeValue* getVariable(const std::string& name);
  bool storeVariable(const std::string name, NodeValue* node_value);
  bool store(const std::string name, NodeValue* node_value);
  bool removeFunction(const std::string name);
  bool removeVariable(const std::string name);
  NodeValue* updateVariable(const std::string name, NodeValue* node_value);
  NodeValue* update(const std::string name, NodeValue* node_value);
};

class NodeValue {
  int type;
  void* value;

 public:
  NodeValue(const std::string& value) : type(TYPE_STRING), value(0) {
    this->value = new std::string(value);
  }
  NodeValue(int value) : type(TYPE_INT), value(0) {
    this->value = new int;
    memcpy(this->value, &value, sizeof(int));
  }
  NodeValue(double value) : type(TYPE_DOUBLE), value(0) {
    this->value = new double;
    memcpy(this->value, &value, sizeof(double));
  }
  NodeValue(long value) : type(TYPE_LONG), value(0) {
    this->value = new long;
    memcpy(this->value, &value, sizeof(long));
  }

  int getType() { return type; }
  void* getRawValue() { return value; }
  Value* codegen();
  void* getValue(int as_type) {
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
};

class ExpNode : public ASTNode {
 public:
  ExpNode(ASTContext* context) : ASTNode(context, AST_NODE_TYPE_EXP_NODE) {}
  ExpNode(ASTContext* context, ASTNodeKind kind) : ASTNode(context, kind) {}
  virtual ~ExpNode() = default;

  void* eval() override {
    NodeValue* node_value = getValue();
    return node_value;
  };
  virtual NodeValue* getValue() = 0;

  ProcessorStrategy* getProcessorStrategy() override {
    return expNodeProcessorStrategy;
  };

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_EXP_NODE; };

  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_EXP_NODE &&
           S->getKind() <= AST_NODE_TYPE_EXP_NODE_LAST;
  }
};

class NumberExpNode : public ExpNode {
 private:
  void* value;
  int type;

 public:
  NumberExpNode(ASTContext* context, double val)
      : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_DOUBLE) {
    value = new double;
    memcpy(value, &val, sizeof(double));
  };
  NumberExpNode(ASTContext* context, float val)
      : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_FLOAT) {
    value = new float;
    memcpy(value, &val, sizeof(float));
  };
  NumberExpNode(ASTContext* context, long val)
      : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_LONG) {
    value = new long;
    memcpy(value, &val, sizeof(long));
  };
  NumberExpNode(ASTContext* context, int val)
      : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_INT) {
    value = new int;
    memcpy(value, &val, sizeof(int));
  };
  NumberExpNode(ASTContext* context, short val)
      : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_SHORT) {
    value = new short;
    memcpy(value, &val, sizeof(short));
  };
  NumberExpNode(ASTContext* context, char val)
      : ExpNode(context, AST_NODE_TYPE_NUMBER), type(TYPE_CHAR) {
    value = new char;
    memcpy(value, &val, sizeof(char));
  };

  // void* eval() override;
  NodeValue* getValue() override;
  virtual Value* codegen() override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_NUMBER; };

  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_NUMBER;
  }
};

class StringExpNode : public ExpNode {
 private:
  std::string value;

 public:
  StringExpNode(ASTContext* context, const std::string& value)
      : ExpNode(context, AST_NODE_TYPE_STRING), value(value){};
  StringExpNode(ASTContext* context, const char* value)
      : ExpNode(context, AST_NODE_TYPE_STRING), value(std::string(value)){};

  // void* eval() override;
  NodeValue* getValue() override;
  virtual Value* codegen() override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_STRING; };

  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_STRING;
  }
};

class VarExpNode : public ExpNode {
 private:
  std::string name;

 public:
  VarExpNode(ASTContext* context, const std::string& name)
      : ExpNode(context, AST_NODE_TYPE_VARIABLE), name(name) {}
  const std::string& getName() const { return name; }

  // void* eval() override;
  NodeValue* getValue() override;
  virtual Value* codegen() override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_VARIABLE; };

  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_VARIABLE;
  }
};

class UnaryExpNode : public ExpNode {
 private:
  char op;
  std::unique_ptr<ExpNode> rhs;

 public:
  UnaryExpNode(ASTContext* context, char op, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, AST_NODE_TYPE_UNARY_EXP),
        op(op),
        rhs(std::move(rhs)) {}
  UnaryExpNode(ASTContext* context, char op, ExpNode* rhs)
      : ExpNode(context, AST_NODE_TYPE_UNARY_EXP),
        op(op),
        rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  // void* eval() override;
  NodeValue* getValue() override;
  virtual Value* codegen() override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_UNARY_EXP; };
  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_UNARY_EXP;
  }
};

class BinaryExpNode : public ExpNode {
 private:
  char op;
  std::unique_ptr<ExpNode> lhs;
  std::unique_ptr<ExpNode> rhs;

 public:
  BinaryExpNode(ASTContext* context, char op, std::unique_ptr<ExpNode> lhs,
                std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, AST_NODE_TYPE_BINARY),
        op(op),
        lhs(std::move(lhs)),
        rhs(std::move(rhs)) {}
  BinaryExpNode(ASTContext* context, char op, ExpNode* lhs, ExpNode* rhs)
      : ExpNode(context, AST_NODE_TYPE_BINARY),
        op(op),
        lhs(std::unique_ptr<ExpNode>(std::move(lhs))),
        rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  // void* eval() override;
  NodeValue* getValue() override;
  virtual Value* codegen() override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_BINARY; };
  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_BINARY;
  }

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

  // void* eval() override;
  ProcessorStrategy* getProcessorStrategy() override {
    return importNodeProcessorStrategy;
  };
  const std::string& getFilename() const { return filename; }

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_IMPORT; };
  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_IMPORT;
  }
};
ASTNode* createAnnonymousFunctionDefNode(ASTContext* context,
                                         ExpNode* exp_node);

// FunctionDefNode - Node class for function definition.
class FunctionDefNode : public ASTNode {
 private:
  std::string name;
  std::vector<std::unique_ptr<arg_t>> args;
  std::vector<std::unique_ptr<ASTNode>> bodyNodes;
  ExpNode* returnNode;
  llvm::Type* returnLLVMType;

 public:
  FunctionDefNode(ASTContext* context, const std::string& name,
                  std::vector<std::unique_ptr<arg_t>>& args,
                  std::vector<std::unique_ptr<ASTNode>>& body_nodes,
                  ExpNode* returnNode);
  FunctionDefNode(ASTContext* context, const std::string& name,
                  arglist_t* head_arg_list, stmtlist_t* head_stmt_list,
                  ExpNode* returnNode);

  // void* eval() override;
  ASTNode* check() override;
  const std::string& getName() const { return name; }
  std::vector<std::unique_ptr<arg_t>>& getArgs() { return args; }
  std::vector<std::unique_ptr<ASTNode>>& getBodyNodes() { return bodyNodes; }
  ExpNode* getReturnNode() { return returnNode; }
  llvm::Type* getReturnLLVMType(LLVMContext& TheContext);

  virtual Value* codegen() override;
  Function* getFunctionDefinition(Value* return_value = nullptr);
  ProcessorStrategy* getProcessorStrategy() override {
    return functionDefNodeProcessorStrategy;
  };

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_DEF_FUNCTION; };
  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_DEF_FUNCTION;
  }

 private:
  llvm::ReturnInst* getLLVMReturnInst(Value* return_value);
  llvm::Type* getLLVMReturnInstType(llvm::Value* return_value);
};

class TopLevelExpNode : public ExpNode {
 private:
  ExpNode* exp_node;
  FunctionDefNode* anonymous_def_node;

 public:
  TopLevelExpNode(ASTContext* context, ExpNode* exp_node,
                  FunctionDefNode* anonymous_def_node)
      : ExpNode(context, AST_NODE_TYPE_TOP_LEVEL_EXP_NODE),
        exp_node(exp_node),
        anonymous_def_node(anonymous_def_node) {}
  virtual ~TopLevelExpNode() = default;

  void* eval() override { return exp_node->eval(); };
  virtual NodeValue* getValue() override { return exp_node->getValue(); };
  virtual Value* codegen() override;
  void* release();
  llvm::Type* getReturnLLVMType(LLVMContext& TheContext) {
    // if (!anonymous_def_node) {
    //   return nullptr;
    // }
    return anonymous_def_node->getReturnLLVMType(TheContext);
  }
  ProcessorStrategy* getProcessorStrategy() override {
    return topLevelExpNodeProcessorStrategy;
  };

  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_TOP_LEVEL_EXP_NODE;
  }
};

/// CallExpNode - Expression class for function calls.
class CallExpNode : public ExpNode {
 private:
  std::string callee;
  std::vector<std::unique_ptr<ExpNode>> args;

 public:
  CallExpNode(ASTContext* context, const std::string& callee,
              std::vector<std::unique_ptr<ExpNode>>& args)
      : ExpNode(context, AST_NODE_TYPE_CALL_EXP),
        callee(callee),
        args(std::move(args)) {}
  CallExpNode(ASTContext* context, const std::string& callee,
              explist_t* head_exp_list)
      : ExpNode(context, AST_NODE_TYPE_CALL_EXP),
        callee(callee),
        args(std::vector<std::unique_ptr<ExpNode>>()) {
    if (head_exp_list) {
      explist_node_t* explist_node_t = head_exp_list->first;
      do {
        if (explist_node_t && explist_node_t->node) {
          args.push_back(
              std::unique_ptr<ExpNode>(std::move(explist_node_t->node)));
          explist_node_t = explist_node_t->next;
        }
      } while (explist_node_t);

      // TODO: free all the expressions not just the head_exp_list one
      // free(head_exp_list);
    }
  }

  // void* eval() override;
  virtual Value* codegen() override;
  NodeValue* getValue() override;
  ProcessorStrategy* getProcessorStrategy() override {
    return callNodeProcessorStrategy;
  };

  const std::string& getCallee() const { return callee; }
  std::vector<std::unique_ptr<ExpNode>>& getArgs() { return args; }

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_CALL_EXP; };
  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_CALL_EXP;
  }
};

class AssignmentNode : public ExpNode {
 protected:
  std::string name;
  std::unique_ptr<ExpNode> rhs;

 public:
  AssignmentNode(ASTContext* context, const std::string& name,
                 std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, AST_NODE_TYPE_ASSIGNMENT),
        name(name),
        rhs(std::move(rhs)) {}
  AssignmentNode(ASTContext* context, const std::string& name, ExpNode* rhs)
      : ExpNode(context, AST_NODE_TYPE_ASSIGNMENT),
        name(name),
        rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  AssignmentNode(ASTContext* context, ASTNodeKind kind, const std::string& name,
                 std::unique_ptr<ExpNode> rhs)
      : ExpNode(context, kind), name(name), rhs(std::move(rhs)) {}
  AssignmentNode(ASTContext* context, ASTNodeKind kind, const std::string& name,
                 ExpNode* rhs)
      : ExpNode(context, kind),
        name(name),
        rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  void* eval() override;
  NodeValue* getValue() override;
  virtual Value* codegen() override;
  ProcessorStrategy* getProcessorStrategy() override {
    return assignmentNodeProcessorStrategy;
  };
  const std::string& getName() const { return name; }

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_ASSIGNMENT; };
  static bool classof(const ASTNode* S) {
    return S->getKind() >= AST_NODE_TYPE_ASSIGNMENT &&
           S->getKind() <= AST_NODE_TYPE_ASSIGNMENT_LAST;
  }
};

class DeclarationAssignmentNode : public AssignmentNode {
 public:
  DeclarationAssignmentNode(ASTContext* context, const std::string& name,
                            std::unique_ptr<ExpNode> rhs)
      : AssignmentNode(context, AST_NODE_TYPE_DECLARATION_ASSIGNMENT, name,
                       std::move(rhs)) {}
  DeclarationAssignmentNode(ASTContext* context, const std::string& name,
                            ExpNode* rhs)
      : AssignmentNode(context, AST_NODE_TYPE_DECLARATION_ASSIGNMENT, name,
                       std::move(rhs)) {}

  void* eval() override;
  virtual Value* codegen() override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_DECLARATION_ASSIGNMENT; };

  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_DECLARATION_ASSIGNMENT;
  }
};

class DeclarationNode : public ASTNode {
 private:
  std::string name;

 public:
  DeclarationNode(ASTContext* context, const std::string& name)
      : ASTNode(context, AST_NODE_TYPE_DECLARATION), name(name) {}

  void* eval() override;
  virtual Value* codegen() override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_DECLARATION; };
  static bool classof(const ASTNode* S) {
    return S->getKind() == AST_NODE_TYPE_DECLARATION;
  }
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

//   NodeValue* getValue() override { return 0; }

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