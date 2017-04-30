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

extern int yydebug;
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

#ifndef AST_NODE_TYPE
#define AST_NODE_TYPE
enum ast_node_type {
  AST_NODE_TYPE_AST_NODE = 32,
  AST_NODE_TYPE_EXP_NODE = 33,
  AST_NODE_TYPE_NUMBER = 34,
  AST_NODE_TYPE_VARIABLE = 35,
  AST_NODE_TYPE_STRING = 36,
  AST_NODE_TYPE_UNARY_EXP = 37,
  AST_NODE_TYPE_BINARY = 38,
  AST_NODE_TYPE_CALL_EXP = 39,
  AST_NODE_TYPE_DEF_FUNCTION = 40,
  AST_NODE_TYPE_ASSIGNMENT = 41,
  AST_NODE_TYPE_DECLARATION = 42,
  AST_NODE_TYPE_DECLARATION_ASSIGNMENT = 43,
  AST_NODE_TYPE_IMPORT = 44,
};
#endif

#define AST_NODE_TYPE_AST_NODE 32
#define AST_NODE_TYPE_EXP_NODE 33
#define AST_NODE_TYPE_NUMBER 34
#define AST_NODE_TYPE_VARIABLE 35
#define AST_NODE_TYPE_STRING 36
#define AST_NODE_TYPE_UNARY_EXP 37
#define AST_NODE_TYPE_BINARY 38
#define AST_NODE_TYPE_CALL_EXP 39
#define AST_NODE_TYPE_DEF_FUNCTION 40
#define AST_NODE_TYPE_ASSIGNMENT 41
#define AST_NODE_TYPE_DECLARATION 42
#define AST_NODE_TYPE_DECLARATION_ASSIGNMENT 43
#define AST_NODE_TYPE_IMPORT 44

class ASTNode;
class ASTContext;
class ErrorNode;
class LogicErrorNode;
class ExpNode;
class NodeValue;
class ImportNode;
class BinaryExpNode;
class VarNode;
class CallExpNode;
class FunctionDefNode;
class DeclarationNode;
class AssignmentNode;
class DeclarationAssignmentNode;

class ProcessorStrategy;
class ASTNodeProcessorStrategy;
class ExpNodeProcessorStrategy;
class AssignmentNodeProcessorStrategy;
class CallExpNodeProcessorStrategy;
class ImportNodeProcessorStrategy;

extern ProcessorStrategy* astNodeProcessorStrategy;
extern ProcessorStrategy* expNodeProcessorStrategy;
extern ProcessorStrategy* assignmentNodeProcessorStrategy;
extern ProcessorStrategy* callNodeProcessorStrategy;
extern ProcessorStrategy* importNodeProcessorStrategy;

extern ASTContext* context;
extern std::vector<std::string> imported_files;
extern std::stack<ASTContext*> context_stack;
extern std::map<int, std::string> map;
extern bool read_from_file_import;

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern std::unique_ptr<Module> TheModule;

/* list of statements */
struct stmtlist_node_t {
  ASTNode* node;
  struct stmtlist_node_t* next;
  stmtlist_node_t& operator=(const stmtlist_node_t&) { return *this; }
};
struct stmtlist_t {
  struct stmtlist_node_t* first;
  struct stmtlist_node_t* last;
  stmtlist_t& operator=(const stmtlist_t&) { return *this; }
};
/* list of expressions */
struct explist_node_t {
  ExpNode* node;
  struct explist_node_t* next;
  explist_node_t& operator=(const explist_node_t&) { return *this; }
};
struct explist_t {
  struct explist_node_t* first;
  struct explist_node_t* last;
  explist_t& operator=(const explist_t&) { return *this; }
};
/* list of args, for an argument list */
struct arg_t {
  char* name;
  ASTNode* default_value;
  arg_t& operator=(const arg_t&) { return *this; }
};
struct arglist_node_t {
  struct arg_t* arg;
  struct arglist_node_t* next;
  arglist_node_t& operator=(const arglist_node_t&) { return *this; }
};
struct arglist_t {
  struct arglist_node_t* first;
  struct arglist_node_t* last;
  arglist_t& operator=(const arglist_t&) { return *this; }
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
stmtlist_t* new_stmt_list(ASTContext* context, stmtlist_t* head_exp_list, ASTNode* node);
explist_t* new_exp_list(ASTContext* context);
explist_t* new_exp_list(ASTContext* context, ExpNode* node);
explist_t* new_exp_list(ASTContext* context, explist_t* head_exp_list, ExpNode* node);
arg_t* new_arg(ASTContext* context, char* arg_t, ASTNode* defaultValue);
arg_t* new_arg(ASTContext* context, char* arg_t, double defaultValue);
arg_t* new_arg(ASTContext* context, char* arg_t, long defaultValue);
arg_t* new_arg(ASTContext* context, char* arg_t, char* defaultValue);
arglist_t* new_arg_list(ASTContext* context);
arglist_t* new_arg_list(ASTContext* context, arg_t* arg_t);
arglist_t* new_arg_list(ASTContext* context, arglist_t* head_arg_list, arg_t* arg_t);

ImportNode* new_import(ASTContext* context, std::string filename);
VarNode* new_var_node(ASTContext* context, const std::string name);
AssignmentNode* new_assignment_node(ASTContext* context, const std::string name, ExpNode* node);
AssignmentNode* new_declaration_node(ASTContext* context, const std::string name);
CallExpNode* new_call_node(ASTContext* context, const std::string name, explist_t* exp_list);
ASTNode* new_function_def(ASTContext* context, const std::string name, arglist_t* arg_list, stmtlist_t* stmt_list,
                          ExpNode* returnNode);

class ASTNode {
 private:
  ASTContext* context;

 public:
  ASTNode(ASTContext* context) : context(context) {}
  virtual ~ASTNode() = default;

  virtual Value* codegen() { return nullptr; };

  virtual ASTNode* check() { return this; };
  virtual void* eval() { return nullptr; };
  virtual ProcessorStrategy* getProcessorStrategy() { return astNodeProcessorStrategy; };

  ASTContext* getContext() const { return context; };

  virtual int getType() const { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_AST_NODE; };
};

class ErrorNode : public ASTNode {
 private:
  std::string _what;

 public:
  ErrorNode(ASTContext* context, const std::string& what) : ASTNode(context), _what(what) {}
  std::string what() { return _what; }
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

 public:
  ASTContext(const std::string& name) : name(name), parent(NULL) {}
  ASTContext(const std::string& name, ASTContext* parent) : name(name), parent(parent) {}
  ASTContext(const ASTContext& copy)
      : name(copy.name), parent(copy.parent), mFunctions(copy.mFunctions), mVariables(copy.mVariables) {}
  ASTContext(const ASTContext& copy, ASTContext* parent)
      : name(copy.name), parent(parent), mFunctions(copy.mFunctions), mVariables(copy.mVariables) {}
  ASTContext(const std::string& name, const ASTContext& copy, ASTContext* parent)
      : name(name), parent(parent), mFunctions(copy.mFunctions), mVariables(copy.mVariables) {}
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
  FunctionDefNode* getFunction(const std::string& name) {
    itFunctions = mFunctions.find(name);
    if (itFunctions != mFunctions.end()) {
      return mFunctions[name];
    }

    ASTContext* parent = this->getParent();

    if (parent) {
      return parent->getFunction(name);
    }

    return nullptr;
  };

  void store(const std::string name, FunctionDefNode* functionNode) { mFunctions[name] = functionNode; }

  // Variables
  NodeValue* getVariableShallow(const std::string& name) {
    itVariables = mVariables.find(name);
    if (itVariables != mVariables.end()) {
      return mVariables[name];
    }

    return nullptr;
  };

  NodeValue* getVariable(const std::string& name) {
    itVariables = mVariables.find(name);
    if (itVariables != mVariables.end()) {
      return mVariables[name];
    }

    ASTContext* parent = this->getParent();

    if (parent) {
      return parent->getVariable(name);
    }

    return nullptr;
  };

  void store(const std::string name, NodeValue* node_value) { mVariables[name] = node_value; }
  NodeValue* update(const std::string name, NodeValue* node_value) {
    if (yydebug >= 2) {
      fprintf(stdout, "\n############ looking '%s' on context %s \n", name.c_str(), this->getName().c_str());
    }

    itVariables = mVariables.find(name);

    if (itVariables != mVariables.end()) {
      mVariables[name] = node_value;
      return node_value;
    }

    ASTContext* parent = this->getParent();

    if (parent) {
      return parent->update(name, node_value);
    }

    std::string error_msg("Variable '" + name + "' is not defined");
    return logErrorNV(new LogicErrorNode(this, error_msg));
  }
};

class NodeValue {
  int type;
  void* value;

 public:
  NodeValue(const std::string& value) : type(TYPE_STRING), value(0) {
    ;
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
  ExpNode(ASTContext* context) : ASTNode(context) {}
  virtual ~ExpNode() = default;

  void* eval() override {
    NodeValue* node_value = getValue();
    return node_value;
  };
  virtual NodeValue* getValue() = 0;

  ProcessorStrategy* getProcessorStrategy() override { return expNodeProcessorStrategy; };

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_EXP_NODE; };
};

class NumberNode : public ExpNode {
 private:
  void* value;
  int type;

 public:
  NumberNode(ASTContext* context, double val) : ExpNode(context), type(TYPE_DOUBLE) {
    value = new double;
    memcpy(value, &val, sizeof(double));
  };
  NumberNode(ASTContext* context, int val) : ExpNode(context), type(TYPE_INT) {
    value = new int;
    memcpy(value, &val, sizeof(int));
  };
  NumberNode(ASTContext* context, long val) : ExpNode(context), type(TYPE_LONG) {
    value = new long;
    memcpy(value, &val, sizeof(long));
  };

  // void* eval() override;
  NodeValue* getValue() override;
  virtual Value* codegen() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_NUMBER; };
};

class StringNode : public ExpNode {
 private:
  std::string value;

 public:
  StringNode(ASTContext* context, const std::string& value) : ExpNode(context), value(value){};
  StringNode(ASTContext* context, const char* value) : ExpNode(context), value(std::string(value)){};

  // void* eval() override;
  NodeValue* getValue() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_STRING; };
};

class VarNode : public ExpNode {
 private:
  std::string name;

 public:
  VarNode(ASTContext* context, const std::string& name) : ExpNode(context), name(name) {}
  const std::string& getName() const { return name; }

  // void* eval() override;
  NodeValue* getValue() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_VARIABLE; };
};

class UnaryExpNode : public ExpNode {
 private:
  char op;
  std::unique_ptr<ExpNode> rhs;

 public:
  UnaryExpNode(ASTContext* context, char op, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context), op(op), rhs(std::move(rhs)) {}
  UnaryExpNode(ASTContext* context, char op, ExpNode* rhs)
      : ExpNode(context), op(op), rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  // void* eval() override;
  NodeValue* getValue() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_UNARY_EXP; };
};

class BinaryExpNode : public ExpNode {
 private:
  char op;
  std::unique_ptr<ExpNode> lhs;
  std::unique_ptr<ExpNode> rhs;

 public:
  BinaryExpNode(ASTContext* context, char op, std::unique_ptr<ExpNode> lhs, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context), op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  BinaryExpNode(ASTContext* context, char op, ExpNode* lhs, ExpNode* rhs)
      : ExpNode(context),
        op(op),
        lhs(std::unique_ptr<ExpNode>(std::move(lhs))),
        rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  // void* eval() override;
  NodeValue* getValue() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_BINARY; };
};

/// CallExpNode - Expression class for function calls.
class CallExpNode : public ExpNode {
 private:
  std::string callee;
  std::vector<std::unique_ptr<ExpNode>> args;

 public:
  CallExpNode(ASTContext* context, const std::string& callee, std::vector<std::unique_ptr<ExpNode>>& args)
      : ExpNode(context), callee(callee), args(std::move(args)) {}
  CallExpNode(ASTContext* context, const std::string& callee, explist_t* head_exp_list)
      : ExpNode(context), callee(callee), args(std::vector<std::unique_ptr<ExpNode>>()) {
    if (head_exp_list) {
      explist_node_t* explist_node_t = head_exp_list->first;
      do {
        if (explist_node_t && explist_node_t->node) {
          args.push_back(std::unique_ptr<ExpNode>(std::move(explist_node_t->node)));
          explist_node_t = explist_node_t->next;
        }
      } while (explist_node_t);

      // TODO: free all the expressions not just the head_exp_list one
      // free(head_exp_list);
    }
  }

  // void* eval() override;
  NodeValue* getValue() override;
  ProcessorStrategy* getProcessorStrategy() override { return callNodeProcessorStrategy; };

  const std::string& getCallee() const { return callee; }
  std::vector<std::unique_ptr<ExpNode>>& getArgs() { return args; }

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_CALL_EXP; };
};

// ImportNode - Node class for file import
class ImportNode : public ASTNode {
 private:
  std::string filename;

 public:
  ImportNode(ASTContext* context, const std::string& filename) : ASTNode(context), filename(filename) {}

  // void* eval() override;
  ProcessorStrategy* getProcessorStrategy() override { return importNodeProcessorStrategy; };
  const std::string& getFilename() const { return filename; }

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_IMPORT; };
};

// FunctionDefNode - Node class for function definition.
class FunctionDefNode : public ASTNode {
 private:
  std::string name;
  std::vector<std::unique_ptr<arg_t>> args;
  std::vector<std::unique_ptr<ASTNode>> bodyNodes;
  ExpNode* returnNode;

 public:
  FunctionDefNode(ASTContext* context, const std::string& name, std::vector<std::unique_ptr<arg_t>>& args,
                  std::vector<std::unique_ptr<ASTNode>>& bodyNodes, ExpNode* returnNode)
      : ASTNode(context),
        name(name),
        args(std::move(args)),
        bodyNodes(std::move(bodyNodes)),
        returnNode(std::move(returnNode)) {}
  FunctionDefNode(ASTContext* context, const std::string& name, arglist_t* head_arg_list, stmtlist_t* head_stmt_list,
                  ExpNode* returnNode)
      : ASTNode(context),
        name(name),
        args(std::vector<std::unique_ptr<arg_t>>()),
        bodyNodes(std::vector<std::unique_ptr<ASTNode>>()),
        returnNode(std::move(returnNode)) {
    arglist_node_t* arglist_node = head_arg_list->first;
    stmtlist_node_t* stmtlist_node = head_stmt_list->first;
    do {
      if (arglist_node && arglist_node->arg) {
        args.push_back(std::unique_ptr<arg_t>(std::move(arglist_node->arg)));
        arglist_node = arglist_node->next;
      }
    } while (arglist_node);

    do {
      if (stmtlist_node && stmtlist_node->node) {
        bodyNodes.push_back(std::unique_ptr<ASTNode>(std::move(stmtlist_node->node)));
        stmtlist_node = stmtlist_node->next;
      }
    } while (stmtlist_node);

    // TODO: free all the args not just the first one
    // free(first_arg);
    // TODO: free all the stmts not just the first one
    // free(first_stmt);
  }

  // void* eval() override;

  ASTNode* check() override {
    ASTContext* context = getContext();
    ASTContext* parent = context->getParent();

    FunctionDefNode* function_node = parent->getFunction(name);
    if (function_node) {
      return new LogicErrorNode(context, "Function already exists in this context");
    }

    std::vector<std::unique_ptr<ASTNode>>::iterator itBodyNodes = bodyNodes.begin();

    for (; itBodyNodes != bodyNodes.end();) {
      std::unique_ptr<ASTNode>& bodyNode = *itBodyNodes;

      if (is_of_type<ImportNode>(*bodyNode.get())) {
        return new InvalidStatement(context, "Cannot import inside function definition");
      }

      if (yydebug >= 2) {
        fprintf(stdout, "\n[## evaluating body: ASTNode of type %d]\n", bodyNode.get()->getType());
      }
      ++itBodyNodes;
    }

    if (returnNode && is_of_type<ImportNode>(*returnNode)) {
      return new InvalidStatement(context, "Cannot import inside function definition");
    }

    return this;
  }

  const std::string& getName() const { return name; }
  std::vector<std::unique_ptr<arg_t>>& getArgs() { return args; }
  std::vector<std::unique_ptr<ASTNode>>& getBodyNodes() { return bodyNodes; }
  ExpNode* getReturnNode() { return returnNode; }

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_DEF_FUNCTION; };
};

class AssignmentNode : public ExpNode {
 protected:
  std::string name;
  std::unique_ptr<ExpNode> rhs;

 public:
  AssignmentNode(ASTContext* context, const std::string& name, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context), name(name), rhs(std::move(rhs)) {}
  AssignmentNode(ASTContext* context, const std::string& name, ExpNode* rhs)
      : ExpNode(context), name(name), rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  void* eval() override;
  NodeValue* getValue() override;
  ProcessorStrategy* getProcessorStrategy() override { return assignmentNodeProcessorStrategy; };

  const std::string& getName() const { return name; }

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_ASSIGNMENT; };
};

class DeclarationAssignmentNode : public AssignmentNode {
 public:
  DeclarationAssignmentNode(ASTContext* context, const std::string& name, std::unique_ptr<ExpNode> rhs)
      : AssignmentNode(context, name, std::move(rhs)) {}
  DeclarationAssignmentNode(ASTContext* context, const std::string& name, ExpNode* rhs)
      : AssignmentNode(context, name, std::move(rhs)) {}

  void* eval() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_DECLARATION_ASSIGNMENT; };
};

class DeclarationNode : public ASTNode {
 private:
  std::string name;

 public:
  DeclarationNode(ASTContext* context, const std::string& name) : ASTNode(context), name(name) {}

  void* eval() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_DECLARATION; };
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
//   ReturnNode(ASTContext* context, ExpNode* rhs) : ExpNode(context), rhs(std::move(rhs)) {}

//   NodeValue* getValue() override { return 0; }

//   int getType() const override { return getClassType(); };
//   static int getClassType() { return AST_NODE_TYPE_ASSIGNMENT; };
// };
// class DeclarationNode : public ASTNode {
//  private:
//   std::string name;

//  public:
//   DeclarationNode(ASTContext* context, const std::string& name) : ASTNode(context), name(name) {}

//   int getType() const override { return getClassType(); };
//   static int getClassType() { return AST_NODE_TYPE_DECLARATION; };
// };
}
#endif