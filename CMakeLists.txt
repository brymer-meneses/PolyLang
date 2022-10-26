cmake_minimum_required(VERSION 3.13.4)
project(PolyLang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/llvm.cmake)
include(cmake/googletest.cmake)

file(GLOB PolyLang_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# PolyLang
add_executable(PolyLang src/main.cpp ${PolyLang_SOURCES})
add_library(PolyLangLib STATIC ${PolyLang_SOURCES})

target_include_directories(PolyLangLib PUBLIC ${PROJECT_SOURCE_DIR}/include) # Link to polylang
target_link_libraries(PolyLang ${llvm_libs} PolyLangLib)

# Tests files
file(GLOB TEST_FILES "${PROJECT_SOURCE_DIR}/tests/*.cpp")
add_executable(PolyLangTester ${PROJECT_SOURCE_DIR}/tests/tester.cpp ${TEST_FILES})

target_include_directories(PolyLangTester PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(PolyLangTester ${llvm_libs} PolyLangLib gtest_main gmock_main)


