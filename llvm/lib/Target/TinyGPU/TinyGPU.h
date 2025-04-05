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

// ===----------------------------------------------------------------------===//
// TinyGPU - This is the top-level class for the TinyGPU target machine.
// ===----------------------------------------------------------------------===//

namespace llvm {
  // Forward declaration of the FunctionPass class, which represents a pass
  // that operates on LLVM functions.
  class FunctionPass;

  // Forward declaration of the PassRegistry class, which is used to register
  // and initialize passes in LLVM.
  class PassRegistry;

  // Forward declaration of the TinyGPUTargetMachine class, which represents
  // the target machine for the TinyGPU backend.
  class TinyGPUTargetMachine;

  // Function to initialize the TinyGPU DAG-to-DAG instruction selection pass
  // in the LLVM pass registry. This is typically used to set up the pass
  // pipeline for the TinyGPU backend.
  void initializeTinyGPUDAGToDAGISelLegacyPass(PassRegistry &);

  // Factory function to create a DAG-to-DAG instruction selection pass
  // for the TinyGPU target. This pass is responsible for converting
  // the LLVM intermediate representation (IR) into target-specific
  // instructions for the TinyGPU backend.
  FunctionPass *createTinyGPUISelDag(TinyGPUTargetMachine &TM,
                                   CodeGenOptLevel OptLevel);
} // end namespace llvm;

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPU_H
