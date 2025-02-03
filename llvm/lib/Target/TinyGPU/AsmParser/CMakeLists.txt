add_llvm_component_library(LLVMTinyGPUAsmParser
  TinyGPUAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  TinyGPUDesc
  TinyGPUInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  TinyGPU
  )
