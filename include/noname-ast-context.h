#ifndef _NONAME_AST_CONTEXT_H
#define _NONAME_AST_CONTEXT_H

#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
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

using namespace llvm;

namespace noname {

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
class FunctionSignature;
class FunctionDefNode;
class DeclarationNode;
class AssignmentNode;
class DeclarationAssignmentNode;

class ASTContext {
 private:
  std::string name;
  ASTContext* parent;

  std::map<std::string, FunctionSignature*> mFunctionSignatures;
  std::map<std::string, FunctionSignature*>::iterator itFunctionSignatures;

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
        mFunctionSignatures(copy.mFunctionSignatures),
        mVariables(copy.mVariables),
        mAllocaInst(copy.mAllocaInst) {}
  ASTContext(const ASTContext& copy, ASTContext* parent)
      : name(copy.name),
        parent(parent),
        mFunctionSignatures(copy.mFunctionSignatures),
        mVariables(copy.mVariables),
        mAllocaInst(copy.mAllocaInst) {}
  ASTContext(const std::string& name, const ASTContext& copy, ASTContext* parent)
      : name(name),
        parent(parent),
        mFunctionSignatures(copy.mFunctionSignatures),
        mVariables(copy.mVariables),
        mAllocaInst(copy.mAllocaInst) {}
  virtual ~ASTContext() = default;
  ASTContext& operator=(const ASTContext& copy) {
    name = copy.name;
    parent = copy.parent;
    mVariables = copy.mVariables;
    mFunctionSignatures = copy.mFunctionSignatures;
    mAllocaInst = copy.mAllocaInst;
    return *this;
  }
  std::string getName() const { return name; }
  ASTContext* getParent() const { return parent; }

  // Functions
  FunctionSignature* getFunctionSignature(const std::string& name);
  bool storeFunctionSignature(const std::string name, FunctionSignature* function_signature);
  bool store(const std::string name, FunctionSignature* function_signature);
  bool removeFunctionSignature(const std::string name);

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
}

#endif