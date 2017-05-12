//===----- NonameJIT.h - A simple JIT for noname lang ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// Contains a simple JIT definition for use in the kaleidoscope tutorials.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_EXECUTIONENGINE_ORC_NONAME_H
#define LLVM_EXECUTIONENGINE_ORC_NONAME_H

#include "llvm/ADT/iterator_range.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbolFlags.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm-c/BitWriter.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <stdio.h>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

extern int yydebug;

namespace llvm {
namespace orc {
class NonameJIT;
}
}

namespace noname {
extern std::unique_ptr<llvm::orc::NonameJIT> TheJIT;
}

namespace llvm {
namespace orc {
class NonameJIT {
 public:
  typedef ObjectLinkingLayer<> ObjLayerT;
  typedef IRCompileLayer<ObjLayerT> CompileLayerT;
  typedef CompileLayerT::ModuleSetHandleT ModuleHandleT;

  NonameJIT();
  virtual ~NonameJIT();

  TargetMachine &getTargetMachine();

  CompileLayerT::ModuleSetHandleT addModule(std::unique_ptr<Module> module);

  void removeModule(ModuleHandleT module_handle);

  JITSymbol findSymbol(const std::string Name);
  llvm::Function* getFunction(const std::string Name);

  void writeToFile(const Module *mod);
  void writeToFile();
  void release();

 private:
  std::string mangle(const std::string &Name);

  template <typename T>
  static std::vector<T> singletonSet(T t);

  JITSymbol findMangledSymbol(const std::string &symbol_name);

  std::unique_ptr<TargetMachine> TM;
  const DataLayout DL;
  ObjLayerT ObjectLayer;
  CompileLayerT CompileLayer;
  std::vector<ModuleHandleT> ModuleHandles;
  std::vector<Module *> Modules;
};

}  // end namespace orc
}  // end namespace llvm

#endif  // LLVM_EXECUTIONENGINE_ORC_NONAME_H