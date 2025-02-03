add_llvm_component_library(LLVMTinyGPUDisassembler
  TinyGPUDisassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  MC
  TinyGPUInfo
  Support

  ADD_TO_COMPONENT
  TinyGPU
  )
