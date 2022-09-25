/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// dynamic_order_avl_tree_utilities implementation

#include "avl_tree.hpp"
namespace rethinking_stl
{

typename dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::m_predecessor_for_erase_ () noexcept
{
    auto curr_ = this;

    if ( m_left_ )
    {
        /*
         * Move down until we find it.
         * Also have to decrease ich node size.
         */

        curr_ = m_left ();
        while ( curr_->m_right_ )
        {
            curr_->m_size_--;
            curr_ = curr_->m_right ();
        }
        return curr_;
    }

    /* move up until we find it or reach the root. */
    for ( ; curr_->m_parent_->m_parent_ && curr_->is_left_child_ (); curr_ = curr_->m_parent_ )
    {
        ;
    }

    auto parent_ = curr_->m_parent_;

    if ( !parent_->m_parent_ ) /* curr_ is a root */
        return nullptr;

    return parent_;
}

typename dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::m_successor_for_erase_ () noexcept
{
    auto curr_ = this;

    if ( m_right_ )
    {
        /*
         * Move down until we find it.
         * Also have to decrease ich node size.
         */

        curr_ = m_right ();
        while ( curr_->m_left_ )
        {
            curr_->m_size_--;
            curr_ = curr_->m_left ();
        }
        return curr_;
    }

    /* move up until we find it or reach the root. */
    for ( ; curr_->m_parent_->m_parent_ && !curr_->is_left_child_ (); curr_ = curr_->m_parent_ )
    {
        ;
    }

    auto parent_ = curr_->m_parent_;

    if ( !parent_->m_parent_ ) /* curr_ is a root */
        return nullptr;

    return parent_;
}

typename dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::dynamic_order_avl_tree_increment_ () noexcept
{
    auto curr_ = this;
    if ( curr_->m_right_ )

        return curr_->m_right_->m_minimum_ ();

    base_ptr_ prev_ = curr_->m_parent_;

    /*       while not root      */
    while ( prev_->m_parent_ && !curr_->is_left_child_ () )
    {
        curr_ = prev_;
        prev_ = prev_->m_parent_;
    }
    if ( prev_->m_parent_ )
        curr_ = prev_;
    else
        curr_ = nullptr;
    return curr_;
}

typename dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::dynamic_order_avl_tree_decrement_ () noexcept
{
    auto curr_ = this;

    if ( curr_->m_left_ )
        return curr_->m_left_->m_maximum_ ();

    base_ptr_ prev_ = curr_->m_parent_;
    /*       while not root      */
    while ( prev_->m_parent_ && curr_ == prev_->m_left () )
    {
        curr_ = prev_;
        prev_ = prev_->m_parent_;
    }
    if ( prev_->m_parent_ )
        curr_ = prev_;
    else
        curr_ = nullptr;

    return curr_;
}

dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::m_fix_left_imbalance_insert_ ()
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

dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::m_fix_right_imbalance_insert_ ()
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
    return curr_;
}

dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::m_fix_right_imbalance_erase_ ()
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

dynamic_order_avl_tree_node_base_::base_ptr_
dynamic_order_avl_tree_node_base_::m_fix_left_imbalance_erase_ ()
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

dynamic_order_avl_tree_node_base_::base_ptr_ dynamic_order_avl_tree_node_base_::rotate_left_ ()
{
    auto node_       = this;
    auto parent_     = this->m_parent_;
    auto rchild_ptr_ = node_->m_right_.release ();

    node_->m_right_ = std::move (rchild_ptr_->m_left_);

    if ( node_->m_right_ )
        node_->m_right_->m_parent_ = node_;

    rchild_ptr_->m_parent_ = parent_;
    if ( node_->is_left_child_ () && parent_ )
    {
        rchild_ptr_->m_left_ = std::move (parent_->m_left_);
        parent_->m_left_     = owning_ptr_ (rchild_ptr_);
    }
    else if ( parent_ )
    {
        rchild_ptr_->m_left_ = std::move (parent_->m_right_);
        parent_->m_right_    = owning_ptr_ (rchild_ptr_);
    }

    node_->m_parent_ = rchild_ptr_;

    /* update rchild's and node's sizes (the only sizes changed) */
    rchild_ptr_->m_size_ = node_->m_size_;
    node_->m_size_       = self_::size (node_->m_left ()) + self_::size (node_->m_right ()) + 1;

    return rchild_ptr_;
}

dynamic_order_avl_tree_node_base_::base_ptr_ dynamic_order_avl_tree_node_base_::rotate_right_ ()
{
    auto node_       = this;
    auto parent_     = this->m_parent_;
    auto lchild_ptr_ = node_->m_left_.release ();

    node_->m_left_ = std::move (lchild_ptr_->m_right_);

    if ( node_->m_left_ )
        node_->m_left_->m_parent_ = node_;

    lchild_ptr_->m_parent_ = parent_;
    if ( node_->is_left_child_ () && parent_ )
    {
        lchild_ptr_->m_right_ = std::move (parent_->m_left_);
        parent_->m_left_      = owning_ptr_ (lchild_ptr_);
    }
    else if ( parent_ )
    {

        lchild_ptr_->m_right_ = std::move (parent_->m_right_);
        parent_->m_right_     = owning_ptr_ (lchild_ptr_);
    }

    node_->m_parent_ = lchild_ptr_;

    /* update rchild's and node's sizes (the only sizes changed) */
    lchild_ptr_->m_size_ = node_->m_size_;
    node_->m_size_       = self_::size (node_->m_left ()) + self_::size (node_->m_right ()) + 1;

    return lchild_ptr_;
}

}   // namespace rethinking_stl