//===-- TinyGPU.td - Describe the TinyGPU Target Machine -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"
include "llvm/TableGen/SearchableTable.td"

//===----------------------------------------------------------------------===//
// TinyGPU Subtarget features.
//

def FeatureSoftMulDiv
  : SubtargetFeature<"soft-mul-div", "UseSoftMulDiv", "true",
                     "Use software emulation for integer multiply and divide">;

def FeatureNoFSMULD
  : SubtargetFeature<"no-fsmuld", "HasNoFSMULD", "true",
                     "Disable the fsmuld instruction.">;
def FeatureNoFMULS
  : SubtargetFeature<"no-fmuls", "HasNoFMULS", "true",
                     "Disable the fmuls instruction.">;

def FeatureV9
  : SubtargetFeature<"v9", "IsV9", "true",
                     "Enable TinyGPU-V9 instructions">;
def FeatureV8Plus
  : SubtargetFeature<"v8plus", "IsV8Plus", "true",
                     "Enable V8+ mode, allowing use of 64-bit V9 instructions in 32-bit code">;
def FeatureV8Deprecated
  : SubtargetFeature<"deprecated-v8", "UseV8DeprecatedInsts", "true",
                     "Enable deprecated V8 instructions in V9 mode">;
def FeatureVIS
  : SubtargetFeature<"vis", "IsVIS", "true",
                     "Enable UltraTinyGPU Visual Instruction Set extensions">;
def FeatureVIS2
  : SubtargetFeature<"vis2", "IsVIS2", "true",
                     "Enable Visual Instruction Set extensions II">;
def FeatureVIS3
  : SubtargetFeature<"vis3", "IsVIS3", "true",
                     "Enable Visual Instruction Set extensions III">;
def FeatureLeon
  : SubtargetFeature<"leon", "IsLeon", "true",
                     "Enable LEON extensions">;
def FeaturePWRPSR
  : SubtargetFeature<"leonpwrpsr", "HasPWRPSR", "true",
                     "Enable the PWRPSR instruction">;

def FeatureHardQuad
  : SubtargetFeature<"hard-quad-float", "HasHardQuad", "true",
                     "Enable quad-word floating point instructions">;

def UsePopc : SubtargetFeature<"popc", "UsePopc", "true",
                               "Use the popc (population count) instruction">;

def FeatureSoftFloat : SubtargetFeature<"soft-float", "UseSoftFloat", "true",
                              "Use software emulation for floating point">;

//===----------------------------------------------------------------------===//
// TinyGPU Subtarget tuning features.
//

def TuneSlowRDPC : SubtargetFeature<"slow-rdpc", "HasSlowRDPC", "true",
                                    "rd %pc, %XX is slow", [FeatureV9]>;

//==== Features added predmoninantly for LEON subtarget support
include "LeonFeatures.td"

//==== Register allocation tweaks needed by some low-level software
foreach i = 1 ... 7  in
    def FeatureReserveG#i : SubtargetFeature<"reserve-g"#i, "ReserveRegister["#i#" + SP::G0]", "true",
                                             "Reserve G"#i#", making it unavailable as a GPR">;
foreach i = 0 ... 5 in
    def FeatureReserveO#i : SubtargetFeature<"reserve-o"#i, "ReserveRegister["#i#" + SP::O0]", "true",
                                             "Reserve O"#i#", making it unavailable as a GPR">;
foreach i = 0 ... 7 in
    def FeatureReserveL#i : SubtargetFeature<"reserve-l"#i, "ReserveRegister["#i#" + SP::L0]", "true",
                                             "Reserve L"#i#", making it unavailable as a GPR">;
foreach i = 0 ... 5 in
    def FeatureReserveI#i : SubtargetFeature<"reserve-i"#i, "ReserveRegister["#i#" + SP::I0]", "true",
                                             "Reserve I"#i#", making it unavailable as a GPR">;

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "TinyGPUASITags.td"
include "TinyGPUPrefetchTags.td"
include "TinyGPURegisterInfo.td"
include "TinyGPUCallingConv.td"
include "TinyGPUSchedule.td"
include "TinyGPUInstrInfo.td"

def TinyGPUInstrInfo : InstrInfo;

def TinyGPUAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = true;
  let AllowDuplicateRegisterNames = true;
}

def TinyGPUAsmParserVariant : AsmParserVariant {
  let RegisterPrefix = "%";
}

//===----------------------------------------------------------------------===//
// TinyGPU processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features,
           list<SubtargetFeature> TuneFeatures = []>
 : Processor<Name, NoItineraries, Features, TuneFeatures>;

def : Proc<"generic",         []>;
def : Proc<"v7",              [FeatureSoftMulDiv, FeatureNoFSMULD]>;
def : Proc<"v8",              []>;
def : Proc<"superTinyGPU",      []>;
def : Proc<"TinyGPUlite",       []>;
def : Proc<"f934",            []>;
def : Proc<"hyperTinyGPU",      []>;
def : Proc<"TinyGPUlite86x",    []>;
def : Proc<"TinyGPUlet",        []>;
def : Proc<"tsc701",          []>;
def : Proc<"myriad2",         [FeatureLeon, LeonCASA]>;
def : Proc<"myriad2.1",       [FeatureLeon, LeonCASA]>;
def : Proc<"myriad2.2",       [FeatureLeon, LeonCASA]>;
def : Proc<"myriad2.3",       [FeatureLeon, LeonCASA]>;
def : Proc<"ma2100",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2150",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2155",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2450",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2455",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2x5x",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2080",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2085",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2480",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2485",          [FeatureLeon, LeonCASA]>;
def : Proc<"ma2x8x",          [FeatureLeon, LeonCASA]>;
def : Proc<"v9",              [FeatureV9]>;
def : Proc<"ultraTinyGPU",      [FeatureV9, FeatureV8Deprecated, FeatureVIS],
                              [TuneSlowRDPC]>;
def : Proc<"ultraTinyGPU3",     [FeatureV9, FeatureV8Deprecated, FeatureVIS,
                               FeatureVIS2],
                              [TuneSlowRDPC]>;
def : Proc<"niagara",         [FeatureV9, FeatureV8Deprecated, FeatureVIS,
                               FeatureVIS2]>;
def : Proc<"niagara2",        [FeatureV9, FeatureV8Deprecated, UsePopc,
                               FeatureVIS, FeatureVIS2]>;
def : Proc<"niagara3",        [FeatureV9, FeatureV8Deprecated, UsePopc,
                               FeatureVIS, FeatureVIS2]>;
def : Proc<"niagara4",        [FeatureV9, FeatureV8Deprecated, UsePopc,
                               FeatureVIS, FeatureVIS2, FeatureVIS3]>;

// LEON 2 FT generic
def : Processor<"leon2", LEON2Itineraries,
                [FeatureLeon]>;

// LEON 2 FT (AT697E)
// TO DO: Place-holder: Processor specific features will be added *very* soon here.
def : Processor<"at697e", LEON2Itineraries,
                [FeatureLeon, InsertNOPLoad]>;

// LEON 2 FT (AT697F)
// TO DO: Place-holder: Processor specific features will be added *very* soon here.
def : Processor<"at697f", LEON2Itineraries,
                [FeatureLeon, InsertNOPLoad]>;


// LEON 3 FT generic
def : Processor<"leon3", LEON3Itineraries,
                [FeatureLeon, UMACSMACSupport]>;

// LEON 3 FT (UT699). Provides features for the UT699 processor
// - covers all the erratum fixes for LEON3, but does not support the CASA instruction.
def : Processor<"ut699", LEON3Itineraries,
                [FeatureLeon, InsertNOPLoad, FeatureNoFSMULD, FeatureNoFMULS, FixAllFDIVSQRT]>;

// LEON3 FT (GR712RC). Provides features for the GR712RC processor.
// - covers all the erratum fixed for LEON3 and support for the CASA instruction.
def : Processor<"gr712rc", LEON3Itineraries,
                [FeatureLeon, LeonCASA]>;

// LEON 4 FT generic
def : Processor<"leon4", LEON4Itineraries,
                [FeatureLeon, UMACSMACSupport, LeonCASA]>;

// LEON 4 FT (GR740)
// TO DO: Place-holder: Processor specific features will be added *very* soon here.
def : Processor<"gr740", LEON4Itineraries,
                [FeatureLeon, UMACSMACSupport, LeonCASA, LeonCycleCounter,
                 FeaturePWRPSR]>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def TinyGPUAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  int PassSubtarget = 1;
  int Variant = 0;
}

def TinyGPU : Target {
  // Pull in Instruction Info:
  let InstructionSet = TinyGPUInstrInfo;
  let AssemblyParsers  = [TinyGPUAsmParser];
  let AssemblyParserVariants = [TinyGPUAsmParserVariant];
  let AssemblyWriters = [TinyGPUAsmWriter];
  let AllowRegisterRenaming = 1;
}
