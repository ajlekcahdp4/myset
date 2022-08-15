/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <utility>

namespace my
{

//===============================avl_tree_node_base===============================
struct avl_tree_node_base_
{
    using height_diff_t  = int;
    using base_ptr       = avl_tree_node_base_ *;
    using const_base_ptr = const avl_tree_node_base_ *;

    height_diff_t m_height_diff_ = 0;
    base_ptr m_parent_           = nullptr;
    base_ptr m_left_             = nullptr;
    base_ptr m_right_            = nullptr;

    static base_ptr s_minimum_ (base_ptr x_) noexcept
    {
        while ( x_->m_left_ )
            x_ = x_->m_left_;
        return x_;
    }

    static const_base_ptr s_minimum_ (const_base_ptr x_) noexcept
    {
        while ( x_->m_left_ )
            x_ = x_->m_left_;
        return x_;
    }

    static base_ptr s_maximum_ (base_ptr x_) noexcept
    {
        while ( x_->m_right_ )
            x_ = x_->m_right_;
        return x_;
    }

    static const_base_ptr s_maximum_ (const_base_ptr x_) noexcept
    {
        while ( x_->m_right_ )
            x_ = x_->m_right_;
        return x_;
    }
};

// Helper type offering value initialization guarantee on the compare functor.
template <typename key_compare_> struct avl_tree_key_compare_
{
    key_compare_ m_key_compare_;

    avl_tree_key_compare_ () : m_key_compare_ () {}

    avl_tree_key_compare_ (const key_compare_ &comp_) : m_key_compare_ (comp_) {}

    avl_tree_key_compare_ (avl_tree_key_compare_ &&x_) : m_key_compare_ (x_.m_key_compare_) {}
};

// Helper type to manage deafault initialization of node count and header.
struct avl_treee_header_
{
    avl_tree_node_base_ m_header_;
    std::size_t m_node_count_ = 0;   // keeps track of size of tree

    avl_treee_header_ () noexcept { m_reset_ (); }

    avl_treee_header_ (avl_treee_header_ &&x_) noexcept
    {
        if ( x_.m_header_.m_parent_ != nullptr )
            m_move_data_ (x_);
        else
        {
            m_header_.m_height_diff_ = 0;
            m_reset_ ();
        }
    }

    void m_move_data_ (avl_treee_header_ &from_) noexcept
    {
        m_header_.m_height_diff_       = from_.m_header_.m_height_diff_;
        m_header_.m_parent_            = from_.m_header_.m_parent_;
        m_header_.m_left_              = from_.m_header_.m_left_;
        m_header_.m_right_             = from_.m_header_.m_right_;
        m_header_.m_parent_->m_parent_ = &m_header_;
        m_node_count_                  = from_.m_node_count_;

        from_.m_reset_ ();
    }

    void m_reset_ () noexcept
    {
        m_header_.m_parent_ = nullptr;
        m_header_.m_left_   = nullptr;
        m_header_.m_right_  = nullptr;
        m_node_count_       = 0;
    }
};

template <typename val_> struct avl_tree_node_ : public avl_tree_node_base_
{
    using link_type = avl_tree_node_<val_> *;

    avl_tree_node_ () {}

    avl_tree_node_ (val_ x_) : m_val_ (x_) {}

    val_ m_val_;

    val_ *m_valptr_ () { return &m_val_; }

    const val_ *m_valptr_ () const { return &m_val_; }
};

avl_tree_node_base_ *avl_tree_increment_ (avl_tree_node_base_ *x_) noexcept;

const avl_tree_node_base_ *avl_tree_increment_ (const avl_tree_node_base_ *x_) noexcept;

avl_tree_node_base_ *avl_tree_decrement_ (avl_tree_node_base_ *x_) noexcept;

const avl_tree_node_base_ *avl_tree_decrement_ (const avl_tree_node_base_ *x_) noexcept;

template <typename Tp_> struct avl_tree_iterator_
{
    using value_type = Tp_;
    using reference  = Tp_ &;
    using pointer    = Tp_ *;

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;

    using self_      = avl_tree_iterator_<Tp_>;
    using base_ptr_  = avl_tree_node_base_::base_ptr;
    using link_type_ = avl_tree_node_<Tp_> *;

    avl_tree_iterator_ () noexcept : m_node_ () {}

    explicit avl_tree_iterator_ (base_ptr_ x_) noexcept : m_node_ (x_) {}

    reference operator* () const noexcept { return *static_cast<link_type_> (m_node_)->m_valptr_ (); }

    pointer operator-> () const noexcept { return static_cast<link_type_> (m_node_)->m_valptr_ (); }

    self_ &operator++ () noexcept   // pre-increment
    {
        m_node_ = avl_tree_increment_ (m_node_);
        return *this;
    }

    self_ operator++ (int) noexcept   // post-increment
    {
        self_ tmp_ = *this;
        m_node_    = avl_tree_increment_ (m_node_);
        return tmp_;
    }

    self_ &operator-- () noexcept   // pre-decrement
    {
        m_node_ = avl_tree_decrement_ (m_node_);
        return *this;
    }

    self_ operator-- (int) noexcept   // post-decrement
    {
        self_ tmp_ = *this;
        m_node_    = avl_tree_decrement_ (m_node_);
        return tmp_;
    }

    friend bool operator== (const self_ &x_, const self_ &y_) noexcept { return x_.m_node_ == y_.m_node_; }

    friend bool operator!= (const self_ &x_, const self_ &y_) noexcept { return x_.m_node_ != y_.m_node_; }

    base_ptr_ m_node_;
};

}   // namespace my