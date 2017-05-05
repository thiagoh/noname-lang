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

extern int yydebug;

namespace noname {

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
extern std::unique_ptr<NonameJIT> TheJIT;

// Functions
FunctionDefNode* ASTContext::getFunction(const std::string& name) {
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

bool ASTContext::storeFunction(const std::string name,
                               FunctionDefNode* function_nomde) {
  mFunctions[name] = function_nomde;
  return true;
}
bool ASTContext::store(const std::string name,
                       FunctionDefNode* function_nomde) {
  return storeFunction(name, function_nomde);
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
bool ASTContext::store(const std::string name, NodeValue* node_value) {
  return storeVariable(name, node_value);
}
bool ASTContext::removeFunction(const std::string name) {
  itFunctions = mFunctions.find(name);
  if (itFunctions != mFunctions.end()) {
    mFunctions.erase(itFunctions);
    return true;
  }
  return false;
}
bool ASTContext::removeVariable(const std::string name) {
  itVariables = mVariables.find(name);
  if (itVariables != mVariables.end()) {
    mVariables.erase(itVariables);
    return true;
  }
  return false;
}
NodeValue* ASTContext::updateVariable(const std::string name,
                                      NodeValue* node_value) {
  if (yydebug >= 2) {
    fprintf(stdout, "\n############ looking '%s' on context %s \n",
            name.c_str(), this->getName().c_str());
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
}
