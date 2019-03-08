cmake_minimum_required(VERSION 3.0)
project("Libc Stub WebAssembly Interpreter" CXX)

add_subdirectory(${WASMJIT_OMR_LOCATION} wasmjit-omr EXCLUDE_FROM_ALL)

add_executable(em-interp em-interp.cpp em-module.cpp)
target_link_libraries(em-interp PRIVATE wabt)
target_include_directories(em-interp PRIVATE ${WASMJIT_OMR_LOCATION} ${CMAKE_BINARY_DIR}/wasmjit-omr)
set_target_properties(em-interp PROPERTIES COMPILE_FLAGS "-g -Wall")
