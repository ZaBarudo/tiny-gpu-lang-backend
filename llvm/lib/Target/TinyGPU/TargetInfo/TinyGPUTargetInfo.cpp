//===-- TinyGPUTargetInfo.cpp - TinyGPU Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/TinyGPUTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheTinyGPUTarget() {
  static Target TheTinyGPUTarget;
  return TheTinyGPUTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTinyGPUTargetInfo() {
  RegisterTarget<Triple::tinygpu> X(getTheTinyGPUTarget(), "tinygpu",
                                  "32-bit RISC-V", "TinyGPU");
}
