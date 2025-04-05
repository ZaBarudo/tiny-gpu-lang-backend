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

// Instruction selector class for TinyGPU target, inheriting from SelectionDAGISel.
class TinyGPUDAGToDAGISel : public SelectionDAGISel {
public:
  // Constructor for TinyGPUDAGToDAGISel.
  // Takes a reference to TinyGPUTargetMachine and optimization level as arguments.
  explicit TinyGPUDAGToDAGISel(TinyGPUTargetMachine &TM, CodeGenOptLevel OL)
      : SelectionDAGISel(TM, OL), Subtarget(nullptr) {}

  // Runs the instruction selection pass on the given MachineFunction.
  // Returns true if the MachineFunction was modified.
  bool runOnMachineFunction(MachineFunction &MF) override;
  // Selects the appropriate machine instruction for the given SDNode.
  void Select(SDNode *Node) override;

#include "TinyGPUGenDAGISel.inc" // Include the auto-generated instruction selector.

private:
  const TinyGPUSubtarget *Subtarget; // Pointer to the subtarget information.
};

} // namespace llvm

#endif // end LLVM_LIB_TARGET_TinyGPU_TinyGPUISELDAGTODAG_H
