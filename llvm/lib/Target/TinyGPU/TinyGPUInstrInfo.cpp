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
