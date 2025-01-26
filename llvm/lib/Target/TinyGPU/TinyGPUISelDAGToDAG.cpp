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

#define DEBUG_TYPE "TinyGPU-isel"
#define PASS_NAME "CPU0 DAG->DAG Pattern Instruction Selection"

bool TinyGPUDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  Subtarget = &static_cast<const TinyGPUSubtarget &>(MF.getSubtarget());
  return SelectionDAGISel::runOnMachineFunction(MF);
}

void TinyGPUDAGToDAGISel::Select(SDNode *Node) {
  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    Node->setNodeId(-1);
    return;
  }

  // Instruction Selection not handled by the auto-generated tablegen selection
  // should be handled here.
  unsigned Opcode = Node->getOpcode();
  SDLoc DL(Node);

  switch(Opcode) {
  case ISD::Constant: {
    auto ConstNode = cast<ConstantSDNode>(Node);
    if (ConstNode->isZero()) {
      SDValue New = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), SDLoc(Node),
                                           TinyGPU::X0, MVT::i32);
      ReplaceNode(Node, New.getNode());
      return;
    }
    break;
  }
  default: break;
  }

  // Select the default instruction
  SelectCode(Node);
}

namespace {
  class TinyGPUDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
  public:
    static char ID;
    TinyGPUDAGToDAGISelLegacy(TinyGPUTargetMachine &TM, CodeGenOptLevel OptLevel)
    : SelectionDAGISelLegacy(ID, std::make_unique<TinyGPUDAGToDAGISel>(TM, OptLevel)) {}
  };
}

// This pass converts a legalized DAG into a TinyGPU-specific DAG, ready
// for instruction scheduling.
FunctionPass *llvm::createTinyGPUISelDag(TinyGPUTargetMachine &TM,
                                       CodeGenOptLevel OptLevel) {
  return new TinyGPUDAGToDAGISelLegacy(TM, OptLevel);
}

char TinyGPUDAGToDAGISelLegacy::ID = 0;

INITIALIZE_PASS(TinyGPUDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)
