//===-- RISCWRegisterInfo.td - RISCW Register defs ---------*- tablegen -*-===//
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

let Namespace = "RISCW" in {
class RISCWReg<bits<5> Enc, string n, list<string> alt = []> : Register<n> {
  let HWEncoding{4-0} = Enc;
  let AltNames = alt;
}
} // end Namespace

def X0  : RISCWReg<0, "x0", ["zero"]>, DwarfRegNum<[0]>;
def X1  : RISCWReg<1, "x1", ["ra"]>, DwarfRegNum<[1]>;
def X2  : RISCWReg<2, "x2", ["sp"]>, DwarfRegNum<[2]>;
def X3  : RISCWReg<3, "x3", ["gp"]>, DwarfRegNum<[3]>;
def X4  : RISCWReg<4, "x4", ["tp"]>, DwarfRegNum<[4]>;
def X5  : RISCWReg<5, "x5", ["t0"]>, DwarfRegNum<[5]>;
def X6  : RISCWReg<6, "x6", ["t1"]>, DwarfRegNum<[6]>;
def X7  : RISCWReg<7, "x7", ["t2"]>, DwarfRegNum<[7]>;
def X8  : RISCWReg<8, "x8", ["s0", "fp"]>, DwarfRegNum<[8]>;
def X9  : RISCWReg<9, "x9", ["s1"]>, DwarfRegNum<[9]>;
def X10 : RISCWReg<10,"x10", ["a0"]>, DwarfRegNum<[10]>;
def X11 : RISCWReg<11,"x11", ["a1"]>, DwarfRegNum<[11]>;
def X12 : RISCWReg<12,"x12", ["a2"]>, DwarfRegNum<[12]>;
def X13 : RISCWReg<13,"x13", ["a3"]>, DwarfRegNum<[13]>;
def X14 : RISCWReg<14,"x14", ["a4"]>, DwarfRegNum<[14]>;
def X15 : RISCWReg<15,"x15", ["a5"]>, DwarfRegNum<[15]>;
def X16 : RISCWReg<16,"x16", ["a6"]>, DwarfRegNum<[16]>;
def X17 : RISCWReg<17,"x17", ["a7"]>, DwarfRegNum<[17]>;
def X18 : RISCWReg<18,"x18", ["s2"]>, DwarfRegNum<[18]>;
def X19 : RISCWReg<19,"x19", ["s3"]>, DwarfRegNum<[19]>;
def X20 : RISCWReg<20,"x20", ["s4"]>, DwarfRegNum<[20]>;
def X21 : RISCWReg<21,"x21", ["s5"]>, DwarfRegNum<[21]>;
def X22 : RISCWReg<22,"x22", ["s6"]>, DwarfRegNum<[22]>;
def X23 : RISCWReg<23,"x23", ["s7"]>, DwarfRegNum<[23]>;
def X24 : RISCWReg<24,"x24", ["s8"]>, DwarfRegNum<[24]>;
def X25 : RISCWReg<25,"x25", ["s9"]>, DwarfRegNum<[25]>;
def X26 : RISCWReg<26,"x26", ["s10"]>, DwarfRegNum<[26]>;
def X27 : RISCWReg<27,"x27", ["s11"]>, DwarfRegNum<[27]>;
def X28 : RISCWReg<28,"x28", ["t3"]>, DwarfRegNum<[28]>;
def X29 : RISCWReg<29,"x29", ["t4"]>, DwarfRegNum<[29]>;
def X30 : RISCWReg<30,"x30", ["t5"]>, DwarfRegNum<[30]>;
def X31 : RISCWReg<31,"x31", ["t6"]>, DwarfRegNum<[31]>;

// The order of registers represents the preferred allocation sequence.
// Registers are listed in the order caller-save, callee-save, specials.
def GPR : RegisterClass<"RISCW", [i32], 32, (add
    (sequence "X%u", 10, 17),
    (sequence "X%u", 5, 7),
    (sequence "X%u", 28, 31),
    (sequence "X%u", 8, 9),
    (sequence "X%u", 18, 27),
    (sequence "X%u", 0, 4)
  )>;

def SP : RegisterClass<"RISCW", [i32], 32, (add X2)>;
