//===--- RISCW.cpp - Implement RISCW target feature support ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements RISCWTargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "TinyGPU.h"
#include "clang/Basic/MacroBuilder.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

const char *const TinyGPUTargetInfo::GCCRegNames[] = {
  // Integer registers
  "R0",  "R1",  "R2",  "R3",  "R4",  "R5",  "R6",  "R7",
  "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15",
};

const TargetInfo::GCCRegAlias GCCRegAliases[] = {
  {{"zero"}, "R0"}, {{"ra"}, "R1"},   {{"sp"}, "R2"},    {{"gp"}, "R3"},
  {{"tp"}, "R4"},   {{"t0"}, "R5"},   {{"t1"}, "R6"},    {{"t2"}, "R7"},
  {{"s0"}, "R8"},   {{"s1"}, "R9"},   {{"a0"}, "R10"},   {{"a1"}, "R11"},
  {{"a2"}, "R12"},  {{"a3"}, "R13"},  {{"a4"}, "R14"},   {{"a5"}, "R15"}};

ArrayRef<const char *> TinyGPUTargetInfo::getGCCRegNames() const {
  return llvm::makeArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> TinyGPUTargetInfo::getGCCRegAliases() const {
  return llvm::makeArrayRef(GCCRegAliases);
}

void TinyGPUTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  // Define the __RISCW__ macro when building for this target
  Builder.defineMacro("__TINY_GPU__");
}
