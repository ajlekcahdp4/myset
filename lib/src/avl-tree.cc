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
template <typename F>
typename std::tuple<avl_tree_node_base_ *, avl_tree_node_base_ *, bool>
avl_tree_<Key_, Comp_>::m_trav_bin_search (key_type key_, F step_)
{
    using res_      = std::tuple<avl_tree_node_base_ *, avl_tree_node_base_ *, bool>;
    using base_ptr_ = avl_tree_node_base_ *;

    base_ptr_ curr_ = m_header_;
    base_ptr_ prev_ = nullptr;

    if ( !curr_ )
        return res_ (nullptr, nullptr, false);

    bool key_less_ {};

    while ( curr_ && s_key_ (curr_) == key_ )
    {
        key_less_ = avl_tree_key_compare_ (key_, s_key_ (curr_));
        step_ (curr_);
        prev_ = curr_;
        if ( key_less_ )
            curr_ = s_left_ (curr_);
        else
            curr_ = s_right_ (curr_);
    }

    return res_ (curr_, prev_, key_less_);
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

// Insert/Erase.

template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::iterator avl_tree_<Key_, Comp_>::m_insert_ (const key_type &key_)
{
    auto to_insert_ = new avl_tree_node_<Key_> (key_);

    auto res = m_insert_node_ (to_insert_);
    m_rebalance_after_insert_ (to_insert_);
    return res;
}

template <typename Key_, typename Comp_>
avl_tree_node_base_ *avl_tree_<Key_, Comp_>::m_insert_node_ (link_type_ to_insert_)
{
    if ( empty () )
    {
        avl_tree_header_::m_header_     = to_insert_;
        avl_tree_header_::m_node_count_ = 1;
        return iterator (to_insert_);
    }

    auto res          = m_trav_bin_search (s_key_ (to_insert_), [] (base_ptr_ &node_) {});
    auto found        = std::get<0> (res);
    auto prev         = std::get<1> (res);
    auto prev_greater = std::get<2> (res);

    if ( found )
    {
        delete to_insert_;
        throw std::out_of_range ("Element already inserted");
    }

    to_insert_->m_parent_ = prev;

    if ( prev_greater )
        prev->m_left_ = to_insert_;
    else
        prev->m_right_ = to_insert_;

    return to_insert_;
}

template <typename Key_, typename Comp_> void avl_tree_<Key_, Comp_>::rotate_left_ (base_ptr_ node_)
{
    base_ptr_ rchild_ = node_->m_right_;
    node_->m_right_   = rchild_->m_left_;

    if ( rchild_->m_left_ )
        rchild_->m_left_->m_parent_ = node_;

    rchild_->m_parent_ = node_->m_parent_;

    if ( node_->m_parent_ == node_->m_parent_ )   // case root
        this->m_impl_.m_header_ = rchild_;
    else if ( node_ == node_->m_parent_->m_left_ )
        node_->m_parent_->m_left_ = rchild_;
    else
        node_->m_parent_->m_right_ = rchild_;
    rchild_->m_left_ = node_;
    node_->m_parent_ = rchild_;
}

template <typename Key_, typename Comp_>
void avl_tree_<Key_, Comp_>::rotate_right_ (base_ptr_ node_)
{
    base_ptr_ lchild_ = node_->m_left_;
    node_->m_left_    = rchild_->m_light_;

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
avl_tree_node_base_ *
avl_tree_<Key_, Comp_>::m_rebalance_after_insert_ (base_ptr_ inserted_) noexcept
{
}

}   // namespace my