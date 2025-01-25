//===-- TinyGPUMCAsmInfo.h - TinyGPU Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the TinyGPUMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TINYGPU_MCTARGETDESC_TINYGPUMCASMINFO_H
#define LLVM_LIB_TARGET_TINYGPU_MCTARGETDESC_TINYGPUMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class Triple;

class TinyGPUMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit TinyGPUMCAsmInfo(const Triple &TheTriple);
};

} // namespace llvm

#endif // end LLVM_LIB_TARGET_TINYGPU_MCTARGETDESC_TINYGPUMCASMINFO_H
