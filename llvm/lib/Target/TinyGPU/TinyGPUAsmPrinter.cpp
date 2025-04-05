//===-- TinyGPUAsmPrinter.cpp - TinyGPU LLVM Assembly Printer -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format CPU0 assembly language.
//
//===----------------------------------------------------------------------===//

#include "TinyGPUInstrInfo.h"
#include "TinyGPUTargetMachine.h"
#include "MCTargetDesc/TinyGPUInstPrinter.h"
#include "TargetInfo/TinyGPUTargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "TinyGPU-asm-printer"

namespace llvm {
class TinyGPUAsmPrinter : public AsmPrinter {
public:
  // Override to specify whether a label should be emitted for a basic block.
  // Returning true ensures that labels are emitted for all basic blocks,
  // even if they are not referenced elsewhere in the code.
  bool shouldEmitLabelForBasicBlock(const MachineBasicBlock &MBB) const {
    return true; // Emit labels even if unreferenced
  }
  
  // Constructor for the TinyGPUAsmPrinter class.
  // Initializes the base AsmPrinter class with the given TargetMachine and
  // MCStreamer. The TargetMachine provides information about the target
  // architecture, and the MCStreamer is used to emit the assembly code.
  explicit TinyGPUAsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
    : AsmPrinter(TM, std::move(Streamer)) {}

  // Returns the name of the pass for debugging and identification purposes.
  // This function provides a human-readable name for the pass, which can be
  // useful when analyzing or debugging the compilation process.
  virtual StringRef getPassName() const override {
    return "TinyGPU Assembly Printer";
  }

  // Emits a single machine instruction to the output stream.
  // This function handles both pseudo-instructions (which are expanded into
  // real instructions) and regular instructions.
  void emitInstruction(const MachineInstr *MI) override;

  // Emits the start of a basic block in the assembly output.
  // This function generates a label for the basic block, which can be used
  // for branching and other control flow operations.
  void emitBasicBlockStart(const MachineBasicBlock &MBB) const; 

  // Emits the end of the function body in the assembly output.
  // This function can be used to perform any cleanup or finalization tasks
  // required at the end of a function's assembly representation.
  void emitFunctionBodyEnd();

  // Emits a machine instruction to the MCStreamer.
  // This function is used internally by the auto-generated function
  // emitPseudoExpansionLowering to expand pseudo-instructions into real
  // instructions and emit them to the output stream.
  void EmitToStreamer(MCStreamer &S, const MCInst &Inst);

  // Auto-generated function in TinyGPUGenMCPseudoLowering.inc.
  // This function checks if a given machine instruction is a pseudo-instruction
  // and expands it into a real instruction. If the expansion is successful,
  // it returns true and provides the expanded instruction in the `Inst` parameter.
  bool lowerPseudoInstExpansion(const MachineInstr *MI, MCInst &Inst);
  
private:
  // Lowers a MachineInstr to an MCInst.
  // This function translates a high-level machine instruction into a lower-level
  // MCInst representation that can be emitted to the assembly output.
  void LowerInstruction(const MachineInstr *MI, MCInst &OutMI) const;

  // Lowers a MachineOperand to an MCOperand.
  // This function converts a machine operand (e.g., register, immediate, symbol)
  // into its corresponding MCOperand representation.
  MCOperand LowerOperand(const MachineOperand& MO) const;

  // Lowers a symbol-related MachineOperand to an MCOperand.
  // This function handles operands that reference symbols, such as global
  // addresses, block addresses, or external symbols, and converts them into
  // MCOperand expressions.
  MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;
};
}

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "TinyGPUGenMCPseudoLowering.inc"
void TinyGPUAsmPrinter::EmitToStreamer(MCStreamer &S, const MCInst &Inst) {
  // Emit the given MCInst to the output stream using the base AsmPrinter's
  // EmitToStreamer method. This function handles the actual emission of the
  // instruction to the assembly output.
  AsmPrinter::EmitToStreamer(*OutStreamer, Inst);
}

void TinyGPUAsmPrinter::emitInstruction(const MachineInstr *MI) {
  // Do any auto-generated pseudo lowerings.
  
  // Check if the given MachineInstr is a pseudo-instruction and attempt to
  // expand it into a real instruction. If successful, emit the expanded
  // instruction to the output stream and return.
  if (MCInst OutInst; lowerPseudoInstExpansion(MI, OutInst)) {
    EmitToStreamer(*OutStreamer, OutInst);
    return;
  }

  // If the instruction is not a pseudo-instruction, lower it to an MCInst
  // representation and emit it to the output stream.
  MCInst TmpInst;
  LowerInstruction(MI, TmpInst);
  EmitToStreamer(*OutStreamer, TmpInst);
}


void TinyGPUAsmPrinter::emitBasicBlockStart(const MachineBasicBlock &MBB) const {
  // Retrieve the subtarget information for the current target.
  const MCSubtargetInfo &STI = getSubtargetInfo();

  // Get the parent MachineFunction of the current MachineBasicBlock.
  auto MF = MBB.getParent();

  // Emit a label for the basic block. The label is constructed using a
  // temporary symbol with the format "LBB<FunctionNumber>_<BlockNumber>".
  // This label is used for branching and control flow in the assembly output.
  OutStreamer->emitLabel(createTempSymbol(
        Twine("LBB") + Twine(MF->getFunctionNumber()) + "_" + 
        Twine(MBB.getNumber())));
}

void TinyGPUAsmPrinter::emitFunctionBodyEnd() {
  // Disable LLVM's default comment emission
}

void TinyGPUAsmPrinter::LowerInstruction(const MachineInstr *MI,
                                       MCInst &OutMI) const {
  // Set the opcode of the MCInst to match the opcode of the MachineInstr.
  OutMI.setOpcode(MI->getOpcode());

  // Iterate through each operand of the MachineInstr.
  for (const MachineOperand &MO : MI->operands()) {
    // Lower the MachineOperand to an MCOperand.
    MCOperand MCOp = LowerOperand(MO);

    // If the MCOperand is valid, add it to the MCInst.
    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}

MCOperand TinyGPUAsmPrinter::LowerOperand(const MachineOperand& MO) const {
  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    // If the operand is a register, ignore implicit registers.
    if (MO.isImplicit()) {
      break;
    }
    // Create an MCOperand for the register.
    return MCOperand::createReg(MO.getReg());

  case MachineOperand::MO_Immediate:
    // If the operand is an immediate value, create an MCOperand for it.
    return MCOperand::createImm(MO.getImm());

  case MachineOperand::MO_MachineBasicBlock:
    // If the operand is a machine basic block, lower it to a symbol operand.
    return LowerSymbolOperand(MO, MO.getMBB()->getSymbol());

  case MachineOperand::MO_GlobalAddress:
    // If the operand is a global address, lower it to a symbol operand.
    return LowerSymbolOperand(MO, getSymbol(MO.getGlobal()));

  case MachineOperand::MO_BlockAddress:
    // If the operand is a block address, lower it to a symbol operand.
    return LowerSymbolOperand(MO, GetBlockAddressSymbol(MO.getBlockAddress()));

  case MachineOperand::MO_ExternalSymbol:
    // If the operand is an external symbol, lower it to a symbol operand.
    return LowerSymbolOperand(MO, GetExternalSymbolSymbol(MO.getSymbolName()));

  case MachineOperand::MO_ConstantPoolIndex:
    // If the operand is a constant pool index, lower it to a symbol operand.
    return LowerSymbolOperand(MO, GetCPISymbol(MO.getIndex()));

  case MachineOperand::MO_RegisterMask:
    // Register masks are ignored as they are not directly lowered.
    break;

  default:
    // Report an error for unknown operand types.
    report_fatal_error("unknown operand type");
 }

  // Return an empty MCOperand if no valid operand was created.
  return MCOperand();
}

MCOperand TinyGPUAsmPrinter::LowerSymbolOperand(const MachineOperand &MO,
                                              MCSymbol *Sym) const {
  // Get the MCContext from the current AsmPrinter's OutContext.
  MCContext &Ctx = OutContext;

  // Create an MCExpr for the symbol reference. This represents the symbol
  // in the assembly output, with no additional modifiers (VK_None).
  const MCExpr *Expr =
    MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, Ctx);

  // If the MachineOperand is not a jump table index or a machine basic block,
  // and it has an offset, create a binary expression to add the offset to the symbol.
  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
    Expr = MCBinaryExpr::createAdd(
        Expr, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  // Create and return an MCOperand that wraps the MCExpr.
  return MCOperand::createExpr(Expr);
}

// Force static initialization.
// This function is an external entry point for initializing the TinyGPUAsmPrinter.
// It is marked with `LLVM_EXTERNAL_VISIBILITY` to ensure it is visible outside
// the shared library or object file where it is defined. This is required for
// LLVM's plugin mechanism to locate and invoke the function.
//
// The function registers the TinyGPUAsmPrinter class as the assembly printer
// for the TinyGPU target. The `RegisterAsmPrinter` template is used to associate
// the TinyGPUAsmPrinter with the TinyGPU target, enabling LLVM to use it for
// generating assembly code for TinyGPU.

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTinyGPUAsmPrinter() {
  // Register the TinyGPUAsmPrinter with the TinyGPU target.
  RegisterAsmPrinter<TinyGPUAsmPrinter> X(getTheTinyGPUTarget());
}
