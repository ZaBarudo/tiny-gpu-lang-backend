add_llvm_component_library(LLVMTinyGPUInfo
  TinyGPUTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  TinyGPU
)
