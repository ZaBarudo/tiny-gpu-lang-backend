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

TinyGPUInstrInfo::TinyGPUInstrInfo(const TinyGPUSubtarget &STI)
    : TinyGPUGenInstrInfo(TinyGPU::ADJCALLSTACKDOWN, TinyGPU::ADJCALLSTACKUP),
      Subtarget(STI)
{
}

void TinyGPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, const DebugLoc &DL,
                           MCRegister DestReg, MCRegister SrcReg, bool KillSrc,
                           bool RenamableDest,
                           bool RenamableSrc)  const {
  // Use a move instruction to copy SrcReg to DestReg
  // Replace `TinyGPU::MOV` with the actual move instruction for your target
  BuildMI(MBB, MI, DL, get(TinyGPU::STR), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
}
