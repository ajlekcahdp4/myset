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
    using height_diff_t   = int;
    using base_ptr_       = avl_tree_node_base_ *;
    using const_base_ptr_ = const avl_tree_node_base_ *;

    height_diff_t m_height_diff_ = 0;
    base_ptr_ m_parent_          = nullptr;
    base_ptr_ m_left_            = nullptr;
    base_ptr_ m_right_           = nullptr;

    base_ptr_ s_minimum_ () noexcept
    {
        base_ptr_ x_ = this;
        while ( x_->m_left_ )
            x_ = x_->m_left_;
        return x_;
    }

    base_ptr_ s_maximum_ () noexcept
    {
        base_ptr_ x_ = this;
        while ( x_->m_right_ )
            x_ = x_->m_right_;
        return x_;
    }

    base_ptr_ m_predecessor_ () noexcept
    {
        if ( m_left_ )
            return m_left_->s_maximum_ ();
        return nullptr;
    }

    base_ptr_ m_successor_ () noexcept
    {
        if ( m_right_ )
            return m_right_->s_minimum_ ();
        return nullptr;
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
    using base_ptr_  = avl_tree_node_base_::base_ptr_;
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

template <typename Tp_> struct avl_tree_const_iterator_
{
    using value_type = Tp_;
    using reference  = const Tp_ &;
    using pointer    = const Tp_ *;

    using iterator = avl_tree_iterator_<Tp_>;

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using self_             = avl_tree_const_iterator_<Tp_>;
    using base_ptr_         = avl_tree_node_base_::const_base_ptr;
    using link_type_        = avl_tree_node_<Tp_> *;
    using const_link_type_  = const avl_tree_node_<Tp_> *;

    avl_tree_const_iterator_ () noexcept : m_node_ () {}

    avl_tree_const_iterator_ (base_ptr_ x_) noexcept : m_node_ (x_) {}

    avl_tree_const_iterator_ (const iterator &it_) noexcept : m_node_ (it_.m_node_) {}

    iterator m_const_cast () const noexcept { return iterator (m_node_); }

    reference operator* () const noexcept { return *static_cast<link_type_> (m_node_)->m_valptr_ (); }

    pointer operator-> () const noexcept { return static_cast<link_type_> (m_node_)->m_valptr_ (); }

    self_ &operator++ ()
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

void avl_tree_insert_and_rebalance_ (avl_tree_node_base_ *x_, avl_tree_node_base_ *y_,
                                     avl_tree_node_base_ &header_) noexcept;

avl_tree_node_base_ *avl_tree_rebalance_for_erase (avl_tree_node_base_ *const erased_,
                                                   avl_tree_node_base_ &header_) noexcept;

template <typename Key_, typename Compare_> class avl_tree_
{
    using base_ptr_        = avl_tree_node_base_ *;
    using const_based_ptr_ = const avl_tree_node_base_ *;
    using link_type_       = avl_tree_node_<Val_> *;
    using const_link_type  = const avl_tree_node_<Val_> *;

  public:
    using key_type        = Key_;
    using value_type      = Key_;
    using pointer         = value_type *;
    using const_pointer   = const value_type *;
    using reference       = value_type &;
    using const_reference = const value_type &;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

  private:
    template <typename key_compare_> struct avl_tree_impl_ : public avl_tree_key_compare_, public avl_treee_header_
    {
        using base_key_comapare_ = avl_tree_key_compare_<key_compare_>;

        avl_tree_impl_ (const avl_tree_impl_ &x_) : base_key_compare_ (x_.m_key_compare_), avl_treee_header_ () {}

        avl_tree_impl_ (const key_compare_ &comp_) : base_key_comapare_ (comp_) {}

        avl_tree_impl_ (avl_tree_impl_ &&x_) noexcept
            : base_key_comapare_ (std::move (x_)), avl_treee_header_ (std::move (x_))
        {
        }
    };

    avl_tree_impl_<Compare_> m_impl_;

  private:
    base_ptr_ &m_root_ () noexcept { return m_impl_.m_header_.m_parent_; }

    const_base_ptr_ m_root_ () const noexcept { return m_impl_.m_header_.m_parent_; }

    base_ptr_ &m_leftmost_ () noexcept { return m_impl_.m_header_.m_left_; }

    const_base_ptr_ m_leftmost_ () const noexcept { return m_impl_.m_header_.m_left_; }

    base_ptr_ &m_rightmost_ () noexcept { return m_impl_.m_header_.m_right_; }

    const_base_ptr_ m_rightmost_ () const noexcept { return m_impl_.m_header_.m_right_; }

    link_type_ m_begin_ () const noexcept { return static_cast<link_type_> (m_impl_.m_header_.m_parent_); }

    const_link_type_ m_begin_ () const noexcept { return static_cast<const_link_type_> (m_impl_.m_header_.m_parent_); }

    base_ptr_ m_end_ () noexcept { return &m_impl_.m_header_; }

    const_base_ptr_ m_end_ () const noexcept { return &m_impl_.m_header_; }

    static const Key_ &s_key_ (const_link_type_ x_) { return *x_->m_valptr_ (); }

    static link_type_ s_left_ (base_ptr_ x_) noexcept { return static_cast<link_type_> (x_->m_left_); }

    static const_link_type_ s_left_ (const_base_ptr_ x_) noexcept
    {
        return static_cast<const_link_type_> (x_->m_left_);
    }

    static link_type_ s_right_ (base_ptr_ x_) noexcept { return static_cast<link_type_> (x_->m_right_); }

    static const_link_type_ s_right_ (const_base_ptr_ x_) noexcept
    {
        return static_cast<const_link_type_> (x_->m_right_);
    }

    static const Key_ &s_key_ (const_base_ptr_ x_) { return s_key_ (static_cast<const_link_type> (x_)); }

    static base_ptr_ s_minimum_ (base_ptr_ x_) noexcept { return avl_tree_node_base_::s_minimum_ (x_); }

    static const_base_ptr_ s_minimum_ (const_base_ptr_ x_) noexcept { return avl_tree_node_base_::s_minimum_ (x_); }

    static base_ptr_ s_minimum_ (base_ptr_ x_) noexcept { return avl_tree_node_base_::s_maximum_ (x_); }

    static const_base_ptr_ s_minimum_ (const_base_ptr_ x_) noexcept { return avl_tree_node_base_::s_maximum_ (x_); }

  public:
    using iterator       = avl_tree_iterator_<value_type>;
    using const_iterator = avl_tree_const_iterator_<value_type>;

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    std::pair<base_ptr_, base_ptr_> m_get_insert_unique_pos_ (const key_type &k_);

    std::pair<base_ptr_, base_ptr_> m_get_insert_equal_pos_ (const key_type &k_);

    std::pair<base_ptr_, base_ptr_> m_get_insert_hint_unique_pos_ (const_iterator pos_, const key_type &k_);

    std::pair<base_ptr_, base_ptr_> m_get_insert_hint_equal_pos_ (const_iterator pos_, const key_type &k_);

    iterator m_insert_node_ (base_ptr_ x_, base_ptr_ y_, link_type_ node_);

    link_type_ m_copy_ (const avl_tree_ &tree_);

    void m_erase_ (link_type_ x_);

    iterator m_lower_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_);

    const_iterator m_lower_bound_ (const_link_type_ x_, const_base_ptr_ y_, const key_type &k_) const;

    iterator m_upper_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_);

    const_iterator m_upper_bound_ (const_link_type_ x_, const_base_ptr_ y_, const key_type &k_) const;

  public:
    avl_tree_ (const Compare_ &comp_) : m_impl_ (comp_) {}

    avl_tree_ (const avl_tree_ &tree_) : m_impl_ (tree_.m_impl_) { m_root_ = m_copy (tree_); }

    // default move ctor

    ~avl_tree_ () noexcept { m_erase_ (m_begin ()); }

    avl_tree_ &operator= (const avl_tree_ &tree_);

    // Accessors.
    Compare_ key_comp () const { return m_impl_.m_key_compare; }

    iterator begin () noexcept { return iterator (m_impl_.m_header_.m_left_;); }

    const_iterator begin () const noexcept { return const_iterator (m_impl_.m_header_.m_left_); }

    iterator end () noexcept { return iterator (m_impl_.m_header_); }

    const_iterator end () const noexcept { return const_iterator (m_impl_.m_header_); }

    reverse_iterator rbegin () noexcept { return reverse_iterator (end ()); }

    const_reverse_iterator rbegin () const noexcept { return const_reverse_iterator (end ()); }

    reverse_iterator rend () noexcept { return reverse_iterator (begin ()); }

    const_reverse_iterator end () const noexcept { return const_reverse_iterator (begin ()); }

    bool empty () const noexcept { return m_impl_.m_node_count_ == 0; }

    size_type size () const noexcept { return m_impl_.m_node_count_; }

    void swap (avl_tree_ &t_);

    // Insert/erase.

    template <typename Arg_> std::pair<iterator, bool> m_insert_unique_ (Arg_ &&x_);

    template <typename Arg_> iterator m_insert_equal_ (Arg_ &&x_);

    template <typename Arg_> iterator m_insert_unique_ (const_iterator pos_, Arg_ &&x_);

    template <typename Arg_> iterator m_insert_equal_ (const_iterator pos_, Arg_ &&x_);

    template <typename... Args_> std::pair<iterator, bool> m_emplace_unique_ (Args_ &&...args_);

    template <typename... Args_> iterator m_emplace_equal_ (Args_ &&...args_);

    template <typename... Args_> iterator m_emplace_hint_unique_ (const_iterator pos_, Args_ &&...args_);

    template <typename... Args_> iterator m_emplace_hint_equal_ (const_iterator pos_, Args_ &&...args_);

    m_insert_range_unique_ (iterator first_, iterator last_)
    {
        for ( ; first_ != last_; ++first_ )
            m_emplace_unique_ (*first_);
    }

    m_insert_range_equal_ (iterator first_, iterator last_)
    {
        for ( ; first_ != last_; ++first_ )
            m_emplace_equal_ (*first_);
    }

  private:
    void m_erase_aux_ (const_iterator pos_);

    void m_erase_aux_ (const_iterator first_, const_iterator last_);

    iterator erase (const_iterator pos_)
    {
        assert (pos_ != end ());
        const_iterator res_ = pos_;
        res_++;
        m_erase_aux_ (pos_);
        return res_;
    }

    size_type erase (const size_type &x_);

    iterator erase (const_iterator first_, const_iterator last_)
    {
        m_erase_aux_ (first_, last_);
        return last_.m_const_cast ();
    }

    void clear () noexcept
    {
        m_erase_ (m_begin_ ());
        m_impl_.m_reset_ ();
    }

    // Set operations.
    iterator find (const key_type &k_);

    const_iterator find (const key_type &k_) const;

    size_type count (const key_type &k_) const;

    iterator lower_bound (const key_type &k_) { return m_lower_bound_ (m_begin_ (), m_end (), k_); }

    const_iterator lower_bound (const key_type &k_) const { return m_lower_bound_ (m_begin_ (), m_end (), k_); }

    iterator upper_bound (const key_type &k_) { return m_upper_bound_ (m_begin_ (), m_end (), k_); }

    const_iterator upper_bound (const key_type &k_) const { return m_upper_bound_ (m_begin_ (), m_end (), k_); }

    std::pair<iterator, iterator> equal_range (const key_type &k_);

    std::pair<const_iterator, const_iterator> equal_range (const key_type &k_) const;

    const key_type closest_left (const key_type &k_);

    const key_type closest_right (const key_type &k_);

  private:
    // Move elements from container.
    void m_move_data_ (avl_tree_ &tree_) { m_impl_.m_move_data_ (tree_.m_impl_); }

    friend bool operator== (const avl_tree_ &x_, const avl_tree_ &y_)
    {
        return x_.size () == y_.size () && std::equal (x_.begin (), x_.end (), y_.begin ());
    }
};

template <typename Key_, typename Val_, typename Comp_>
void swap (avl_tree_<Key_, Comp_> &x_, avl_tree_<Key_, Comp_> &y_)
{
    x_.swap (y_);
}

}   // namespace my