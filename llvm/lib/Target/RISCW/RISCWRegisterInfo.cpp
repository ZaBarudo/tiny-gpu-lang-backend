//===-- RISCWRegisterInfo.cpp - RISCW Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RISCW implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "RISCWRegisterInfo.h"
#include "RISCWSubtarget.h"
#include "llvm/Support/Debug.h"

#define GET_REGINFO_TARGET_DESC
#include "RISCWGenRegisterInfo.inc"

#define DEBUG_TYPE "riscw-reginfo"

using namespace llvm;

RISCWRegisterInfo::RISCWRegisterInfo(const RISCWSubtarget &ST)
  : RISCWGenRegisterInfo(RISCW::X1, /*DwarfFlavour*/0, /*EHFlavor*/0,
                         /*PC*/0), Subtarget(ST) {}

const MCPhysReg *
RISCWRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return RISCW_CalleeSavedRegs_SaveList;
}

const TargetRegisterClass *RISCWRegisterInfo::intRegClass(unsigned Size) const {
  return &RISCW::GPRRegClass;
}

const uint32_t *
RISCWRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID) const {
  return RISCW_CalleeSavedRegs_RegMask;
}

BitVector RISCWRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  markSuperRegs(Reserved, RISCW::X0); // zero
  markSuperRegs(Reserved, RISCW::X2); // sp
  markSuperRegs(Reserved, RISCW::X3); // gp
  markSuperRegs(Reserved, RISCW::X4); // tp

  return Reserved;
}

void RISCWRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                           int SPAdj,
                                           unsigned FIOperandNum,
                                           RegScavenger *RS) const {
  llvm_unreachable("Unsupported eliminateFrameIndex");
}

bool
RISCWRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
RISCWRegisterInfo::requiresFrameIndexScavenging(
                                            const MachineFunction &MF) const {
  return true;
}

bool
RISCWRegisterInfo::requiresFrameIndexReplacementScavenging(
                                            const MachineFunction &MF) const {
  return true;
}

bool
RISCWRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

Register RISCWRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  llvm_unreachable("Unsupported getFrameRegister");
}

