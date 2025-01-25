//===-- TinyGPUMCTargetDesc.h - TinyGPU Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides TinyGPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TINYGPU_MCTARGETDESC_TINYGPUMCTARGETDESC_H
#define LLVM_LIB_TARGET_TINYGPU_MCTARGETDESC_TINYGPUMCTARGETDESC_H

// Defines symbolic names for TinyGPU registers. This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "TinyGPUGenRegisterInfo.inc"

// Defines symbolic names for the TinyGPU instructions.
#define GET_INSTRINFO_ENUM
#include "TinyGPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "TinyGPUGenSubtargetInfo.inc"

#endif // end LLVM_LIB_TARGET_TINYGPU_MCTARGETDESC_TINYGPUMCTARGETDESC_H
