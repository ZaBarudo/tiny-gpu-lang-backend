add_llvm_component_library(LLVMTinyGPUDesc
  TinyGPUMCAsmInfo.cpp
  TinyGPUMCTargetDesc.cpp
  TinyGPUInstPrinter.cpp
  
  LINK_COMPONENTS
  MC
  TinyGPUInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  TinyGPU
)
