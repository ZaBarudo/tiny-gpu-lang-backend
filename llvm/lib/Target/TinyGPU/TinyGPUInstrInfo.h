//===-- TinyGPUInstrInfo.h - TinyGPU Instruction Information ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TinyGPU implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPUINSTRINFO_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPUINSTRINFO_H

#include "TinyGPU.h"
#include "TinyGPURegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "TinyGPUGenInstrInfo.inc"

namespace llvm {

class TinyGPUInstrInfo : public TinyGPUGenInstrInfo {
public:
  // Constructor for TinyGPUInstrInfo, initializes with the given subtarget.
  explicit TinyGPUInstrInfo(const TinyGPUSubtarget &STI);

  // Copies the contents of a physical register to another physical register.
  // Parameters:
  // - MBB: The machine basic block where the instruction is inserted.
  // - MI: The iterator pointing to the insertion point in the basic block.
  // - DL: Debug location for the instruction.
  // - DestReg: The destination register.
  // - SrcReg: The source register.
  // - KillSrc: Indicates if the source register can be marked as killed.
  // - RenamableDest: Indicates if the destination register is renamable.
  // - RenamableSrc: Indicates if the source register is renamable.
  void copyPhysReg(MachineBasicBlock &MBB,
                   MachineBasicBlock::iterator MI, const DebugLoc &DL,
                   MCRegister DestReg, MCRegister SrcReg, bool KillSrc,
                   bool RenamableDest = false,
                   bool RenamableSrc = false) const;

protected:
  // Reference to the subtarget information for TinyGPU.
  const TinyGPUSubtarget &Subtarget;
};
}

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUINSTRINFO_H
