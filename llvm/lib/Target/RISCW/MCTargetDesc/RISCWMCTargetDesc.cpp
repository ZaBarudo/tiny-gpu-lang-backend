//===-- RISCWMCTargetDesc.cpp - RISCW Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides RISCW specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "RISCWMCTargetDesc.h"
#include "RISCWInstPrinter.h"
#include "RISCWMCAsmInfo.h"
#include "TargetInfo/RISCWTargetInfo.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "RISCWGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "RISCWGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "RISCWGenRegisterInfo.inc"

static MCInstrInfo *createRISCWMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitRISCWMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createRISCWMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  return X;
}

static MCSubtargetInfo *
createRISCWMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = "generic";
  return createRISCWMCSubtargetInfoImpl(TT, CPUName, FS);
}

static MCInstPrinter *createRISCWMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new RISCWInstPrinter(MAI, MII, MRI);
}

static MCAsmInfo *createRISCWMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new RISCWMCAsmInfo(TT);

  unsigned WP = MRI.getDwarfRegNum(RISCW::X2, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr, WP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

extern "C" void LLVMInitializeRISCWTargetMC() {
  for (Target *T : {&getTheRISCWTarget()}) {
    // Register the MC asm info.
    TargetRegistry::RegisterMCAsmInfo(*T, createRISCWMCAsmInfo);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createRISCWMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createRISCWMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T, createRISCWMCSubtargetInfo);

    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*T, createRISCWMCInstPrinter);
  }
}
