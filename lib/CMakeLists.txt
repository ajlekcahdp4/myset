# creating library

set (LIBRARY_SOURCES
    src/avl-tree.cc
)

add_library (lmyset ${LIBRARY_SOURCES})
target_include_directories(lmyset PUBLIC include)