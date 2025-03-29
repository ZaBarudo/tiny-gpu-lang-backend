//=== TinyGPU.td - Describe the TinyGPU Target Machine ---------*- tablegen -*-===//
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

include "TinyGPUSchedule.td"
include "TinyGPURegisterInfo.td"
include "TinyGPUCallingConv.td"
include "TinyGPUInstrInfo.td"

def TinyGPUInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// TinyGPU processors supported
//===----------------------------------------------------------------------===//

def : ProcessorModel<"generic",  NoSchedModel, []>;

//===----------------------------------------------------------------------===//
// TinyGPU Target declaration
//===----------------------------------------------------------------------===//

def TinyGPU : Target {
  let InstructionSet = TinyGPUInstrInfo;
}
