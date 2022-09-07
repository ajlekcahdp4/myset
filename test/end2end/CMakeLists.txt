set (QUERIES_SOURCES 
    src/queries.cc
)

add_executable(queries ${QUERIES_SOURCES})
target_include_directories(queries PRIVATE ${MYSET_INCLUDE_DIR})
target_link_libraries(queries ${GTEST_BOTH_LIBRARIES})
target_link_libraries(queries myset)