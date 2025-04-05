//===-- TinyGPUFrameLowering.h - Define frame lowering for TinyGPU ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TinyGPUTargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPUFRAMELOWERING_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPUFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
  class TinyGPUSubtarget;

// This class defines the frame lowering for the TinyGPU target.
// It handles stack frame setup, teardown, and related operations.
class TinyGPUFrameLowering : public TargetFrameLowering {
protected:
  // Reference to the subtarget information for TinyGPU.
  const TinyGPUSubtarget &STI;

public:
  // Constructor for TinyGPUFrameLowering.
  // Initializes the frame lowering with stack growth direction, alignment, and offsets.
  explicit TinyGPUFrameLowering(const TinyGPUSubtarget &STI)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown,
                          /*StackAlignment*/Align(4), // Stack alignment is 4 bytes.
                          /*LocalAreaOffset*/0,      // No local area offset.
                          /*TransAl*/Align(4)),      // Transition alignment is 4 bytes.
      STI(STI) {}

  // Emits the prologue for a function.
  // This sets up the stack frame at the beginning of the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  // Emits the epilogue for a function.
  // This tears down the stack frame at the end of the function.
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  // Determines if the function has a reserved call frame.
  // Returns true if the call frame is reserved, false otherwise.
  bool hasReservedCallFrame(const MachineFunction &MF) const override;

  // Eliminates pseudo instructions for call frame setup/teardown.
  // This replaces pseudo instructions with actual machine instructions.
  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  // Determines which callee-saved registers need to be saved.
  // Updates the SavedRegs bit vector and optionally uses a register scavenger.
  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  // Computes the total stack size for the function.
  // Returns the size of the stack in bytes.
  uint64_t computeStackSize(MachineFunction &MF) const;

private:
  // Determines if the function has a frame pointer.
  // Returns true if a frame pointer is used, false otherwise.
  bool hasFPImpl(const MachineFunction &MF) const override;
};
} // end llvm namespace

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUFRAMELOWERING_H
