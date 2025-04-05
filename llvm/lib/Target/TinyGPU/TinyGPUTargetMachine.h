//===-- TinyGPUTargetMachine.h - Define TargetMachine for TinyGPU ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TinyGPU specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETMACHINE_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETMACHINE_H

#include "TinyGPUSubtarget.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

namespace llvm {

// TinyGPUTargetMachine is a subclass of CodeGenTargetMachineImpl, which
// represents the target-specific machine implementation for the TinyGPU target.
class TinyGPUTargetMachine : public CodeGenTargetMachineImpl {
  // TargetLoweringObjectFile is used to describe how object files are
  // generated for this target.
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

  // A map to store subtarget information for different functions.
  mutable StringMap<std::unique_ptr<TinyGPUSubtarget>> SubtargetMap;

public:
  // Constructor for TinyGPUTargetMachine. It initializes the target machine
  // with the given parameters such as target, triple, CPU, feature string,
  // options, relocation model, code model, optimization level, and JIT flag.
  TinyGPUTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options,
                    std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
                    CodeGenOptLevel OL, bool JIT);

  // Returns the subtarget implementation for a specific function.
  const TinyGPUSubtarget *getSubtargetImpl(const Function &F) const override;

  // Deleted function to prevent calling getSubtargetImpl without arguments.
  const TinyGPUSubtarget *getSubtargetImpl() const = delete;

  // Creates and returns a target-specific pass configuration.
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  // Returns the object file lowering implementation for this target.
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};

} // namespace llvm

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETMACHINE_H
