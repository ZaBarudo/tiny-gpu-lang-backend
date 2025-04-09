//===-- TinyGPUFrameLowering.cpp - TinyGPU Frame Information ------------------===//
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

#include "TinyGPUFrameLowering.h"
#include "TinyGPUSubtarget.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

// RoundUpToAlignment - This utility function rounds up the given value to the 
// nearest multiple of the specified alignment. It ensures that the value is 
// properly aligned according to the alignment requirement.
uint64_t RoundUpToAlignment(uint64_t value, unsigned alignment) {
  return (value + (alignment - 1)) & ~(alignment - 1);
}

// computeStackSize - This function calculates the total size of the stack
// required for the current function. It ensures that the stack size is
// properly aligned according to the target's stack alignment requirements.
uint64_t TinyGPUFrameLowering::computeStackSize(MachineFunction &MF) const {
  // Get the MachineFrameInfo object, which contains information about the stack frame.
  MachineFrameInfo *MFI = &MF.getFrameInfo();

  // Retrieve the current stack size from the MachineFrameInfo.
  uint64_t StackSize = MFI->getStackSize();

  // Get the required stack alignment for the target.
  unsigned StackAlign = getStackAlignment();

  // If the stack alignment is greater than 0, round up the stack size to the
  // nearest multiple of the alignment to ensure proper alignment.
  if (StackAlign > 0) {
    StackSize = RoundUpToAlignment(StackSize, StackAlign);
  }

  // Return the computed stack size.
  return StackSize;
}

// materializeOffset - This function generates instructions to materialize a 
// large offset value into a register. If the offset is small enough to fit 
// within an immediate value, it returns 0. Otherwise, it splits the offset 
// into lower and higher parts and materializes it into a register.
static unsigned materializeOffset(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, unsigned Offset) {
  // Retrieve the target instruction information for the current subtarget.
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();

  // Get the debug location of the current instruction if available, otherwise
  // use an empty debug location. This helps in associating debug information
  // with the generated instructions.
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Define the maximum immediate value that can be directly encoded.
  const uint64_t MaxSubImm = 0xfff;

  // If the offset is small enough to fit in an immediate, no need to materialize it.
  if (Offset <= MaxSubImm) {
    return 0;
  } else {
    // Use a temporary register to hold the offset value.
    unsigned OffsetReg = TinyGPU::R2;

    // Extract the lower 16 bits of the offset.
    unsigned OffsetLo = (unsigned)(Offset & 0xffff);

    // Extract the higher 16 bits of the offset.
    unsigned OffsetHi = (unsigned)((Offset & 0xffff0000) >> 16);

    // Materialize the lower part of the offset into the register.
    BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), OffsetReg)
        .addReg(OffsetReg) // Add the lower part of the offset.
        .setMIFlag(MachineInstr::FrameSetup);

    // If there is a higher part, materialize it as well.
    if (OffsetHi) {
      BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), OffsetReg)
          .addReg(OffsetReg) // Add the higher part of the offset.
          .setMIFlag(MachineInstr::FrameSetup);
    }

    // Return the register containing the materialized offset.
    return OffsetReg;
  }
}


// hasFPImpl - This function determines whether the current function requires
// a dedicated frame pointer (FP) register. For TinyGPU, this implementation
// always returns true, indicating that a frame pointer is always used.
bool TinyGPUFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  // Always return true to indicate that a frame pointer is required.
  return true;
}

// eliminateCallFramePseudoInstr - This function removes pseudo instructions
// related to call frame setup and teardown. These pseudo instructions are
// placeholders and do not correspond to actual machine instructions. They
// are eliminated during the frame lowering phase.
MachineBasicBlock::iterator TinyGPUFrameLowering::eliminateCallFramePseudoInstr(
                    MachineFunction &MF,
                    MachineBasicBlock &MBB,
                    MachineBasicBlock::iterator I) const {
  // Erase the pseudo instruction from the machine basic block.
  // The iterator pointing to the next instruction is returned.
  return MBB.erase(I);
}

// emitPrologue - This function generates the prologue code for the current
// function. The prologue is responsible for setting up the stack frame by
// adjusting the stack pointer (SP) to allocate space for local variables
// and saved registers.
void TinyGPUFrameLowering::emitPrologue(MachineFunction &MF,
                   MachineBasicBlock &MBB) const {
  // Retrieve the target instruction information for the current subtarget.
  const TargetInstrInfo &TII = *(MF.getSubtarget().getInstrInfo());

  // Get an iterator to the beginning of the machine basic block.
  MachineBasicBlock::iterator MBBI = MBB.begin();

  // Initialize a debug location object to associate debug information with
  // the generated instructions.
  DebugLoc dl;

  // Retrieve the size of the stack frame for the current function.
  int StackSize = (int)MF.getFrameInfo().getStackSize();

  // If the stack size is non-zero, adjust the stack pointer (SP) to allocate
  // the required space on the stack.
  if (StackSize != 0) {
  // Generate a CONST instruction to load the stack size into a temporary
  // register (TinyGPU::R0).
  auto ConstInst = BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::CONST))
    .addReg(TinyGPU::GPR) // Specify the destination register.
    .addImm(StackSize); // Add the immediate stack size value.

  // Retrieve the register that holds the result of the CONST instruction.
  auto resultRegister = ConstInst->getOperand(0).getReg();

  // Define the stack pointer register (TinyGPU::SP).
  unsigned StackReg = TinyGPU::SP;

  // Generate an ADDrr instruction to adjust the stack pointer by adding
  // the stack size (stored in the temporary register) to the current SP value.
  BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), StackReg)
    .addReg(StackReg) // Add the current stack pointer value.
    .addReg(resultRegister); // Add the stack size value.
  }
}

// emitEpilogue - This function generates the epilogue code for the current
// function. The epilogue is responsible for restoring the stack pointer (SP)
// to its original value, effectively deallocating the stack frame.
void TinyGPUFrameLowering::emitEpilogue(MachineFunction &MF,
                   MachineBasicBlock &MBB) const {
  // Retrieve the target instruction information for the current subtarget.
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();

  // Get an iterator to the last non-debug instruction in the machine basic block.
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();

  // Retrieve the debug location of the last instruction for associating debug info.
  DebugLoc dl = MBBI->getDebugLoc();

  // Compute the total size of the stack frame for the current function.
  uint64_t StackSize = computeStackSize(MF);

  // If the stack size is zero, there is no need to adjust the stack pointer.
  if (!StackSize) {
  return;
  }

  // Define the stack pointer register (TinyGPU::SP).
  unsigned StackReg = TinyGPU::SP;

  // Materialize the stack size offset into a register if it cannot fit in an immediate.
  unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, (unsigned)StackSize);

  // If the offset was materialized into a register, generate an ADDrr instruction
  // to restore the stack pointer by adding the offset to the current SP value.
  if (OffsetReg) {
  BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), StackReg)
    .addReg(StackReg) // Add the current stack pointer value.
    .addReg(OffsetReg) // Add the offset value.
    .setMIFlag(MachineInstr::FrameSetup); // Mark as part of frame setup.
  } else {
  // If the offset fits in an immediate, generate a CONST instruction to load
  // the stack size into a temporary register (TinyGPU::R0).
  auto ConstInst = BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::CONST))
    .addReg(TinyGPU::GPR) // Specify the destination register.
    .addImm(StackSize); // Add the immediate stack size value.

  // Retrieve the register that holds the result of the CONST instruction.
  auto resultRegister = ConstInst->getOperand(0).getReg();

  // Generate an ADDrr instruction to restore the stack pointer by adding
  // the stack size (stored in the temporary register) to the current SP value.
  BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), StackReg)
    .addReg(StackReg) // Add the current stack pointer value.
    .addReg(resultRegister); // Add the stack size value.
  }
}

// hasReservedCallFrame - This function determines whether the target reserves
// space in the stack frame for call frame adjustments. For TinyGPU, this
// implementation always returns true, indicating that the call frame is
// reserved.
bool TinyGPUFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  // Always return true to indicate that the call frame is reserved.
  return true;
}

// This method is called immediately before PrologEpilogInserter scans the
// physical registers used to determine what callee saved registers should be
// spilled. This method is optional.
// determineCalleeSaves - This function determines which callee-saved registers
// need to be saved and restored for the current function. It delegates the
// task to the base class implementation and can be overridden to add
// target-specific behavior.
void TinyGPUFrameLowering::determineCalleeSaves(MachineFunction &MF,
                        BitVector &SavedRegs,
                        RegScavenger *RS) const {
  // Call the base class implementation to handle the default behavior for
  // determining callee-saved registers.
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
}
