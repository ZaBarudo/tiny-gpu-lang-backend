//===-- TinyGPUAsmPrinter.cpp - TinyGPU LLVM assembly writer ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format TinyGPU assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TinyGPUInstPrinter.h"
#include "MCTargetDesc/TinyGPUMCExpr.h"
#include "MCTargetDesc/TinyGPUMCTargetDesc.h"
#include "MCTargetDesc/TinyGPUTargetStreamer.h"
#include "TinyGPU.h"
#include "TinyGPUInstrInfo.h"
#include "TinyGPUTargetMachine.h"
#include "TargetInfo/TinyGPUTargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
  class TinyGPUAsmPrinter : public AsmPrinter {
    TinyGPUTargetStreamer &getTargetStreamer() {
      return static_cast<TinyGPUTargetStreamer &>(
          *OutStreamer->getTargetStreamer());
    }
  public:
    explicit TinyGPUAsmPrinter(TargetMachine &TM,
                             std::unique_ptr<MCStreamer> Streamer)
        : AsmPrinter(TM, std::move(Streamer)) {}

    StringRef getPassName() const override { return "TinyGPU Assembly Printer"; }

    void printOperand(const MachineInstr *MI, int opNum, raw_ostream &OS);
    void printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &OS);

    void emitFunctionBodyStart() override;
    void emitInstruction(const MachineInstr *MI) override;

    static const char *getRegisterName(MCRegister Reg) {
      return TinyGPUInstPrinter::getRegisterName(Reg);
    }

    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                         const char *ExtraCode, raw_ostream &O) override;
    bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                               const char *ExtraCode, raw_ostream &O) override;

    void LowerGETPCXAndEmitMCInsts(const MachineInstr *MI,
                                   const MCSubtargetInfo &STI);

  };
} // end of anonymous namespace

static MCOperand createTinyGPUMCOperand(TinyGPUMCExpr::VariantKind Kind,
                                      MCSymbol *Sym, MCContext &OutContext) {
  const MCSymbolRefExpr *MCSym = MCSymbolRefExpr::create(Sym,
                                                         OutContext);
  const TinyGPUMCExpr *expr = TinyGPUMCExpr::create(Kind, MCSym, OutContext);
  return MCOperand::createExpr(expr);

}
static MCOperand createPCXCallOP(MCSymbol *Label,
                                 MCContext &OutContext) {
  return createTinyGPUMCOperand(TinyGPUMCExpr::VK_TinyGPU_WDISP30, Label, OutContext);
}

static MCOperand createPCXRelExprOp(TinyGPUMCExpr::VariantKind Kind,
                                    MCSymbol *GOTLabel, MCSymbol *StartLabel,
                                    MCSymbol *CurLabel,
                                    MCContext &OutContext)
{
  const MCSymbolRefExpr *GOT = MCSymbolRefExpr::create(GOTLabel, OutContext);
  const MCSymbolRefExpr *Start = MCSymbolRefExpr::create(StartLabel,
                                                         OutContext);
  const MCSymbolRefExpr *Cur = MCSymbolRefExpr::create(CurLabel,
                                                       OutContext);

  const MCBinaryExpr *Sub = MCBinaryExpr::createSub(Cur, Start, OutContext);
  const MCBinaryExpr *Add = MCBinaryExpr::createAdd(GOT, Sub, OutContext);
  const TinyGPUMCExpr *expr = TinyGPUMCExpr::create(Kind,
                                                Add, OutContext);
  return MCOperand::createExpr(expr);
}

static void EmitCall(MCStreamer &OutStreamer,
                     MCOperand &Callee,
                     const MCSubtargetInfo &STI)
{
  MCInst CallInst;
  CallInst.setOpcode(SP::CALL);
  CallInst.addOperand(Callee);
  OutStreamer.emitInstruction(CallInst, STI);
}

static void EmitRDPC(MCStreamer &OutStreamer, MCOperand &RD,
                     const MCSubtargetInfo &STI) {
  MCInst RDPCInst;
  RDPCInst.setOpcode(SP::RDASR);
  RDPCInst.addOperand(RD);
  RDPCInst.addOperand(MCOperand::createReg(SP::ASR5));
  OutStreamer.emitInstruction(RDPCInst, STI);
}

static void EmitSETHI(MCStreamer &OutStreamer,
                      MCOperand &Imm, MCOperand &RD,
                      const MCSubtargetInfo &STI)
{
  MCInst SETHIInst;
  SETHIInst.setOpcode(SP::SETHIi);
  SETHIInst.addOperand(RD);
  SETHIInst.addOperand(Imm);
  OutStreamer.emitInstruction(SETHIInst, STI);
}

static void EmitBinary(MCStreamer &OutStreamer, unsigned Opcode,
                       MCOperand &RS1, MCOperand &Src2, MCOperand &RD,
                       const MCSubtargetInfo &STI)
{
  MCInst Inst;
  Inst.setOpcode(Opcode);
  Inst.addOperand(RD);
  Inst.addOperand(RS1);
  Inst.addOperand(Src2);
  OutStreamer.emitInstruction(Inst, STI);
}

static void EmitOR(MCStreamer &OutStreamer,
                   MCOperand &RS1, MCOperand &Imm, MCOperand &RD,
                   const MCSubtargetInfo &STI) {
  EmitBinary(OutStreamer, SP::ORri, RS1, Imm, RD, STI);
}

static void EmitADD(MCStreamer &OutStreamer,
                    MCOperand &RS1, MCOperand &RS2, MCOperand &RD,
                    const MCSubtargetInfo &STI) {
  EmitBinary(OutStreamer, SP::ADDrr, RS1, RS2, RD, STI);
}

static void EmitSHL(MCStreamer &OutStreamer,
                    MCOperand &RS1, MCOperand &Imm, MCOperand &RD,
                    const MCSubtargetInfo &STI) {
  EmitBinary(OutStreamer, SP::SLLri, RS1, Imm, RD, STI);
}


static void EmitHiLo(MCStreamer &OutStreamer,  MCSymbol *GOTSym,
                     TinyGPUMCExpr::VariantKind HiKind,
                     TinyGPUMCExpr::VariantKind LoKind,
                     MCOperand &RD,
                     MCContext &OutContext,
                     const MCSubtargetInfo &STI) {

  MCOperand hi = createTinyGPUMCOperand(HiKind, GOTSym, OutContext);
  MCOperand lo = createTinyGPUMCOperand(LoKind, GOTSym, OutContext);
  EmitSETHI(OutStreamer, hi, RD, STI);
  EmitOR(OutStreamer, RD, lo, RD, STI);
}

void TinyGPUAsmPrinter::LowerGETPCXAndEmitMCInsts(const MachineInstr *MI,
                                                const MCSubtargetInfo &STI)
{
  MCSymbol *GOTLabel   =
    OutContext.getOrCreateSymbol(Twine("_GLOBAL_OFFSET_TABLE_"));

  const MachineOperand &MO = MI->getOperand(0);
  assert(MO.getReg() != SP::O7 &&
         "%o7 is assigned as destination for getpcx!");

  MCOperand MCRegOP = MCOperand::createReg(MO.getReg());


  if (!isPositionIndependent()) {
    // Just load the address of GOT to MCRegOP.
    switch(TM.getCodeModel()) {
    default:
      llvm_unreachable("Unsupported absolute code model");
    case CodeModel::Small:
      EmitHiLo(*OutStreamer, GOTLabel,
               TinyGPUMCExpr::VK_TinyGPU_HI, TinyGPUMCExpr::VK_TinyGPU_LO,
               MCRegOP, OutContext, STI);
      break;
    case CodeModel::Medium: {
      EmitHiLo(*OutStreamer, GOTLabel,
               TinyGPUMCExpr::VK_TinyGPU_H44, TinyGPUMCExpr::VK_TinyGPU_M44,
               MCRegOP, OutContext, STI);
      MCOperand imm = MCOperand::createExpr(MCConstantExpr::create(12,
                                                                   OutContext));
      EmitSHL(*OutStreamer, MCRegOP, imm, MCRegOP, STI);
      MCOperand lo = createTinyGPUMCOperand(TinyGPUMCExpr::VK_TinyGPU_L44,
                                          GOTLabel, OutContext);
      EmitOR(*OutStreamer, MCRegOP, lo, MCRegOP, STI);
      break;
    }
    case CodeModel::Large: {
      EmitHiLo(*OutStreamer, GOTLabel,
               TinyGPUMCExpr::VK_TinyGPU_HH, TinyGPUMCExpr::VK_TinyGPU_HM,
               MCRegOP, OutContext, STI);
      MCOperand imm = MCOperand::createExpr(MCConstantExpr::create(32,
                                                                   OutContext));
      EmitSHL(*OutStreamer, MCRegOP, imm, MCRegOP, STI);
      // Use register %o7 to load the lower 32 bits.
      MCOperand RegO7 = MCOperand::createReg(SP::O7);
      EmitHiLo(*OutStreamer, GOTLabel,
               TinyGPUMCExpr::VK_TinyGPU_HI, TinyGPUMCExpr::VK_TinyGPU_LO,
               RegO7, OutContext, STI);
      EmitADD(*OutStreamer, MCRegOP, RegO7, MCRegOP, STI);
    }
    }
    return;
  }

  MCSymbol *StartLabel = OutContext.createTempSymbol();
  MCSymbol *EndLabel   = OutContext.createTempSymbol();
  MCSymbol *SethiLabel = OutContext.createTempSymbol();

  MCOperand RegO7   = MCOperand::createReg(SP::O7);

  // <StartLabel>:
  //   <GET-PC> // This will be either `call <EndLabel>` or `rd %pc, %o7`.
  // <SethiLabel>:
  //     sethi %hi(_GLOBAL_OFFSET_TABLE_+(<SethiLabel>-<StartLabel>)), <MO>
  // <EndLabel>:
  //   or  <MO>, %lo(_GLOBAL_OFFSET_TABLE_+(<EndLabel>-<StartLabel>))), <MO>
  //   add <MO>, %o7, <MO>

  OutStreamer->emitLabel(StartLabel);
  if (!STI.getTargetTriple().isTinyGPU64() ||
      STI.hasFeature(TinyGPU::TuneSlowRDPC)) {
    MCOperand Callee = createPCXCallOP(EndLabel, OutContext);
    EmitCall(*OutStreamer, Callee, STI);
  } else {
    // TODO find out whether it is possible to store PC
    // in other registers, to enable leaf function optimization.
    // (On the other hand, approx. over 97.8% of GETPCXes happen
    // in non-leaf functions, so would this be worth the effort?)
    EmitRDPC(*OutStreamer, RegO7, STI);
  }
  OutStreamer->emitLabel(SethiLabel);
  MCOperand hiImm = createPCXRelExprOp(TinyGPUMCExpr::VK_TinyGPU_PC22,
                                       GOTLabel, StartLabel, SethiLabel,
                                       OutContext);
  EmitSETHI(*OutStreamer, hiImm, MCRegOP, STI);
  OutStreamer->emitLabel(EndLabel);
  MCOperand loImm = createPCXRelExprOp(TinyGPUMCExpr::VK_TinyGPU_PC10,
                                       GOTLabel, StartLabel, EndLabel,
                                       OutContext);
  EmitOR(*OutStreamer, MCRegOP, loImm, MCRegOP, STI);
  EmitADD(*OutStreamer, MCRegOP, RegO7, MCRegOP, STI);
}

void TinyGPUAsmPrinter::emitInstruction(const MachineInstr *MI) {
  TinyGPU_MC::verifyInstructionPredicates(MI->getOpcode(),
                                        getSubtargetInfo().getFeatureBits());

  switch (MI->getOpcode()) {
  default: break;
  case TargetOpcode::DBG_VALUE:
    // FIXME: Debug Value.
    return;
  case SP::CASArr:
  case SP::SWAPrr:
  case SP::SWAPri:
    if (MF->getSubtarget<TinyGPUSubtarget>().fixTN0011())
      OutStreamer->emitCodeAlignment(Align(16), &getSubtargetInfo());
    break;
  case SP::GETPCX:
    LowerGETPCXAndEmitMCInsts(MI, getSubtargetInfo());
    return;
  }
  MachineBasicBlock::const_instr_iterator I = MI->getIterator();
  MachineBasicBlock::const_instr_iterator E = MI->getParent()->instr_end();
  do {
    MCInst TmpInst;
    LowerTinyGPUMachineInstrToMCInst(&*I, TmpInst, *this);
    EmitToStreamer(*OutStreamer, TmpInst);
  } while ((++I != E) && I->isInsideBundle()); // Delay slot check.
}

void TinyGPUAsmPrinter::emitFunctionBodyStart() {
  if (!MF->getSubtarget<TinyGPUSubtarget>().is64Bit())
    return;

  const MachineRegisterInfo &MRI = MF->getRegInfo();
  const unsigned globalRegs[] = { SP::G2, SP::G3, SP::G6, SP::G7, 0 };
  for (unsigned i = 0; globalRegs[i] != 0; ++i) {
    unsigned reg = globalRegs[i];
    if (MRI.use_empty(reg))
      continue;

    if  (reg == SP::G6 || reg == SP::G7)
      getTargetStreamer().emitTinyGPURegisterIgnore(reg);
    else
      getTargetStreamer().emitTinyGPURegisterScratch(reg);
  }
}

void TinyGPUAsmPrinter::printOperand(const MachineInstr *MI, int opNum,
                                   raw_ostream &O) {
  const DataLayout &DL = getDataLayout();
  const MachineOperand &MO = MI->getOperand (opNum);
  TinyGPUMCExpr::VariantKind TF = (TinyGPUMCExpr::VariantKind) MO.getTargetFlags();

  bool CloseParen = TinyGPUMCExpr::printVariantKind(O, TF);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    O << "%" << StringRef(getRegisterName(MO.getReg())).lower();
    break;

  case MachineOperand::MO_Immediate:
    O << MO.getImm();
    break;
  case MachineOperand::MO_MachineBasicBlock:
    MO.getMBB()->getSymbol()->print(O, MAI);
    return;
  case MachineOperand::MO_GlobalAddress:
    PrintSymbolOperand(MO, O);
    break;
  case MachineOperand::MO_BlockAddress:
    O <<  GetBlockAddressSymbol(MO.getBlockAddress())->getName();
    break;
  case MachineOperand::MO_ExternalSymbol:
    O << MO.getSymbolName();
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    O << DL.getPrivateGlobalPrefix() << "CPI" << getFunctionNumber() << "_"
      << MO.getIndex();
    break;
  case MachineOperand::MO_Metadata:
    MO.getMetadata()->printAsOperand(O, MMI->getModule());
    break;
  default:
    llvm_unreachable("<unknown operand type>");
  }
  if (CloseParen) O << ")";
}

void TinyGPUAsmPrinter::printMemOperand(const MachineInstr *MI, int opNum,
                                      raw_ostream &O) {
  printOperand(MI, opNum, O);

  if (MI->getOperand(opNum+1).isReg() &&
      MI->getOperand(opNum+1).getReg() == SP::G0)
    return;   // don't print "+%g0"
  if (MI->getOperand(opNum+1).isImm() &&
      MI->getOperand(opNum+1).getImm() == 0)
    return;   // don't print "+0"

  O << "+";
  printOperand(MI, opNum+1, O);
}

/// PrintAsmOperand - Print out an operand for an inline asm expression.
///
bool TinyGPUAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                      const char *ExtraCode,
                                      raw_ostream &O) {
  if (ExtraCode && ExtraCode[0]) {
    if (ExtraCode[1] != 0) return true; // Unknown modifier.

    switch (ExtraCode[0]) {
    default:
      // See if this is a generic print operand
      return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, O);
    case 'L': // Low order register of a twin word register operand
    case 'H': // High order register of a twin word register operand
    {
      const TinyGPUSubtarget &Subtarget = MF->getSubtarget<TinyGPUSubtarget>();
      const MachineOperand &MO = MI->getOperand(OpNo);
      const TinyGPURegisterInfo *RegisterInfo = Subtarget.getRegisterInfo();
      Register MOReg = MO.getReg();

      Register HiReg, LoReg;
      if (!SP::IntPairRegClass.contains(MOReg)) {
        // If we aren't given a register pair already, find out which pair it
        // belongs to. Note that here, the specified register operand, which
        // refers to the high part of the twinword, needs to be an even-numbered
        // register.
        MOReg = RegisterInfo->getMatchingSuperReg(MOReg, SP::sub_even,
                                                  &SP::IntPairRegClass);
        if (!MOReg) {
          SMLoc Loc;
          OutContext.reportError(
              Loc, "Hi part of pair should point to an even-numbered register");
          OutContext.reportError(
              Loc, "(note that in some cases it might be necessary to manually "
                   "bind the input/output registers instead of relying on "
                   "automatic allocation)");
          return true;
        }
      }

      HiReg = RegisterInfo->getSubReg(MOReg, SP::sub_even);
      LoReg = RegisterInfo->getSubReg(MOReg, SP::sub_odd);

      Register Reg;
      switch (ExtraCode[0]) {
      case 'L':
        Reg = LoReg;
        break;
      case 'H':
        Reg = HiReg;
        break;
      }

      O << '%' << TinyGPUInstPrinter::getRegisterName(Reg);
      return false;
    }
    case 'f':
    case 'r':
     break;
    }
  }

  printOperand(MI, OpNo, O);

  return false;
}

bool TinyGPUAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                            unsigned OpNo,
                                            const char *ExtraCode,
                                            raw_ostream &O) {
  if (ExtraCode && ExtraCode[0])
    return true;  // Unknown modifier

  O << '[';
  printMemOperand(MI, OpNo, O);
  O << ']';

  return false;
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTinyGPUAsmPrinter() {
  RegisterAsmPrinter<TinyGPUAsmPrinter> X(getTheTinyGPUTarget());
  RegisterAsmPrinter<TinyGPUAsmPrinter> Y(getTheTinyGPUV9Target());
  RegisterAsmPrinter<TinyGPUAsmPrinter> Z(getTheTinyGPUelTarget());
}
