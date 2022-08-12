#include "myset.hpp"
#include <gtest/gtest.h>

TEST (TestMySet, TestInint)
{
    my::set<int> set {};
    ASSERT_EQ (set.size (), 0);
    ASSERT_EQ (set.head (), nullptr);
}

TEST (TestMySet, TestInsertSize)
{
    my::set<int> set {};
    std::vector<int> entries {1, 3, 2, 4};
    size_t cnt {};

    for ( auto &entry : entries )
    {
        set.insert (entry);
        cnt++;
        ASSERT_EQ (set.size (), cnt);
    }
}

TEST (TestIterator, TestDereference)
{
    my::set<int>::avl_tree_node_t node (69);
    my::set<int>::iterator it (&node);

    ASSERT_EQ (*it, 69);
}

struct entry
{
    int a;
    int b;

    entry (int val1, int val2) : a (val1), b (val2) {}
};

TEST (TestIterator, TestPointer)
{
    my::set<entry>::avl_tree_node_t node ({66, 25});
    my::set<entry>::iterator it (&node);

    ASSERT_EQ (it->a, 66);
    ASSERT_EQ (it->b, 25);
}

int main (int argc, char *argv[])
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}