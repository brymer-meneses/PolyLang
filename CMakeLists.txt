cmake_minimum_required(VERSION 3.13.4)
project(PolyLang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-std=c++20")
set(CMAKE_BUILD_TYPE RelWithDebInfo)

include(cmake/llvm.cmake)
include(cmake/googletest.cmake)

file(GLOB PolyLang_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB TEST_FILES "${PROJECT_SOURCE_DIR}/tests/*.cpp")

# PolyLang
add_executable(PolyLang src/main.cpp ${PolyLang_SOURCES})
add_library(PolyLangLib STATIC ${PolyLang_SOURCES})

target_include_directories(PolyLangLib PUBLIC ${PROJECT_SOURCE_DIR}/include) # Link to polylang
target_link_libraries(PolyLang PolyLangLib ${llvm_libs})

# Tests files
add_executable(PolyLangTester ${PROJECT_SOURCE_DIR}/tests/test_runner.cpp ${TEST_FILES})

target_include_directories(PolyLangTester PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(PolyLangTester PolyLangLib ${llvm_libs} gtest_main gmock_main)


