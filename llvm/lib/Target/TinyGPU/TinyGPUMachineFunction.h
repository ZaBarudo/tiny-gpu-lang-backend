//=== TinyGPUMachineFunctionInfo.h - Private data used for TinyGPU ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TinyGPU specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPUMACHINEFUNCTION_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPUMACHINEFUNCTION_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// TinyGPUFunctionInfo - This class is derived from MachineFunction private
/// TinyGPU target-specific information for each MachineFunction.
class TinyGPUFunctionInfo : public MachineFunctionInfo {
private:
  MachineFunction &MF;

public:
  TinyGPUFunctionInfo(MachineFunction &MF) : MF(MF) {}
};

} // end of namespace llvm

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUMACHINEFUNCTION_H
