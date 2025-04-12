//===-- TinyGPUInstrInfo.cpp - TinyGPU Instruction Information ----------------===//
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

#include "TinyGPUInstrInfo.h"

#include "TinyGPUTargetMachine.h"
#include "TinyGPUMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "TinyGPU-instrinfo"

#define GET_INSTRINFO_CTOR_DTOR
#include "TinyGPUGenInstrInfo.inc"

void dumpOperandDetails(const MachineInstr &MI) {
  dbgs() << "Instruction " << MI.getOpcode() << " has " << MI.getNumOperands() << " operands:\n";
  
  for (unsigned i = 0; i < MI.getNumOperands(); ++i) {
    const MachineOperand &MO = MI.getOperand(i);
    dbgs() << "  Op" << i << ": ";
    
    switch (MO.getType()) {
    case MachineOperand::MO_Register:
      dbgs() << "REG ";
      if (MO.isDef()) dbgs() << "[def] ";
      if (MO.isUse()) dbgs() << "[use] ";
      if (MO.isImplicit()) dbgs() << "[implicit] ";
      if (MO.isKill()) dbgs() << "[kill] ";
      dbgs() << "Reg=" << MO.getReg();
      break;
      
    case MachineOperand::MO_Immediate:
      dbgs() << "IMM " << MO.getImm();
      break;
      
    case MachineOperand::MO_FPImmediate:
      dbgs() << "FPIMM " << MO.getFPImm();
      break;
      
    case MachineOperand::MO_MachineBasicBlock:
      dbgs() << "MBB " << MO.getMBB()->getName();
      break;
      
    case MachineOperand::MO_FrameIndex:
      dbgs() << "FrameIdx " << MO.getIndex();
      break;
      
    case MachineOperand::MO_ConstantPoolIndex:
      dbgs() << "ConstantPool " << MO.getIndex();
      break;
      
    case MachineOperand::MO_TargetIndex:
      dbgs() << "TargetIndex " << MO.getIndex();
      break;
      
    case MachineOperand::MO_JumpTableIndex:
      dbgs() << "JumpTable " << MO.getIndex();
      break;
      
    case MachineOperand::MO_ExternalSymbol:
      dbgs() << "Symbol " << MO.getSymbolName();
      break;
      
    case MachineOperand::MO_GlobalAddress:
      dbgs() << "Global " << MO.getGlobal()->getName();
      break;
      
    case MachineOperand::MO_RegisterMask:
      dbgs() << "RegMask";
      break;
      
    case MachineOperand::MO_RegisterLiveOut:
      dbgs() << "RegLiveOut";
      break;
      
    case MachineOperand::MO_Metadata:
      dbgs() << "Metadata";
      break;
      
    case MachineOperand::MO_MCSymbol:
      dbgs() << "MCSymbol";
      break;
      
    case MachineOperand::MO_CFIIndex:
      dbgs() << "CFIIndex";
      break;
      
    case MachineOperand::MO_IntrinsicID:
      dbgs() << "IntrinsicID";
      break;
      
    case MachineOperand::MO_Predicate:
      dbgs() << "Predicate";
      break;
    }
    
    dbgs() << "\n";
  }
}

void TinyGPUInstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg,
    int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg,
    MachineInstr::MIFlag Flags) const {
  
  // DebugLoc DL;
  // if (MI != MBB.end()) DL = MI->getDebugLoc();
  
  // // Emit LDR instruction: ldr DestReg, [SP + FrameIndex]
  // BuildMI(MBB, MI, DL, get(TinyGPU::LDR), DestReg)
  //   .addFrameIndex(FrameIndex)  // Stack slot index
  //   .setMIFlags(Flags);
}

void TinyGPUInstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
    bool IsKill, int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg,MachineInstr::MIFlag Flags) const {
  
  // DebugLoc DL;
  // if (MI != MBB.end()) DL = MI->getDebugLoc();
  
  // // Emit STR instruction: str SrcReg, [SP + FrameIndex]
  // BuildMI(MBB, MI, DL, get(TinyGPU::STR))
  //   .addReg(SrcReg, getKillRegState(IsKill))
  //   .addFrameIndex(FrameIndex)  // Stack slot index
  //   .setMIFlags(Flags);
}


// Constructor for the TinyGPUInstrInfo class.
// Initializes the base class TinyGPUGenInstrInfo with the stack adjustment opcodes
// and stores a reference to the TinyGPUSubtarget.
TinyGPUInstrInfo::TinyGPUInstrInfo(const TinyGPUSubtarget &STI)
  : TinyGPUGenInstrInfo(TinyGPU::ADJCALLSTACKDOWN, TinyGPU::ADJCALLSTACKUP), // Initialize base class with stack adjustment opcodes
    Subtarget(STI) // Store the subtarget reference
{
}

// This function copies the value from a source physical register (SrcReg)
// to a destination physical register (DestReg) using a move instruction.
// It is part of the TinyGPUInstrInfo class and overrides the base class method.
void TinyGPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
               MachineBasicBlock::iterator MI, const DebugLoc &DL,
               MCRegister DestReg, MCRegister SrcReg, bool KillSrc,
               bool RenamableDest,
               bool RenamableSrc)  const {
  // Emit a move instruction (TinyGPU) to copy the value from SrcReg to DestReg.
  // The `BuildMI` function is used to construct the machine instruction.
  // - `MBB`: The machine basic block where the instruction is inserted.
  // - `MI`: The position in the block where the instruction is inserted.
  // - `DL`: Debug location for the instruction.
  // - `get(TinyGPU::STR)`: Retrieves the move instruction for the TinyGPU target.
  // - `DestReg`: The destination register to which the value is copied.
  // - `SrcReg`: The source register from which the value is copied.
  // - `getKillRegState(KillSrc)`: Indicates whether the source register is killed after use.
  BuildMI(MBB, MI, DL, get(TinyGPU::STR), DestReg)
    .addReg(SrcReg, getKillRegState(KillSrc)); // Add the source register with kill state.
}


// bool TinyGPUInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
//    LLVM_DEBUG({
//     dbgs() << "\n=== Pseudo Instruction Expansion ===\n";
//     dbgs() << "Opcode: " << MI.getOpcode() 
//            << " (" << getName(MI.getOpcode()) << ")\n";
//     dumpOperandDetails(MI);
//     dbgs() << "In BB: " << MI.getParent()->getName() 
//            << " (Function: " << MI.getParent()->getParent()->getName() << ")\n";
//     dumpOperandDetails(MI);
    
//     dbgs() << "  Expansion Time \n ";
//   });
//   if (MI.getOpcode() == TinyGPU::CALLL) {
//     auto res = expandCallPseudo(MI);
//     return res;
//   }
//   return true;
// }

// bool TinyGPUInstrInfo::expandRETPseudo(MachineInstr &MI) const {
//   MachineBasicBlock *MBB = MI.getParent();
//   MachineFunction *MF = MBB->getParent();
//   DebugLoc DL = MI.getDebugLoc();
//   BuildMI(*MBB, MI.getIterator(), DL, get(TinyGPU::BRNCH), TinyGPU::LR);
//   MI.eraseFromParent();
//   return false;
// }


// bool TinyGPUInstrInfo::expandCallPseudo(MachineInstr &MI) const {
//   MachineBasicBlock *MBB = MI.getParent();
//   MachineFunction *MF = MBB->getParent();
//   DebugLoc DL = MI.getDebugLoc();
//   MachineBasicBlock *PostCallMBB = MBB->getSingleSuccessor();
//   BuildMI(*MBB, MI.getIterator(), DL, get(TinyGPU::CONST), TinyGPU::LR)
//     .addMBB(PostCallMBB);
//   const MachineOperand &CalleeOp = MI.getOperand(0);
//   const GlobalValue *GV = CalleeOp.getGlobal();
//   BuildMI(*MBB, MI.getIterator(), DL, get(TinyGPU::BRNCH))
//     .addGlobalAddress(GV, CalleeOp.getOffset());
//   MI.eraseFromParent();
//   return true;
// }
