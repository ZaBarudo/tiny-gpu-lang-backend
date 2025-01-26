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
class TinyGPUTargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  mutable StringMap<std::unique_ptr<TinyGPUSubtarget>> SubtargetMap;

public:
  TinyGPUTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options,
                    std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
                    CodeGenOptLevel OL, bool JIT);

  const TinyGPUSubtarget *getSubtargetImpl(const Function &F) const override;
  const TinyGPUSubtarget *getSubtargetImpl() const = delete;

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};
}

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETMACHINE_H
