//===-- TinyGPUMCTargetDesc.cpp - TinyGPU Target Descriptions -----------------===//
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

#include "TinyGPUMCTargetDesc.h"
#include "TinyGPUInstPrinter.h"
#include "TinyGPUMCAsmInfo.h"
#include "TargetInfo/TinyGPUTargetInfo.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "TinyGPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "TinyGPUGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "TinyGPUGenRegisterInfo.inc"

static MCInstrInfo *createTinyGPUMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTinyGPUMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTinyGPUMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  return X;
}

static MCSubtargetInfo *
createTinyGPUMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  std::string CPUName(CPU);
  if (CPUName.empty())
    CPUName = "generic";
  return createTinyGPUMCSubtargetInfoImpl(TT, CPUName, CPUName, FS);
}

static MCInstPrinter *createTinyGPUMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new TinyGPUInstPrinter(MAI, MII, MRI);
}

static MCAsmInfo *createTinyGPUMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new TinyGPUMCAsmInfo(TT);

  unsigned WP = MRI.getDwarfRegNum(TinyGPU::X2, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, WP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

extern "C" void LLVMInitializeTinyGPUTargetMC() {
  for (Target *T : {&getTheTinyGPUTarget()}) {
    // Register the MC asm info.
    TargetRegistry::RegisterMCAsmInfo(*T, createTinyGPUMCAsmInfo);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createTinyGPUMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createTinyGPUMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T, createTinyGPUMCSubtargetInfo);

    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*T, createTinyGPUMCInstPrinter);
  }
}
