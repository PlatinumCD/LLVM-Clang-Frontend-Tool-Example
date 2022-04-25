cmake_minimum_required(VERSION 3.10)

execute_process(
    COMMAND spack location --install-dir llvm
    OUTPUT_VARIABLE LLVM_PATH
)

string(STRIP "${LLVM_PATH}" LLVM_PATH)

set(CMAKE_C_COMPILER ${LLVM_PATH}/bin/clang)
set(CMAKE_CXX_COMPILER ${LLVM_PATH}/bin/clang++)

#set(CMAKE_INSTALL_RPATH ${LLVM_PATH}/lib)
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_compile_options(
    --gcc-toolchain=/packages/gcc/7.3
    -pedantic 
    -w
    -Wno-unused-parameter 
    -fvisibility=hidden
    -std=c++14
    -fno-exceptions
    -fno-rtti 
    -D_GNU_SOURCE 
    -D_DEBUG 
    -D__STDC_CONSTANT_MACROS 
    -D__STDC_FORMAT_MACROS 
    -D__STDC_LIMIT_MACROS
)

project(mini-instrumentor)

include_directories(
    ${LLVM_PATH}/include
)

link_directories(
    ${LLVM_PATH}/lib
)

add_link_options(
    --gcc-toolchain=/packages/gcc/7.3
    -pedantic
    -w
    -Wno-unused-parameter
    -fvisibility=hidden
    -std=c++14
    -fno-exceptions
    -fno-rtti
    -D_GNU_SOURCE
    -D_DEBUG
    -D__STDC_CONSTANT_MACROS
    -D__STDC_FORMAT_MACROS
    -D__STDC_LIMIT_MACROS
)



link_libraries(
   rt
   dl
   m
   z
   tinfo
   pthread
   clang 
   clangFrontendTool 
   clangRewriteFrontend 
   clangDynamicASTMatchers 
   clangFormat 
   clangTooling 
   clangFrontend 
   clangToolingCore 
   clangASTMatchers 
   clangCrossTU 
   clangStaticAnalyzerCore 
   clangStaticAnalyzerCheckers 
   clangStaticAnalyzerFrontend 
   clangARCMigrate 
   clangParse 
   clangDriver 
   clangSerialization 
   clangRewrite 
   clangSema 
   clangEdit 
   clangIndex 
   clangAnalysis 
   clangAST 
   clangLex 
   clangBasic
   clangToolingInclusions
   clangCodeGen
   LLVMWindowsManifest 
   LLVMXRay 
   LLVMLibDriver 
   LLVMDlltoolDriver 
   LLVMCoverage 
   LLVMLineEditor 
   LLVMX86Disassembler 
   LLVMX86AsmParser 
   LLVMX86CodeGen 
   LLVMX86Desc 
   LLVMX86Info 
   LLVMMCJIT 
   LLVMJITLink 
   LLVMInterpreter 
   LLVMExecutionEngine 
   LLVMRuntimeDyld 
   LLVMOrcTargetProcess 
   LLVMOrcShared 
   LLVMDWP 
   LLVMSymbolize 
   LLVMDebugInfoPDB 
   LLVMDebugInfoGSYM 
   LLVMOption 
   LLVMObjectYAML 
   LLVMMCA 
   LLVMMCDisassembler 
   LLVMLTO 
   LLVMPasses 
   LLVMCFGuard 
   LLVMCoroutines 
   LLVMObjCARCOpts 
   LLVMipo 
   LLVMVectorize 
   LLVMLinker 
   LLVMInstrumentation 
   LLVMFrontendOpenMP 
   LLVMFrontendOpenACC 
   LLVMExtensions 
   LLVMDWARFLinker 
   LLVMGlobalISel 
   LLVMMIRParser 
   LLVMAsmPrinter 
   LLVMDebugInfoMSF 
   LLVMDebugInfoDWARF 
   LLVMSelectionDAG 
   LLVMCodeGen 
   LLVMIRReader 
   LLVMAsmParser 
   LLVMInterfaceStub 
   LLVMFileCheck 
   LLVMFuzzMutate 
   LLVMTarget 
   LLVMScalarOpts 
   LLVMInstCombine 
   LLVMAggressiveInstCombine 
   LLVMTransformUtils 
   LLVMBitWriter 
   LLVMAnalysis 
   LLVMProfileData 
   LLVMObject 
   LLVMTextAPI 
   LLVMMCParser 
   LLVMMC 
   LLVMDebugInfoCodeView 
   LLVMBitReader 
   LLVMCore 
   LLVMRemarks 
   LLVMBitstreamReader 
   LLVMBinaryFormat 
   LLVMTableGen 
   LLVMSupport 
   LLVMDemangle
)


add_executable(mini-instrumentor mini-instrumentor.cpp)
