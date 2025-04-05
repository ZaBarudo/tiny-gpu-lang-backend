//===-- TinyGPURegisterInfo.cpp - TinyGPU Register Information
//----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TinyGPU implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "TinyGPURegisterInfo.h"
#include "TinyGPUSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Support/Debug.h"

#define GET_REGINFO_TARGET_DESC
#include "TinyGPUGenRegisterInfo.inc"

#define DEBUG_TYPE "TinyGPU-reginfo"

using namespace llvm;



// Constructor for TinyGPURegisterInfo
// Initializes the base class TinyGPUGenRegisterInfo with specific parameters
// - TinyGPU::R1: Specifies the Return Address Register (R1)
// - DwarfFlavour, EHFlavor, PC: Set to 0 as default values
// Also initializes the Subtarget member with the provided TinyGPUSubtarget

// Here R1 is used as the Return Address Register but do we need it?
// Not sure what the other initializations are

TinyGPURegisterInfo::TinyGPURegisterInfo(const TinyGPUSubtarget &ST)
  : TinyGPUGenRegisterInfo(TinyGPU::R1, /*DwarfFlavour*/ 0, /*EHFlavor*/ 0,
               /*PC*/ 0),
    Subtarget(ST) {}

// Returns the list of callee-saved registers for the TinyGPU target.
// These registers are preserved across function calls and must be saved
// and restored by the callee if they are modified.
const MCPhysReg *
TinyGPURegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  // Return the predefined list of callee-saved registers for TinyGPU.
  return TinyGPU_CalleeSavedRegs_SaveList;
}

// Returns the register class for integer registers of the specified size.
// For TinyGPU, all integer registers belong to the general-purpose register class (GPR).
const TargetRegisterClass *
TinyGPURegisterInfo::intRegClass(unsigned Size) const {
  // Return the general-purpose register class for TinyGPU.
  return &TinyGPU::GPRRegClass;
}

// Returns the call-preserved register mask for the TinyGPU target.
// This mask specifies which registers are preserved across function calls
// for a given calling convention. These registers do not need to be saved
// and restored by the caller.
const uint32_t *
TinyGPURegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                      CallingConv::ID) const {
  // Return the predefined call-preserved register mask for TinyGPU.
  return TinyGPU_CalleeSavedRegs_RegMask;
}

// Returns a BitVector indicating which registers are reserved for the TinyGPU target.
// Reserved registers cannot be used for general-purpose operations and are typically
// used for specific purposes such as stack pointers, zero registers, or special hardware
// functions.
BitVector
TinyGPURegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  // Create a BitVector with the size equal to the number of registers.
  BitVector Reserved(getNumRegs());
  
  // Mark R13, R14, and R15 as reserved registers.
  // These registers are reserved for specific purposes (e.g., zero registers).
  markSuperRegs(Reserved, TinyGPU::R13); // Reserve R13
  markSuperRegs(Reserved, TinyGPU::R14); // Reserve R14
  markSuperRegs(Reserved, TinyGPU::R15); // Reserve R15

  // Return the BitVector indicating reserved registers.
  return Reserved;
}

// This function eliminates frame indices in machine instructions by replacing
// them with actual stack pointer (SP) offsets. It is used during the frame
// index elimination phase of code generation.
//
// Parameters:
// - II: Iterator pointing to the current machine instruction.
// - SPAdj: Stack pointer adjustment (not used in this implementation).
// - FIOperandNum: The operand number of the frame index in the instruction.
// - RS: Register scavenger (not used in this implementation).
//
// Returns:
// - false: Indicates that no further processing is required.

bool TinyGPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                        int SPAdj, unsigned FIOperandNum,
                        RegScavenger *RS) const {
  // Get the current machine instruction.
  MachineInstr &MI = *II;

  // Get the previous machine instruction (assumes II is not the first instruction).
  MachineInstr &prevMI = *std::prev(II);

  // Retrieve the frame index from the specified operand.
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

  // Calculate the offset from the stack pointer (SP) for the frame index.
  MachineFunction &MF = *MI.getParent()->getParent();
  const TargetFrameLowering *TFI = Subtarget.getFrameLowering();
  int Offset = MF.getFrameInfo().getObjectOffset(FrameIndex);

  // Replace the frame index operand with the stack pointer (SP).
  MI.getOperand(FIOperandNum).ChangeToRegister(TinyGPU::SP, false);

  // Update the immediate operand in the previous instruction with the calculated offset.
  prevMI.getOperand(1).ChangeToImmediate(Offset);

  // Return false to indicate no further processing is required.
  return false;
}

/// Determines whether register scavenging is required for the target.
/// 
/// Register scavenging is a process used to allocate additional registers
/// during code generation when the register allocator runs out of available
/// registers. This function indicates whether the TinyGPU target requires
/// this mechanism.
///
/// @param MF The machine function being compiled.
/// @return Always returns true, indicating that register scavenging is
///         required for the TinyGPU target.
bool TinyGPURegisterInfo::requiresRegisterScavenging(
  const MachineFunction &MF) const {
  // Always return true as the TinyGPU target requires register scavenging.
  return true;
}

bool TinyGPURegisterInfo::requiresFrameIndexScavenging(
    const MachineFunction &MF) const {
  return true;
}


/// Determines whether frame index replacement scavenging is required for the target.
///
/// Frame index replacement scavenging is a process used to allocate additional
/// registers during the replacement of frame indices with actual stack pointer
/// offsets. This function indicates whether the TinyGPU target requires this mechanism.
///
/// @param MF The machine function being compiled.
/// @return Always returns true, indicating that frame index replacement scavenging
///         is required for the TinyGPU target.
bool TinyGPURegisterInfo::requiresFrameIndexReplacementScavenging(
  const MachineFunction &MF) const {
  // Always return true as the TinyGPU target requires frame index replacement scavenging.
  return true;
}

/// Determines whether liveness tracking is required after register allocation.
///
/// Liveness tracking after register allocation is used to determine which
/// registers are live at specific points in the code after the register
/// allocation phase. This function indicates whether the TinyGPU target
/// requires this mechanism.
///
/// @param MF The machine function being compiled.
/// @return Always returns true, indicating that liveness tracking after
///         register allocation is required for the TinyGPU target.
bool TinyGPURegisterInfo::trackLivenessAfterRegAlloc(
  const MachineFunction &MF) const {
  // Always return true as the TinyGPU target requires liveness tracking
  // after the register allocation phase.
  return true;
}

/// Returns the frame register for the TinyGPU target.
///
/// The frame register is used as a base register for accessing stack frame
/// variables. For the TinyGPU target, the stack pointer (SP) is used as the
/// frame register.
///
/// @param MF The machine function being compiled.
/// @return The stack pointer (SP) register as the frame register.
Register
TinyGPURegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  // Return the stack pointer (SP) register as the frame register.
  return TinyGPU::SP;
}
