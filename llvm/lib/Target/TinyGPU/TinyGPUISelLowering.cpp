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
#include <cassert>

using namespace llvm;

#define DEBUG_TYPE "TinyGPU-isellower"

#include "TinyGPUGenCallingConv.inc"

// Constructor for TinyGPUTargetLowering
// This initializes the target lowering for the TinyGPU target, setting up
// register classes, operation actions, and other target-specific properties.
TinyGPUTargetLowering::TinyGPUTargetLowering(const TargetMachine &TM,
                     const TinyGPUSubtarget &STI)
  : TargetLowering(TM), Subtarget(STI)
{
  // Set up the register classes for the target
  addRegisterClass(MVT::i32, &TinyGPU::GPRRegClass);

  // Compute derived register properties after adding all register classes
  computeRegisterProperties(Subtarget.getRegisterInfo());

  // Set the scheduling preference to prioritize register pressure
  setSchedulingPreference(Sched::RegPressure);

  // Specify the stack pointer register for save/restore operations
  setStackPointerRegisterToSaveRestore(TinyGPU::R2);

  // Use i32 for the results of setcc operations (e.g., slt, sgt)
  setBooleanContents(ZeroOrOneBooleanContent);

  // Set actions for arithmetic operations
  setOperationAction(ISD::SDIVREM,   MVT::i32, Expand); // Expand signed division and remainder
  setOperationAction(ISD::UDIVREM,   MVT::i32, Expand); // Expand unsigned division and remainder
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand); // Expand signed multiplication (low/high)
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand); // Expand unsigned multiplication (low/high)

  // Set custom lowering for shift operations
  setOperationAction(ISD::SHL_PARTS, MVT::i32, Custom); // Custom lowering for shift left
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Custom); // Custom lowering for logical shift right
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Custom); // Custom lowering for arithmetic shift right

  // Expand other operations that are not natively supported
  setOperationAction(ISD::ROTL,  MVT::i32, Expand); // Expand rotate left
  setOperationAction(ISD::ROTR,  MVT::i32, Expand); // Expand rotate right
  setOperationAction(ISD::BSWAP, MVT::i32, Expand); // Expand byte swap
  setOperationAction(ISD::CTTZ,  MVT::i32, Expand); // Expand count trailing zeros
  setOperationAction(ISD::CTLZ,  MVT::i32, Expand); // Expand count leading zeros
  setOperationAction(ISD::CTPOP, MVT::i32, Expand); // Expand count population (number of 1 bits)

  // Set custom lowering for address resolution and constant pool
  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom); // Custom lowering for global addresses
  setOperationAction(ISD::BlockAddress,  MVT::i32, Custom); // Custom lowering for block addresses
  setOperationAction(ISD::ConstantPool,  MVT::i32, Custom); // Custom lowering for constant pool

  // Set minimum and preferred function alignment (log2)
  setMinFunctionAlignment(Align(1)); // Minimum alignment of 2^1 = 2 bytes
  setPrefFunctionAlignment(Align(1)); // Preferred alignment of 2^1 = 2 bytes

  // Set preferred loop alignment (log2)
  setPrefLoopAlignment(Align(1)); // Preferred loop alignment of 2^1 = 2 bytes
}

SDValue TinyGPUTargetLowering::LowerINTRINSIC_WO_CHAIN(
    SDValue Op, SelectionDAG &DAG) const {
  unsigned IntrinsicID = cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue();
  
  // switch (IntrinsicID) {
  //  case Intrinsic::tinygpu_cmp: { 
  //   SDLoc dl(Op);
  //   SDValue Op1 = Op.getOperand(1);
  //   SDValue Op2 = Op.getOperand(2);
  //   return DAG.getNode(TinyGPU::CMP, dl, MVT::Glue, Op1, Op2);
  //   // return SDValue();
  // }
  // }
  return SDValue();
}

/// getTargetNodeName - This method returns the name of a target-specific 
/// SelectionDAG node based on its opcode. It is used for debugging and 
/// visualization purposes to provide meaningful names for custom opcodes.
const char *TinyGPUTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case TinyGPUISD::Ret: 
    // Return the name for the TinyGPUISD::Ret opcode, which represents a return instruction.
    return "TinyGPUISD::Ret";
  case TinyGPUISD::BRNCZ: 
    // Return the name for the TinyGPUISD::BRNCZ opcode, which represents a branch instruction.
    return "TinyGPUISD::BRNCZ";
  default:            
    // Return NULL for unknown opcodes.
    return NULL;
  }
}

/// ReplaceNodeResults - This method is used to replace the results of a node
/// with custom target-specific nodes during SelectionDAG lowering. It is
/// typically used when a target wants to provide custom expansion or
/// transformation for certain operations.
///
/// \param N - The node whose results need to be replaced.
/// \param Results - A vector to store the new results of the node.
/// \param DAG - The SelectionDAG being processed.
void TinyGPUTargetLowering::ReplaceNodeResults(SDNode *N,
                       SmallVectorImpl<SDValue> &Results,
                       SelectionDAG &DAG) const {
  switch (N->getOpcode()) {
  default:
  // If the opcode is not recognized, this indicates that the target does not
  // know how to handle this operation. Emit an error in such cases.
  llvm_unreachable("Don't know how to custom expand this!");
  }
}

//===----------------------------------------------------------------------===//
//@            Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

// The BeyondRISC calling convention parameter registers.
// Define the general-purpose registers (GPRs) used for passing arguments
// in the TinyGPU calling convention. These registers are used in order
// for passing function arguments before spilling to the stack.
static const MCPhysReg GPRArgRegs[] = {
  TinyGPU::R0, // First argument register
  TinyGPU::R1, // Second argument register
  TinyGPU::R2, // Third argument register
  TinyGPU::R3  // Fourth argument register
};

/// LowerFormalArguments - This function is responsible for lowering the formal
/// arguments of a function into the SelectionDAG. It maps physical registers
/// into virtual registers and generates load operations for arguments placed
/// on the stack.
///
/// \param Chain - The incoming chain for the DAG.
/// \param CallConv - The calling convention used for the function.
/// \param isVarArg - Indicates if the function is variadic.
/// \param Ins - The list of input arguments.
/// \param dl - The source location for debugging.
/// \param DAG - The SelectionDAG being constructed.
/// \param InVals - A vector to store the lowered argument values.
///
/// \returns The updated chain after processing the arguments.
SDValue TinyGPUTargetLowering::LowerFormalArguments(
                  SDValue Chain,
                  CallingConv::ID CallConv,
                  bool isVarArg,
                  const SmallVectorImpl<ISD::InputArg> &Ins,
                  const SDLoc &dl, SelectionDAG &DAG,
                  SmallVectorImpl<SDValue> &InVals) const {
  // Ensure the calling convention is supported.
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

  // Determine the starting register for arguments passed in registers.
  unsigned ArgRegBegin = TinyGPU::R4;
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    // Check if all in-register parameters have been processed.
    if (CCInfo.getInRegsParamsProcessed() >= CCInfo.getInRegsParamsCount())
      break;

    CCValAssign &VA = ArgLocs[i];
    unsigned Index = VA.getValNo();
    ISD::ArgFlagsTy Flags = Ins[Index].Flags;

    // Skip arguments that are not passed by value.
    if (!Flags.isByVal())
      continue;

    // Ensure the argument is stored in memory (byval pointer).
    assert(VA.isMemLoc() && "unexpected byval pointer in reg");

    // Retrieve the range of registers used for the current byval argument.
    unsigned RBegin, REnd;
    CCInfo.getInRegsParamInfo(CCInfo.getInRegsParamsProcessed(), RBegin, REnd);

    // Update the starting register for arguments passed in registers.
    ArgRegBegin = std::min(ArgRegBegin, RBegin);

    // Move to the next in-register parameter.
    CCInfo.nextInRegsParam();
  }

  // Rewind the in-register parameter information for further processing.
  CCInfo.rewindByValRegsInfo();

  // Handle variadic arguments if applicable.
  int lastInsIndex = -1;
  if (isVarArg && MFI.hasVAStart()) {
    // Find the first unallocated register for variadic arguments.
    unsigned RegIdx = CCInfo.getFirstUnallocated(GPRArgRegs);
    if (RegIdx != std::size(GPRArgRegs))
      ArgRegBegin = std::min(ArgRegBegin, (unsigned)GPRArgRegs[RegIdx]);
  }

  // Process each argument location.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];

    // Check if the argument corresponds to an original function argument.
    if (Ins[VA.getValNo()].isOrigArg()) {
      // Advance to the original argument index.
      std::advance(CurOrigArg,
                   Ins[VA.getValNo()].getOrigArgIndex() - CurArgIdx);
      CurArgIdx = Ins[VA.getValNo()].getOrigArgIndex();
    }

    // Handle arguments stored in registers.
    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();

      // Ensure no custom value assignment is required.
      if (VA.needsCustom()) {
        llvm_unreachable("Custom val assignment not supported by "
                         "FORMAL_ARGUMENTS Lowering");
      } else {
        const TargetRegisterClass *RC;

        // Determine the register class based on the value type.
        if (RegVT == MVT::i32)
          RC = &TinyGPU::GPRRegClass;
        else
          llvm_unreachable("RegVT not supported by FORMAL_ARGUMENTS Lowering");

        // Transform the arguments in physical registers into virtual ones.
        unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
        ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);
      }

      // Handle cases where the argument is promoted to a larger type.
      switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: 
        // No transformation needed.
        break;
      case CCValAssign::BCvt:
        // Perform bitcast to the expected value type.
        ArgValue = DAG.getNode(ISD::BITCAST, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::SExt:
        // Sign-extend the argument and truncate to the expected value type.
        ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::ZExt:
        // Zero-extend the argument and truncate to the expected value type.
        ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      }

      // Store the processed argument value.
      InVals.push_back(ArgValue);
    } else { 
      // Handle arguments stored in memory.
      assert(VA.isMemLoc());
      assert(VA.getValVT() != MVT::i64 && "i64 should already be lowered");

      int index = VA.getValNo();

      // Ensure each input argument is processed only once.
      if (index != lastInsIndex) {
        llvm_unreachable("Cannot retrieve arguments from the stack");
      }
    }
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

// Check if the return values can be lowered for the given calling convention
bool TinyGPUTargetLowering::CanLowerReturn(CallingConv::ID CallConv,
                MachineFunction &MF, bool isVarArg,
                const SmallVectorImpl<ISD::OutputArg> &Outs,
                LLVMContext &Context) const {
  // Analyze the return value locations
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);

  // Check if the return values fit within the calling convention
  return CCInfo.CheckReturn(Outs, TinyGPU_CRetConv);
}

// Lower a function call into the SelectionDAG
SDValue TinyGPUTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                     SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG; // Reference to the SelectionDAG
  SDLoc DL = CLI.DL;           // Debug location for the call
  SmallVector<SDValue, 8> Ops; // Vector to store operands for the call

// Handle OpenCL built-ins
if (CLI.CB && CLI.CB->getCalledFunction()) {
  StringRef Name = CLI.CB->getCalledFunction()->getName();
  
  if (Name.starts_with("_Z")) { // OpenCL built-ins
    SDValue Dim = CLI.Args[0].Node;
    bool isDim0 = false;
    if (auto *C = dyn_cast<ConstantSDNode>(Dim))
      isDim0 = (C->getZExtValue() == 0);

    // Handle get_local_id (direct register access)
    if (Name == "_Z12get_local_idj") {
      if (isDim0) {
        InVals.push_back(DAG.getRegister(TinyGPU::R15, MVT::i32)); // threadIdx
        return CLI.Chain;
      }
    }
    // Handle get_group_id (direct register access)
    else if (Name == "_Z12get_group_idj") {
      if (isDim0) {
        InVals.push_back(DAG.getRegister(TinyGPU::R13, MVT::i32)); // blockIdx
        return CLI.Chain;
      }
    }
    // Handle get_local_size (direct register access)
    else if (Name == "_Z14get_local_sizej") {
      if (isDim0) {
        InVals.push_back(DAG.getRegister(TinyGPU::R14, MVT::i32)); // blockDim
        return CLI.Chain;
      }
    }
    // Handle get_global_id (requires calculation)
    else if (Name == "_Z13get_global_idj") {
      if (isDim0) {
        // global_id = group_id * group_size + local_id
        SDValue GroupID = DAG.getRegister(TinyGPU::R13, MVT::i32);
        SDValue GroupSize = DAG.getRegister(TinyGPU::R14, MVT::i32);
        SDValue LocalID = DAG.getRegister(TinyGPU::R15, MVT::i32);
        
        // Multiply group ID by group size
        SDValue Mul = DAG.getNode(ISD::MUL, DL, MVT::i32, GroupID, GroupSize);
        // Add local ID
        SDValue GlobalID = DAG.getNode(ISD::ADD, DL, MVT::i32, Mul, LocalID);
        
        InVals.push_back(GlobalID);
        return CLI.Chain;
      }
    }
  }
}

  // 1. Handle Arguments
  for (unsigned i = 0; i < CLI.Args.size(); ++i) {
    const TargetLoweringBase::ArgListEntry &Arg = CLI.Args[i]; // Get the argument
    SDValue ArgValue = Arg.Node; // Extract the SDValue from the argument

    // Assign the argument to physical registers R0, R1, etc.
    unsigned Reg = TinyGPU::R0 + i;
    SDValue Chain = CLI.Chain;

    // Copy the argument value to the physical register
    Chain = DAG.getCopyToReg(Chain, DL, Reg, ArgValue);
    Ops.push_back(Chain); // Add the updated chain to the operands
  }

  // 2. Add Callee (function address)
  GlobalAddressSDNode *G = cast<GlobalAddressSDNode>(CLI.Callee); // Get the callee as a global address
  SDValue Callee = DAG.getTargetGlobalAddress(G->getGlobal(), DL, MVT::i32); // Create a target-specific global address
  Ops.push_back(Callee); // Add the callee to the operands

  // 3. Create BR node for the call
  Ops.push_back(CLI.Chain); // Add the chain as the last operand
  SDVTList VTs = DAG.getVTList(MVT::Other); // Specify the return value type list
  SDValue Call = DAG.getNode(TinyGPUISD::BRNCZ2, DL, VTs, Ops); // Create the call node

  // 4. Handle return value (if any)
  if (!CLI.RetTy->isVoidTy()) {
    // Convert the LLVM return type to an EVT
    EVT RetVT = getValueType(DAG.getDataLayout(), CLI.RetTy);
    // Get the return value from the appropriate register (R0)
    SDValue RetVal = DAG.getRegister(TinyGPU::R0, RetVT);
    InVals.push_back(RetVal); // Add the return value to the input values
  }


  return Call; // Return the call node
}

/// LowerReturn - This function is responsible for lowering the return values
/// of a function into the SelectionDAG. It assigns return values to physical
/// registers and generates the necessary instructions to return from the
/// function.
///
/// \param Chain - The incoming chain for the DAG.
/// \param CallConv - The calling convention used for the function.
/// \param isVarArg - Indicates if the function is variadic.
/// \param Outs - The list of output arguments (return values).
/// \param OutVals - The list of SDValues corresponding to the return values.
/// \param dl - The source location for debugging.
/// \param DAG - The SelectionDAG being constructed.
///
/// \returns The updated chain after processing the return values.
SDValue
TinyGPUTargetLowering::LowerReturn(SDValue Chain,
                 CallingConv::ID CallConv, bool isVarArg,
                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                 const SmallVectorImpl<SDValue> &OutVals,
                 const SDLoc &dl, SelectionDAG &DAG) const {
  // CCValAssign - Represents the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Holds information about the registers and stack slots.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
         *DAG.getContext());

  // Analyze the outgoing return values and assign them to locations.
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

  // Retrieve the return value.
  SDValue Arg = OutVals[realRVLocIdx];
  bool ReturnF16 = false; // Indicates if the return value is f16.

  // Handle different location information for the return value.
  switch (VA.getLocInfo()) {
  default: 
    llvm_unreachable("Unknown loc info!");
  case CCValAssign::Full: 
    // No transformation needed.
    break;
  case CCValAssign::BCvt:
    // Perform bitcast if required.
    if (!ReturnF16)
    Arg = DAG.getNode(ISD::BITCAST, dl, VA.getLocVT(), Arg);
    break;
  }

  // Handle custom value assignments (not supported here).
  if (VA.needsCustom()) {
    llvm_unreachable("Custom val assignment not supported by "
             "RETURN Lowering");
  } else {
    // Copy the return value to the assigned register.
    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), Arg, Flag);
  }

  // Ensure all emitted copies are contiguous to avoid issues.
  Flag = Chain.getValue(1);
  RetOps.push_back(DAG.getRegister(VA.getLocReg(),
                   ReturnF16 ? MVT::f16 : VA.getLocVT()));
  }

  // Update the chain and glue.
  RetOps[0] = Chain;
  if (Flag.getNode())
  RetOps.push_back(Flag);

  // Create the return node with the updated chain and return values.
  return DAG.getNode(TinyGPUISD::Ret2, dl, MVT::Other, RetOps);
}

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

/// LowerGlobalAddress - This function is responsible for lowering a global
/// address into a target-specific SelectionDAG node. It converts the global
/// address into a target-specific representation that can be used during
/// code generation.
///
/// \param Op - The SDValue representing the global address.
/// \param DAG - The SelectionDAG being constructed.
///
/// \returns A target-specific global address node.
SDValue
TinyGPUTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const {
  // Retrieve the global value associated with the global address node.
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();

  // Get the value type of the pointer.
  EVT PtrVT = Op.getValueType();

  // Create a target-specific global address node for the given global value.
  return DAG.getTargetGlobalAddress(GV, SDLoc(Op), PtrVT);
}

/// LowerBlockAddress - This function is responsible for lowering a block
/// address into a target-specific SelectionDAG node. It converts the block
/// address into a target-specific representation that can be used during
/// code generation.
///
/// \param Op - The SDValue representing the block address.
/// \param DAG - The SelectionDAG being constructed.
///
/// \returns A target-specific block address node.
SDValue
TinyGPUTargetLowering::LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const {
  // Emit an error as block addresses are not supported for this target.
  llvm_unreachable("Unsupported block address");
}

/// LowerConstantPool - This function is responsible for lowering a constant
/// pool entry into a target-specific SelectionDAG node. It converts the
/// constant pool entry into a target-specific representation that can be
/// used during code generation.
///
/// \param Op - The SDValue representing the constant pool entry.
/// \param DAG - The SelectionDAG being constructed.
///
/// \returns A target-specific constant pool node.
SDValue
TinyGPUTargetLowering::LowerConstantPool(SDValue Op, SelectionDAG &DAG) const {
  // Emit an error as constant pool entries are not supported for this target.
  llvm_unreachable("Unsupported constant pool");
}

/// LowerRETURNADDR - This function is responsible for lowering the RETURNADDR
/// operation into a target-specific SelectionDAG node. It is used to retrieve
/// the return address of the current or a parent function frame.
///
/// \param Op - The SDValue representing the RETURNADDR operation.
/// \param DAG - The SelectionDAG being constructed.
///
/// \returns A target-specific node or an empty SDValue if unsupported.
SDValue
TinyGPUTargetLowering::LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const {
  // Currently, the TinyGPU target does not support retrieving the return
  // address. Return an empty SDValue to indicate this.
  return SDValue();
}

/// LowerShlParts - This function is responsible for lowering a long shift left
/// operation (SHL_PARTS) into a target-specific SelectionDAG node. It handles
/// the case where the shift amount is greater than the bit width of the type,
/// splitting the operation into parts.
///
/// \param Op - The SDValue representing the SHL_PARTS operation.
/// \param DAG - The SelectionDAG being constructed.
///
/// \returns A target-specific node representing the lowered SHL_PARTS operation.
SDValue
TinyGPUTargetLowering::LowerShlParts(SDValue Op, SelectionDAG &DAG) const {
  assert(Op.getNumOperands() == 3 && "Not a long shift");

  EVT VT = Op.getValueType(); // Get the value type of the operands.
  unsigned VTBits = VT.getSizeInBits(); // Get the bit width of the type.
  SDLoc DL(Op); // Debug location for the operation.

  // if Shamt-32 < 0: // Shamt < 32
  //   Lo = Lo << Shamt
  //   Hi = (Hi << Shamt) | ((Lo >>u 1) >>u (32-1 - Shamt))
  // else:
  //   Lo = 0
  //   Hi = Lo << (Shamt-32)

  SDValue Lo = Op.getOperand(0); // Low part of the value.
  SDValue Hi = Op.getOperand(1); // High part of the value.
  SDValue Shamt = Op.getOperand(2); // Shift amount.

  // Precompute constants and intermediate values.
  SDValue Zero = DAG.getConstant(0, DL, VT); // Constant value 0.
  SDValue One = DAG.getConstant(1, DL, VT); // Constant value 1.
  SDValue NegWsize = DAG.getConstant(-VTBits, DL, VT); // -32.
  SDValue WsizeMinus1 = DAG.getConstant(VTBits - 1, DL, VT); // 32 - 1.
  SDValue ShamtMinusWsize = DAG.getNode(ISD::ADD, DL, VT, Shamt, NegWsize); // shamt - 32.
  SDValue WsizeMinus1MinusShamt =
      DAG.getNode(ISD::SUB, DL, VT, WsizeMinus1, Shamt); // (32 - 1) - shamt.

  // Compute the 'then' part of the if (Shamt < 32).
  SDValue LoTrue = DAG.getNode(ISD::SHL, DL, VT, Lo, Shamt); // Lo << Shamt.
  SDValue LoShr1 = DAG.getNode(ISD::SRL, DL, VT, Lo, One); // Lo >> 1.
  SDValue HiLsb = DAG.getNode(ISD::SRL, DL, VT, LoShr1, WsizeMinus1MinusShamt); // (Lo >> 1) >> (32-1 - Shamt).
  SDValue HiMsb = DAG.getNode(ISD::SHL, DL, VT, Hi, Shamt); // Hi << Shamt.
  SDValue HiTrue = DAG.getNode(ISD::OR, DL, VT, HiMsb, HiLsb); // (Hi << Shamt) | ((Lo >>u 1) >>u (32-1 - Shamt)).

  // Compute the 'else' part of the if (Shamt >= 32).
  SDValue LoFalse = Zero; // Lo = 0.
  SDValue HiFalse = DAG.getNode(ISD::SHL, DL, VT, Lo, ShamtMinusWsize); // Hi = Lo << (Shamt-32).

  // Compute the if condition (Shamt < 32).
  SDValue CC = DAG.getSetCC(DL, VT, ShamtMinusWsize, Zero, ISD::SETLT);

  // Select the results based on the condition.
  Lo = DAG.getNode(ISD::SELECT, DL, VT, CC, LoTrue, LoFalse);
  Hi = DAG.getNode(ISD::SELECT, DL, VT, CC, HiTrue, HiFalse);

  // Merge the results into a single SDValue.
  SDValue Ops[2] = {Lo, Hi};
  return DAG.getMergeValues(Ops, DL);
}

/// LowerShrParts - This function is responsible for lowering a long shift
/// right operation (SRL_PARTS or SRA_PARTS) into a target-specific
/// SelectionDAG node. It handles the case where the shift amount is greater
/// than the bit width of the type, splitting the operation into parts.
///
/// \param Op - The SDValue representing the SHR_PARTS operation.
/// \param DAG - The SelectionDAG being constructed.
/// \param arith - A boolean indicating whether the shift is arithmetic (true)
///                or logical (false).
///
/// \returns A target-specific node representing the lowered SHR_PARTS operation.
SDValue
TinyGPUTargetLowering::LowerShrParts(SDValue Op, SelectionDAG &DAG,
                   bool arith) const {
  assert(Op.getNumOperands() == 3 && "Not a long shift");

  EVT VT = Op.getValueType(); // Get the value type of the operands.
  unsigned VTBits = VT.getSizeInBits(); // Get the bit width of the type.
  SDLoc DL(Op); // Debug location for the operation.

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

  SDValue Lo = Op.getOperand(0); // Low part of the value.
  SDValue Hi = Op.getOperand(1); // High part of the value.
  SDValue Shamt = Op.getOperand(2); // Shift amount.

  // Determine the shift operation: arithmetic (SRA) or logical (SRL).
  unsigned ShrOp = arith ? ISD::SRA : ISD::SRL;

  // Precompute constants and intermediate values.
  SDValue Zero = DAG.getConstant(0, DL, VT); // Constant value 0.
  SDValue One = DAG.getConstant(1, DL, VT); // Constant value 1.
  SDValue NegWsize = DAG.getConstant(-VTBits, DL, VT); // -32.
  SDValue WsizeMinus1 = DAG.getConstant(VTBits - 1, DL, VT); // 32 - 1.
  SDValue ShamtMinusWsize = DAG.getNode(ISD::ADD, DL, VT, Shamt, NegWsize); // shamt - 32.
  SDValue WsizeMinus1MinusShamt =
    DAG.getNode(ISD::SUB, DL, VT, WsizeMinus1, Shamt); // (32 - 1) - shamt.

  // Compute the 'then' part of the if (Shamt < 32).
  SDValue HiShr1 = DAG.getNode(ISD::SHL, DL, VT, Lo, One); // Hi << 1.
  SDValue LoMsb = DAG.getNode(ISD::SRL, DL, VT, HiShr1, WsizeMinus1MinusShamt); // (Hi << 1) >> (32-1 - Shamt).
  SDValue LoLsb = DAG.getNode(ISD::SRL, DL, VT, Lo, Shamt); // Lo >>u Shamt.
  SDValue LoTrue = DAG.getNode(ISD::OR, DL, VT, LoMsb, LoLsb); // (Lo >>u Shamt) | ((Hi << 1) >> (32-1 - Shamt)).
  SDValue HiTrue = DAG.getNode(ShrOp, DL, VT, Hi, Shamt); // Hi >>s/u Shamt.

  // Compute the 'else' part of the if (Shamt >= 32).
  SDValue LoFalse = DAG.getNode(ShrOp, DL, VT, Hi, ShamtMinusWsize); // Lo = Hi >>s/u (Shamt-32).
  SDValue HiFalse =
    arith ? DAG.getNode(ISD::SRA, DL, VT, Hi, WsizeMinus1) : Zero; // Hi = Hi >>s (32-1) or 0.

  // Compute the if condition (Shamt < 32).
  SDValue CC = DAG.getSetCC(DL, VT, ShamtMinusWsize, Zero, ISD::SETLT);

  // Select the results based on the condition.
  Lo = DAG.getNode(ISD::SELECT, DL, VT, CC, LoTrue, LoFalse);
  Hi = DAG.getNode(ISD::SELECT, DL, VT, CC, HiTrue, HiFalse);

  // Merge the results into a single SDValue.
  SDValue Ops[2] = {Lo, Hi};
  return DAG.getMergeValues(Ops, DL);
}


/// LowerOperation - This function is responsible for lowering target-specific
/// operations into SelectionDAG nodes. It handles various operations that
/// require custom lowering for the TinyGPU target.
///
/// \param Op - The SDValue representing the operation to be lowered.
/// \param DAG - The SelectionDAG being constructed.
///
/// \returns A target-specific node representing the lowered operation.
SDValue
TinyGPUTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    // Emit an error for unimplemented operands.
    llvm_unreachable("unimplemented operand");

  case ISD::GlobalAddress:
    // Lower a global address into a target-specific node.
    return LowerGlobalAddress(Op, DAG);

  case ISD::BlockAddress:
    // Lower a block address into a target-specific node.
    return LowerBlockAddress(Op, DAG);

  case ISD::ConstantPool:
    // Lower a constant pool entry into a target-specific node.
    return LowerConstantPool(Op, DAG);

  case ISD::RETURNADDR:
    // Lower the RETURNADDR operation into a target-specific node.
    return LowerRETURNADDR(Op, DAG);

  case ISD::SHL_PARTS:
    // Lower a long shift left operation (SHL_PARTS) into a target-specific node.
    return LowerShlParts(Op, DAG);

  case ISD::SRL_PARTS:
    // Lower a long logical shift right operation (SRL_PARTS) into a target-specific node.
    return LowerShrParts(Op, DAG, false);

  case ISD::SRA_PARTS:
    // Lower a long arithmetic shift right operation (SRA_PARTS) into a target-specific node.
    return LowerShrParts(Op, DAG, true);
  }
}
