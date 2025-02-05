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

uint64_t RoundUpToAlignment(uint64_t value, unsigned alignment) {
    return (value + (alignment - 1)) & ~(alignment - 1);
}

uint64_t TinyGPUFrameLowering::computeStackSize(MachineFunction &MF) const {
  MachineFrameInfo *MFI = &MF.getFrameInfo();
  uint64_t StackSize = MFI->getStackSize();
  unsigned StackAlign = getStackAlignment();
  if (StackAlign > 0) {
    StackSize = RoundUpToAlignment(StackSize, StackAlign);
  }
  return StackSize;
}

static unsigned materializeOffset(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, unsigned Offset) {
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  const uint64_t MaxSubImm = 0xfff;
  if (Offset <= MaxSubImm) {
    return 0;
  } else {
    unsigned OffsetReg = TinyGPU::R2;
    unsigned OffsetLo = (unsigned)(Offset & 0xffff);
    unsigned OffsetHi = (unsigned)((Offset & 0xffff0000) >> 16);
    BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), OffsetReg)
        .addReg(OffsetReg)
        .setMIFlag(MachineInstr::FrameSetup);
    if (OffsetHi) {
      BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), OffsetReg)
          .addReg(OffsetReg)
          .setMIFlag(MachineInstr::FrameSetup);
    }
    return OffsetReg;
  }
}

// hasFP - Return true if the specified function should have a dedicated frame
// pointer register.  This is true if the function has variable sized allocas,
// if it needs dynamic stack realignment, if frame pointer elimination is
// disabled, or if the frame address is taken.
bool TinyGPUFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  return true;
}

MachineBasicBlock::iterator TinyGPUFrameLowering::eliminateCallFramePseudoInstr(
                                        MachineFunction &MF,
                                        MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator I) const {
  return MBB.erase(I);
}

void TinyGPUFrameLowering::emitPrologue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
  const TargetInstrInfo &TII = * (MF.getSubtarget().getInstrInfo());
  // MachineBasicBlock &MBB = MF.front();
  // MachineBasicBlock::iterator MBBI = MBB.begin();
  // DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  // uint64_t StackSize = computeStackSize(MF);
  // if (!StackSize) {
  //   return;
  // }
  // unsigned StackReg = TinyGPU::SP;
  // unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, (unsigned)StackSize);
  // if (OffsetReg) {
  //   BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::SUBrr), StackReg)
  //       .addReg(StackReg)
  //       .addReg(OffsetReg)
  //       .setMIFlag(MachineInstr::FrameSetup);
  // } else {
  //   BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::SUBri), StackReg)
  //       .addReg(StackReg)
  //       .addImm(StackSize)
  //       .setMIFlag(MachineInstr::FrameSetup);                          
  // }
  MachineBasicBlock::iterator MBBI = MBB.begin();
  // const MyTargetInstrInfo &TII = *Subtarget.getInstrInfo();
  DebugLoc dl;

  // Adjust SP by stack size
  int StackSize = (int)MF.getFrameInfo().getStackSize();
  if (StackSize != 0) {
    BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDI))
        .addReg(TinyGPU::SP)
        .addReg(TinyGPU::SP)
        .addImm(-StackSize);
  }
}

void TinyGPUFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  DebugLoc dl = MBBI->getDebugLoc();
  uint64_t StackSize = computeStackSize(MF);
  if (!StackSize) {
    return;
  }
  unsigned StackReg = TinyGPU::SP;
  unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, (unsigned)StackSize);
  if (OffsetReg) {
    BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDrr), StackReg)
        .addReg(StackReg)
        .addReg(OffsetReg)
        .setMIFlag(MachineInstr::FrameSetup);
  } else {
    BuildMI(MBB, MBBI, dl, TII.get(TinyGPU::ADDri), StackReg)
        .addReg(StackReg)
        .addImm(StackSize)
        .setMIFlag(MachineInstr::FrameSetup);
  }
}

bool
TinyGPUFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  return true;
}

// This method is called immediately before PrologEpilogInserter scans the
// physical registers used to determine what callee saved registers should be
// spilled. This method is optional.
void TinyGPUFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                              BitVector &SavedRegs,
                                              RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
}
