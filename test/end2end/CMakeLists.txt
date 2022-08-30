set (DUMP_SOURCES 
    src/dump.cc
)

add_executable(dump ${DUMP_SOURCES})
target_include_directories(dump PRIVATE ${MYSET_INCLUDE_DIR})
target_link_libraries(dump ${GTEST_BOTH_LIBRARIES})
target_link_libraries(dump myset)