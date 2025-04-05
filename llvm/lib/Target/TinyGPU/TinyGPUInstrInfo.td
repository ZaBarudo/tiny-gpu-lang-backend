//== TinyGPUInstrInfo.td - Target Description for TinyGPU Target -*- tablegen -*-=//
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

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

// Procedure return
def TinyGPU_ret : SDNode<"TinyGPUISD::Ret", SDTNone,
                       [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def TinyGPU_brncz : SDNode<"TinyGPUISD::BRNCZ", SDTNone,
                       [SDNPHasChain, SDNPOutGlue]>;

// Procedure calling
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                    SDTCisVT<1, i32>]>;

// These are target-independent nodes, but have target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

//===----------------------------------------------------------------------===//
// Operand and SDNode transformation definitions.
//===----------------------------------------------------------------------===//

class ImmAsmOperand<string prefix, int width, string suffix> : AsmOperandClass {
  let Name = prefix # "Imm" # width # suffix;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", Name);
}

class SImmAsmOperand<int width, string suffix = "">
    : ImmAsmOperand<"S", width, suffix> {
}

class UImmAsmOperand<int width, string suffix = "">
    : ImmAsmOperand<"U", width, suffix> {
}

def UImmLog2WSizeAsmOperand : AsmOperandClass {
  let Name = "UImmLog2WSize";
  let RenderMethod = "addImmOperands";
  let DiagnosticType = "InvalidUImmLog2WSize";
}

def uimmlog2wsize : Operand<i32>, ImmLeaf<i32, [{
  return isUInt<5>(Imm);
}]> {
  let ParserMatchClass = UImmLog2WSizeAsmOperand;
  // TODO: should ensure invalid shamt is rejected when decoding.
  let DecoderMethod = "decodeUImmOperand<5>";
  let MCOperandPredicate = [{
    int64_t Imm;
    if (!MCOp.evaluateAsConstantImm(Imm))
      return false;
    return isUInt<5>(Imm);
  }];
  let OperandType = "OPERAND_UIMMLOG2WSIZE";
  let OperandNamespace = "TinyGPUOp";
}

def simm12 : Operand<i32>, ImmLeaf<i32, [{return isInt<12>(Imm);}]> {
  let ParserMatchClass = SImmAsmOperand<12>;
  let EncoderMethod = "getImmOpValue";
  let DecoderMethod = "decodeSImmOperand<12>";
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<12>(Imm);
    return MCOp.isBareSymbolRef();
  }];
  let OperandType = "OPERAND_SIMM12";
  let OperandNamespace = "TinyGPUOp";
}

class UImm20Operand : Operand<i32> {
  let EncoderMethod = "getImmOpValue";
  let DecoderMethod = "decodeUImmOperand<20>";
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isUInt<20>(Imm);
    return MCOp.isBareSymbolRef();
  }];
  let OperandType = "OPERAND_UIMM20";
  let OperandNamespace = "TinyGPUOp";
}

def uimm20_lui : UImm20Operand {
  let ParserMatchClass = UImmAsmOperand<20, "LUI">;
}
def uimm20_auipc : UImm20Operand {
  let ParserMatchClass = UImmAsmOperand<20, "AUIPC">;
}

// Standalone (codegen-only) immleaf patterns.
def simm32     : ImmLeaf<i32, [{return isInt<32>(Imm);}]>;
def simm32hi20 : ImmLeaf<i32, [{return isShiftedInt<20, 12>(Imm);}]>;
// A mask value that won't affect significant shift bits.
def immbottomwsizeset : ImmLeaf<i32, [{
  return countr_one<uint64_t>(Imm) >= 5;
}]>;

def simm13_lsb0 : Operand<OtherVT> {
  let ParserMatchClass = SImmAsmOperand<13>;
  // let EncoderMethod = "getImmOpValue";
  // let DecoderMethod = "decodeSImmOperand<13>";
  // let MCOperandPredicate = [{
  //   int64_t Imm;
  //   if (MCOp.evaluateAsConstantImm(Imm))
  //     return isShiftedInt<12, 1>(Imm);
  //   return MCOp.isBareSymbolRef();
  // }];
  let OperandType = "OPERAND_SIMM12";
  let OperandNamespace = "TinyGPUOp";
  let PrintMethod = "printBrTarget";
}

// Extract least significant 12 bits from an immediate value and sign extend
// them.
def LO12Sext : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(SignExtend64<12>(N->getZExtValue()),
                                   SDLoc(N), N->getValueType(0));
}]>;

// Extract the most significant 20 bits from an immediate value. Add 1 if bit
// 11 is 1, to compensate for the low 12 bits in the matching immediate addi
// or ld/st being negative.
def HI20 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(((N->getZExtValue()+0x800) >> 12) & 0xfffff,
                                   SDLoc(N), N->getValueType(0));
}]>;

//===----------------------------------------------------------------------===//
// Instruction Formats
//===----------------------------------------------------------------------===//

include "TinyGPUInstrFormats.td"

//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU_ri<bits<3> funct3, string opcodestr>
    : RWInstI<funct3, OPC_OP_IMM, (outs GPR:$rd), (ins GPR:$rs1, simm12:$imm12),
              opcodestr, "$rd, $rs1, $imm12">,
      Sched<[WriteIALU, ReadIALU]>;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class Shift_ri<bit arithshift, bits<3> funct3, string opcodestr>
    : RWInstIShift<arithshift, funct3, OPC_OP_IMM, (outs GPR:$rd),
                   (ins GPR:$rs1, uimmlog2wsize:$shamt), opcodestr,
                   "$rd, $rs1, $shamt">,
      Sched<[WriteShift, ReadShift]>;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU_rr<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWInstR<funct7, funct3, OPC_OP, (outs GPR:$rd), (ins GPR:$rs1, GPR:$rs2),
              opcodestr, "$rd, $rs1, $rs2">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class CMP_rr<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWInstR<funct7, funct3, OPC_OP, (outs GPR:$rd), (ins GPR:$rs1, GPR:$rs2),
              opcodestr, "$rs1, $rs2">;

let hasSideEffects = 0, mayLoad = 1, mayStore = 0 in
class LDR_rr<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWInstRR<funct7, funct3, OPC_LOAD, (outs GPR:$rd), (ins GPR:$rs),
              opcodestr, "$rd, $rs">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class BRANCH_r<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWJInstR<funct7, funct3, OPC_OP, (outs), (ins simm13_lsb0:$rs),
              opcodestr, "$rs">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class BRANCH_rtg<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWJInstR<funct7, funct3, OPC_OP, (outs), (ins i32imm:$rs),
              opcodestr, "$rs">;

class BRANCH_r2<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWJInstR2<funct7, funct3, OPC_OP, (outs), (ins GPR:$rs2, simm13_lsb0:$rs),
              opcodestr, "$rs">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
class STR_rr<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWInstRR<funct7, funct3, OPC_STORE, (outs), (ins GPR:$rd, GPR:$rs),
              opcodestr, "$rd, $rs">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
class STR_ir<bits<3> funct3, string opcodestr>
    : RWInstRI<funct3, OPC_OP_IMM, (outs), (ins simm12:$imm12, GPR:$rs),
              opcodestr, "$rs, #$imm12">,Sched<[WriteIALU]>;

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
class ALLOCA_i<bits<3> funct3, string opcodestr>
    : RWInstRI<funct3, OPC_OP_IMM, (outs GPR:$rs), (ins simm12:$imm12),
              opcodestr, "$rs , #$imm12">,Sched<[WriteIALU]>;
//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
let isReMaterializable = 1, isAsCheapAsAMove = 1 in
def LUI : RWInstU<OPC_LUI, (outs GPR:$rd), (ins uimm20_lui:$imm20),
                  "lui", "$rd, $imm20">, Sched<[WriteIALU]>;

def AUIPC : RWInstU<OPC_AUIPC, (outs GPR:$rd), (ins uimm20_auipc:$imm20),
                    "auipc", "$rd, $imm20">, Sched<[WriteIALU]>;
} // hasSideEffects = 0, mayLoad = 0, mayStore = 0

// ADDI isn't always rematerializable, but isReMaterializable will be used as
// a hint which is verified in isReallyTriviallyReMaterializable.


// Arithmetic instructions
let isReMaterializable = 1, isAsCheapAsAMove = 1 in{
  def ADDI  : ALU_ri<0b000, "ADDI">;
  def ADDri  : ALU_ri<0b000, "addi">;
  def SUBri  : ALU_ri<0b000, "subi">;
}

def ADD  : ALU_rr<0b0000000, 0b000, "ADD">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
def ADDrr  : ALU_rr<0b0000000, 0b000, "ADD">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
def SUB  : ALU_rr<0b0100000, 0b000, "SUB">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
def SUBrr  : ALU_rr<0b0100000, 0b000, "SUB">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

def MUL     : ALU_rr<0b0000001, 0b000, "MUL">, Sched<[WriteIMul, ReadIMul, ReadIMul]>;
def DIV     : ALU_rr<0b0000001, 0b100, "DIV">,
              Sched<[WriteIDiv, ReadIDiv, ReadIDiv]>;

// Load, Store and Constant Instructions 
def LDR : LDR_rr<0b0000010, 0b000, "LDR">,
              Sched<[WriteLoad, ReadLoad]>;
def STR : STR_rr<0b0000010, 0b001, "STR">,
              Sched<[ReadLoad, WriteLoad]>;
def CONST : STR_ir<0b001,"CONST">;


// Branch instructions
def BRNCH : BRANCH_r<0b0000011, 0b000, "BR">,
              Sched<[ReadLoad]>;
def BRNCH_RTG : BRANCH_rtg<0b0000011, 0b000, "BR">,
              Sched<[ReadLoad]>;
def BRNCH_2 : BRANCH_r2<0b0000011, 0b000, "BR">,
              Sched<[ReadLoad]>;
def BRNCH_3 : BRANCH_r2<0b0000011, 0b000, "BR">,
              Sched<[ReadLoad]>;

// Compare Statements
def CMP  : CMP_rr<0b0000000, 0b000, "CMP">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// Branch instructions with different conditions
def BRNCHN : BRANCH_r2<0b0000011, 0b000, "BRn">,
              Sched<[ReadLoad]>;
def BRNCHP : BRANCH_r2<0b0000011, 0b000, "BRp">,
              Sched<[ReadLoad]>;
def BRNCHZ : BRANCH_r2<0b0000011, 0b000, "BRz">,
              Sched<[ReadLoad]>;
def BRNCHNP : BRANCH_r2<0b0000011, 0b000, "BRnp">,
              Sched<[ReadLoad]>;
def BRNCHNZ : BRANCH_r2<0b0000011, 0b000, "BRnz">,
              Sched<[ReadLoad]>;
def BRNCHPZ : BRANCH_r2<0b0000011, 0b000, "BRpz">,
              Sched<[ReadLoad]>;
def BRNCHNZP : BRANCH_r2<0b0000011, 0b000, "BRnzp">,
              Sched<[ReadLoad]>;


// Const instructions allocation for immediate instructions
def I_ALLOCA : ALLOCA_i<0b011,"CONST">;

// Dummy instruction for return statement
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
let isCall = 1 in
def JALR : RWInstI<0b000, OPC_JALR, (outs GPR:$rd),
                   (ins GPR:$rs1, i32imm:$imm12),
                   "jalr", "$rd, ${imm12}(${rs1})">,
           Sched<[WriteJalr, ReadJalr]>;
}


// Return Instruction
let isReturn = 1, isBarrier = 1, isTerminator = 1 in
def PseudoRET : Pseudo<(outs), (ins), [(TinyGPU_ret)]>,
                PseudoInstExpansion<(JALR R0, R1, 0)>;

def : InstAlias<"RET",(JALR R0,  R1, 0), 4>;

def : MnemonicAlias<"move", "mv">;

//===----------------------------------------------------------------------===//
// Pseudo-instructions and codegen patterns
//
// Naming convention: For 'generic' pattern classes, we use the naming
// convention PatTy1Ty2. For pattern classes which offer a more complex
// expension, prefix the class name, e.g. BccPat.
//===----------------------------------------------------------------------===//

/// Generic pattern classes

class PatGprGpr<SDPatternOperator OpNode, RWInst Inst>
    : Pat<(OpNode GPR:$rs1, GPR:$rs2), (Inst GPR:$rs1, GPR:$rs2)>;
class PatGprSimm12<SDPatternOperator OpNode, RWInstI Inst>
    : Pat<(OpNode GPR:$rs1, simm12:$imm12), (Inst GPR:$rs1, simm12:$imm12)>;
class PatGprUimmLog2WSize<SDPatternOperator OpNode, RWInstIShift Inst>
    : Pat<(OpNode GPR:$rs1, uimmlog2wsize:$shamt),
          (Inst GPR:$rs1, uimmlog2wsize:$shamt)>;
class PatGpr<SDPatternOperator OpNode, RWInstRR Inst>
    : Pat<(OpNode GPR:$rs), (Inst GPR:$rs)>;


/// Immediates

// def : Pat<(simm12:$imm), (ADDI R0, simm12:$imm)>;
def : Pat<(simm32hi20:$imm), (LUI (HI20 imm:$imm))>;
// def : Pat<(simm32:$imm), (ADDI (LUI (HI20 imm:$imm)), (LO12Sext imm:$imm))>;

def : PatGpr<load, LDR>;
def : Pat<(store GPR:$rd, GPR:$rs), (STR $rd, $rs)>;
def : Pat<(store simm12:$imm12, GPR:$rs), (CONST simm12:$imm12, GPR:$rs)>;
// def : Pat<(alloca), (CONST_ALLOCA)>;

// // Match load from FrameIndex
// def : Pat<(i32 (load (frameindex FI))),
//           (LW (add SP, (i32 (simm32 FI))), 0)>;

// // Match store to FrameIndex
// def : Pat<(store i32:$src, (frameindex FI)),
//           (SW i32:$src, (add SP, (i32 (simm32 FI))), 0)>;


/// Simple arithmetic operations
def : PatGprGpr<sub, SUB>;
def : PatGprGpr<mul, MUL>;
def : PatGprGpr<sdiv, DIV>;
def : PatGprGpr<add, ADD>;

// Arithmetic instructions with one immediate operand
def : Pat<(add GPR:$rs1, simm12:$imm12), (ADD GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(sub GPR:$rs1, simm12:$imm12), (SUB GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(mul GPR:$rs1, simm12:$imm12), (MUL GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(sdiv GPR:$rs1, simm12:$imm12), (DIV GPR:$rs1, (I_ALLOCA simm12:$imm12))>;

def : Pat<(add simm12:$imm12, GPR:$rs1), (ADD GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(sub simm12:$imm12, GPR:$rs1), (SUB GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(mul simm12:$imm12, GPR:$rs1), (MUL GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(sdiv simm12:$imm12, GPR:$rs1), (DIV GPR:$rs1, (I_ALLOCA simm12:$imm12))>;

// Comparison statements
def : Pat<(setlt GPR:$lhs, GPR:$rhs), (ADD GPR:$lhs, GPR:$rhs)>;
def : Pat<(setgt GPR:$lhs, GPR:$rhs), (SUB GPR:$lhs, GPR:$rhs)>;
def : Pat<(seteq GPR:$lhs, GPR:$rhs), (MUL GPR:$lhs, GPR:$rhs)>;
def : Pat<(setne GPR:$lhs, GPR:$rhs), (MUL GPR:$lhs, GPR:$rhs)>;

def : Pat<(setlt simm12:$lhs, GPR:$rhs), (ADD (I_ALLOCA simm12:$lhs), GPR:$rhs)>;
def : Pat<(setgt simm12:$lhs, GPR:$rhs), (SUB (I_ALLOCA simm12:$lhs), GPR:$rhs)>;
def : Pat<(seteq simm12:$lhs, GPR:$rhs), (MUL (I_ALLOCA simm12:$lhs), GPR:$rhs)>;
def : Pat<(setne simm12:$lhs, GPR:$rhs), (MUL (I_ALLOCA simm12:$lhs), GPR:$rhs)>;

def : Pat<(setlt GPR:$lhs, simm12:$rhs), (ADD GPR:$lhs, (I_ALLOCA simm12:$rhs))>;
def : Pat<(setgt GPR:$lhs, simm12:$rhs), (SUB GPR:$lhs, (I_ALLOCA simm12:$rhs))>;
def : Pat<(seteq GPR:$lhs, simm12:$rhs), (MUL GPR:$lhs, (I_ALLOCA simm12:$rhs))>;
def : Pat<(setne GPR:$lhs, simm12:$rhs), (MUL GPR:$lhs, (I_ALLOCA simm12:$rhs))>;

// Unconditional branches
def : Pat<(br bb:$dst), (BRNCH bb:$dst)>;




// Branch condition with comparison
def : Pat<(brcc SETLE,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHNZ (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETLT,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHN (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETGT,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHP (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETGE,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHPZ (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETNE,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHNP (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETEQ,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHZ (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;


def : Pat<(brcc SETLE,   simm12:$lhs, GPR:$rhs, bb:$dst), (BRNCHNZ (CMP (I_ALLOCA simm12:$lhs), GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETLT,   simm12:$lhs, GPR:$rhs, bb:$dst), (BRNCHN (CMP (I_ALLOCA simm12:$lhs), GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETGT,   simm12:$lhs, GPR:$rhs, bb:$dst), (BRNCHP (CMP (I_ALLOCA simm12:$lhs), GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETGE,   simm12:$lhs, GPR:$rhs, bb:$dst), (BRNCHPZ (CMP (I_ALLOCA simm12:$lhs), GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETNE,   simm12:$lhs, GPR:$rhs, bb:$dst), (BRNCHNP (CMP (I_ALLOCA simm12:$lhs), GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETEQ,   simm12:$lhs, GPR:$rhs, bb:$dst), (BRNCHZ (CMP (I_ALLOCA simm12:$lhs), GPR:$rhs), bb:$dst)>;

def : Pat<(brcc SETLE,   GPR:$lhs, simm12:$rhs, bb:$dst), (BRNCHNZ (CMP GPR:$lhs, (I_ALLOCA simm12:$rhs)), bb:$dst)>;
def : Pat<(brcc SETLT,   GPR:$lhs, simm12:$rhs, bb:$dst), (BRNCHN (CMP GPR:$lhs, (I_ALLOCA simm12:$rhs)), bb:$dst)>;
def : Pat<(brcc SETGT,   GPR:$lhs, simm12:$rhs, bb:$dst), (BRNCHP (CMP GPR:$lhs, (I_ALLOCA simm12:$rhs)), bb:$dst)>;
def : Pat<(brcc SETGE,   GPR:$lhs, simm12:$rhs, bb:$dst), (BRNCHPZ (CMP GPR:$lhs, (I_ALLOCA simm12:$rhs)), bb:$dst)>;
def : Pat<(brcc SETNE,   GPR:$lhs, simm12:$rhs, bb:$dst), (BRNCHNP (CMP GPR:$lhs, (I_ALLOCA simm12:$rhs)), bb:$dst)>;
def : Pat<(brcc SETEQ,   GPR:$lhs, simm12:$rhs, bb:$dst), (BRNCHZ (CMP GPR:$lhs, (I_ALLOCA simm12:$rhs)), bb:$dst)>;


// Frame Lowering
def to_tframeindex : SDNodeXForm<frameindex, [{
  return CurDAG->getTargetFrameIndex(N->getIndex(), N->getValueType(0));
}]>;

def : Pat<(frameindex:$fi), (ADD (to_tframeindex $fi), (I_ALLOCA 0))>;



// Match both a plain shift and one where the shift amount is masked (this is
// typically introduced when the legalizer promotes the shift amount and
// zero-extends it). For RISC-V, the mask is unnecessary as shifts in the base
// ISA only read the least significant 5 bits (RV32I) or 6 bits (RV64I).
class shiftop<SDPatternOperator operator>
    : PatFrags<(ops node:$val, node:$count),
               [(operator node:$val, node:$count),
                (operator node:$val, (and node:$count, immbottomwsizeset))]>;


let Defs = [R2], Uses = [R2] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_end timm:$amt1, timm:$amt2)]>;
} // end Defs = [X2], Uses = [X2]


// def : Pat<(br bb:$func), (LDR GPR:$func)>;
