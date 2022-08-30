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
        while ( curr_ == prev_->m_right_ )
        {
            curr_ = prev_;
            prev_ = prev_->m_parent_;
        }
        if ( curr_->m_right_ != prev_ )
            curr_ = prev_;
    }
    return curr_;
}

typename avl_tree_node_base_::base_ptr_ avl_tree_node_base_::avl_tree_decrement_ () noexcept
{
    auto curr_ = this;
    if ( curr_->m_parent_->m_parent_ == curr_ )
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
        while ( curr_ == prev_->m_left_ )
        {
            curr_ = prev_;
            prev_ = prev_->m_parent_;
        }
        curr_ = prev_;
    }
    return curr_;
}

// Accessors.
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
            x_ = x_->m_left_;
        }
        else
            x_ = x_->m_right_;
    }
    return iterator (y_, this);
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
            x_ = x_->m_left_;
        }
        else
            x_ = x_->m_right_;
    }
    return const_iterator (y_, this);
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
            x_ = x_->m_left_;
        }
        else
            x_ = x_->m_right_;
    }
    return iterator (y_, this);
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
            x_ = x_->m_left_;
        }
        else
            x_ = x_->m_right_;
    }
    return const_iterator (y_, this);
}

// Insert/Erase.

template <typename Key_, typename Comp_>
void avl_tree_impl_<Key_, Comp_>::rotate_left_ (base_ptr_ node_)
{
    base_ptr_ rchild_ = node_->m_right_;
    node_->m_right_   = rchild_->m_left_;

    if ( rchild_->m_left_ )
        rchild_->m_left_->m_parent_ = node_;

    rchild_->m_parent_ = node_->m_parent_;

    if ( node_->m_parent_ == node_ )   // case root
        this->m_impl_.m_header_ = rchild_;
    else if ( node_ == node_->m_parent_->m_left_ )
        node_->m_parent_->m_left_ = rchild_;
    else
        node_->m_parent_->m_right_ = rchild_;
    rchild_->m_left_ = node_;
    node_->m_parent_ = rchild_;
}

template <typename Key_, typename Comp_>
void avl_tree_impl_<Key_, Comp_>::rotate_right_ (base_ptr_ node_)
{
    base_ptr_ lchild_ = node_->m_left_;
    node_->m_left_    = lchild_->m_right_;

    if ( lchild_->m_right_ )
        lchild_->m_right_->m_parent_ = node_;

    lchild_->m_parent_ = node_->m_parent_;

    if ( node_->m_parent_ == node_ )   // case root
        this->m_impl_.m_header_ = lchild_;
    else if ( node_ == node_->m_parent_->m_right_ )
        node_->m_parent_->m_right_ = lchild_;
    else
        node_->m_parent_->m_left_ = lchild_;
    lchild_->m_right_ = node_;
    node_->m_parent_  = lchild_;
}
template <typename Key_, typename Comp_>
void avl_tree_impl_<Key_, Comp_>::rotate_with_parent_ (base_ptr_ node_)
{
    if ( node_->is_left_child_ () )
        rotate_right_ (node_->m_parent_);
    else
        rotate_left_ (node_->m_parent_);
}

}   // namespace my