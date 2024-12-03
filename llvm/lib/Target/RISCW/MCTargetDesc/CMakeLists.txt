add_llvm_component_library(LLVMRISCWDesc
  RISCWMCAsmInfo.cpp
  RISCWMCTargetDesc.cpp
  RISCWInstPrinter.cpp
  
  LINK_COMPONENTS
  MC
  RISCWInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  RISCW
)
