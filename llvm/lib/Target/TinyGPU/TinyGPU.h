//=== TinyGPU.h - Top-level interface for TinyGPU representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM TinyGPU backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPU_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPU_H

#include "MCTargetDesc/TinyGPUMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class FunctionPass;
  class PassRegistry;
  class TinyGPUTargetMachine;

  void initializeTinyGPUDAGToDAGISelLegacyPass(PassRegistry &);

  FunctionPass *createTinyGPUISelDag(TinyGPUTargetMachine &TM,
                                   CodeGenOptLevel OptLevel);
} // end namespace llvm;

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPU_H
