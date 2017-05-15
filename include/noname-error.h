#ifndef _NONAME_ERROR_H
#define _NONAME_ERROR_H

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

ASTNode* logError(const char* str);
FunctionDefNode* logErrorF(const char* str);
AssignmentNode* logErrorV(const char* str);
ASTNode* logError(ErrorNode* error_node);
NodeValue* logErrorNV(ErrorNode* error_node);

/// LogError* - These are little helper functions for error handling.
void createError(Error& error, const char* str);
ASTNode* logError(const char* str);
FunctionDefNode* logErrorF(const char* str);
AssignmentNode* logErrorV(const char* str);
llvm::Value* logErrorLLVM(const char* str);
llvm::Function* logErrorLLVMF(const char* str);
ASTNode* logError(ErrorNode* error_node);
NodeValue* logErrorNV(ErrorNode* error_node);
llvm::Value* logErrorLLVM(ErrorNode* error_node);
llvm::AllocaInst* logErrorLLVMA(ErrorNode* error_node);
llvm::Function* logErrorLLVMF(ErrorNode* error_node);

class Error {
 private:
  std::string _what;
  int _code;

 public:
  explicit Error() : _what(""), _code(0) {}
  explicit Error(const std::string& what) : _what(what), _code(1) {}
  explicit Error(const Error& error) : _what(error._what), _code(error._code) {}
  virtual ~Error() = default;

  const std::string& what() const { return _what; }
  void what(int code, const std::string& what) {
    this->_what = what;
    this->_code = code;
  }
  void what(const std::string& what) { this->what(1, what); }
  int code() const { return this->_code; }
};
}

#endif