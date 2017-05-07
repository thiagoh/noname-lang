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

  NonameJIT()
      : TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()), CompileLayer(ObjectLayer, SimpleCompiler(*TM)) {
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }

  TargetMachine &getTargetMachine() { return *TM; }

  ModuleHandleT addModule(std::unique_ptr<Module> module) {
    // We need a memory manager to allocate memory and resolve symbols for this
    // new module. Create one that resolves symbols by looking back into the
    // JIT.
    auto Resolver = createLambdaResolver(
        [&](const std::string &Name) {
          if (auto Sym = findMangledSymbol(Name)) return Sym.toRuntimeDyldSymbol();
          return RuntimeDyld::SymbolInfo(nullptr);
        },
        [](const std::string &S) { return nullptr; });

    Modules.push_back(module.get());

    auto module_handle = CompileLayer.addModuleSet(singletonSet(std::move(module)), make_unique<SectionMemoryManager>(),
                                                   std::move(Resolver));

    ModuleHandles.push_back(module_handle);
    return module_handle;
  }

  void removeModule(ModuleHandleT module_handle) {
    ModuleHandles.erase(std::find(ModuleHandles.begin(), ModuleHandles.end(), module_handle));
    CompileLayer.removeModuleSet(module_handle);
  }

  JITSymbol findSymbol(const std::string Name) { return findMangledSymbol(mangle(Name)); }

  // TODO FIXME
  void writeToFile(const Module *mod) {
    // then run llvm-dis output.bc

    std::string sname = "bc-output/";
    sname += mod->getName().str();
    sname += ".bc";
    const char *output_filename = sname.c_str();
    FILE *output_file = fopen(output_filename, "w");
    int output_filedescriptor = fileno(output_file);

    raw_fd_ostream os(output_filedescriptor, false, false);
    if (yydebug >= 1) {
      fprintf(stdout, "\n[module write]");
    }
    llvm::WriteBitcodeToFile(mod, os);
    // fclose(output_file);
  }

  // TODO FIXME
  void writeToFile() {
    // raw_fd_ostream os(output_filedescriptor, false, false);
    for (auto &mod : Modules) {
      if (yydebug >= 1) {
        fprintf(stdout, "\n[module write]");
      }
      // llvm::WriteBitcodeToFile(mod, os);
      writeToFile(mod);
    }
  }

  // TODO FIXME
  void release() {
    if (yydebug >= 1) {
      fprintf(stdout, "\n[release]");
    }

    Modules.clear();
  }

 private:
  std::string mangle(const std::string &Name) {
    std::string MangledName;
    {
      raw_string_ostream MangledNameStream(MangledName);
      Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
    }
    return MangledName;
  }

  template <typename T>
  static std::vector<T> singletonSet(T t) {
    std::vector<T> vec;
    vec.push_back(std::move(t));
    return vec;
  }

  JITSymbol findMangledSymbol(const std::string &Name) {
    // Search modules in reverse order: from last added to first added.
    // This is the opposite of the usual search order for dlsym, but makes more
    // sense in a REPL where we want to bind to the newest available definition.
    for (auto H : make_range(ModuleHandles.rbegin(), ModuleHandles.rend())) {
      if (auto Sym = CompileLayer.findSymbolIn(H, Name, true)) {
        return Sym;
      }
    }

    // If we can't find the symbol in the JIT, try looking in the host process.
    if (auto SymAddr = RTDyldMemoryManager::getSymbolAddressInProcess(Name)) {
      return JITSymbol(SymAddr, JITSymbolFlags::Exported);
    }

    return nullptr;
  }

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