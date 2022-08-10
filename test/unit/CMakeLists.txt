# Test application for AVL tree

set (UNITT_SOURCES
    src/main.cc
)

add_executable(unitt ${UNITT_SOURCES})
target_include_directories(unitt PRIVATE ${MYSET_INCLUDE_DIR})
gtest_discover_tests (unitt)
