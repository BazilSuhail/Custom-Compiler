cmake_minimum_required(VERSION 3.13.4)
project(MyCustomLoopInterchange)

# Find LLVM
find_package(LLVM 17 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add LLVM definitions and include directories
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Build the pass as a shared library
add_library(MyCustomLoopInterchange MODULE
    MyCustomLoopInterchange.cpp
)

# Disable RTTI if LLVM was built without it
if(NOT LLVM_ENABLE_RTTI)
    target_compile_options(MyCustomLoopInterchange PRIVATE -fno-rtti)
endif()

# Set output directory
set_target_properties(MyCustomLoopInterchange PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
