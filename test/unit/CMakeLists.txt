# Test application for AVL tree

set (UNITT_SOURCES
    src/main.cc
    src/test_avl_tree.cc
    src/test_set.cc
)

add_executable(unitt ${UNITT_SOURCES})
target_include_directories(unitt PRIVATE ${MYSET_INCLUDE_DIR})
target_link_libraries(unitt ${GTEST_BOTH_LIBRARIES} Threads::Threads)
target_link_libraries(unitt Threads::Threads)

gtest_discover_tests (unitt)
