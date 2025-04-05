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

/// TinyGPUFunctionInfo - This class is derived from MachineFunctionInfo and
/// holds TinyGPU-specific information for each MachineFunction. It provides
/// a way to store and access target-specific data associated with a 
/// MachineFunction.
class TinyGPUFunctionInfo : public MachineFunctionInfo {
private:
  /// Reference to the MachineFunction instance associated with this
  /// TinyGPUFunctionInfo. This allows access to the MachineFunction's
  /// properties and methods.
  MachineFunction &MF;

public:
  /// Constructor - Initializes the TinyGPUFunctionInfo with a reference
  /// to the associated MachineFunction.
  TinyGPUFunctionInfo(MachineFunction &MF) : MF(MF) {}
};

} // end of namespace llvm

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUMACHINEFUNCTION_H
