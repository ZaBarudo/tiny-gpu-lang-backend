//===-- TinyGPUTargetObjectFile.cpp - TinyGPU Object Files --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TinyGPUTargetObjectFile.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

void
TinyGPUTargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM) {
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
}
