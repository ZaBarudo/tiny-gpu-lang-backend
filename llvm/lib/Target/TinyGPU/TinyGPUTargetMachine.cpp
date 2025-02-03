//===-- TinyGPUTargetMachine.cpp - Define TargetMachine for TinyGPU -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "TinyGPUTargetMachine.h"
#include "LeonPasses.h"
#include "TinyGPU.h"
#include "TinyGPUMachineFunctionInfo.h"
#include "TinyGPUTargetObjectFile.h"
#include "TargetInfo/TinyGPUTargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include <optional>
using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTinyGPUTarget() {
  // Register the target.
  RegisterTargetMachine<TinyGPUV8TargetMachine> X(getTheTinyGPUTarget());
  RegisterTargetMachine<TinyGPUV9TargetMachine> Y(getTheTinyGPUV9Target());
  RegisterTargetMachine<TinyGPUelTargetMachine> Z(getTheTinyGPUelTarget());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeTinyGPUDAGToDAGISelLegacyPass(PR);
  initializeErrataWorkaroundPass(PR);
}

static cl::opt<bool>
    BranchRelaxation("TinyGPU-enable-branch-relax", cl::Hidden, cl::init(true),
                     cl::desc("Relax out of range conditional branches"));

static std::string computeDataLayout(const Triple &T, bool is64Bit) {
  // TinyGPU is typically big endian, but some are little.
  std::string Ret = T.getArch() == Triple::TinyGPUel ? "e" : "E";
  Ret += "-m:e";

  // Some ABIs have 32bit pointers.
  if (!is64Bit)
    Ret += "-p:32:32";

  // Alignments for 64 bit integers.
  Ret += "-i64:64";

  // Alignments for 128 bit integers.
  // This is not specified in the ABI document but is the de facto standard.
  Ret += "-i128:128";

  // On TinyGPUV9 128 floats are aligned to 128 bits, on others only to 64.
  // On TinyGPUV9 registers can hold 64 or 32 bits, on others only 32.
  if (is64Bit)
    Ret += "-n32:64";
  else
    Ret += "-f128:64-n32";

  if (is64Bit)
    Ret += "-S128";
  else
    Ret += "-S64";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

// Code models. Some only make sense for 64-bit code.
//
// SunCC  Reloc   CodeModel  Constraints
// abs32  Static  Small      text+data+bss linked below 2^32 bytes
// abs44  Static  Medium     text+data+bss linked below 2^44 bytes
// abs64  Static  Large      text smaller than 2^31 bytes
// pic13  PIC_    Small      GOT < 2^13 bytes
// pic32  PIC_    Medium     GOT < 2^32 bytes
//
// All code models require that the text segment is smaller than 2GB.
static CodeModel::Model
getEffectiveTinyGPUCodeModel(std::optional<CodeModel::Model> CM, Reloc::Model RM,
                           bool Is64Bit, bool JIT) {
  if (CM) {
    if (*CM == CodeModel::Tiny)
      report_fatal_error("Target does not support the tiny CodeModel", false);
    if (*CM == CodeModel::Kernel)
      report_fatal_error("Target does not support the kernel CodeModel", false);
    return *CM;
  }
  if (Is64Bit) {
    if (JIT)
      return CodeModel::Large;
    return RM == Reloc::PIC_ ? CodeModel::Small : CodeModel::Medium;
  }
  return CodeModel::Small;
}

/// Create an ILP32 architecture model
TinyGPUTargetMachine::TinyGPUTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT,
                                       bool is64bit)
    : CodeGenTargetMachineImpl(
          T, computeDataLayout(TT, is64bit), TT, CPU, FS, Options,
          getEffectiveRelocModel(RM),
          getEffectiveTinyGPUCodeModel(CM, getEffectiveRelocModel(RM), is64bit,
                                     JIT),
          OL),
      TLOF(std::make_unique<TinyGPUELFTargetObjectFile>()), is64Bit(is64bit) {
  initAsmInfo();
}

TinyGPUTargetMachine::~TinyGPUTargetMachine() = default;

const TinyGPUSubtarget *
TinyGPUTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute TuneAttr = F.getFnAttribute("tune-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU =
      CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  std::string TuneCPU =
      TuneAttr.isValid() ? TuneAttr.getValueAsString().str() : CPU;
  std::string FS =
      FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

  // FIXME: This is related to the code below to reset the target options,
  // we need to know whether or not the soft float flag is set on the
  // function, so we can enable it as a subtarget feature.
  bool softFloat = F.getFnAttribute("use-soft-float").getValueAsBool();

  if (softFloat)
    FS += FS.empty() ? "+soft-float" : ",+soft-float";

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<TinyGPUSubtarget>(CPU, TuneCPU, FS, *this,
                                         this->is64Bit);
  }
  return I.get();
}

MachineFunctionInfo *TinyGPUTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return TinyGPUMachineFunctionInfo::create<TinyGPUMachineFunctionInfo>(Allocator,
                                                                    F, STI);
}

namespace {
/// TinyGPU Code Generator Pass Configuration Options.
class TinyGPUPassConfig : public TargetPassConfig {
public:
  TinyGPUPassConfig(TinyGPUTargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  TinyGPUTargetMachine &getTinyGPUTargetMachine() const {
    return getTM<TinyGPUTargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *TinyGPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new TinyGPUPassConfig(*this, PM);
}

void TinyGPUPassConfig::addIRPasses() {
  addPass(createAtomicExpandLegacyPass());

  TargetPassConfig::addIRPasses();
}

bool TinyGPUPassConfig::addInstSelector() {
  addPass(createTinyGPUISelDag(getTinyGPUTargetMachine()));
  return false;
}

void TinyGPUPassConfig::addPreEmitPass(){
  if (BranchRelaxation)
    addPass(&BranchRelaxationPassID);

  addPass(createTinyGPUDelaySlotFillerPass());
  addPass(new InsertNOPLoad());
  addPass(new DetectRoundChange());
  addPass(new FixAllFDIVSQRT());
  addPass(new ErrataWorkaround());
}

void TinyGPUV8TargetMachine::anchor() { }

TinyGPUV8TargetMachine::TinyGPUV8TargetMachine(const Target &T, const Triple &TT,
                                           StringRef CPU, StringRef FS,
                                           const TargetOptions &Options,
                                           std::optional<Reloc::Model> RM,
                                           std::optional<CodeModel::Model> CM,
                                           CodeGenOptLevel OL, bool JIT)
    : TinyGPUTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}

void TinyGPUV9TargetMachine::anchor() { }

TinyGPUV9TargetMachine::TinyGPUV9TargetMachine(const Target &T, const Triple &TT,
                                           StringRef CPU, StringRef FS,
                                           const TargetOptions &Options,
                                           std::optional<Reloc::Model> RM,
                                           std::optional<CodeModel::Model> CM,
                                           CodeGenOptLevel OL, bool JIT)
    : TinyGPUTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, true) {}

void TinyGPUelTargetMachine::anchor() {}

TinyGPUelTargetMachine::TinyGPUelTargetMachine(const Target &T, const Triple &TT,
                                           StringRef CPU, StringRef FS,
                                           const TargetOptions &Options,
                                           std::optional<Reloc::Model> RM,
                                           std::optional<CodeModel::Model> CM,
                                           CodeGenOptLevel OL, bool JIT)
    : TinyGPUTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}
