add_llvm_component_group(TinyGPU)

set(LLVM_TARGET_DEFINITIONS TinyGPU.td)

tablegen(LLVM TinyGPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TinyGPUGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TinyGPUGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM TinyGPUGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM TinyGPUGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TinyGPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TinyGPUGenCallingConv.inc -gen-callingconv)
tablegen(LLVM TinyGPUGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM TinyGPUGenMCPseudoLowering.inc -gen-pseudo-lowering)

# TinyGPUCommonTableGen must be defined
add_public_tablegen_target(TinyGPUCommonTableGen)

# TinyGPUCodeGen should match with LLVMBuild.txt TinyGPUCodeGen
add_llvm_target(TinyGPUCodeGen
  TinyGPUAsmPrinter.cpp
  TinyGPUFrameLowering.cpp
  TinyGPUInstrInfo.cpp
  TinyGPUISelLowering.cpp
  TinyGPURegisterInfo.cpp
  TinyGPUSubtarget.cpp
  TinyGPUTargetObjectFile.cpp
  TinyGPUISelDAGToDAG.cpp
  TinyGPUTargetMachine.cpp
  
  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  TinyGPUDesc
  TinyGPUInfo
  SelectionDAG
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  TinyGPU
)

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
