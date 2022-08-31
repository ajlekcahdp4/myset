/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// avl_tree_utilities implementation

#include "avl_tree.hpp"
namespace my
{

typename avl_tree_node_base_::base_ptr_ avl_tree_node_base_::avl_tree_increment_ () noexcept
{
    auto curr_ = this;
    if ( curr_->m_right_ )
    {
        curr_ = curr_->m_right_->m_minimum_ ();
    }
    else
    {
        base_ptr_ prev_ = curr_->m_parent_;
        assert (prev_);
        /*       while not root      */
        while ( prev_->m_parent_ && curr_ == prev_->m_right_ )
        {
            curr_ = prev_;
            prev_ = prev_->m_parent_;
        }
        if ( prev_->m_parent_ )
            curr_ = prev_;
        else
            curr_ = nullptr;
    }
    return curr_;
}

typename avl_tree_node_base_::base_ptr_ avl_tree_node_base_::avl_tree_decrement_ () noexcept
{
    auto curr_ = this;
    if ( curr_->m_parent_ )
    {
        curr_ = curr_->m_right_;
    }
    else if ( curr_->m_left_ )
    {
        base_ptr_ prev_ = curr_->m_left_->m_maximum_ ();
        curr_           = prev_;
    }
    else
    {
        base_ptr_ prev_ = curr_->m_parent_;
        /*       while not root      */
        while ( prev_->m_parent_ && curr_ == prev_->m_left_ )
        {
            curr_ = prev_;
            prev_ = prev_->m_parent_;
        }
        if ( prev_->m_parent_ )
            curr_ = prev_;
        else
            curr_ = nullptr;
    }
    return curr_;
}

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::m_fix_left_imbalance_insert_ ()
{
    auto curr_ = this;

    if ( curr_->m_left_->m_bf_ == curr_->m_bf_ )
    {
        curr_                  = curr_->rotate_right_ ();
        curr_->m_bf_           = 0;
        curr_->m_right_->m_bf_ = 0;
    }
    else
    {
        auto old_bf_ = curr_->m_left_->m_right_->m_bf_;
        curr_->m_left_->rotate_left_ ();
        curr_        = curr_->rotate_right_ ();
        curr_->m_bf_ = 0;

        if ( old_bf_ == -1 )
        {
            curr_->m_left_->m_bf_  = 0;
            curr_->m_right_->m_bf_ = 1;
        }
        else if ( old_bf_ == 1 )
        {
            curr_->m_left_->m_bf_  = -1;
            curr_->m_right_->m_bf_ = 0;
        }
        else if ( old_bf_ == 0 )
        {
            curr_->m_left_->m_bf_  = 0;
            curr_->m_right_->m_bf_ = 0;
        }
        else
            throw std::out_of_range ("Unexpected value of balance factor");
    }
    return curr_;
}

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::m_fix_right_imbalance_insert_ ()
{
    auto curr_ = this;

    if ( curr_->m_right_->m_bf_ == curr_->m_bf_ )
    {
        curr_                 = curr_->rotate_left_ ();
        curr_->m_bf_          = 0;
        curr_->m_left_->m_bf_ = 0;
    }
    else
    {
        auto old_bf_ = curr_->m_bf_;
        curr_->m_right_->rotate_right_ ();
        curr_        = curr_->rotate_left_ ();
        curr_->m_bf_ = 0;

        if ( old_bf_ == -1 )
        {
            curr_->m_left_->m_bf_  = 0;
            curr_->m_right_->m_bf_ = 1;
        }
        else if ( old_bf_ == 1 )
        {
            curr_->m_left_->m_bf_  = -1;
            curr_->m_right_->m_bf_ = 0;
        }
        else if ( old_bf_ == 0 )
        {
            curr_->m_left_->m_bf_  = 0;
            curr_->m_right_->m_bf_ = 0;
        }
        else
            throw std::out_of_range ("Unexpected value of balance factor");
    }
    return curr_;
}

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::rotate_left_ ()
{
    auto node_      = this;
    auto rchild_    = node_->m_right_;
    node_->m_right_ = rchild_->m_left_;

    if ( node_->m_right_ )
        node_->m_right_->m_parent_ = node_;

    rchild_->m_parent_ = node_->m_parent_;
    if ( node_->is_left_child_ () && node_->m_parent_ != nullptr )
        node_->m_parent_->m_left_ = rchild_;
    else if ( node_->m_parent_ )
        node_->m_parent_->m_right_ = rchild_;

    rchild_->m_left_ = node_;
    node_->m_parent_ = rchild_;

    return rchild_;
}

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::rotate_right_ ()
{
    auto node_     = this;
    auto lchild_   = node_->m_left_;
    node_->m_left_ = lchild_->m_right_;

    if ( node_->m_left_ )
        node_->m_left_->m_parent_ = node_;

    lchild_->m_parent_ = node_->m_parent_;
    if ( node_->is_left_child_ () )
        node_->m_parent_->m_left_ = lchild_;
    else if ( node_->m_parent_ )
        node_->m_parent_->m_right_ = lchild_;

    lchild_->m_right_ = node_;
    node_->m_parent_  = lchild_;

    return lchild_;
}

}   // namespace my