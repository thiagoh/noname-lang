#include "noname-jit.h"
#include "noname-types.h"
#include <limits.h>
#include <unistd.h>
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

namespace llvm {
namespace orc {
typedef ObjectLinkingLayer<> ObjLayerT;
typedef IRCompileLayer<ObjLayerT> CompileLayerT;
typedef CompileLayerT::ModuleSetHandleT ModuleHandleT;

// extern LLVMContext TheContext;
// extern IRBuilder<> Builder;
// extern std::unique_ptr<Module> TheModule;
// extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;

NonameJIT::NonameJIT()
    : TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()), CompileLayer(ObjectLayer, SimpleCompiler(*TM)) {
  ;
  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
}

NonameJIT::~NonameJIT() {
  ;
  if (noname::debug >= 1) {
    fprintf(stderr, "\n[NonameJIT::~NonameJIT() called]");
  }
}

TargetMachine &NonameJIT::getTargetMachine() { return *TM; }

CompileLayerT::ModuleSetHandleT NonameJIT::addModule(std::unique_ptr<Module> module) {
  // We need a memory manager to allocate memory and resolve symbols for this
  // new module. Create one that resolves symbols by looking back into the JIT.
  auto Resolver = createLambdaResolver(
      [&](const std::string &Name) {
        if (auto Sym = findMangledSymbol(Name)) {
          return Sym.toRuntimeDyldSymbol();
        }
        return RuntimeDyld::SymbolInfo(nullptr);
      },
      [](const std::string &S) { return nullptr; });

  Modules.push_back(module.get());

  auto module_set_handle = CompileLayer.addModuleSet(singletonSet(std::move(module)),
                                                     make_unique<SectionMemoryManager>(), std::move(Resolver));

  ModuleHandles.push_back(module_set_handle);
  return module_set_handle;
}

void NonameJIT::removeModule(ModuleHandleT module_handle) {
  ModuleHandles.erase(std::find(ModuleHandles.begin(), ModuleHandles.end(), module_handle));
  CompileLayer.removeModuleSet(module_handle);
}

JITSymbol NonameJIT::findSymbol(const std::string name) { return findMangledSymbol(mangle(name)); }

std::string NonameJIT::mangle(const std::string &Name) {
  std::string MangledName;
  {
    raw_string_ostream MangledNameStream(MangledName);
    Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
  }
  return MangledName;
}

template <typename T>
std::vector<T> NonameJIT::singletonSet(T t) {
  std::vector<T> vec;
  vec.push_back(std::move(t));
  return vec;
}

JITSymbol NonameJIT::findMangledSymbol(const std::string &symbol_name) {
  // Search modules in reverse order: from last added to first added.
  // This is the opposite of the usual search order for dlsym, but makes more
  // sense in a REPL where we want to bind to the newest available definition.
  for (auto module_handle : make_range(ModuleHandles.rbegin(), ModuleHandles.rend())) {
    if (auto symbol = CompileLayer.findSymbolIn(module_handle, symbol_name, true)) {
      return symbol;
    }
  }

  // If we can't find the symbol in the JIT, try looking in the host process.
  if (auto symbol_addr = RTDyldMemoryManager::getSymbolAddressInProcess(symbol_name)) {
    return JITSymbol(symbol_addr, JITSymbolFlags::Exported);
  }

  return nullptr;
}

// TODO FIXME
void NonameJIT::writeToFile(const Module *mod) {
  // if (true) return;

  std::string output_filenam = "bc-output/";
  output_filenam += mod->getName().str() + ".bc";
  FILE *output_file = fopen(output_filenam.c_str(), "w");
  int output_filedescriptor = fileno(output_file);

  raw_fd_ostream os(output_filedescriptor, true, true);
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[module write]");
    fflush(stdout);
  }
  llvm::WriteBitcodeToFile(mod, os);
  // fflush(output_file);
  // fclose(output_file);
  // close(output_filedescriptor);
}

// TODO FIXME
void NonameJIT::writeToFile() {
  for (auto &mod : Modules) {
    if (noname::debug >= 1) {
      fprintf(stdout, "\n[module write]");
      fflush(stdout);
    }
    writeToFile(mod);
  }
}

// TODO FIXME
void NonameJIT::release() {
  if (noname::debug >= 1) {
    fprintf(stdout, "\n[NonameJIT::release()]");
  }

  Modules.clear();
}
}
}
