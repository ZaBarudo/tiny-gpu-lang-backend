//=== TinyGPUCallingConv.td - Calling Conventions for TinyGPU --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for TinyGPU architecture.
//===----------------------------------------------------------------------===//

def TinyGPU_CCallingConv : CallingConv<[
  // Handles byval parameters. Tell LLVM what the size and alignment
  // requirements of each stack slot are
  CCIfByVal<CCPassByVal<1, 1>>,

  // Promote i8/i16 to i8
  // CCIfType<[i1, i8, i16], CCPromoteToType<i8>>,

  // The first 4 arguments are passed in X0, X1, X2, X3
  CCIfType<[i8], CCAssignToReg<[R0, R1, R2, R3]>>,

  // Integer values get stored in stack slots that are 4 bytes in size and
  // 4-byte aligned
  CCIfType<[i8], CCAssignToStack<1, 1>>
]>;

// BeyondRISC C Return Convention
def TinyGPU_CRetConv : CallingConv<[
  // CCIfType<[i1, i8, i16], CCPromoteToType<i8>>,

  // i8 are returned in registers X0, X1
  CCIfType<[i8], CCAssignToReg<[R0, R1, R2, R3]>>,

  // Integer values get stored in stack slots that are 4 bytes in size and
  // 4-byte aligned
  CCIfType<[i8], CCAssignToStack<1, 1>>
]>;

def TinyGPU_CalleeSavedRegs : CalleeSavedRegs<(add R8, R9)>;
