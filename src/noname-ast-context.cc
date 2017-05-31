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
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Looking FunctionSignature '%s' on context %s]", name.c_str(), this->getName().c_str());
  }
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
// Variables
NodeValue* ASTContext::getVariableShallow(const std::string& name) {
  itVariables = mVariables.find(name);
  if (itVariables != mVariables.end()) {
    return mVariables[name];
  }

  return nullptr;
};
NodeValue* ASTContext::getVariable(const std::string& name) {
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Looking NodeValue '%s' on context %s]", name.c_str(), this->getName().c_str());
  }
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
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Storing NodeValue '%s' on context %s]", name.c_str(), this->getName().c_str());
  }
  mVariables[name] = node_value;
  return true;
}
bool ASTContext::removeVariable(const std::string name) {
  itVariables = mVariables.find(name);
  if (itVariables != mVariables.end()) {
    mVariables.erase(itVariables);
    return true;
  }
  return false;
}
NodeValue* ASTContext::updateVariable(const std::string name, NodeValue* node_value) {
  if (noname::debug >= 3) {
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
NodeValue* ASTContext::update(const std::string name, NodeValue* node_value) { return updateVariable(name, node_value); }
// AllocaInst
llvm::AllocaInst* ASTContext::getAllocaInst(const std::string& name) {
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Looking AllocaInst '%s' on context %s]", name.c_str(), this->getName().c_str());
  }

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
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Storing AllocaInst '%s' on context %s]", name.c_str(), this->getName().c_str());
  }
  mAllocaInst[name] = alloca_inst;
  return true;
}
bool ASTContext::removeAllocaInst(const std::string name) {
  itAllocaInst = mAllocaInst.find(name);
  if (itAllocaInst != mAllocaInst.end()) {
    mAllocaInst.erase(itAllocaInst);
    return true;
  }
  return false;
}
llvm::AllocaInst* ASTContext::updateAllocaInst(const std::string name, AllocaInst* alloca_inst) {
  if (noname::debug >= 3) {
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
llvm::AllocaInst* ASTContext::update(const std::string name, AllocaInst* alloca_inst) { return updateAllocaInst(name, alloca_inst); }

llvm::Value* ASTContext::getValue(const std::string& name) {
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Looking Value '%s' on context %s]", name.c_str(), this->getName().c_str());
  }

  itValue = mValue.find(name);
  if (itValue != mValue.end()) {
    return mValue[name];
  }

  ASTContext* parent = this->getParent();

  if (parent) {
    return parent->getValue(name);
  }

  return nullptr;
}

llvm::Type* ASTContext::getValueType(const std::string& name) {
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Looking Value type '%s' on context %s]", name.c_str(), this->getName().c_str());
  }

  itValueType = mValueType.find(name);
  if (itValueType != mValueType.end()) {
    return mValueType[name];
  }

  ASTContext* parent = this->getParent();

  if (parent) {
    return parent->getValueType(name);
  }

  return nullptr;
}
bool ASTContext::storeValue(const std::string name, llvm::Value* value) {
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Storing Value '%s' on context %s]", name.c_str(), this->getName().c_str());
  }

  mValue[name] = value;
  mValueType[name] = toLLVMType(value);

  return true;
}
bool ASTContext::removeValue(const std::string name) {
  itValue = mValue.find(name);
  if (itValue != mValue.end()) {
    mValue.erase(itValue);

    itValueType = mValueType.find(name);
    if (itValueType != mValueType.end()) {
      mValueType.erase(itValueType);
    }

    return true;
  }
  return false;
}
llvm::Value* ASTContext::updateValue(const std::string name, llvm::Value* value) {
  if (noname::debug >= 3) {
    fprintf(stdout, "\n[Looking Value '%s' on context %s]", name.c_str(), this->getName().c_str());
  }

  itValue = mValue.find(name);

  if (itValue != mValue.end()) {
    mValue[name] = value;
    mValueType[name] = toLLVMType(value);
    return value;
  }

  ASTContext* parent = this->getParent();

  if (parent) {
    return parent->updateValue(name, value);
  }

  std::string error_msg("Value '" + name + "' is not set");
  return logErrorLLVMA(new LogicErrorNode(this, error_msg));
}
}
