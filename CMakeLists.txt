cmake_minimum_required(VERSION 3.16)

set(LLVM_IR_CMAKE_UTILS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvm-ir-cmake-utils/cmake/)
list(APPEND CMAKE_MODULE_PATH ${LLVM_IR_CMAKE_UTILS_DIR})

include(LLVMIRUtil)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld")

# Find the opt-12 executable
unset(LLVMIR_OPT)
find_program(LLVMIR_OPT opt-12)

if(NOT LLVMIR_OPT)
  message(FATAL_ERROR "opt-12 not found. Please make sure it is installed or provide the correct path.")
endif()

# Example
set(ANDROID_TOOLCHAIN clang)
set(ANDROID_SYSTEM_NAME Android)
set(ANDROID_NDK $ENV{HOME}/ndk/android-ndk-r23c)
set(ANDROID_PLATFORM android-23)
set(ANDROID_ABI armeabi-v7a)
set(ANDROID_ARM_MODE thumb)
set(ANDROID_CPP_FEATURES "no-rtti exceptions")
set(ANDROID_STL c++_static)
set(ANDROID_DISABLE_RELRO TRUE)
set(ANDROID_NOEXECSTACK FALSE)

set(ANDROID_NDK $ENV{HOME}/ndk/android-ndk-r23c)
set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)

project(llvm-seacrits CXX)

set(SOURCES
  src/example.cpp
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(plugins)

include(CheckIPOSupported)
check_ipo_supported()

set(CMAKE_C_COMPILER ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi23-clang)
set(CMAKE_CXX_COMPILER ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi23-clang++)
find_program(NDK_STRIP strip HINTS ${ANDROID_NDK}/toolchains/llvm/prebuilt/${CMAKE_HOST_SYSTEM_PROCESSOR}/bin)

function(add_custom_optimized_executable target_name sources)
  set(fake_target_name "_${target_name}")
  add_executable(${fake_target_name} ${sources})
  set_target_properties(${fake_target_name} PROPERTIES LINKER_LANGUAGE CXX)
  set_target_properties(${fake_target_name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)

  set(custom_bc_target ${target_name}_bc)
  llvmir_attach_bc_target(${custom_bc_target} ${fake_target_name} DEPENDS_TRGT)

  set(custom_pass1_target ${target_name}_pass1)
  llvmir_attach_opt_pass_target(${custom_pass1_target} ${custom_bc_target} -load=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libTypeInfoHashPass.so -modify-type-info)

  add_dependencies(${custom_pass1_target} TypeInfoHashPass)
  llvmir_attach_executable(${target_name} ${custom_pass1_target})
  add_dependencies(${target_name} ${custom_pass1_target})
  set_target_properties(${fake_target_name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
  set_target_properties(${target_name} PROPERTIES EXCLUDE_FROM_ALL FALSE)

  # Add custom command to strip symbols in Release mode
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
      TARGET ${target_name}
      POST_BUILD
      COMMAND ${NDK_STRIP} -s -x -R .comment $<TARGET_FILE:${target_name}>
      COMMENT "Stripping symbols from ${target_name} in Release mode"
    )
  endif()
endfunction()

function(add_custom_optimized_library target_name type sources)
  set(fake_target_name "_${target_name}")
  add_library(${fake_target_name} ${type} ${sources})
  set_target_properties(${fake_target_name} PROPERTIES LINKER_LANGUAGE CXX)
  set_target_properties(${fake_target_name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)

  set(custom_bc_target ${target_name}_bc)
  llvmir_attach_bc_target(${custom_bc_target} ${fake_target_name} DEPENDS_TRGT)

  set(custom_pass1_target ${target_name}_pass1)
  llvmir_attach_opt_pass_target(${custom_pass1_target} ${custom_bc_target} -load=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libTypeInfoHashPass.so -modify-type-info)

  add_dependencies(${custom_pass1_target} TypeInfoHashPass)
  llvmir_attach_library(${target_name} ${custom_pass1_target})
  add_dependencies(${target_name} ${custom_pass1_target})

  # Add custom command to strip symbols in Release mode
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
      TARGET ${target_name}
      POST_BUILD
      COMMAND ${NDK_STRIP} -s -x -R .comment $<TARGET_FILE:${target_name}>
      COMMENT "Stripping symbols from ${target_name} in Release mode"
    )
  endif()
  set_target_properties(${fake_target_name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
  set_target_properties(${target_name} PROPERTIES EXCLUDE_FROM_ALL FALSE)
endfunction()

function(print_cmake_variables)
  get_cmake_property(_variableNames VARIABLES)
  list(SORT _variableNames)

  foreach(_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endfunction()

add_custom_optimized_executable(example
  ${SOURCES}
)
