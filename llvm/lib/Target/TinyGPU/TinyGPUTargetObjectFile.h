//===-- TinyGPUTargetObjectFile.h - TinyGPU Object Info -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

// This class represents the object file information specific to the TinyGPU target.
// It inherits from TargetLoweringObjectFileELF, which provides ELF-specific object file support.
class TinyGPUTargetObjectFile : public TargetLoweringObjectFileELF {
public:
  // Initializes the object file with the given MCContext and TargetMachine.
  // This function is overridden to provide target-specific initialization.
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};

} // end namespace llvm

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUTARGETOBJECTFILE_H
