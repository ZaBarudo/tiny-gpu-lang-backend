//===-- TinyGPUISelDAGToDAG.cpp - A Dag to Dag Inst Selector for TinyGPU ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the TinyGPU target.
//
//===----------------------------------------------------------------------===//

#include "TinyGPUISelDAGToDAG.h"
#include "TinyGPUSubtarget.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/SelectionDAGISel.h"

using namespace llvm;

#define DEBUG_TYPE "riscw-isel"

bool TinyGPUDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  Subtarget = &static_cast<const TinyGPUSubtarget &>(MF.getSubtarget());
  return SelectionDAGISel::runOnMachineFunction(MF);
}

void TinyGPUDAGToDAGISel::Select(SDNode *Node) {
  unsigned Opcode = Node->getOpcode();

  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    Node->setNodeId(-1);
    return;
  }

  // Instruction Selection not handled by the auto-generated tablegen selection
  // should be handled here.
  switch(Opcode) {
  default: break;
  }

  // Select the default instruction
  SelectCode(Node);
}
