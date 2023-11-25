cmake_minimum_required(VERSION 3.16)

find_program(CLANGXX_COMPILER clang++-12)

if(NOT CLANGXX_COMPILER)
  message(FATAL_ERROR "clang++-12 compiler not found. Please make sure it is installed or provide the correct path.")
endif()

set(CMAKE_CXX_COMPILER ${CLANGXX_COMPILER})
set(CMAKE_LINKER ${CLANGXX_LINKER})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(TypeInfoHashPass CXX)

unset(CMAKE_CXX_FLAGS)
unset(CMAKE_CXX_COMPILER_TARGET)
unset(CMAKE_CXX_COMPILE_OPTIONS_EXTERNAL_TOOLCHAIN)
unset(CMAKE_TOOLCHAIN_FILE)
unset(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES)
unset(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
unset(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
unset(CMAKE_SYSROOT)
unset(CMAKE_SYSROOT_COMPILE)
unset(CMAKE_COMPILER_SYSROOT)
unset(CMAKE_CXX_COMPILE_OPTIONS_SYSROOT)
unset(CMAKE_SHARED_LIBRARY_CXX_FLAGS)
unset(CMAKE_C_STANDARD_LIBRARIES_INIT)
unset(CMAKE_CXX_STANDARD_LIBRARIES_INIT)
unset(CMAKE_CXX_STANDARD_LIBRARIES)
unset(CMAKE_SHARED_LINKER_FLAGS_INIT)
unset(CMAKE_SHARED_LINKER_FLAGS)

set(SOURCES
  src/TypeInfoHashPass.cpp
)

add_library(
  TypeInfoHashPass
  SHARED
  ${SOURCES}
)
target_link_libraries(TypeInfoHashPass PUBLIC
  LLVM-12
)

# Add include directories to the target
target_include_directories(TypeInfoHashPass
  PRIVATE
  /usr/include/llvm-c-12/
  /usr/include/llvm-12/
)

target_compile_options(TypeInfoHashPass
  PRIVATE
  -fPIC -fno-exceptions
  -fdiagnostics-color=always
)