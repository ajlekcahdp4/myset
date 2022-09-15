cmake_minimum_required(VERSION 3.11)

project (myset)

set (CMAKE_CXX_COMPILER clang++)

option(INSOURCEBUILD OFF)
if((${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR}) AND NOT ${INSOURCEBUILD})
    message(FATAL_ERROR "In-source building disabled. Provide -DINSOURCEBUILD option if you are sure about it.")
endif()

add_compile_options(-Wall -Wextra)

set (MYSET_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set (MYSET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

find_package(GTest)
set(NOGTEST FALSE ON CACHE BOOL "Disable GoogleTest")
set(NOCOMPARE FALSE ON CACHE BOOL "Enable comparation with std::set.")

if (COMPARE)
    message(WARNING "Comparation with std::test enabled. It should take a lot of time. If you want to disable it rerun with -DNOCOMPARE=TRUE")
endif()

enable_testing()
if(NOT NOGTEST AND GTEST_FOUND AND CMAKE_MINOR_VERSION GREATER_EQUAL 20)
    set(ENABLE_GTEST ON)
else()
    message(WARNING "Google Test disabled")
endif()

set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program (BASH_PROGRAM bash)

add_subdirectory(lib)
add_subdirectory(test)