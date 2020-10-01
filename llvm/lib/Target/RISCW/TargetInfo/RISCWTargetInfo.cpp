//===-- RISCWTargetInfo.cpp - RISCW Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/RISCWTargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheRISCWTarget() {
  static Target TheRISCWTarget;
  return TheRISCWTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRISCWTargetInfo() {
  RegisterTarget<Triple::riscw> X(getTheRISCWTarget(), "riscw",
                                  "32-bit RISC-V", "RISCW");
}
