add_llvm_component_group(TinyGPU)

set(LLVM_TARGET_DEFINITIONS TinyGPU.td)

tablegen(LLVM TinyGPUGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM TinyGPUGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TinyGPUGenCallingConv.inc -gen-callingconv)
tablegen(LLVM TinyGPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TinyGPUGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM TinyGPUGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TinyGPUGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM TinyGPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TinyGPUGenSearchableTables.inc -gen-searchable-tables)
tablegen(LLVM TinyGPUGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(TinyGPUCommonTableGen)

add_llvm_target(TinyGPUCodeGen
  DelaySlotFiller.cpp
  LeonPasses.cpp
  TinyGPUAsmPrinter.cpp
  TinyGPUInstrInfo.cpp
  TinyGPUISelDAGToDAG.cpp
  TinyGPUISelLowering.cpp
  TinyGPUFrameLowering.cpp
  TinyGPUMachineFunctionInfo.cpp
  TinyGPURegisterInfo.cpp
  TinyGPUSubtarget.cpp
  TinyGPUTargetMachine.cpp
  TinyGPUMCInstLower.cpp
  TinyGPUTargetObjectFile.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  TinyGPUDesc
  TinyGPUInfo
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  TinyGPU
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
