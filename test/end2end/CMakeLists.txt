set (QUERIES_SOURCES 
    src/queries.cc
)

set (STD_TIME_QUERIES_SOURCES 
    src/std-time-queries.cc
)
set (MY_TIME_QUERIES_SOURCES 
    src/my-time-queries.cc
)

add_executable(queries ${QUERIES_SOURCES})
target_include_directories(queries PRIVATE ${MYSET_INCLUDE_DIR})
target_link_libraries(queries ${GTEST_BOTH_LIBRARIES})
target_link_libraries(queries myset)
install (TARGETS queries DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if (BASH_PROGRAM)
    add_test (NAME test.queries COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/test.sh "$<TARGET_FILE:queries>" ${CMAKE_CURRENT_SOURCE_DIR})    
endif()

add_executable(std_time_queries ${STD_TIME_QUERIES_SOURCES})
add_executable(my_time_queries ${MY_TIME_QUERIES_SOURCES})
target_include_directories(my_time_queries PRIVATE ${MYSET_INCLUDE_DIR})
target_link_libraries(my_time_queries myset)

if (BASH_PROGRAM AND NOT NOCOMPARE)
    add_test (NAME test.compare_with_std COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/compare.sh "$<TARGET_FILE:my_time_queries>" "$<TARGET_FILE:std_time_queries>" ${CMAKE_CURRENT_SOURCE_DIR})
endif()