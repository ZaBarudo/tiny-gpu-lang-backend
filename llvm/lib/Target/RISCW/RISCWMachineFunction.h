//=== RISCWMachineFunctionInfo.h - Private data used for RISCW ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the RISCW specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCW_RISCWMACHINEFUNCTION_H
#define LLVM_LIB_TARGET_RISCW_RISCWMACHINEFUNCTION_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// RISCWFunctionInfo - This class is derived from MachineFunction private
/// RISCW target-specific information for each MachineFunction.
class RISCWFunctionInfo : public MachineFunctionInfo {
private:
  MachineFunction &MF;

public:
  RISCWFunctionInfo(MachineFunction &MF) : MF(MF) {}
};

} // end of namespace llvm

#endif // end LLVM_LIB_TARGET_RISCW_RISCWMACHINEFUNCTION_H
