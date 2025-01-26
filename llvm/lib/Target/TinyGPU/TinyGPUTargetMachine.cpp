//===-- TinyGPUTargetMachine.cpp - Define TargetMachine for TinyGPU -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about TinyGPU target spec.
//
//===----------------------------------------------------------------------===//

#include "TinyGPUTargetMachine.h"
#include "TinyGPUISelDAGToDAG.h"
#include "TinyGPUSubtarget.h"
#include "TinyGPUTargetObjectFile.h"
#include "TargetInfo/TinyGPUTargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTinyGPUTarget() {
  // Register the target.
  //- Little endian Target Machine
  RegisterTargetMachine<TinyGPUTargetMachine> X(getTheTinyGPUTarget());
  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeTinyGPUDAGToDAGISelLegacyPass(PR);
}

static std::string computeDataLayout() {
  std::string Ret = "";

  // Little endian
  Ret += "e";

  // ELF name mangling
  Ret += "-m:e";

  // 32-bit pointers, 32-bit aligned
  Ret += "-p:32:32";

  // 64-bit integers, 64 bit aligned
  Ret += "-i64:64";

  // 32-bit native integer width i.e register are 32-bit
  Ret += "-n32";

  // 128-bit natural stack alignment
  Ret += "-S128";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(std::optional<CodeModel::Model> CM,
                                           std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

TinyGPUTargetMachine::TinyGPUTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL,
                                       bool JIT)
    : CodeGenTargetMachineImpl(T, computeDataLayout(), TT, CPU, FS, Options,
                        getEffectiveRelocModel(CM, RM),
                        getEffectiveCodeModel(CM, CodeModel::Medium), OL),
      TLOF(std::make_unique<TinyGPUTargetObjectFile>()) {
  // initAsmInfo will display features by llc -march=TinyGPU on 3.7
  initAsmInfo();
}

const TinyGPUSubtarget *
TinyGPUTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<TinyGPUSubtarget>(TargetTriple, CPU, FS, *this);
  }
  return I.get();
}

namespace {
class TinyGPUPassConfig : public TargetPassConfig {
public:
  TinyGPUPassConfig(TinyGPUTargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  TinyGPUTargetMachine &getTinyGPUTargetMachine() const {
    return getTM<TinyGPUTargetMachine>();
  }

  bool addInstSelector() override;
  void addPreEmitPass() override;
};
}

TargetPassConfig *TinyGPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new TinyGPUPassConfig(*this, PM);
}

// Install an instruction selector pass using
// the ISelDag to gen TinyGPU code.
bool TinyGPUPassConfig::addInstSelector() {
  addPass(createTinyGPUISelDag(getTinyGPUTargetMachine(), getOptLevel()));
  return false;
}

// Implemented by targets that want to run passes immediately before
// machine code is emitted. return true if -print-machineinstrs should
// print out the code after the passes.
void TinyGPUPassConfig::addPreEmitPass() {
}
