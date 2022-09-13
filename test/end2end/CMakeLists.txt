set (QUERIES_SOURCES 
    src/queries.cc
)

add_executable(queries ${QUERIES_SOURCES})
target_include_directories(queries PRIVATE ${MYSET_INCLUDE_DIR})
target_link_libraries(queries ${GTEST_BOTH_LIBRARIES})
target_link_libraries(queries myset)
install (TARGETS queries DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if (BASH_PROGRAM)
    add_test (NAME test.queries COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/test.sh "$<TARGET_FILE:queries>" ${CMAKE_CURRENT_SOURCE_DIR})
endif()