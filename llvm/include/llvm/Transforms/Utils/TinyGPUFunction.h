#ifndef LLVM_TRANSFORMS_HELLONEW_HELLOWORLD2_H
#define LLVM_TRANSFORMS_HELLONEW_HELLOWORLD2_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class TinyGPUFunctionPass : public PassInfoMixin<TinyGPUFunctionPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_HELLONEW_HELLOWORLD2_H