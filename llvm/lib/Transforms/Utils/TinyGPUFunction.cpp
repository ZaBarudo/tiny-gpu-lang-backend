#include "llvm/Transforms/Utils/TinyGPUFunction.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

PreservedAnalyses TinyGPUFunctionPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  bool modified = false;

    // Collect call instructions to avoid iterator invalidation
    std::vector<Instruction *> callInsts;
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (isa<CallInst>(&I)) {
          callInsts.push_back(&I);
        }
      }
    }

    // Insert new basic blocks after each call instruction
    for (auto *callInst : callInsts) {
      BasicBlock *currentBB = callInst->getParent();
      if (callInst != currentBB->getTerminator()) {
        // Split the basic block after the call instruction
        BasicBlock *newBB = currentBB->splitBasicBlock(
            callInst->getNextNode(), currentBB->getName() + ".afterCall");

        // (Optional) Insert additional instructions into the new basic block
        IRBuilder<> builder(&newBB->front());
        // Example: Insert a debug message
        // builder.CreateCall(Intrinsic::getDeclaration(F.getParent(), Intrinsic::dbg_value), {});

        modified = true;
      }
    }

    return (modified ? PreservedAnalyses::none() : PreservedAnalyses::all());
}
