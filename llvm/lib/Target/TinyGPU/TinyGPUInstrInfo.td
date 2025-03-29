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

class BRANCH_r2<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWJInstR2<funct7, funct3, OPC_OP, (outs), (ins GPR:$rs2, simm13_lsb0:$rs),
              opcodestr, "$rs">;

class BRANCH_r3<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWJInstR<funct7, funct3, OPC_OP, (outs), (ins simm12:$rs),
              opcodestr, "$rs">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
class STR_rr<bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWInstRR<funct7, funct3, OPC_STORE, (outs), (ins GPR:$rd, GPR:$rs),
              opcodestr, "$rd, $rs">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
class STR_ir<bits<3> funct3, string opcodestr>
    : RWInstRI<funct3, OPC_OP_IMM, (outs), (ins simm12:$imm12, GPR:$rs),
              opcodestr, "$rs, #$imm12">,Sched<[WriteIALU]>;

// let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
// class ALLOCA_ir<bits<3> funct3, string opcodestr>
//     : RWInstRI<funct3, OPC_OP_IMM, (outs GPR:$rs), (ins ),
//               opcodestr, "$rs , #0">,Sched<[WriteIALU]>;

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
let isReMaterializable = 1, isAsCheapAsAMove = 1 in{
  def ADDI  : ALU_ri<0b000, "ADDI">;
  def ADDri  : ALU_ri<0b000, "addi">;
  def SUBri  : ALU_ri<0b000, "subi">;
}

// def SLTI  : ALU_ri<0b010, "slti">;
// def SLTIU : ALU_ri<0b011, "sltiu">;

// let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
// // def XORI  : ALU_ri<0b100, "xori">;
// def ORI   : ALU_ri<0b110, "ori">;
// }

// def ANDI  : ALU_ri<0b111, "andi">;

// def SLLI : Shift_ri<0, 0b001, "slli">;
// def SRLI : Shift_ri<0, 0b101, "srli">;
// def SRAI : Shift_ri<1, 0b101, "srai">;

def ADD  : ALU_rr<0b0000000, 0b000, "ADD">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
def ADDrr  : ALU_rr<0b0000000, 0b000, "ADD">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
def SUB  : ALU_rr<0b0100000, 0b000, "SUB">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
def SUBrr  : ALU_rr<0b0100000, 0b000, "SUB">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

def CMP  : CMP_rr<0b0000000, 0b000, "CMP">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;


// def SLL  : ALU_rr<0b0000000, 0b001, "sll">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
// def SLT  : ALU_rr<0b0000000, 0b010, "slt">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
// def SLTU : ALU_rr<0b0000000, 0b011, "sltu">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
// def XOR  : ALU_rr<0b0000000, 0b100, "xor">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
// def SRL  : ALU_rr<0b0000000, 0b101, "srl">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
// def SRA  : ALU_rr<0b0100000, 0b101, "sra">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
// def OR   : ALU_rr<0b0000000, 0b110, "or">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;
// def AND  : ALU_rr<0b0000000, 0b111, "and">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// M extension
def MUL     : ALU_rr<0b0000001, 0b000, "MUL">, Sched<[WriteIMul, ReadIMul, ReadIMul]>;
// def MULH    : ALU_rr<0b0000001, 0b001, "mulh">, Sched<[WriteIMul, ReadIMul, ReadIMul]>;
// def MULHSU  : ALU_rr<0b0000001, 0b010, "mulhsu">, Sched<[WriteIMul, ReadIMul, ReadIMul]>;
// def MULHU   : ALU_rr<0b0000001, 0b011, "mulhu">, Sched<[WriteIMul, ReadIMul, ReadIMul]>;
def DIV     : ALU_rr<0b0000001, 0b100, "DIV">,
              Sched<[WriteIDiv, ReadIDiv, ReadIDiv]>;
// def DIVU    : ALU_rr<0b0000001, 0b101, "divu">,
//               Sched<[WriteIDiv, ReadIDiv, ReadIDiv]>;
// def REM     : ALU_rr<0b0000001, 0b110, "rem">,
//               Sched<[WriteIDiv, ReadIDiv, ReadIDiv]>;
// def REMU    : ALU_rr<0b0000001, 0b111, "remu">,
//               Sched<[WriteIDiv, ReadIDiv, ReadIDiv]>;
def LDR : LDR_rr<0b0000010, 0b000, "LDR">,
              Sched<[WriteLoad, ReadLoad]>;
def STR : STR_rr<0b0000010, 0b001, "STR">,
              Sched<[ReadLoad, WriteLoad]>;
def CONST : STR_ir<0b001,"CONST">;
def BRNCH : BRANCH_r<0b0000011, 0b000, "BR">,
              Sched<[ReadLoad]>;

def BRNCH_2 : BRANCH_r2<0b0000011, 0b000, "BR">,
              Sched<[ReadLoad]>;
def BRNCH_3 : BRANCH_r3<0b0000011, 0b000, "BR">,
              Sched<[ReadLoad]>;

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


// def CONST_ALLOCA : ALLOCA_ir<0b001,"CONST">;
def I_ALLOCA : ALLOCA_i<0b011,"CONST">;

/* Procedure calling instructions */
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
let isCall = 1 in
def JALR : RWInstI<0b000, OPC_JALR, (outs GPR:$rd),
                   (ins GPR:$rs1, i32imm:$imm12),
                   "jalr", "$rd, ${imm12}(${rs1})">,
           Sched<[WriteJalr, ReadJalr]>;
}

//===----------------------------------------------------------------------===//
// Assembler Pseudo Instructions (User-Level ISA, Version 2.2, Chapter 20)
//===----------------------------------------------------------------------===//

let isReturn = 1, isBarrier = 1, isTerminator = 1 in
def PseudoRET : Pseudo<(outs), (ins), [(TinyGPU_ret)]>,
                PseudoInstExpansion<(JALR R0, R1, 0)>;

// def : InstAlias<"mv $rd, $rs",   (ADDI GPR:$rd, GPR:$rs,       0)>;
// def : InstAlias<"not $rd, $rs",  (XORI GPR:$rd, GPR:$rs,      -1)>;
// def : InstAlias<"neg $rd, $rs",  (SUB  GPR:$rd,      R0, GPR:$rs)>;

// def : InstAlias<"seqz $rd, $rs", (SLTIU GPR:$rd, GPR:$rs,       1)>;
// def : InstAlias<"snez $rd, $rs", (SLTU  GPR:$rd,      R0, GPR:$rs)>;
// def : InstAlias<"sltz $rd, $rs", (SLT   GPR:$rd, GPR:$rs,      R0)>;
// def : InstAlias<"sgtz $rd, $rs", (SLT   GPR:$rd,      R0, GPR:$rs)>;

// def : InstAlias<"alloca $rd, $rs", (SLT   GPR:$rd,      R0, GPR:$rs)>;

// sgt/sgtu are recognised by the GNU assembler but the canonical slt/sltu
// form will always be printed. Therefore, set a zero weight.
// def : InstAlias<"sgt $rd, $rs, $rt", (SLT GPR:$rd, GPR:$rt, GPR:$rs), 0>;
// def : InstAlias<"sgtu $rd, $rs, $rt", (SLTU GPR:$rd, GPR:$rt, GPR:$rs), 0>;

// let EmitPriority = 0 in {
// def : InstAlias<"slt $rd, $rs1, $imm12",
                // (SLTI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;
// def : InstAlias<"sltu $rd, $rs1, $imm12",
                // (SLTIU  GPR:$rd, GPR:$rs1, simm12:$imm12)>;
// }

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

def func_addr : Operand<i32> {
  let PrintMethod = "printGlobalAddress";  // Custom printer for addresses
  let EncoderMethod = "encodeGlobalAddress";  // Custom encoder
  let MIOperandInfo = (ops ptr_rc:$addr);  // Operand type metadata
}
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

// def to_tblockaddr : SDNodeXForm<tglobaladdr, [{
//   const GlobalAddressSDNode *GA = cast<GlobalAddressSDNode>(N);
//   const GlobalValue *GV = GA->getGlobal();
//   EVT VT = GA->getValueType(0);
//   SDLoc dl(N);

//   // Check if this global is a blockaddress wrapper
//   if (const auto *GVar = dyn_cast<GlobalVariable>(GV)) {
//     const Constant *Init = GVar->getInitializer();
    
//     // Unwrap constant casts (bitcast/ptrtoint)
//     while (const auto *CE = dyn_cast<ConstantExpr>(Init)) {
//       if (CE->isCast()) 
//         Init = CE->getOperand(0);
//       else 
//         break;
//     }

//     // Convert BlockAddress to immediate label
//     if (const auto *BA = dyn_cast<BlockAddress>(Init)) {
//       const BasicBlock *BB = BA->getBasicBlock();
//       MCContext &Ctx = CurDAG->getContext();
//       MCSymbol *BBSym = Ctx.getOrCreateSymbol(BB->getName());
      
//       // Return as target-specific immediate (basic block label)
//       return CurDAG->getTargetBlockAddress(BBSym, VT);
//     }
//   }

//   // Fallback: regular global address
//   return CurDAG->getTargetGlobalAddress(GV, dl, VT);
// }]>;

def to_funcptr : SDNodeXForm<tglobaladdr, [{
  const GlobalAddressSDNode *GA = cast<GlobalAddressSDNode>(N);
  
  // Get symbol name (e.g., @_Z9getNumberii becomes "_Z9getNumberii")
  std::string SymName = GA->getGlobal()->getName().str();
  
  // Create target-specific symbol reference
  return CurDAG->getTargetExternalSymbol(SymName.c_str(), N->getValueType(0));
}]>;

// Direct branch to global symbol
// def BR_DIRECT : InstTinyGPU<(outs), (ins GPR:$dst),"br $dst",[(br tglobaladdr:$dst)]>;
def BR_DIRECT : InstTinyGPU<(outs), 
                          (ins GPR:$dst),  // Changed from GPR to tglobaladdr
                          "br $dst"> {  // Match call nodes with global addresses
  let isBranch = 1;
  let isTerminator = 1;
  let isCall = 1;
  let hasDelaySlot = 0;
  let Defs = [R7];  // If return address is stored in R7
}

/// Simple arithmetic operations

def : PatGprGpr<add, ADD>;
// def : PatGprSimm12<add, ADDI>;
def : Pat<(add GPR:$rs1, simm12:$imm12), (ADD GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(sub GPR:$rs1, simm12:$imm12), (SUB GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(mul GPR:$rs1, simm12:$imm12), (MUL GPR:$rs1, (I_ALLOCA simm12:$imm12))>;
def : Pat<(sdiv GPR:$rs1, simm12:$imm12), (DIV GPR:$rs1, (I_ALLOCA simm12:$imm12))>;

def : Pat<(setlt GPR:$lhs, GPR:$rhs), (ADD GPR:$lhs, GPR:$rhs)>;
def : Pat<(setgt GPR:$lhs, GPR:$rhs), (SUB GPR:$lhs, GPR:$rhs)>;
def : Pat<(seteq GPR:$lhs, GPR:$rhs), (MUL GPR:$lhs, GPR:$rhs)>;
def : Pat<(setne GPR:$lhs, GPR:$rhs), (MUL GPR:$lhs, GPR:$rhs)>;
def : Pat<(br bb:$dst), (BRNCH bb:$dst)>;
// def : Pat<(TinyGPU::CALL (i32 tglobaladdr:$dst)),
//           (BRNCH $dst)>;
// def : Pat<(call tglobaladdr: $dst), (BRNCH tglobaladdr:$dst)>;
// def CALL : SDNode<"TinyGPUISD::CALL", SDTBr, [SDNPHasChain]>;



// def : Pat<(brcc BPF_CC_EQ, GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCH_2 (ADD GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETLE,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHNZ (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETLT,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHN (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETGT,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHP (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETGE,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHPZ (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETNE,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHNP (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;
def : Pat<(brcc SETEQ,   GPR:$lhs, GPR:$rhs, bb:$dst), (BRNCHZ (CMP GPR:$lhs, GPR:$rhs), bb:$dst)>;

// def BRN : InstTinyGPU<(outs), (ins (i32(func_addr:$dst))), "br $dst", [(CALL func_addr:$dst)]>;



// def : Pat<(setcc GPR:$lhs, GPR:$rhs, setgt), (ADD GPR:$lhs, GPR:$rhs)>;
def : PatGprGpr<sub, SUB>;

def to_tframeindex : SDNodeXForm<frameindex, [{
  return CurDAG->getTargetFrameIndex(N->getIndex(), N->getValueType(0));
}]>;

//THIS WORKS
// def : Pat<(frameindex:$fi), (ADDI (to_tframeindex $fi), 0)>;
def : Pat<(frameindex:$fi), (ADD (to_tframeindex $fi), (I_ALLOCA 0))>;

// def : Pat<(frameindex:$fi), (ADD (to_tframeindex $fi), (I_ALLOCA 0))>;
// def : Pat<(frameindex:$fi), ( (to_tframeindex $fi), (I_ALLOCA 0))>




// def : PatGprGpr<or, OR>;
// def : PatGprSimm12<or, ORI>;
// def : PatGprGpr<and, AND>;
// def : PatGprSimm12<and, ANDI>;
// def : PatGprGpr<xor, XOR>;
// def : PatGprSimm12<xor, XORI>;
// def : PatGprUimmLog2WSize<shl, SLLI>;
// def : PatGprUimmLog2WSize<srl, SRLI>;
// def : PatGprUimmLog2WSize<sra, SRAI>;

def : PatGprGpr<mul, MUL>;
// def : PatGprGpr<mulhs, MULH>;
// def : PatGprGpr<mulhu, MULHU>;
// No ISDOpcode for mulhsu
def : PatGprGpr<sdiv, DIV>;
// def : PatGprGpr<udiv, DIVU>;
// def : PatGprGpr<srem, REM>;
// def : PatGprGpr<urem, REMU>;

// Match both a plain shift and one where the shift amount is masked (this is
// typically introduced when the legalizer promotes the shift amount and
// zero-extends it). For RISC-V, the mask is unnecessary as shifts in the base
// ISA only read the least significant 5 bits (RV32I) or 6 bits (RV64I).
class shiftop<SDPatternOperator operator>
    : PatFrags<(ops node:$val, node:$count),
               [(operator node:$val, node:$count),
                (operator node:$val, (and node:$count, immbottomwsizeset))]>;

// def : PatGprGpr<shiftop<shl>, SLL>;
// def : PatGprGpr<shiftop<srl>, SRL>;
// def : PatGprGpr<shiftop<sra>, SRA>;

let Defs = [R2], Uses = [R2] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_end timm:$amt1, timm:$amt2)]>;
} // end Defs = [X2], Uses = [X2]

