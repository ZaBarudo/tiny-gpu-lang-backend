//=== RISCW.td - Describe the RISCW Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Top tablegen file that pulls in code from the LLVM framework along with
// code specific to this target.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Calling Conv, Instruction Descriptions, Register information, etc
//===----------------------------------------------------------------------===//

include "RISCWSchedule.td"
include "RISCWRegisterInfo.td"
include "RISCWCallingConv.td"
include "RISCWInstrInfo.td"

def RISCWInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// RISCW processors supported
//===----------------------------------------------------------------------===//

def : ProcessorModel<"generic",  NoSchedModel, []>;

//===----------------------------------------------------------------------===//
// RISCW Target declaration
//===----------------------------------------------------------------------===//

def RISCW : Target {
  let InstructionSet = RISCWInstrInfo;
}
