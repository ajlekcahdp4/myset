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
        while ( prev_->m_parent_ && curr_ == prev_->m_right_.get () )
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
        curr_ = curr_->m_right_.get ();
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
        while ( prev_->m_parent_ && curr_ == prev_->m_left_.get () )
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

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::m_fix_right_imbalance_erase_ ()
{
    auto curr_      = this;
    auto rchild_bf_ = curr_->m_right_->m_bf_;

    if ( rchild_bf_ == 1 )
    {
        curr_                 = curr_->rotate_left_ ();
        curr_->m_bf_          = 0;
        curr_->m_left_->m_bf_ = 0;
    }
    else if ( rchild_bf_ == 0 )
    {
        curr_                 = curr_->rotate_left_ ();
        curr_->m_bf_          = -1;
        curr_->m_left_->m_bf_ = 1;
    }
    else if ( rchild_bf_ == -1 )
    {
        auto old_bf_ = curr_->m_right_->m_left_->m_bf_;

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
    else
        throw std::out_of_range ("Unexpected value of balance factor");
    return curr_;
}

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::m_fix_left_imbalance_erase_ ()
{
    auto curr_      = this;
    auto lchild_bf_ = curr_->m_left_->m_bf_;

    if ( lchild_bf_ == -1 )
    {
        curr_                  = curr_->rotate_right_ ();
        curr_->m_bf_           = 0;
        curr_->m_right_->m_bf_ = 0;
    }
    else if ( lchild_bf_ == 0 )
    {
        curr_                  = curr_->rotate_right_ ();
        curr_->m_bf_           = 1;
        curr_->m_right_->m_bf_ = -1;
    }
    else if ( lchild_bf_ == 1 )
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
    else
        throw std::out_of_range ("Unexpected value of balance factor");

    return curr_;
}

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::rotate_left_ ()
{
    auto node_       = this;
    auto parent_     = this->m_parent_;
    auto rchild_     = std::move (node_->m_right_);
    auto rchild_ptr_ = rchild_.get ();

    node_->m_right_ = std::move (rchild_->m_left_);

    if ( node_->m_right_ )
        node_->m_right_->m_parent_ = node_;

    rchild_->m_parent_ = parent_;
    if ( node_->is_left_child_ () && parent_ )
    {
        rchild_ptr_->m_left_ = std::move (parent_->m_left_);
        parent_->m_left_     = std::move (rchild_);
    }
    else if ( parent_ )
    {
        rchild_ptr_->m_left_ = std::move (parent_->m_right_);
        parent_->m_right_    = std::move (rchild_);
    }

    node_->m_parent_ = rchild_ptr_;

    return rchild_ptr_;
}

avl_tree_node_base_::base_ptr_ avl_tree_node_base_::rotate_right_ ()
{
    auto node_       = this;
    auto parent_     = this->m_parent_;
    auto lchild_     = std::move (node_->m_left_);
    auto lchild_ptr_ = lchild_.get ();

    node_->m_left_ = std::move (lchild_->m_right_);

    if ( node_->m_left_ )
        node_->m_left_->m_parent_ = node_;

    lchild_->m_parent_ = parent_;
    if ( node_->is_left_child_ () && parent_ )
    {
        lchild_ptr_->m_right_ = std::move (parent_->m_left_);
        parent_->m_left_      = std::move (lchild_);
    }
    else if ( parent_ )
    {

        lchild_ptr_->m_right_ = std::move (parent_->m_right_);
        parent_->m_right_     = std::move (lchild_);
    }

    node_->m_parent_ = lchild_ptr_;

    return lchild_ptr_;
}

}   // namespace my