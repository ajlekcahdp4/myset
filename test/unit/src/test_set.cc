/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#include "myset.hpp"
#include <gtest/gtest.h>

template class rethinking_stl::set<int>;

TEST (Test_set, Test_insert_1)
{
    rethinking_stl::set<int> set;
    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    for ( auto &i : v )
        set.insert (i);

    EXPECT_EQ (set.size (), v.size ());

    for ( auto &i : set )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}