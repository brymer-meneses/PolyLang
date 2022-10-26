cmake_minimum_required(VERSION 3.13.4)
project(PolyLang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/llvm.cmake)
include(cmake/googletest.cmake)

add_executable(PolyLang src/main.cpp)

target_link_libraries(PolyLang ${llvm_libs})
target_include_directories(PolyLang PUBLIC "${CMAKE_SOURCE_DIR}/include")
