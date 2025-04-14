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
#define PASS_NAME "TinyGPU DAG->DAG Pattern Instruction Selection"

// This function is called to run the instruction selection pass on a given MachineFunction.
// It initializes the Subtarget pointer and delegates the rest of the work to the base class's implementation.
bool TinyGPUDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  // Initialize the Subtarget pointer with the specific TinyGPU subtarget for this MachineFunction.
  Subtarget = &static_cast<const TinyGPUSubtarget &>(MF.getSubtarget());
  
  // Call the base class's runOnMachineFunction to perform the actual instruction selection.
  return SelectionDAGISel::runOnMachineFunction(MF);
}

// This function is responsible for selecting the appropriate machine instruction
// for a given SelectionDAG node. It handles custom instruction selection logic
// and delegates to the default instruction selector when no custom logic applies.
// Used when the tablegen-generated code is not sufficient for the target.
// The function takes a SelectionDAG node as input and modifies it in place.
// It replaces the node with a new machine instruction if necessary.
// The function is called during the instruction selection phase of the compilation process.

void TinyGPUDAGToDAGISel::Select(SDNode *Node) {
  // If the node is already a machine opcode, it has been selected, so we skip it.
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    Node->setNodeId(-1); // Mark the node as selected.
    return;
  }

  // Retrieve the opcode of the current node and its debug location.
  unsigned Opcode = Node->getOpcode();
  SDLoc DL(Node);

  // Handle specific opcodes with custom selection logic.
  switch (Opcode) {
  case ISD::Constant: {
    // Handle constant values. If the constant is zero, replace the node with
    // a copy from the R0 register, which is assumed to hold zero.
    // auto ConstNode = cast<ConstantSDNode>(Node);
    // if (ConstNode->isZero()) {
    //   SDValADDIue New = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), SDLoc(Node),
    //                                        TinyGPU::R0, MVT::i32);
    //   ReplaceNode(Node, New.getNode()); // Replace the current node with the new one.
    //   return;
    // }
    break; // For non-zero constants, fall through to default handling.
  }
  case TinyGPUISD::BRNCZ: {
    // Handle the custom TinyGPU BRNCZ (branch for function call lowering) instruction.
    SDValue Chain = Node->getOperand(1); // Get the chain operand.
    SDValue Target = Node->getOperand(2); // Get the target operand.
    // Create a machine node for the BRNCH_RTG instruction.
    auto Br = CurDAG->getMachineNode(TinyGPU::BRNCH_RTG, DL, MVT::Other, Target, Chain);
    ReplaceNode(Node, Br); // Replace the current node with the new machine node.
    return;
  }
  case TinyGPUISD::BRNCZ2: {
    // Handle the custom TinyGPU BRNCZ (branch for function call lowering) instruction.
    SDValue Chain = Node->getOperand(1); // Get the chain operand.
    SDValue Target = Node->getOperand(2); // Get the target operand.
    // Create a machine node for the BRNCH_RTG instruction.
    auto Br = CurDAG->getMachineNode(TinyGPU::CALLL, DL, MVT::Other, Target, Chain);
    ReplaceNode(Node, Br); // Replace the current node with the new machine node.
    return;
  }
   case TinyGPUISD::Ret2: {
    SDValue Chain = Node->getOperand(0); // Get the chain operand.
    auto Ret = CurDAG->getMachineNode(TinyGPU::RETT, DL, MVT::Other,Chain);
    ReplaceNode(Node, Ret); // Replace the current node with the new machine node.
    return;
  }
  default:
    break; // For all other opcodes, fall through to default handling.
  }

  // If no custom handling was performed, use the default instruction selector.
  SelectCode(Node);
}

// This namespace contains the legacy implementation of the TinyGPU DAG-to-DAG instruction selector.
// The legacy pass is used for compatibility with the older LLVM pass manager.
namespace {
  // This class defines the legacy TinyGPU DAG-to-DAG instruction selector pass.
  // It inherits from SelectionDAGISelLegacy and provides a constructor to initialize
  // the pass with the TinyGPU-specific instruction selector.
  class TinyGPUDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
  public:
    // Unique identifier for the pass.
    static char ID;

    // Constructor for the TinyGPUDAGToDAGISelLegacy class.
    // It initializes the base class with the unique ID and creates an instance
    // of the TinyGPUDAGToDAGISel class, which performs the actual instruction selection.
    TinyGPUDAGToDAGISelLegacy(TinyGPUTargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISelLegacy(ID, std::make_unique<TinyGPUDAGToDAGISel>(TM, OptLevel)) {}
  };
}

// This pass converts a legalized DAG into a TinyGPU-specific DAG, ready
// for instruction scheduling.
// This function creates a new instance of the TinyGPU DAG-to-DAG instruction selector pass.
// It is used to convert a legalized DAG into a TinyGPU-specific DAG, ready for instruction scheduling.
// The function takes a TinyGPUTargetMachine reference and a CodeGen optimization level as parameters.
// It returns a pointer to a newly created instance of the TinyGPUDAGToDAGISelLegacy pass.

FunctionPass *llvm::createTinyGPUISelDag(TinyGPUTargetMachine &TM,
                     CodeGenOptLevel OptLevel) {
  // Create and return a new instance of the TinyGPUDAGToDAGISelLegacy pass,
  // initialized with the target machine and optimization level.
  return new TinyGPUDAGToDAGISelLegacy(TM, OptLevel);
}

// Define the static member variable ID for the TinyGPUDAGToDAGISelLegacy class.
// This is used to uniquely identify the pass in the LLVM pass manager.
char TinyGPUDAGToDAGISelLegacy::ID = 0;

// Initialize the TinyGPUDAGToDAGISelLegacy pass with its debug type, name, and properties.
// The first `false` indicates that the pass does not modify the control flow graph (CFG).
// The second `false` indicates that the pass is not an analysis pass.
INITIALIZE_PASS(TinyGPUDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)
