#ifndef LLVM_TRANSFORMS_UTILS_PEEPHOLEPASS_H
#define LLVM_TRANSFORMS_UTILS_PEEPHOLEPASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class PeepholePass : public PassInfoMixin<PeepholePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_PEEPHOLEPASS_H