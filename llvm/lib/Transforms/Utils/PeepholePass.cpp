// PeepholePass.cpp
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"

#include "llvm/IR/Function.h"
#include "llvm/Transforms/Utils/PeepholePass.h"

using namespace llvm;
using namespace llvm::PatternMatch;

PreservedAnalyses PeepholePass::run(Function &F, FunctionAnalysisManager &FAM) {
  bool Changed = false;

  for (auto &BB : F) {
    for (auto Inst = BB.begin(), E = BB.end(); Inst != E;) {
      Instruction *I = &*Inst++;

      // -------------------------------
      // Constant folding
      // -------------------------------
      if (auto *BinOp = dyn_cast<BinaryOperator>(I)) {
        if (auto *C1 = dyn_cast<ConstantInt>(BinOp->getOperand(0))) {
          if (auto *C2 = dyn_cast<ConstantInt>(BinOp->getOperand(1))) {
            Constant *Folded = ConstantExpr::get(BinOp->getOpcode(), C1, C2);
            I->replaceAllUsesWith(Folded);
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }
      }

      // -------------------------------
      // Algebraic simplifications
      // -------------------------------
      Value *X;
      if (match(I, m_Mul(m_Value(X), m_ConstantInt<2>()))) {
        IRBuilder<> Builder(I);
        Value *Add = Builder.CreateAdd(X, X);
        I->replaceAllUsesWith(Add);
        I->eraseFromParent();
        Changed = true;
        continue;
      }

      if (match(I, m_Add(m_Value(X), m_Zero())) ||
          match(I, m_Add(m_Zero(), m_Value(X))) ||
          match(I, m_Mul(m_Value(X), m_One())) ||
          match(I, m_Mul(m_One(), m_Value(X))) ||
          match(I, m_Sub(m_Value(X), m_Zero())) ||
          match(I, m_UDiv(m_Value(X), m_One())) ||
          match(I, m_SDiv(m_Value(X), m_One()))) {
        I->replaceAllUsesWith(X);
        I->eraseFromParent();
        Changed = true;
        continue;
      }

      ConstantInt *C;
      if (match(I, m_And(m_Value(X), m_ConstantInt(C))) && C->isAllOnesValue()) {
        I->replaceAllUsesWith(X);
        I->eraseFromParent();
        Changed = true;
        continue;
      }

      // -------------------------------
      // Dead Code Elimination (DCE)
      // -------------------------------
      if (I->use_empty() && I->isSafeToRemove()) {
        I->eraseFromParent();
        Changed = true;
        continue;
      }
    }
  }

  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
