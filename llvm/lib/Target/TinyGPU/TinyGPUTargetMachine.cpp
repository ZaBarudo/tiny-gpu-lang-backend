//===-- TinyGPUTargetMachine.cpp - Define TargetMachine for TinyGPU -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about TinyGPU target spec.
//
//===----------------------------------------------------------------------===//

#include "TinyGPUTargetMachine.h"
#include "TinyGPUISelDAGToDAG.h"
#include "TinyGPUSubtarget.h"
#include "TinyGPUTargetObjectFile.h"
#include "TargetInfo/TinyGPUTargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

// This function initializes the TinyGPU target in the LLVM framework.
// It registers the TinyGPU target machine and ensures that the necessary
// passes for instruction selection are initialized.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTinyGPUTarget() {
  // Register the TinyGPU target machine with the LLVM target registry.
  RegisterTargetMachine<TinyGPUTargetMachine> R(getTheTinyGPUTarget());

  // Get the global pass registry to register the TinyGPU-specific passes.
  PassRegistry &PR = *PassRegistry::getPassRegistry();

  // Initialize the TinyGPU DAG-to-DAG instruction selector pass.
  initializeTinyGPUDAGToDAGISelLegacyPass(PR);
}

// This function computes the data layout string for the TinyGPU target.
// The data layout string describes the memory layout of the target architecture,
// including endianness, pointer size, alignment, and other characteristics.
static std::string computeDataLayout() {
  std::string Ret = "";

  // Specify little-endian format.
  Ret += "e";

  // Use ELF name mangling.
  Ret += "-m:e";

  // Define 32-bit pointers with 32-bit alignment.
  Ret += "-p:32:32";

  // Define 64-bit integers with 64-bit alignment.
  Ret += "-i64:64";

  // Specify 32-bit native integer width (registers are 32-bit).
  Ret += "-n32";

  // Specify 128-bit natural stack alignment.
  Ret += "-S128";

  return Ret;
}

// This function determines the effective relocation model to be used
// for the TinyGPU target. If a relocation model is explicitly provided
// via the RM parameter, it is used. Otherwise, the default relocation
// model (Reloc::Static) is returned.
//
// Parameters:
// - CM: An optional code model parameter (not used in this function).
// - RM: An optional relocation model parameter.
//
// Returns:
// - The effective relocation model to be used.
static Reloc::Model getEffectiveRelocModel(std::optional<CodeModel::Model> CM,
                       std::optional<Reloc::Model> RM) {
  // Use the provided relocation model if available; otherwise, default to Reloc::Static.
  return RM.value_or(Reloc::Static);
}

// Constructor for the TinyGPUTargetMachine class.
// This initializes the target machine for the TinyGPU architecture.
//
// Parameters:
// - T: The target object representing the TinyGPU target.
// - TT: The target triple, which specifies the target architecture, vendor, and OS.
// - CPU: The CPU string specifying the target CPU.
// - FS: The feature string specifying the target-specific features.
// - Options: Target options for code generation.
// - RM: An optional relocation model parameter.
// - CM: An optional code model parameter.
// - OL: The optimization level for code generation.
// - JIT: A boolean indicating whether the target machine is for JIT compilation.
TinyGPUTargetMachine::TinyGPUTargetMachine(const Target &T, const Triple &TT,
                     StringRef CPU, StringRef FS,
                     const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM,
                     CodeGenOptLevel OL,
                     bool JIT)
  // Initialize the base class with the computed data layout, target triple,
  // CPU, feature string, target options, effective relocation model, effective
  // code model, and optimization level.
  : CodeGenTargetMachineImpl(T, computeDataLayout(), TT, CPU, FS, Options,
            getEffectiveRelocModel(CM, RM),
            getEffectiveCodeModel(CM, CodeModel::Medium), OL),
    // Initialize the target-specific object file implementation.
    TLOF(std::make_unique<TinyGPUTargetObjectFile>()) {
  // Initialize assembly information for the target machine.
  // This will display features when using llc with -march=TinyGPU.
  initAsmInfo();
}

// This function retrieves the subtarget implementation for a given function.
// It determines the CPU and feature set for the function based on its attributes
// and creates or retrieves a cached subtarget instance.
//
// Parameters:
// - F: The function for which the subtarget is being queried.
//
// Returns:
// - A pointer to the TinyGPUSubtarget instance corresponding to the function.
const TinyGPUSubtarget *
TinyGPUTargetMachine::getSubtargetImpl(const Function &F) const {
  // Retrieve the "target-cpu" attribute from the function, if present.
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  // Retrieve the "target-features" attribute from the function, if present.
  Attribute FSAttr = F.getFnAttribute("target-features");

  // Determine the CPU string. Use the attribute value if available; otherwise, use the default.
  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  // Determine the feature string. Use the attribute value if available; otherwise, use the default.
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  // Construct a unique key for the subtarget map using the CPU and feature strings.
  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // Reset the target options based on the function's attributes before creating a new subtarget.
    resetTargetOptions(F);
    // Create a new TinyGPUSubtarget instance and cache it in the subtarget map.
    I = std::make_unique<TinyGPUSubtarget>(TargetTriple, CPU, FS, *this);
  }
  // Return the cached or newly created subtarget instance.
  return I.get();
}

// This anonymous namespace contains the TinyGPUPassConfig class, which is used
// to configure the passes for the TinyGPU target during code generation.
namespace {

// The TinyGPUPassConfig class is a subclass of TargetPassConfig and is responsible
// for configuring the pass pipeline for the TinyGPU target. It overrides methods
// to add target-specific passes, such as the instruction selector and pre-emit passes.
class TinyGPUPassConfig : public TargetPassConfig {
public:
  // Constructor for the TinyGPUPassConfig class.
  // It initializes the base TargetPassConfig class with the target machine and pass manager.
  //
  // Parameters:
  // - TM: The TinyGPUTargetMachine instance representing the target machine.
  // - PM: The pass manager that manages the pipeline of passes.
  TinyGPUPassConfig(TinyGPUTargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  // This function retrieves the TinyGPUTargetMachine instance associated with this pass config.
  //
  // Returns:
  // - A reference to the TinyGPUTargetMachine instance.
  TinyGPUTargetMachine &getTinyGPUTargetMachine() const {
    return getTM<TinyGPUTargetMachine>();
  }

  // This function adds the instruction selector pass to the pass pipeline.
  // The instruction selector pass is responsible for converting the LLVM IR
  // into target-specific machine instructions.
  //
  // Returns:
  // - A boolean indicating whether the pass was successfully added.
  bool addInstSelector() override;

  // This function adds any passes that need to run immediately before machine code is emitted.
  // It is typically used for target-specific optimizations or final adjustments to the code.
  void addPreEmitPass() override;
};

} // end anonymous namespace

TargetPassConfig *TinyGPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new TinyGPUPassConfig(*this, PM);
}

// Install an instruction selector pass using
// the ISelDag to gen TinyGPU code.
bool TinyGPUPassConfig::addInstSelector() {
  addPass(createTinyGPUISelDag(getTinyGPUTargetMachine(), getOptLevel()));
  return false;
}

// Implemented by targets that want to run passes immediately before
// machine code is emitted. return true if -print-machineinstrs should
// print out the code after the passes.
void TinyGPUPassConfig::addPreEmitPass() {
}
