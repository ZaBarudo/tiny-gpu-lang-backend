//===-- TinyGPUInstrInfo.h - TinyGPU Instruction Information ----------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_TINYGPU_TINYGPUINSTRINFO_H
#define LLVM_LIB_TARGET_TINYGPU_TINYGPUINSTRINFO_H

#include "TinyGPU.h"
#include "TinyGPURegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "TinyGPUGenInstrInfo.inc"

namespace llvm {

class TinyGPUInstrInfo : public TinyGPUGenInstrInfo {
public:
  explicit TinyGPUInstrInfo(const TinyGPUSubtarget &STI);

protected:
  const TinyGPUSubtarget &Subtarget;
};
}

#endif // end LLVM_LIB_TARGET_TINYGPU_TINYGPUINSTRINFO_H
