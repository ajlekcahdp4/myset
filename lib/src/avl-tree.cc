// avl_tree_utilities implementation

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#include "avl_tree.hpp"
namespace my
{

avl_tree_node_base_ *avl_tree_increment_impl_ (avl_tree_node_base_ *x_) noexcept
{
    if ( x_->m_right_ )
    {
        x_ = x_->m_right_;
        while ( x_->m_left_ )
            x_ = x_->m_left_;
    }
    else
    {
        avl_tree_node_base_ *y_ = x_->m_parent_;
        while ( x_ == y_->m_right_ )
        {
            x_ = y_;
            y_ = y_->m_parent_;
        }
        if ( x_->m_right_ != y_ )
            x_ = y_;
    }
    return x_;
}

avl_tree_node_base_ *avl_tree_increment_ (avl_tree_node_base_ *x_) noexcept { return avl_tree_increment_impl_ (x_); }

const avl_tree_node_base_ *avl_tree_increment_ (const avl_tree_node_base_ *x_) noexcept
{
    return avl_tree_increment_impl_ (const_cast<avl_tree_node_base_ *> (x_));
}

avl_tree_node_base_ *avl_tree_decrement_impl_ (avl_tree_node_base_ *x_) noexcept
{
    if ( x_->m_parent_->m_parent_ == x_ )   // if head
    {
        x_ = x_->m_right_;
    }
    else if ( x_->m_left_ )
    {
        avl_tree_node_base_ *y_ = x_->m_left_;
        while ( y_->m_right_ )
            y_ = y_->m_right_;
        x_ = y_;
    }
    else
    {
        avl_tree_node_base_ *y_ = x_->m_parent_;
        while ( x_ == y_->m_left_ )
        {
            x_ = y_;
            y_ = y_->m_parent_;
        }
        x_ = y_;
    }
    return x_;
}

avl_tree_node_base_ *avl_tree_decrement_ (avl_tree_node_base_ *x_) noexcept { return avl_tree_decrement_impl_ (x_); }

const avl_tree_node_base_ *avl_tree_decrement_ (const avl_tree_node_base_ *x_) noexcept
{
    return avl_tree_decrement_impl_ (const_cast<avl_tree_node_base_ *> (x_));
}

}   // namespace my