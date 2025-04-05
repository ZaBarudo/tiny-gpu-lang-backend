//===-- TinyGPUSubtarget.cpp - TinyGPU Subtarget Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TinyGPU specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "TinyGPU.h"
#include "TinyGPUSubtarget.h"
#include "TinyGPUMachineFunction.h"
#include "TinyGPURegisterInfo.h"
#include "TinyGPUTargetMachine.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "TinyGPU-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "TinyGPUGenSubtargetInfo.inc"

/// Constructor for the TinyGPUSubtarget class.
/// 
/// This initializes the subtarget for the TinyGPU target, setting up various
/// components such as instruction information, frame lowering, target lowering,
/// and register information. It also processes the CPU and feature string
/// options to configure the subtarget appropriately.
///
/// \param TT The target triple, representing the target platform.
/// \param CPU The CPU string, specifying the target CPU.
/// \param FS The feature string, specifying the target-specific features.
/// \param TM The target machine, providing context for the target configuration.
TinyGPUSubtarget::TinyGPUSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                 const TargetMachine &TM)
  : TinyGPUGenSubtargetInfo(TT, CPU, /*TuneCPU*/ CPU, FS),
    TSInfo(), // Initializes target-specific information.
    InstrInfo(initializeSubtargetDependencies(TT, CPU, FS, TM)), // Sets up instruction information.
    FrameLowering(*this), // Configures frame lowering for the subtarget.
    TLInfo(TM, *this), // Sets up target lowering information.
    RegInfo(*this) { } // Initializes register information.


/// Initializes subtarget-specific dependencies for the TinyGPU target.
///
/// This function processes the CPU and feature string to configure the
/// subtarget appropriately. It ensures that a valid CPU name is set and
/// parses the feature string to enable or disable specific target features.
///
/// \param TT The target triple, representing the target platform.
/// \param CPU The CPU string, specifying the target CPU.
/// \param FS The feature string, specifying the target-specific features.
/// \param TM The target machine, providing context for the target configuration.
/// \returns A reference to the initialized subtarget.
TinyGPUSubtarget &
TinyGPUSubtarget::initializeSubtargetDependencies(const Triple &TT, StringRef CPU,
                        StringRef FS,
                        const TargetMachine &TM) {
  // If no CPU name is provided, default to "generic".
  std::string CPUName(CPU);
  if (CPUName.empty())
  CPUName = "generic";

  // Parse the feature string to configure subtarget features.
  ParseSubtargetFeatures(CPUName, /*TuneCPU*/ CPUName, FS);

  // Return a reference to the initialized subtarget.
  return *this;
}
