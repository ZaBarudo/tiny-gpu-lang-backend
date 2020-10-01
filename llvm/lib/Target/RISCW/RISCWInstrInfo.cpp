//===-- RISCWInstrInfo.cpp - RISCW Instruction Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RISCW implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "RISCWInstrInfo.h"

#include "RISCWTargetMachine.h"
#include "RISCWMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "riscw-instrinfo"

#define GET_INSTRINFO_CTOR_DTOR
#include "RISCWGenInstrInfo.inc"

RISCWInstrInfo::RISCWInstrInfo(const RISCWSubtarget &STI)
    : RISCWGenInstrInfo(RISCW::ADJCALLSTACKDOWN, RISCW::ADJCALLSTACKUP),
      Subtarget(STI)
{
}
