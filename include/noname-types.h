#ifndef _NONAME_TREE_H
#define _NONAME_TREE_H

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
#include <vector>

using namespace llvm;
// using namespace llvm::orc;

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

class ASTNode;
class ExpNode;
class BinaryExpNode;
class NodeValue;
class VarNode;
class CallExprNode;
class FunctionDefNode;
class AssignmentNode;
class DeclarationNode;
class ASTContext;

/* list of statements */
struct stmtlist_node {
  ASTNode* node;
  struct stmtlist_node* next;
  stmtlist_node& operator=(const stmtlist_node&) { return *this; }
};
struct stmtlist {
  struct stmtlist_node* first;
  struct stmtlist_node* last;
  stmtlist& operator=(const stmtlist&) { return *this; }
};
/* list of expressions */
struct explist_node {
  ExpNode* node;
  struct explist_node* next;
  explist_node& operator=(const explist_node&) { return *this; }
};
struct explist {
  struct explist_node* first;
  struct explist_node* last;
  explist& operator=(const explist&) { return *this; }
};
/* list of args, for an argument list */
struct arg {
  char* name;
  ASTNode* defaultValue;
  arg& operator=(const arg&) { return *this; }
};
struct arglist_node {
  struct arg* arg;
  struct arglist_node* next;
  arglist_node& operator=(const arglist_node&) { return *this; }
};
struct arglist {
  struct arglist_node* first;
  struct arglist_node* last;
  arglist& operator=(const arglist&) { return *this; }
};

typedef struct explist explist;
typedef struct explist_node explist_node;
typedef struct arglist arglist;
typedef struct arglist_node arglist_node;
typedef struct stmtlist stmtlist;
typedef struct stmtlist_node stmtlist_node;
typedef struct arg arg;

void print_node_value(NodeValue& nodeValue);
void print_node_value(FILE* file, NodeValue& nodeValue);
stmtlist* new_stmt_list(ASTContext* context);
stmtlist* new_stmt_list(ASTContext* context, ASTNode* node);
stmtlist* new_stmt_list(ASTContext* context, stmtlist* head_exp_list, ASTNode* node);
explist* new_exp_list(ASTContext* context);
explist* new_exp_list(ASTContext* context, ExpNode* node);
explist* new_exp_list(ASTContext* context, explist* head_exp_list, ExpNode* node);
arg* new_arg(ASTContext* context, char* arg, ASTNode* defaultValue);
arg* new_arg(ASTContext* context, char* arg, double defaultValue);
arg* new_arg(ASTContext* context, char* arg, long defaultValue);
arg* new_arg(ASTContext* context, char* arg, char* defaultValue);
arglist* new_arg_list(ASTContext* context);
arglist* new_arg_list(ASTContext* context, arglist* head_arg_list);
arglist* new_arg_list(ASTContext* context, arglist* head_arg_list, arg* arg);

VarNode* new_var_node(ASTContext* context, const std::string& name);
AssignmentNode* new_assignment_node(ASTContext* context, const std::string& name, ExpNode* node);
AssignmentNode* new_declaration_node(ASTContext* context, const std::string& name);
CallExprNode* new_call_node(ASTContext* context, const std::string& name, explist* exp_list);
FunctionDefNode* new_function_def(ASTContext* context, const std::string& name, arglist* arg_list, stmtlist* stmt_list,
                                  ExpNode* returnNode);

class ASTNode {
 private:
  ASTContext* context;

 public:
  ASTNode(ASTContext* context) : context(context) {}
  virtual ~ASTNode() = default;
  virtual void eval(){};
  virtual int getType() const { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_AST_NODE; };
  ASTContext* getContext() const { return context; };
};

template <typename To, typename From>
struct is_of_type_impl {
  static inline bool doit(const From& from) {
    // fprintf(stderr, "\ncomparing %d with %d", from.getType(),
    // To::getClassType());
    return from.getType() == To::getClassType() || std::is_base_of<To, From>::value;
  }
};

template <class To, class From>
inline bool is_of_type(const From& from) {
  return is_of_type_impl<To, From>::doit(from);
}

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
  virtual ~ASTContext() = default;
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

    return NULL;
  };

  void store(const std::string name, FunctionDefNode* functionNode) { mFunctions[name] = functionNode; }

  // Variables
  NodeValue* getVariable(const std::string& name) {
    itVariables = mVariables.find(name);
    if (itVariables != mVariables.end()) {
      return mVariables[name];
    }

    ASTContext* parent = this->getParent();

    if (parent) {
      return parent->getVariable(name);
    }

    return NULL;
  };

  void store(const std::string name, NodeValue* node_value) { mVariables[name] = node_value; }
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
    if (as_type == TYPE_INT) {
      if (type == TYPE_INT) {
        return value;
      }
      int* intp_res = new int;

      if (type == TYPE_LONG) {
        int int_res;
        memcpy(this->value, &value, sizeof(int));
      }
      return intp_res;
    }
    return value;
  }
};

class ExpNode : public ASTNode {
 public:
  ExpNode(ASTContext* context) : ASTNode(context) {}
  virtual ~ExpNode() = default;

  void eval() override { NodeValue* node_value = getValue(); };
  virtual NodeValue* getValue() = 0;

  virtual int getType() const override { return getClassType(); };
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

  NodeValue* getValue() override {
    if (type == TYPE_DOUBLE) {
      return new NodeValue(*(double*)value);
    }
    if (type == TYPE_LONG) {
      return new NodeValue(*(long*)value);
    }
    if (type == TYPE_INT) {
      return new NodeValue(*(int*)value);
    }
    return NULL;
  }

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_NUMBER; };
};

class StringNode : public ExpNode {
 private:
  std::string value;

 public:
  StringNode(ASTContext* context, std::string& value) : ExpNode(context), value(value){};
  StringNode(ASTContext* context, const char* value) : ExpNode(context), value(std::string(value)){};
  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_STRING; };

  // void eval() override;
  NodeValue* getValue() override {
    NodeValue* node = new NodeValue(value);
    return node;
  }
};

class VarNode : public ExpNode {
 private:
  std::string name;

 public:
  VarNode(ASTContext* context, const std::string& name) : ExpNode(context), name(name) {}
  const std::string& getName() const { return name; }

  // void eval() override;
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

  // void eval() override;
  NodeValue* getValue() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_UNARY_EXP; };
};

class BinaryExpNode : public ExpNode {
 private:
  char op;
  std::unique_ptr<ExpNode> lhs, rhs;

 public:
  BinaryExpNode(ASTContext* context, char op, std::unique_ptr<ExpNode> lhs, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context), op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  BinaryExpNode(ASTContext* context, char op, ExpNode* lhs, ExpNode* rhs)
      : ExpNode(context),
        op(op),
        lhs(std::unique_ptr<ExpNode>(std::move(lhs))),
        rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  // void eval() override;
  NodeValue* getValue() override;

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_BINARY; };
};

/// CallExprNode - Expression class for function calls.
class CallExprNode : public ExpNode {
 private:
  std::string callee;
  std::vector<std::unique_ptr<ExpNode>> args;

 public:
  CallExprNode(ASTContext* context, const std::string& callee, std::vector<std::unique_ptr<ExpNode>>& args)
      : ExpNode(context), callee(callee), args(std::move(args)) {}
  CallExprNode(ASTContext* context, const std::string& callee, explist* head_exp_list)
      : ExpNode(context), callee(callee), args(std::vector<std::unique_ptr<ExpNode>>()) {
    if (head_exp_list) {
      explist_node* explist_node = head_exp_list->first;
      do {
        if (explist_node && explist_node->node) {
          args.push_back(std::unique_ptr<ExpNode>(std::move(explist_node->node)));
          explist_node = explist_node->next;
        }
      } while (explist_node);

      // TODO: free all the expressions not just the head_exp_list one
      // free(head_exp_list);
    }
  }

  // void eval() override;
  NodeValue* getValue() override;

  const std::string& getCallee() const { return callee; }
  std::vector<std::unique_ptr<ExpNode>>& getArgs() { return args; }

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_CALL_EXP; };
};

// FunctionDefNode - Node class for function definition.
class FunctionDefNode : public ASTNode {
 private:
  std::string name;
  std::vector<std::unique_ptr<arg>> args;
  std::vector<std::unique_ptr<ASTNode>> bodyNodes;
  ExpNode* returnNode;

 public:
  FunctionDefNode(ASTContext* context, const std::string& name, std::vector<std::unique_ptr<arg>>& args,
                  std::vector<std::unique_ptr<ASTNode>>& bodyNodes, ExpNode* returnNode)
      : ASTNode(context),
        name(name),
        args(std::move(args)),
        bodyNodes(std::move(bodyNodes)),
        returnNode(std::move(returnNode)) {}
  FunctionDefNode(ASTContext* context, const std::string& name, arglist* head_arg_list, stmtlist* head_stmt_list,
                  ExpNode* returnNode)
      : ASTNode(context),
        name(name),
        args(std::vector<std::unique_ptr<arg>>()),
        bodyNodes(std::vector<std::unique_ptr<ASTNode>>()),
        returnNode(std::move(returnNode)) {
    arglist_node* arglist_node = head_arg_list->first;
    stmtlist_node* stmtlist_node = head_stmt_list->first;
    do {
      if (arglist_node && arglist_node->arg) {
        args.push_back(std::unique_ptr<arg>(std::move(arglist_node->arg)));
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

  // void eval() override;

  const std::string& getName() const { return name; }
  std::vector<std::unique_ptr<arg>>& getArgs() { return args; }
  std::vector<std::unique_ptr<ASTNode>>& getBodyNodes() { return bodyNodes; }
  ExpNode* getReturnNode() { return returnNode; }

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_DEF_FUNCTION; };
};

class AssignmentNode : public ExpNode {
 private:
  std::string name;
  std::unique_ptr<ExpNode> rhs;

 public:
  AssignmentNode(ASTContext* context, const std::string& name, std::unique_ptr<ExpNode> rhs)
      : ExpNode(context), name(name), rhs(std::move(rhs)) {}
  AssignmentNode(ASTContext* context, const std::string& name, ExpNode* rhs)
      : ExpNode(context), name(name), rhs(std::unique_ptr<ExpNode>(std::move(rhs))) {}

  void eval() override;
  NodeValue* getValue() override;

  const std::string& getName() const { return name; }
  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_ASSIGNMENT; };
};
class DeclarationNode : public ASTNode {
 private:
  std::string name;

 public:
  DeclarationNode(ASTContext* context, const std::string& name) : ASTNode(context), name(name) {}

  int getType() const override { return getClassType(); };
  static int getClassType() { return AST_NODE_TYPE_DECLARATION; };
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

#endif