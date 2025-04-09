//===-- TinyGPURegisterInfo.td - TinyGPU Register defs ---------*- tablegen
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the RISC-W register file
//===----------------------------------------------------------------------===//

class TinyGPUReg<bits<4> Enc, string n, list<string> alt = []> : Register<n> {
  let HWEncoding{3 - 0} = Enc;
  let AltNames = alt;
  let Namespace = "TinyGPU";
  string Name = n;
}

def R0 : TinyGPUReg<0, "R0", ["zero"]>, DwarfRegNum<[0]>;
def R1 : TinyGPUReg<1, "R1", ["ra"]>, DwarfRegNum<[1]>;
def R2 : TinyGPUReg<2, "R2", ["nzp"]>, DwarfRegNum<[2]>;
def R3 : TinyGPUReg<3, "R3", ["gp"]>, DwarfRegNum<[3]>;
def R4 : TinyGPUReg<4, "R4", ["tp"]>, DwarfRegNum<[4]>;
def R5 : TinyGPUReg<5, "R5", ["t0"]>, DwarfRegNum<[5]>;
def R6 : TinyGPUReg<6, "R6", ["t1"]>, DwarfRegNum<[6]>;
def R7 : TinyGPUReg<7, "R7", ["t2"]>, DwarfRegNum<[7]>;
def R8 : TinyGPUReg<8, "R8", ["s0", "fp"]>, DwarfRegNum<[8]>;
def R9 : TinyGPUReg<9, "R9", ["s1"]>, DwarfRegNum<[9]>;
def R10 : TinyGPUReg<10, "R10", ["a0"]>, DwarfRegNum<[10]>;
def R11 : TinyGPUReg<11, "R11", ["a1"]>, DwarfRegNum<[11]>;
def R13 : TinyGPUReg<13, "R13", ["bi"]>, DwarfRegNum<[13]>;
def R14 : TinyGPUReg<14, "R14", ["bd"]>, DwarfRegNum<[14]>;
def R15 : TinyGPUReg<15, "R15", ["ti"]>, DwarfRegNum<[15]>;

// The order of registers represents the preferred allocation sequence.
// Registers are listed in the order caller-save, callee-save, specials.
def GPR : RegisterClass<"TinyGPU", [i32], 32, (sequence "R%u", 0, 10)>;
def NZP : RegisterClass<"TinyGPU", [i32], 32, (add R2)>;
def SP : TinyGPUReg<12, "SP", ["sp"]>, DwarfRegNum<[12]>;
def LR : TinyGPUReg<11, "LR", ["lr"]>, DwarfRegNum<[11]>;
