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

avl_tree_node_base_ *avl_tree_increment_ (avl_tree_node_base_ *x_) noexcept
{
    return avl_tree_increment_impl_ (x_);
}

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

avl_tree_node_base_ *avl_tree_decrement_ (avl_tree_node_base_ *x_) noexcept
{
    return avl_tree_decrement_impl_ (x_);
}

const avl_tree_node_base_ *avl_tree_decrement_ (const avl_tree_node_base_ *x_) noexcept
{
    return avl_tree_decrement_impl_ (const_cast<avl_tree_node_base_ *> (x_));
}

template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::iterator
avl_tree_<Key_, Comp_>::m_lower_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_)
{
    while ( x_ )
    {
        bool key_bigger_ = avl_tree_key_compare_ (s_key_ (x_), k_);
        if ( !key_bigger_ )
        {
            y_ = x_;
            x_ = s_left_ (x_);
        }
        else
            x_ = s_right_ (x_);
    }
    return iterator (y_);
}

template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::const_iterator
avl_tree_<Key_, Comp_>::m_lower_bound_ (const_link_type_ x_, const_base_ptr_ y_,
                                        const key_type &k_) const
{
    while ( x_ )
    {
        bool key_bigger_ = avl_tree_key_compare_ (s_key_ (x_), k_);
        if ( !key_bigger_ )
        {
            y_ = x_;
            x_ = s_left_ (x_);
        }
        else
            x_ = s_right_ (x_);
    }
    return const_iterator (y_);
}

template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::iterator
avl_tree_<Key_, Comp_>::m_upper_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_)
{
    while ( x_ )
    {
        bool key_less_ = avl_tree_key_compare_ (k_, s_key_ (x_));
        if ( key_less_ )
        {
            y_ = x_;
            x_ = s_left_ (x_);
        }
        else
            x_ = s_right_ (x_);
    }
    return iterator (y_);
}

template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::const_iterator
avl_tree_<Key_, Comp_>::m_upper_bound_ (const_link_type_ x_, const_base_ptr_ y_,
                                        const key_type &k_) const
{
    while ( x_ )
    {
        bool key_less_ = avl_tree_key_compare_ (k_, s_key_ (x_));
        if ( key_less_ )
        {
            y_ = x_;
            x_ = s_left_ (x_);
        }
        else
            x_ = s_right_ (x_);
    }
    return const_iterator (y_);
}

}   // namespace my