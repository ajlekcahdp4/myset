
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#include "avl_tree.hpp"
#include <gtest/gtest.h>

using owning_ptr_ = typename my::avl_tree_<int>::owning_ptr_;
using base_ptr_   = typename my::avl_tree_<int>::base_ptr_;

template struct my::avl_tree_<int>;

TEST (Test_avl_tree_node_base_, Test_m_maximum_)
{
    auto head = new my::avl_tree_node_base_ {0};

    head->m_left_            = owning_ptr_ (new my::avl_tree_node_base_ {-1});
    head->m_right_           = owning_ptr_ (new my::avl_tree_node_base_ {1});
    head->m_right_->m_right_ = owning_ptr_ (new my::avl_tree_node_base_ {2});

    auto max = head->m_maximum_ ();
    EXPECT_EQ (max->m_bf_, 2);
}

TEST (Test_avl_tree_node_base_, Test_m_minimum_)
{
    auto head = new my::avl_tree_node_base_ {0};

    head->m_right_         = owning_ptr_ (new my::avl_tree_node_base_ {1});
    head->m_left_          = owning_ptr_ (new my::avl_tree_node_base_ {-1});
    head->m_left_->m_left_ = owning_ptr_ (new my::avl_tree_node_base_ {-2});

    auto max = head->m_minimum_ ();
    EXPECT_EQ (max->m_bf_, -2);
}

TEST (Test_avl_tree_iterator_, TestCtorDeref)
{
    auto head = new my::avl_tree_node_<int> (666);

    my::avl_tree_<int>::avl_tree_iterator_ pos (head);

    EXPECT_EQ (*pos, 666);
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

    EXPECT_EQ (pos->a, 9);
}

TEST (Test_avl_tree_iterator_, TestPostIncrement)
{
    auto head       = new my::avl_tree_node_<int> (0);
    head->m_parent_ = head;

    auto new_node             = new my::avl_tree_node_<int> (1);
    head->m_right_            = owning_ptr_ (static_cast<base_ptr_> (new_node));
    head->m_right_->m_parent_ = head;

    my::avl_tree_<int>::avl_tree_iterator_ head_pos (head);

    EXPECT_EQ (*head_pos, 0);

    EXPECT_EQ (*(head_pos++), 0);

    EXPECT_EQ (*head_pos, 1);
}

TEST (Test_avl_tree_iterator_, TestPreIncrement)
{
    auto head       = new my::avl_tree_node_<int> (0);
    head->m_parent_ = head;

    auto new_node             = new my::avl_tree_node_<int> (1);
    head->m_right_            = owning_ptr_ (static_cast<base_ptr_> (new_node));
    head->m_right_->m_parent_ = head;

    my::avl_tree_<int>::avl_tree_iterator_ head_pos (head);

    EXPECT_EQ (*head_pos, 0);

    EXPECT_EQ (*(++head_pos), 1);

    EXPECT_EQ (*head_pos, 1);
}

TEST (Test_avl_tree_iterator_, TestPreDecrement)
{
    auto head       = new my::avl_tree_node_<int> (0);
    head->m_parent_ = head;

    auto new_node            = new my::avl_tree_node_<int> (-1);
    head->m_left_            = owning_ptr_ (static_cast<base_ptr_> (new_node));
    head->m_left_->m_parent_ = head;

    my::avl_tree_<int>::avl_tree_iterator_ head_pos (head);

    EXPECT_EQ (*head_pos, 0);

    EXPECT_EQ (*(--head_pos), -1);

    EXPECT_EQ (*head_pos, -1);
}

TEST (Test_avl_tree_iterator_, TestDecrement)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (*(--tree.find (6)), 5);
    EXPECT_EQ (*(--tree.find (8)), 6);
    EXPECT_EQ (*(--tree.find (10)), 9);
    EXPECT_EQ (*(--tree.find (11)), 10);
    EXPECT_EQ (*(--tree.find (3)), 2);
    EXPECT_EQ ((--tree.find (2)).m_node_, nullptr);
}

TEST (Test_avl_tree_iterator_, TestIncrement)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (*(++tree.find (6)), 8);
    EXPECT_EQ (*(++tree.find (8)), 9);
    EXPECT_EQ ((++tree.find (10)).m_node_, nullptr);
    EXPECT_EQ (*(++tree.find (3)), 5);
}

TEST (Test_avl_tree_, TestInsert)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    EXPECT_EQ (*(tree.begin ()), 1);
}

TEST (Test_avl_tree_, TestEraseLastOne)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.erase (tree.begin ());

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
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
    EXPECT_EQ (*tree.begin (), 7);
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
    EXPECT_EQ (*last, 9);
}

TEST (Test_avl_tree_, TestEraseThroughTheKeyOnly)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.erase (1);

    EXPECT_TRUE (tree.empty ());
}

TEST (Test_avl_tree_, TestEraseThroughTheKeyFirst)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (1);

    EXPECT_EQ (*tree.begin (), 2);
    EXPECT_EQ (*tree.rbegin (), 2);
}

TEST (Test_avl_tree_, TestEraseThroughTheKeyLast)
{
    my::avl_tree_<int> tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 1);
}

TEST (Test_avl_tree_, TestDoubleInsert)
{
    my::avl_tree_<int> tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = 0; i < 10; i++ )
    {
        EXPECT_THROW (tree.insert (i), std::out_of_range);
        EXPECT_EQ (tree.size (), old_size);
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
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (Test_avl_tree_, TestDeleteAll)
{
    my::avl_tree_<int> tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 10; i++ )
        EXPECT_NO_THROW (tree.erase (i));

    EXPECT_EQ (tree.size (), 0);
    EXPECT_TRUE (tree.empty ());
}

TEST (Test_avl_tree_, Test_select_1)
{
    my::avl_tree_<int> tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 5; i++ )
        tree.erase (i);

    EXPECT_EQ (tree.m_os_select_ (1), 5);
    EXPECT_EQ (tree.m_os_select_ (2), 6);
    EXPECT_EQ (tree.m_os_select_ (3), 7);
    EXPECT_EQ (tree.m_os_select_ (4), 8);
    EXPECT_EQ (*tree.begin (), 5);

    EXPECT_THROW (tree.m_os_select_ (0), std::out_of_range);

    EXPECT_THROW (tree.m_os_select_ (10), std::out_of_range);
}

TEST (Test_avl_tree_, Test_select_2)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    for ( int i = 5; i <= 10; i++ )
        tree.erase (i);

    EXPECT_EQ (tree.m_os_select_ (1), 1);
    EXPECT_EQ (tree.m_os_select_ (2), 2);
    EXPECT_EQ (tree.m_os_select_ (3), 3);
    EXPECT_EQ (tree.m_os_select_ (4), 4);
    EXPECT_EQ (*std::prev (tree.end ()), 4);

    EXPECT_THROW (tree.m_os_select_ (0), std::out_of_range);

    EXPECT_THROW (tree.m_os_select_ (10), std::out_of_range);
}

TEST (Test_avl_tree_, Test_number_less_then)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.m_get_number_less_then_ (11), 7);
    EXPECT_EQ (tree.m_get_number_less_then_ (4), 2);
}

TEST (Test_avl_tree_, Test_lower_bound)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.lower_bound (11), tree.end ());
    EXPECT_EQ (*tree.lower_bound (2), 2);
    EXPECT_EQ (*tree.lower_bound (1), 2);
    EXPECT_EQ (*tree.lower_bound (-1), 2);
}

TEST (Test_avl_tree_, Test_upper_bound)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.upper_bound (11), tree.end ());
    EXPECT_EQ (*tree.upper_bound (2), 3);
    EXPECT_EQ (*tree.upper_bound (1), 2);
    EXPECT_EQ (*tree.upper_bound (-1), 2);
}

TEST (Test_avl_tree_, Test_range_for)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    std::vector<int> v {10, 9, 8, 6, 5, 3, 2};

    for ( auto &i : tree )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (Test_avl_tree_, Test_clear)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    tree.clear ();

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin ().m_node_, tree.end ().m_node_);
}

TEST (Test_avl_tree_, Test_equal_1)
{
    my::avl_tree_<int> tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    my::avl_tree_<int> tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (i);

    tree2.erase (1);
    tree2.erase (4);

    my::avl_tree_<int> tree3;

    for ( int i = 1; i <= 10; i++ )
        tree3.insert (i);

    tree3.erase (1);
    tree3.erase (7);
    tree3.erase (5);

    EXPECT_FALSE (tree1 == tree2);
    EXPECT_FALSE (tree1 == tree3);
    EXPECT_FALSE (tree2 == tree3);

    EXPECT_TRUE (tree1 != tree2);
    EXPECT_TRUE (tree1 != tree3);
    EXPECT_TRUE (tree2 != tree3);
}

TEST (Test_avl_tree_, Test_equal_2)
{
    my::avl_tree_<int> tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    my::avl_tree_<int> tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (i);

    tree2.erase (1);
    tree2.erase (7);
    tree2.erase (4);

    EXPECT_TRUE (tree1 == tree2);
    EXPECT_FALSE (tree1 != tree2);
}

TEST (Test_avl_tree_, Test_move_ctor)
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    std::vector<int> v {10, 9, 8, 6, 5, 3, 2};
    my::avl_tree_<int> tree2 = std::move (tree);
    for ( auto &i : tree2 )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}