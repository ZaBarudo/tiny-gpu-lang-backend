//===-- TinyGPUInstrInfo.cpp - TinyGPU Instruction Information ----------------===//
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

#include "TinyGPUInstrInfo.h"

#include "TinyGPUTargetMachine.h"
#include "TinyGPUMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "TinyGPU-instrinfo"

#define GET_INSTRINFO_CTOR_DTOR
#include "TinyGPUGenInstrInfo.inc"

// Constructor for the TinyGPUInstrInfo class.
// Initializes the base class TinyGPUGenInstrInfo with the stack adjustment opcodes
// and stores a reference to the TinyGPUSubtarget.
TinyGPUInstrInfo::TinyGPUInstrInfo(const TinyGPUSubtarget &STI)
  : TinyGPUGenInstrInfo(TinyGPU::ADJCALLSTACKDOWN, TinyGPU::ADJCALLSTACKUP), // Initialize base class with stack adjustment opcodes
    Subtarget(STI) // Store the subtarget reference
{
}

// This function copies the value from a source physical register (SrcReg)
// to a destination physical register (DestReg) using a move instruction.
// It is part of the TinyGPUInstrInfo class and overrides the base class method.
void TinyGPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
               MachineBasicBlock::iterator MI, const DebugLoc &DL,
               MCRegister DestReg, MCRegister SrcReg, bool KillSrc,
               bool RenamableDest,
               bool RenamableSrc)  const {
  // Emit a move instruction (TinyGPU) to copy the value from SrcReg to DestReg.
  // The `BuildMI` function is used to construct the machine instruction.
  // - `MBB`: The machine basic block where the instruction is inserted.
  // - `MI`: The position in the block where the instruction is inserted.
  // - `DL`: Debug location for the instruction.
  // - `get(TinyGPU::STR)`: Retrieves the move instruction for the TinyGPU target.
  // - `DestReg`: The destination register to which the value is copied.
  // - `SrcReg`: The source register from which the value is copied.
  // - `getKillRegState(KillSrc)`: Indicates whether the source register is killed after use.
  BuildMI(MBB, MI, DL, get(TinyGPU::STR), DestReg)
    .addReg(SrcReg, getKillRegState(KillSrc)); // Add the source register with kill state.
}
