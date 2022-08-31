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

int main (int argc, char *argv[])
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}