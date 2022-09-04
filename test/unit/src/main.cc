#include "avl_tree.hpp"
#include <gtest/gtest.h>
#include <iostream>

using owning_ptr_ = typename my::avl_tree_<int>::owning_ptr_;
using base_ptr_   = typename my::avl_tree_<int>::base_ptr_;

TEST (Test_avl_tree_node_base_, Test_m_maximum_)
{
    auto head = new my::avl_tree_node_base_ {0};

    head->m_left_            = owning_ptr_ (new my::avl_tree_node_base_ {-1});
    head->m_right_           = owning_ptr_ (new my::avl_tree_node_base_ {1});
    head->m_right_->m_right_ = owning_ptr_ (new my::avl_tree_node_base_ {2});

    auto max = head->m_maximum_ ();
    ASSERT_EQ (max->m_bf_, 2);
}

TEST (Test_avl_tree_node_base_, Test_m_minimum_)
{
    auto head = new my::avl_tree_node_base_ {0};

    head->m_right_         = owning_ptr_ (new my::avl_tree_node_base_ {1});
    head->m_left_          = owning_ptr_ (new my::avl_tree_node_base_ {-1});
    head->m_left_->m_left_ = owning_ptr_ (new my::avl_tree_node_base_ {-2});

    auto max = head->m_minimum_ ();
    ASSERT_EQ (max->m_bf_, -2);
}

TEST (Test_avl_tree_iterator_, TestCtorDeref)
{
    auto head = new my::avl_tree_node_<int> (666);

    my::avl_tree_<int>::avl_tree_iterator_ pos (head);

    ASSERT_EQ (*pos, 666);
}

struct entry
{
    int a;
};

TEST (Test_avl_tree_iterator_, TestArrowOperator)
{
    entry b   = {9};
    auto head = new my::avl_tree_node_<entry> (b);

    my::avl_tree_<entry>::avl_tree_iterator_ pos (head);

    ASSERT_EQ (pos->a, 9);
}

TEST (Test_avl_tree_iterator_, TestPostIncrement)
{
    auto head       = new my::avl_tree_node_<int> (0);
    head->m_parent_ = head;

    auto new_node             = new my::avl_tree_node_<int> (1);
    head->m_right_            = owning_ptr_ (static_cast<base_ptr_> (new_node));
    head->m_right_->m_parent_ = head;

    my::avl_tree_<int>::avl_tree_iterator_ head_pos (head);

    ASSERT_EQ (*head_pos, 0);

    ASSERT_EQ (*(head_pos++), 0);

    ASSERT_EQ (*head_pos, 1);
}

TEST (Test_avl_tree_iterator_, TestPreIncrement)
{
    auto head       = new my::avl_tree_node_<int> (0);
    head->m_parent_ = head;

    auto new_node             = new my::avl_tree_node_<int> (1);
    head->m_right_            = owning_ptr_ (static_cast<base_ptr_> (new_node));
    head->m_right_->m_parent_ = head;

    my::avl_tree_<int>::avl_tree_iterator_ head_pos (head);

    ASSERT_EQ (*head_pos, 0);

    ASSERT_EQ (*(++head_pos), 1);

    ASSERT_EQ (*head_pos, 1);
}

TEST (Test_avl_tree_, TestInsert)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    ASSERT_EQ (*(tree.begin ()), 1);
}

TEST (Test_avl_tree_, TestEraseLastOne)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.erase (tree.begin ());

    ASSERT_TRUE (tree.empty ());
    ASSERT_EQ (tree.begin (), tree.end ());
}

TEST (Test_avl_tree_, TestEraseLeftmost)
{
    my::avl_tree_<int> tree;
    for ( int i = 6; i <= 10; i++ )
    {
        tree.insert (i);
        std::stringstream ss;
        ss << "dump" << i;
        tree.dump (ss.str ());
    }

    EXPECT_EQ (*tree.begin (), 6);
    tree.erase (tree.begin ());
    ASSERT_EQ (*tree.begin (), 7);
}

TEST (Test_avl_tree_, TestEraseRightmost)
{
    my::avl_tree_<int> tree;
    for ( int i = 6; i <= 10; i++ )
    {
        tree.insert (i);
        std::stringstream ss;
        ss << "dump" << i;
        tree.dump (ss.str ());
    }

    auto last = std::prev (tree.end ());
    EXPECT_EQ (*last, 10);

    tree.erase (last);

    last = std::prev (tree.end ());
    ASSERT_EQ (*last, 9);
}

TEST (Test_avl_tree_, TestEraseThroughTheKeyOnly)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.erase (1);

    ASSERT_TRUE (tree.empty ());
}

TEST (Test_avl_tree_, TestEraseThroughTheKeyFirst)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (1);

    ASSERT_EQ (*tree.begin (), 2);
    ASSERT_EQ (*tree.rbegin (), 2);
}

TEST (Test_avl_tree_, TestEraseThroughTheKeyLast)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (2);

    ASSERT_EQ (*tree.begin (), 1);
    ASSERT_EQ (*tree.rbegin (), 1);
}

TEST (Test_avl_tree_, TestEraseWrongKey)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (666);

    ASSERT_EQ (*tree.begin (), 1);
    ASSERT_EQ (*tree.rbegin (), 2);
}

TEST (Test_avl_tree_, TestDoubleInsert)
{
    my::avl_tree_<int> tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = 0; i < 10; i++ )
    {
        ASSERT_THROW (tree.insert (i), std::out_of_range);
        ASSERT_EQ (tree.size (), old_size);
    }
}

TEST (Test_avl_tree_, TestDeleteWrong)
{
    my::avl_tree_<int> tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = -1; i < -10; i-- )
    {
        tree.insert (i);
        ASSERT_EQ (tree.size (), old_size);
    }
}

TEST (Test_avl_tree_, TestDeleteAll)
{
    my::avl_tree_<int> tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 10; i++ )
        ASSERT_NO_THROW (tree.erase (i));

    ASSERT_EQ (tree.size (), 0);
    ASSERT_TRUE (tree.empty ());
}

int main (int argc, char *argv[])
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}