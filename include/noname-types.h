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
#include "llvm/Support/Debug.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "noname-ast-context.h"
#include "noname-utils.h"
#include "noname-error.h"
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
  TYPE_DATATYPE = 32,
  TYPE_VOID = 33,
  TYPE_VOID_POINTER = 34,
  TYPE_CHAR = 35,
  TYPE_SHORT = 36,
  TYPE_INT = 37,
  TYPE_FLOAT = 38,
  TYPE_LONG = 39,
  TYPE_DOUBLE = 40,
  TYPE_STRING = 41,
};
#endif

class ASTNode;
class ASTContext;
class ErrorNode;
class LogicErrorNode;
class ExpNode;
class ReturnExpNode;
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

extern Type* VoidTy;
extern PointerType* PointerTy_32;
extern PointerType* PointerTy_64;
extern PointerType* PointerTy_16;
extern PointerType* PointerTy_8;
extern PointerType* PointerTy_Double;
extern PointerType* PointerTy_Float;
extern StructType* StructTy_struct_datatype_t;

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
typedef struct datatype_t {
  int type;
  void* v;
} datatype_t;
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

void CreateNewModuleAndInitialize();
void InitializeNonameEnvironment();
void ReleaseNonameEnvironment();
void InitializeModuleAndPassManager();

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
    AST_NODE_TYPE_RETURN_NODE,
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
  std::vector<Value*> codegen_elements_vector;
  bool generated;

 public:
  ASTNode(ASTContext* context) : context(context), kind(AST_NODE_TYPE_AST_NODE), generated(false) {}
  ASTNode(ASTContext* context, ASTNodeKind kind) : context(context), kind(kind), generated(false) {}
  virtual ~ASTNode() = default;
  ASTNodeKind getKind() const { return kind; }
  std::vector<Value*> get_codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) {
    generate(error, bb);
    return codegen_elements_vector;
  }
  virtual void* eval() { return nullptr; };
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) {
    Error error;
    generate(error, bb);

    if (error.code()) {
      return logErrorLLVM(error.what().c_str());
    }

    return codegen_elements_vector.back();
  }
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const {
    return codegen_elements_vector;
  }

  virtual ASTNode* check() const { return nullptr; };
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
      AST_NODE_KIND_PROCESS_VAL(ASTNode::AST_NODE_TYPE_RETURN_NODE)
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

 private:
  void generate(Error& error, llvm::BasicBlock* bb = nullptr) {
    if (!generated) {
      std::vector<Value*> elements = codegen_elements(error, bb);

      if (error.code()) {
        return;
      }

      codegen_elements_vector.clear();
      codegen_elements_vector.insert(codegen_elements_vector.end(), elements.begin(), elements.end());

      generated = true;
    }
  }
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

class NodeValue {
  int type;
  void* value;

 public:
  NodeValue(const std::string& value);
  NodeValue(long value);
  NodeValue(int value);
  NodeValue(short value);
  NodeValue(char value);
  NodeValue(double value);
  NodeValue(float value);
  virtual ~NodeValue();

  int getType() { return type; }
  void* getRawValue() { return value; }
  Value* constant_codegen(llvm::BasicBlock* bb = nullptr);
  void* getValue(int as_type) const;
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

  virtual std::unique_ptr<NodeValue> getValue() const = 0;

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
  virtual std::unique_ptr<NodeValue> getValue() const override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

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
  virtual std::unique_ptr<NodeValue> getValue() const override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

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
  virtual std::unique_ptr<NodeValue> getValue() const override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

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
  virtual std::unique_ptr<NodeValue> getValue() const override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

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
  virtual std::unique_ptr<NodeValue> getValue() const override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_BINARY; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_BINARY; }

 private:
  Value* CreatePow(Value* L, Value* R, const char* name) const;
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
ASTNode* create_anonymous_function_def_node(ASTContext* context, ExpNode* exp_node);

class FunctionArgument {
  friend class FunctionSignature;
  friend class FunctionDefNode;

 private:
  std::string name;
  llvm::Type* type;
  ExpNode* default_value;

 public:
  FunctionArgument(const std::string name, llvm::Type* type, ExpNode* default_value = nullptr)
      : name(name), type(type), default_value(default_value) {}
};

// FunctionDefNode - Node class for function definition.
class FunctionSignature {
  friend class FunctionDefNode;

 private:
  std::string name;
  std::vector<FunctionArgument*> args_defs;
  llvm::Type* return_type;

 public:
  FunctionSignature(const std::string& name, std::vector<FunctionArgument*> args_defs, llvm::Type* return_type);
  FunctionSignature(const FunctionSignature& copy);
  virtual ~FunctionSignature();
  const std::string& getName() const { return name; }
  std::vector<FunctionArgument*>& getArgsDefs() { return args_defs; }
  llvm::Type* getReturnType() {
    if (!return_type) {
      fprintf(stderr, "FunctionSignature %s has no return type set", name.c_str());
      exit(1);
      return llvm::Type::getVoidTy(TheContext);
    }

    return return_type;
  }

  llvm::Function* codegen();
};

// FunctionDefNode - Node class for function definition.
class FunctionDefNode : public ASTNode {
 private:
  std::vector<std::unique_ptr<ASTNode>> body_nodes;
  FunctionSignature* function_signature;

 public:
  FunctionDefNode(ASTContext* context, const std::string& name, std::vector<FunctionArgument*> args_defs,
                  std::vector<std::unique_ptr<ASTNode>> body_nodes);
  FunctionDefNode(ASTContext* context, const std::string& name, arglist_t* head_arg_list, stmtlist_t* head_stmt_list);
  virtual ~FunctionDefNode();

  // virtual void* eval() override;
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;

  ASTNode* check() const override;
  std::unique_ptr<ASTNode>& getReturnNode() {
    if (body_nodes.size() <= 0) {
      static std::unique_ptr<ASTNode> null_return_node(nullptr);
      return null_return_node;
    }
    return body_nodes.back();
  }

  const std::string& getName() const { return function_signature->getName(); }
  std::vector<std::unique_ptr<ASTNode>>& getBodyNodes() { return body_nodes; }

  std::vector<FunctionArgument*>& getFunctionArguments() { return function_signature->args_defs; }
  llvm::Type* getReturnLLVMType() { return function_signature->getReturnType(); }
  FunctionSignature* getFunctionSignature() { return function_signature; }

  Function* getFunctionDefinition();
  ProcessorStrategy* getProcessorStrategy() override { return functionDefNodeProcessorStrategy; };

  // int getType() const override { return getClassType(); };
  // static int getClassType() { return AST_NODE_TYPE_DEF_FUNCTION; };
  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_DEF_FUNCTION; }

 private:
  FunctionSignature* createFunctionSignature(Error& error, const std::string& name,
                                             std::vector<FunctionArgument*> args_defs);
  llvm::ReturnInst* getLLVMReturnInst(Value* return_value) const;
};

class TopLevelExpNode : public ExpNode {
 private:
  ExpNode* exp_node;
  CallExpNode* call_exp_node;
  Function* anonymous_function;

 public:
  TopLevelExpNode(ASTContext* context, ExpNode* exp_node, CallExpNode* call_exp_node, Function* anonymous_function);
  virtual ~TopLevelExpNode();

  virtual void* eval() override { return exp_node->eval(); };
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

  virtual std::unique_ptr<NodeValue> getValue() const override { return exp_node->getValue(); };
  void* release();
  llvm::Type* getReturnLLVMType() { return anonymous_function->getReturnType(); }
  ProcessorStrategy* getProcessorStrategy() override { return topLevelExpNodeProcessorStrategy; };

  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_TOP_LEVEL_EXP_NODE; }
};

class ReturnExpNode : public ExpNode {
 private:
  ExpNode* exp_node;

 public:
  ReturnExpNode(ASTContext* context, ExpNode* exp_node);
  virtual ~ReturnExpNode();

  virtual void* eval() override { return exp_node->eval(); };
  virtual std::unique_ptr<NodeValue> getValue() const override { return exp_node->getValue(); };
  virtual Value* codegen(llvm::BasicBlock* bb) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb) const override;

  static bool classof(const ASTNode* S) { return S->getKind() == AST_NODE_TYPE_RETURN_NODE; }
};

/// CallExpNode - Expression class for function calls.
class CallExpNode : public ExpNode {
 private:
  std::string callee;
  llvm::Function* called_function;
  std::vector<std::unique_ptr<ExpNode>> args;

 public:
  CallExpNode(ASTContext* context, const std::string& callee, explist_t* head_exp_list = nullptr);
  CallExpNode(ASTContext* context, llvm::Function* called_function, explist_t* head_exp_list = nullptr);
  virtual ~CallExpNode() override;

  // virtual void* eval() override;
  virtual std::unique_ptr<NodeValue> getValue() const override { return std::unique_ptr<NodeValue>(nullptr); };
  virtual Value* codegen(llvm::BasicBlock* bb = nullptr) override;
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

  virtual ProcessorStrategy* getProcessorStrategy() override { return callNodeProcessorStrategy; };

  const std::string& getCallee() const { return callee; }
  llvm::Function* getCalledFunction(Error& error) const;
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
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

  virtual std::unique_ptr<NodeValue> getValue() const override;
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
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;
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
  virtual std::vector<Value*> codegen_elements(Error& error, llvm::BasicBlock* bb = nullptr) const override;

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

// class ReturnExpNode : public ExpNode {
//  private:
//   ExpNode* rhs;

//  public:
//   ReturnExpNode(ASTContext* context, ExpNode* rhs) : ExpNode(context),
//   rhs(std::move(rhs)) {}

//   virtual std::unique_ptr<NodeValue> getValue() const override { return 0; }

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