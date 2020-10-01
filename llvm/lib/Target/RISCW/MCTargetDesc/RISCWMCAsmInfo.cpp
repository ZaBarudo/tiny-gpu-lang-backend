//===-- RISCWMCAsmInfo.cpp - RISCW Asm Properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the RISCWMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "RISCWMCAsmInfo.h"

#include "llvm/ADT/Triple.h"

using namespace llvm;

void RISCWMCAsmInfo::anchor() { }

RISCWMCAsmInfo::RISCWMCAsmInfo(const Triple &TheTriple) {
  // This architecture is little endian only
  IsLittleEndian = false;

  AlignmentIsInBytes          = false;
  Data16bitsDirective         = "\t.hword\t";
  Data32bitsDirective         = "\t.word\t";
  Data64bitsDirective         = "\t.dword\t";

  PrivateGlobalPrefix         = ".L";
  PrivateLabelPrefix          = ".L";

  LabelSuffix                 = ":";

  CommentString               = ";";

  ZeroDirective               = "\t.zero\t";

  UseAssignmentForEHBegin     = true;

  SupportsDebugInformation    = true;
  ExceptionsType              = ExceptionHandling::DwarfCFI;
  DwarfRegNumForCFI           = true;
}
