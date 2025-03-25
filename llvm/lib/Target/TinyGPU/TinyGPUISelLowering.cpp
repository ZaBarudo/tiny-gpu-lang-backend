//===-- TinyGPUISelLowering.cpp - TinyGPU DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that TinyGPU uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "TinyGPUISelLowering.h"
#include "TinyGPUSubtarget.h"
#include "TinyGPUTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/Debug.h"
#include "llvm/MC/MCContext.h"
#include <cassert>

using namespace llvm;

#define DEBUG_TYPE "TinyGPU-isellower"

#include "TinyGPUGenCallingConv.inc"

TinyGPUTargetLowering::TinyGPUTargetLowering(const TargetMachine &TM,
                                         const TinyGPUSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI)
{
  // Set up the register classes
  addRegisterClass(MVT::i32, &TinyGPU::GPRRegClass);

  // Must, computeRegisterProperties - Once all of the register classes are
  // added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget.getRegisterInfo());

  // Set scheduling preference
  setSchedulingPreference(Sched::RegPressure);

  setStackPointerRegisterToSaveRestore(TinyGPU::R2);

  // Use i32 for setcc operations results (slt, sgt, ...).
  setBooleanContents(ZeroOrOneBooleanContent);

  // Arithmetic operations
  setOperationAction(ISD::SDIVREM,   MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM,   MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);

  setOperationAction(ISD::SHL_PARTS, MVT::i32, Custom);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Custom);
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Custom);

  setOperationAction(ISD::ROTL,  MVT::i32, Expand);
  setOperationAction(ISD::ROTR,  MVT::i32, Expand);
  setOperationAction(ISD::BSWAP, MVT::i32, Expand);
  setOperationAction(ISD::CTTZ,  MVT::i32, Expand);
  setOperationAction(ISD::CTLZ,  MVT::i32, Expand);
  setOperationAction(ISD::CTPOP, MVT::i32, Expand);

  // Address resolution and constant pool
  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
  setOperationAction(ISD::BlockAddress,  MVT::i32, Custom);
  setOperationAction(ISD::ConstantPool,  MVT::i32, Custom);

  // Set minimum and preferred function alignment (log2)
  setMinFunctionAlignment(Align(1));
  setPrefFunctionAlignment(Align(1));

  // Set preferred loop alignment (log2)
  setPrefLoopAlignment(Align(1));
}

const char *TinyGPUTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case TinyGPUISD::Ret: return "TinyGPUISD::Ret";
  default:            return NULL;
  }
}

void TinyGPUTargetLowering::ReplaceNodeResults(SDNode *N,
                                             SmallVectorImpl<SDValue> &Results,
                                             SelectionDAG &DAG) const {
  switch (N->getOpcode()) {
  default:
    llvm_unreachable("Don't know how to custom expand this!");
  }
}

//===----------------------------------------------------------------------===//
//@            Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

// The BeyondRISC calling convention parameter registers.
static const MCPhysReg GPRArgRegs[] = {
  TinyGPU::R0, TinyGPU::R1, TinyGPU::R2, TinyGPU::R3
};

/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue TinyGPUTargetLowering::LowerFormalArguments(
                                    SDValue Chain,
                                    CallingConv::ID CallConv,
                                    bool isVarArg,
                                    const SmallVectorImpl<ISD::InputArg> &Ins,
                                    const SDLoc &dl, SelectionDAG &DAG,
                                    SmallVectorImpl<SDValue> &InVals) const {
  assert((CallingConv::C == CallConv || CallingConv::Fast == CallConv || CallConv == CallingConv::SPIR_KERNEL) &&
		 "Unsupported CallingConv to FORMAL_ARGS");

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, TinyGPU_CCallingConv);

  SmallVector<SDValue, 16> ArgValues;
  SDValue ArgValue;
  Function::const_arg_iterator CurOrigArg = MF.getFunction().arg_begin();
  unsigned CurArgIdx = 0;

  // Calculate the amount of stack space that we need to allocate to store
  // byval and variadic arguments that are passed in registers.
  // We need to know this before we allocate the first byval or variadic
  // argument, as they will be allocated a stack slot below the CFA (Canonical
  // Frame Address, the stack pointer at entry to the function).
  unsigned ArgRegBegin = TinyGPU::R4;
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    if (CCInfo.getInRegsParamsProcessed() >= CCInfo.getInRegsParamsCount())
      break;

    CCValAssign &VA = ArgLocs[i];
    unsigned Index = VA.getValNo();
    ISD::ArgFlagsTy Flags = Ins[Index].Flags;
    if (!Flags.isByVal())
      continue;

    assert(VA.isMemLoc() && "unexpected byval pointer in reg");
    unsigned RBegin, REnd;
    CCInfo.getInRegsParamInfo(CCInfo.getInRegsParamsProcessed(), RBegin, REnd);
    ArgRegBegin = std::min(ArgRegBegin, RBegin);

    CCInfo.nextInRegsParam();
  }
  CCInfo.rewindByValRegsInfo();

  int lastInsIndex = -1;
  if (isVarArg && MFI.hasVAStart()) {
    unsigned RegIdx = CCInfo.getFirstUnallocated(GPRArgRegs);
    if (RegIdx != std::size(GPRArgRegs))
      ArgRegBegin = std::min(ArgRegBegin, (unsigned)GPRArgRegs[RegIdx]);
  }

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (Ins[VA.getValNo()].isOrigArg()) {
      std::advance(CurOrigArg,
                   Ins[VA.getValNo()].getOrigArgIndex() - CurArgIdx);
      CurArgIdx = Ins[VA.getValNo()].getOrigArgIndex();
    }
    // Arguments stored in registers.
    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();

      if (VA.needsCustom()) {
        llvm_unreachable("Custom val assignment not supported by "
                         "FORMAL_ARGUMENTS Lowering");
      } else {
        const TargetRegisterClass *RC;

        if (RegVT == MVT::i32)
          RC = &TinyGPU::GPRRegClass;
        else
          llvm_unreachable("RegVT not supported by FORMAL_ARGUMENTS Lowering");

        // Transform the arguments in physical registers into virtual ones.
        unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
        ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);
      }

      // If this is an 8 or 16-bit value, it is really passed promoted
      // to 32 bits.  Insert an assert[sz]ext to capture this, then
      // truncate to the right size.
      switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: break;
      case CCValAssign::BCvt:
        ArgValue = DAG.getNode(ISD::BITCAST, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::SExt:
        ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::ZExt:
        ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      }

      InVals.push_back(ArgValue);
    } else { // VA.isRegLoc()
      // sanity check
      assert(VA.isMemLoc());
      assert(VA.getValVT() != MVT::i64 && "i64 should already be lowered");

      int index = VA.getValNo();

      // Some Ins[] entries become multiple ArgLoc[] entries.
      // Process them only once.
      if (index != lastInsIndex)
      {
        llvm_unreachable("Cannot retrieve arguments from the stack");
      }
    }
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool TinyGPUTargetLowering::CanLowerReturn(CallingConv::ID CallConv,
                                MachineFunction &MF, bool isVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, TinyGPU_CRetConv);
}

MCSymbol *TinyGPUTargetLowering::getPostCallLabel(CallLoweringInfo &CLI) const {
  // Generate a unique symbol for the return address
  MCContext &Ctx = CLI.DAG.getMachineFunction().getContext();
  return Ctx.createNamedTempSymbol("postcall");
}

SDValue TinyGPUTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                       SmallVectorImpl<SDValue> &InVals) const {
 SelectionDAG &DAG = CLI.DAG;
  SDLoc dl = CLI.DL;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;

  // 1. Get stack pointer register
  SDValue SP = DAG.getCopyFromReg(Chain, dl, TinyGPU::SP,
                                   getPointerTy(DAG.getDataLayout()));

  // 2. Allocate stack space for return address
  SDValue NewSP = DAG.getNode(ISD::SUB, dl, getPointerTy(DAG.getDataLayout()),
                               SP, DAG.getConstant(4, dl, MVT::i32));

  // 3. Generate return address label
  MCSymbol *RetSym = getPostCallLabel(CLI);
  SDValue RetAddr = DAG.getMCSymbol(RetSym, getPointerTy(DAG.getDataLayout()));

  // 4. Store return address to stack (SAFE: Use MachinePointerInfo)
  MachinePointerInfo MPI = MachinePointerInfo::getStack(DAG.getMachineFunction(), 0);
  Chain = DAG.getStore(Chain, dl, RetAddr, SP, MPI);

  // 5. Update SP register
  Chain = DAG.getCopyToReg(Chain, dl, TinyGPU::SP, NewSP);

  // 6. Emit jump to callee
  SDValue Jump = DAG.getNode(ISD::BR, dl, MVT::Other, Chain, Callee);

  // 7. Handle return value (SAFELY)
  if (!CLI.Ins.empty()) {
    // Read from return register (e.g., $V0) instead of memory
    SDValue RetVal = DAG.getCopyFromReg(Jump, dl, TinyGPU::R0, CLI.Ins[0].VT);
    InVals.push_back(RetVal);
  }

  return Jump;
  
}

SDValue
TinyGPUTargetLowering::LowerReturn(SDValue Chain,
                                 CallingConv::ID CallConv, bool isVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &dl, SelectionDAG &DAG) const {
  // CCValAssign - represent the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slots.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Analyze outgoing return values.
  CCInfo.AnalyzeReturn(Outs, TinyGPU_CRetConv);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps;
  RetOps.push_back(Chain); // Operand #0 = Chain (updated below)

  // Copy the result values into the output registers.
  for (unsigned i = 0, realRVLocIdx = 0;
       i != RVLocs.size();
       ++i, ++realRVLocIdx) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    SDValue Arg = OutVals[realRVLocIdx];
    bool ReturnF16 = false;

    switch (VA.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full: break;
    case CCValAssign::BCvt:
      if (!ReturnF16)
        Arg = DAG.getNode(ISD::BITCAST, dl, VA.getLocVT(), Arg);
      break;
    }

    if (VA.needsCustom()) {
      llvm_unreachable("Custom val assignment not supported by "
                       "RETURN Lowering");
    } else {
      Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), Arg, Flag);
    }

    // Guarantee that all emitted copies are stuck together, avoiding something
    // bad.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(),
                                     ReturnF16 ? MVT::f16 : VA.getLocVT()));
  }

  // Update chain and glue.
  RetOps[0] = Chain;
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(TinyGPUISD::Ret, dl, MVT::Other, RetOps);
}

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//
SDValue
TinyGPUTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const {

  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  EVT VT = Op.getValueType();
  SDLoc dl(Op);

  if (const auto *GVar = dyn_cast<GlobalVariable>(GV)) {
    // Directly check for BlockAddress constant
    if (const auto *BA = dyn_cast<BlockAddress>(GVar->getInitializer())) {
      const BasicBlock *BB = BA->getBasicBlock();
      
      // Create basic block label
      MCContext &Ctx = DAG.getMachineFunction().getContext();
      MCSymbol *BBSym = Ctx.getOrCreateSymbol(BB->getName());
      
      return DAG.getMCSymbol(BBSym, VT);
    }
  }

  // Default handling for regular globals
  return DAG.getTargetGlobalAddress(GV, dl, VT);


  //  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  // EVT VT = Op.getValueType();
  // SDLoc dl(Op);

  // // Check if the GlobalValue is a GlobalVariable initialized with a BlockAddress
  // if (const auto *GVar = dyn_cast<GlobalVariable>(GV)) {
  //   if (const auto *Init = dyn_cast<ConstantExpr>(GVar->getInitializer())) {
  //     if (Init->getOpcode() == Instruction::BlockAddress) {
  //       const auto *BA = cast<BlockAddress>(Init->getOperand(0));
  //       return LowerBlockAddress(BA, dl, VT, DAG);
  //     }
  //   }
  // }

  // // Default handling for other globals
  // return DAG.getTargetGlobalAddress(GV, dl, VT);

  
  // const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  // EVT PtrVT = Op.getValueType();

  // // Create a TargetGlobalAddress node.
  // SDValue Result = DAG.getTargetGlobalAddress(GV, SDLoc(Op), PtrVT);

  // // If the target requires a specific sequence of instructions to load a global
  // // address (e.g., using a base register and an offset), emit those instructions here.
  // // For example, TinyGPU might use a LOAD instruction to load the address into a register.

  // // Example: Emit a LOAD instruction to load the global address into a register.
  // return DAG.getNode(ISD::LOAD, SDLoc(Op), PtrVT, Result);
}

SDValue
TinyGPUTargetLowering::LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const {
  const GlobalAddressSDNode *GA = cast<GlobalAddressSDNode>(Op.getNode());
  const GlobalVariable *GVar = cast<GlobalVariable>(GA->getGlobal());
  const ConstantExpr *CE = cast<ConstantExpr>(GVar->getInitializer());
  const BlockAddress *BA = cast<BlockAddress>(CE->getOperand(0));

  // Extract the basic block and its parent function
  const Function *Func = BA->getFunction();
  const BasicBlock *BB = BA->getBasicBlock();

  // Create a label for the basic block
  MCContext &Ctx = DAG.getMachineFunction().getContext();
  MCSymbol *BBSym = Ctx.getOrCreateSymbol(BB->getName());

  // Return the symbol as an SDValue
  return DAG.getMCSymbol(BBSym, Op.getValueType());
}

SDValue
TinyGPUTargetLowering::LowerConstantPool(SDValue Op, SelectionDAG &DAG) const {
  llvm_unreachable("Unsupported constant pool");
}

SDValue
TinyGPUTargetLowering::LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const {
  return SDValue();
}

SDValue
TinyGPUTargetLowering::LowerShlParts(SDValue Op, SelectionDAG &DAG) const {
  assert(Op.getNumOperands() == 3 && "Not a long shift");

  EVT VT = Op.getValueType();
  unsigned VTBits = VT.getSizeInBits();
  SDLoc DL(Op);

  // if Shamt-32 < 0: // Shamt < 32
  //   Lo = Lo << Shamt
  //   Hi = (Hi << Shamt) | ((Lo >>u 1) >>u (32-1 - Shamt))
  // else:
  //   Lo = 0
  //   Hi = Lo << (Shamt-32)

  SDValue Lo = Op.getOperand(0);
  SDValue Hi = Op.getOperand(1);
  SDValue Shamt = Op.getOperand(2);

  // Precompute a node with ...
  // ... constant value 0
  SDValue Zero = DAG.getConstant(0, DL, VT);
  // ... constant value 1
  SDValue One = DAG.getConstant(1, DL, VT);
  // ... -32
  SDValue NegWsize = DAG.getConstant(-VTBits, DL, VT);
  // ... 32 - 1
  SDValue WsizeMinus1 = DAG.getConstant(VTBits - 1, DL, VT);
  // ... shamt - 32
  SDValue ShamtMinusWsize = DAG.getNode(ISD::ADD, DL, VT, Shamt, NegWsize);
  // ... (32 - 1) - shamt
  SDValue WsizeMinus1MinusShamt =
      DAG.getNode(ISD::SUB, DL, VT, WsizeMinus1, Shamt);

  // Compute the 'then' part of the if
  // Lo << Shamt
  SDValue LoTrue = DAG.getNode(ISD::SHL, DL, VT, Lo, Shamt);
  // Lo >> 1
  SDValue LoShr1 = DAG.getNode(ISD::SRL, DL, VT, Lo, One);
  // (Lo >> 1) >> (32-1 - Shamt)
  SDValue HiLsb = DAG.getNode(ISD::SRL, DL, VT, LoShr1, WsizeMinus1MinusShamt);
  // Hi << Shamt
  SDValue HiMsb = DAG.getNode(ISD::SHL, DL, VT, Hi, Shamt);
  // (Hi << Shamt) | ((Lo >>u 1) >>u (32-1 - Shamt))
  SDValue HiTrue = DAG.getNode(ISD::OR, DL, VT, HiMsb, HiLsb);

  // Compute the 'else' part of the if
  SDValue LoFalse = Zero;
  SDValue HiFalse = DAG.getNode(ISD::SHL, DL, VT, Lo, ShamtMinusWsize);

  // Compute the if condition
  SDValue CC = DAG.getSetCC(DL, VT, ShamtMinusWsize, Zero, ISD::SETLT);

  // and (finally) select the results based on the condition!
  Lo = DAG.getNode(ISD::SELECT, DL, VT, CC, LoTrue, LoFalse);
  Hi = DAG.getNode(ISD::SELECT, DL, VT, CC, HiTrue, HiFalse);

  SDValue Ops[2] = {Lo, Hi};
  return DAG.getMergeValues(Ops, DL);
}

SDValue
TinyGPUTargetLowering::LowerShrParts(SDValue Op, SelectionDAG &DAG,
                                   bool arith) const {
  assert(Op.getNumOperands() == 3 && "Not a long shift");

  EVT VT = Op.getValueType();
  unsigned VTBits = VT.getSizeInBits();
  SDLoc DL(Op);

  // SRA expansion:
  //   if Shamt-32 < 0: // Shamt < 32
  //     Lo = (Lo >>u Shamt) | ((Hi << 1) << (32-1 - Shamt))
  //     Hi = Hi >>s Shamt
  //   else:
  //     Lo = Hi >>s (Shamt-32);
  //     Hi = Hi >>s (32-1)
  //
  // SRL expansion:
  //   if Shamt-32 < 0: // Shamt < 32
  //     Lo = (Lo >>u Shamt) | ((Hi << 1) << (32-1 - Shamt))
  //     Hi = Hi >>u Shamt
  //   else:
  //     Lo = Hi >>u (Shamt-32);
  //     Hi = 0;

  SDValue Lo = Op.getOperand(0);
  SDValue Hi = Op.getOperand(1);
  SDValue Shamt = Op.getOperand(2);

  // The only differences between the SRA and SRL expansions are the right
  // shift operations: arithmetic for SRA and logical for SRL; and Hi
  unsigned ShrOp = arith ? ISD::SRA : ISD::SRL;

  // Precompute a node with ...
  // ... constant value 0
  SDValue Zero = DAG.getConstant(0, DL, VT);
  // ... constant value 1
  SDValue One = DAG.getConstant(1, DL, VT);
  // ... -32
  SDValue NegWsize = DAG.getConstant(-VTBits, DL, VT);
  // ... 32 - 1
  SDValue WsizeMinus1 = DAG.getConstant(VTBits - 1, DL, VT);
  // ... shamt - 32
  SDValue ShamtMinusWsize = DAG.getNode(ISD::ADD, DL, VT, Shamt, NegWsize);
  // ... (32 - 1) - shamt
  SDValue WsizeMinus1MinusShamt =
      DAG.getNode(ISD::SUB, DL, VT, WsizeMinus1, Shamt);

  // Compute the 'then' part of the if
  // Hi << 1
  SDValue HiShr1 = DAG.getNode(ISD::SHL, DL, VT, Lo, One);
  // (Hi << 1) << (32-1 - Shamt)
  SDValue LoMsb = DAG.getNode(ISD::SRL, DL, VT, HiShr1, WsizeMinus1MinusShamt);
  // Lo >>u Shamt
  SDValue LoLsb = DAG.getNode(ISD::SRL, DL, VT, Lo, Shamt);
  // (Lo >>u Shamt) | ((Hi << 1) << (32-1 - Shamt))
  SDValue LoTrue = DAG.getNode(ISD::OR, DL, VT, LoMsb, LoLsb);
  // Hi >>s Shamt
  SDValue HiTrue = DAG.getNode(ShrOp, DL, VT, Hi, Shamt);

  // Compute the 'else' part of the if
  SDValue LoFalse = DAG.getNode(ShrOp, DL, VT, Hi, ShamtMinusWsize);
  SDValue HiFalse =
      arith ? DAG.getNode(ISD::SRA, DL, VT, Hi, WsizeMinus1) : Zero;

  // Compute the if condition
  SDValue CC = DAG.getSetCC(DL, VT, ShamtMinusWsize, Zero, ISD::SETLT);

  // and (finally) select the results based on the condition!
  Lo = DAG.getNode(ISD::SELECT, DL, VT, CC, LoTrue, LoFalse);
  Hi = DAG.getNode(ISD::SELECT, DL, VT, CC, HiTrue, HiFalse);

  SDValue Ops[2] = {Lo, Hi};
  return DAG.getMergeValues(Ops, DL);
}



SDValue TinyGPUTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDLoc dl(Op);
  return DAG.getNode(ISD::BR_CC, dl, MVT::Other, 
                     Op.getOperand(1), Op.getOperand(0));
}

SDValue TinyGPUTargetLowering::LowerBRCOND(SDValue Op, SelectionDAG &DAG) const {
  // SDLoc dl(Op);
  // SDValue Chain = Op.getOperand(0);
  // SDValue Cond = Op.getOperand(1);
  // SDValue Dest = Op.getOperand(2);

  // if (Cond.getOpcode() == ISD::SETCC) {
  //   SDValue LHS = Cond.getOperand(0);
  //   SDValue RHS = Cond.getOperand(1);
  //   ISD::CondCode CC = cast<CondCodeSDNode>(Cond.getOperand(2))->get();

  //   // Emit CMPOp with glue output
  //   if (CC == ISD::SETLE) {
  //     SDValue Cmp = DAG.getNode(TinyGPUISD::CMP, dl, MVT::Glue, LHS, RHS);
  //     return DAG.getNode(TinyGPUISD::BR, dl, MVT::Other, Chain, Dest, Cmp);
  //   }
  // }

  return SDValue();
}

SDValue
TinyGPUTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:                        llvm_unreachable("unimplemented operand");
  case ISD::GlobalAddress:        return LowerGlobalAddress(Op, DAG);
  case ISD::BlockAddress:         return LowerBlockAddress(Op, DAG);
  case ISD::ConstantPool:         return LowerConstantPool(Op, DAG);
  case ISD::RETURNADDR:           return LowerRETURNADDR(Op, DAG);
  case ISD::SHL_PARTS:            return LowerShlParts(Op, DAG);
  case ISD::SRL_PARTS:            return LowerShrParts(Op, DAG, false);
  case ISD::SRA_PARTS:            return LowerShrParts(Op, DAG, true);
  // case ISD::BRCOND:               return LowerBRCOND(Op, DAG);
  // case ISD::BR_CC:                return LowerBR_CC(Op, DAG);
  }
}
