//===---- RISCWISelDAGToDAG.h - A Dag to Dag Inst Selector for RISCW ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the RISCW target.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCW_RISCWISELDAGTODAG_H
#define LLVM_LIB_TARGET_RISCW_RISCWISELDAGTODAG_H

#include "RISCWSubtarget.h"
#include "RISCWTargetMachine.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {
class RISCWDAGToDAGISel : public SelectionDAGISel {
public:
  explicit RISCWDAGToDAGISel(RISCWTargetMachine &TM, CodeGenOpt::Level OL)
      : SelectionDAGISel(TM, OL), Subtarget(nullptr) {}

  // Pass Name
  StringRef getPassName() const override {
    return "CPU0 DAG->DAG Pattern Instruction Selection";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void Select(SDNode *Node) override;

#include "RISCWGenDAGISel.inc"

private:
  const RISCWSubtarget *Subtarget;
};
}

#endif // end LLVM_LIB_TARGET_RISCW_RISCWISELDAGTODAG_H
