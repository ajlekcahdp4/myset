if (NOT NOGTEST AND GTEST_FOUND)
    add_subdirectory(unit)
endif()
add_subdirectory(end2end)