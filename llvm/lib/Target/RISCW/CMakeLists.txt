set(LLVM_TARGET_DEFINITIONS RISCW.td)

tablegen(LLVM RISCWGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM RISCWGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM RISCWGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM RISCWGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM RISCWGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM RISCWGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM RISCWGenCallingConv.inc -gen-callingconv)
tablegen(LLVM RISCWGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM RISCWGenMCPseudoLowering.inc -gen-pseudo-lowering)

# RISCWCommonTableGen must be defined
add_public_tablegen_target(RISCWCommonTableGen)

# RISCWCodeGen should match with LLVMBuild.txt RISCWCodeGen
add_llvm_target(RISCWCodeGen
  RISCWAsmPrinter.cpp
  RISCWFrameLowering.cpp
  RISCWInstrInfo.cpp
  RISCWISelLowering.cpp
  RISCWRegisterInfo.cpp
  RISCWSubtarget.cpp
  RISCWTargetObjectFile.cpp
  RISCWISelDAGToDAG.cpp
  RISCWTargetMachine.cpp
)

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
