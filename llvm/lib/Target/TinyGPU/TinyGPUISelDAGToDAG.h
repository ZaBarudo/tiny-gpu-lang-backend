//===---- TinyGPUISelDAGToDAG.h - A Dag to Dag Inst Selector for TinyGPU ------===//
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

#ifndef LLVM_LIB_TARGET_TinyGPU_TinyGPUISELDAGTODAG_H
#define LLVM_LIB_TARGET_TinyGPU_TinyGPUISELDAGTODAG_H

#include "TinyGPUSubtarget.h"
#include "TinyGPUTargetMachine.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {
class TinyGPUDAGToDAGISel : public SelectionDAGISel {
public:
  explicit TinyGPUDAGToDAGISel(TinyGPUTargetMachine &TM, CodeGenOptLevel OL)
      : SelectionDAGISel(TM, OL), Subtarget(nullptr) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

  void Select(SDNode *Node) override;

#include "TinyGPUGenDAGISel.inc"

private:
  const TinyGPUSubtarget *Subtarget;
};
}

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUISELDAGTODAG_H
