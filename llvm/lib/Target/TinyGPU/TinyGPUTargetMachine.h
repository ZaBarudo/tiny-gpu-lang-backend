//===-- TinyGPUTargetMachine.h - Define TargetMachine for TinyGPU ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the TinyGPU specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETMACHINE_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETMACHINE_H

#include "TinyGPUInstrInfo.h"
#include "TinyGPUSubtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

class TinyGPUTargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  bool is64Bit;
  mutable StringMap<std::unique_ptr<TinyGPUSubtarget>> SubtargetMap;

public:
  TinyGPUTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT, bool is64bit);
  ~TinyGPUTargetMachine() override;

  const TinyGPUSubtarget *getSubtargetImpl(const Function &F) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};

/// TinyGPU 32-bit target machine
///
class TinyGPUV8TargetMachine : public TinyGPUTargetMachine {
  virtual void anchor();

public:
  TinyGPUV8TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                       StringRef FS, const TargetOptions &Options,
                       std::optional<Reloc::Model> RM,
                       std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                       bool JIT);
};

/// TinyGPU 64-bit target machine
///
class TinyGPUV9TargetMachine : public TinyGPUTargetMachine {
  virtual void anchor();

public:
  TinyGPUV9TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                       StringRef FS, const TargetOptions &Options,
                       std::optional<Reloc::Model> RM,
                       std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                       bool JIT);
};

class TinyGPUelTargetMachine : public TinyGPUTargetMachine {
  virtual void anchor();

public:
  TinyGPUelTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                       StringRef FS, const TargetOptions &Options,
                       std::optional<Reloc::Model> RM,
                       std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                       bool JIT);
};

} // end namespace llvm

#endif
