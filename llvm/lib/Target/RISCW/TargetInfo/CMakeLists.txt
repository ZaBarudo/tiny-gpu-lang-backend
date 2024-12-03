add_llvm_component_library(LLVMRISCWInfo
  RISCWTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  RISCW
)
