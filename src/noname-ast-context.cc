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

namespace noname {

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
extern std::unique_ptr<NonameJIT> TheJIT;

// Function Signatures
FunctionSignature* ASTContext::getFunctionSignature(const std::string& name) {
  itFunctionSignatures = mFunctionSignatures.find(name);
  if (itFunctionSignatures != mFunctionSignatures.end()) {
    return mFunctionSignatures[name];
  }

  ASTContext* parent = this->getParent();

  if (parent) {
    return parent->getFunctionSignature(name);
  }

  return nullptr;
};
bool ASTContext::storeFunctionSignature(const std::string name, FunctionSignature* function_signature) {
  mFunctionSignatures[name] = function_signature;
  return true;
}
bool ASTContext::store(const std::string name, FunctionSignature* function_signature) {
  return storeFunctionSignature(name, function_signature);
}

// Variables
NodeValue* ASTContext::getVariableShallow(const std::string& name) {
  itVariables = mVariables.find(name);
  if (itVariables != mVariables.end()) {
    return mVariables[name];
  }

  return nullptr;
};
NodeValue* ASTContext::getVariable(const std::string& name) {
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
bool ASTContext::storeVariable(const std::string name, NodeValue* node_value) {
  mVariables[name] = node_value;
  return true;
}
bool ASTContext::store(const std::string name, NodeValue* node_value) { return storeVariable(name, node_value); }
bool ASTContext::removeVariable(const std::string name) {
  itVariables = mVariables.find(name);
  if (itVariables != mVariables.end()) {
    mVariables.erase(itVariables);
    return true;
  }
  return false;
}
NodeValue* ASTContext::updateVariable(const std::string name, NodeValue* node_value) {
  if (noname::debug >= 2) {
    fprintf(stdout, "\n[Looking Variable '%s' on context %s]", name.c_str(), this->getName().c_str());
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
NodeValue* ASTContext::update(const std::string name, NodeValue* node_value) {
  return updateVariable(name, node_value);
}
// AllocaInst
llvm::AllocaInst* ASTContext::getAllocaInst(const std::string& name) {
  itAllocaInst = mAllocaInst.find(name);
  if (itAllocaInst != mAllocaInst.end()) {
    return mAllocaInst[name];
  }

  ASTContext* parent = this->getParent();

  if (parent) {
    return parent->getAllocaInst(name);
  }

  return nullptr;
}
bool ASTContext::storeAllocaInst(const std::string name, AllocaInst* alloca_inst) {
  mAllocaInst[name] = alloca_inst;
  return true;
}
bool ASTContext::store(const std::string name, AllocaInst* alloca_inst) { return storeAllocaInst(name, alloca_inst); }
bool ASTContext::removeAllocaInst(const std::string name) {
  itAllocaInst = mAllocaInst.find(name);
  if (itAllocaInst != mAllocaInst.end()) {
    mAllocaInst.erase(itAllocaInst);
    return true;
  }
  return false;
}
llvm::AllocaInst* ASTContext::updateAllocaInst(const std::string name, AllocaInst* alloca_inst) {
  if (noname::debug >= 2) {
    fprintf(stdout, "\n[Looking AllocaInst '%s' on context %s]", name.c_str(), this->getName().c_str());
  }

  itAllocaInst = mAllocaInst.find(name);

  if (itAllocaInst != mAllocaInst.end()) {
    mAllocaInst[name] = alloca_inst;
    return alloca_inst;
  }

  ASTContext* parent = this->getParent();

  if (parent) {
    return parent->update(name, alloca_inst);
  }

  std::string error_msg("AllocaInst '" + name + "' is not set");
  return logErrorLLVMA(new LogicErrorNode(this, error_msg));
}
llvm::AllocaInst* ASTContext::update(const std::string name, AllocaInst* alloca_inst) {
  return updateAllocaInst(name, alloca_inst);
}
}
