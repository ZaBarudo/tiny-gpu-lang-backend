//===-- TinyGPURegisterInfo.cpp - TinyGPU Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TinyGPU implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "TinyGPURegisterInfo.h"
#include "TinyGPUSubtarget.h"
#include "llvm/Support/Debug.h"

#define GET_REGINFO_TARGET_DESC
#include "TinyGPUGenRegisterInfo.inc"

#define DEBUG_TYPE "tinygpu-reginfo"

using namespace llvm;

TinyGPURegisterInfo::TinyGPURegisterInfo(const TinyGPUSubtarget &ST)
  : TinyGPUGenRegisterInfo(TinyGPU::R1, /*DwarfFlavour*/0, /*EHFlavor*/0,
                         /*PC*/0), Subtarget(ST) {}

const MCPhysReg *
TinyGPURegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return TinyGPU_CalleeSavedRegs_SaveList;
}

const TargetRegisterClass *TinyGPURegisterInfo::intRegClass(unsigned Size) const {
  return &TinyGPU::GPRRegClass;
}

const uint32_t *
TinyGPURegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID) const {
  return TinyGPU_CalleeSavedRegs_RegMask;
}

BitVector TinyGPURegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  markSuperRegs(Reserved, TinyGPU::R0); // zero
  markSuperRegs(Reserved, TinyGPU::R2); // sp
  markSuperRegs(Reserved, TinyGPU::R3); // gp
  markSuperRegs(Reserved, TinyGPU::R4); // tp

  return Reserved;
}

void TinyGPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                           int SPAdj,
                                           unsigned FIOperandNum,
                                           RegScavenger *RS) const {
  llvm_unreachable("Unsupported eliminateFrameIndex");
}

bool
TinyGPURegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
TinyGPURegisterInfo::requiresFrameIndexScavenging(
                                            const MachineFunction &MF) const {
  return true;
}

bool
TinyGPURegisterInfo::requiresFrameIndexReplacementScavenging(
                                            const MachineFunction &MF) const {
  return true;
}

bool
TinyGPURegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

Register TinyGPURegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  llvm_unreachable("Unsupported getFrameRegister");
}

