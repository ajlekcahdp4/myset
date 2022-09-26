/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// avl tree implementation header

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <tuple>
#include <utility>

namespace rethinking_stl
{

//===============================do_avl_tree_node_===============================
template <typename Val_> struct do_avl_tree_node_
{
    using height_diff_t = int;
    using value_type    = Val_;
    using size_type     = std::size_t;
    using node_ptr_     = do_avl_tree_node_<Val_> *;
    using self_         = do_avl_tree_node_<Val_>;
    using owning_ptr_   = typename std::unique_ptr<self_>;

    height_diff_t m_bf_  = 0;
    size_type m_size_    = 1;
    node_ptr_ m_parent_  = nullptr;
    owning_ptr_ m_left_  = nullptr;
    owning_ptr_ m_right_ = nullptr;
    value_type m_key_;

    do_avl_tree_node_ (value_type val_) : m_key_ {val_} {}
    do_avl_tree_node_ () {}

    static size_type size (node_ptr_ node_) { return (node_ ? node_->m_size_ : 0); }

    node_ptr_ m_left () noexcept { return m_left_.get (); }

    node_ptr_ m_right () noexcept { return m_right_.get (); }

    node_ptr_ m_minimum_ () noexcept
    {
        node_ptr_ x_ = this;
        while ( x_->m_left () )
            x_ = x_->m_left ();
        return x_;
    }

    node_ptr_ m_maximum_ () noexcept
    {
        node_ptr_ x_ = this;
        while ( x_->m_right () )
            x_ = x_->m_right ();
        return x_;
    }

    node_ptr_ m_predecessor_for_erase_ () noexcept;

    node_ptr_ m_successor_for_erase_ () noexcept;

    node_ptr_ dynamic_order_avl_tree_increment_ () noexcept;
    node_ptr_ dynamic_order_avl_tree_decrement_ () noexcept;

    // Fix left imbalance after insertion. Return the new root.
    node_ptr_ m_fix_left_imbalance_insert_ ();
    // Fix right imbalance after insertion. Return the new root.
    node_ptr_ m_fix_right_imbalance_insert_ ();

    // Fix left imbalance for erase. Return the new root.
    node_ptr_ m_fix_left_imbalance_erase_ ();

    // Fixright imbalance for erase. Return the new root.
    node_ptr_ m_fix_right_imbalance_erase_ ();

    node_ptr_ rotate_left_ ();
    node_ptr_ rotate_right_ ();

    bool is_left_child_ () const noexcept
    {
        return (m_parent_ ? this == m_parent_->m_left () : false);
    }
};

// Helper type offering value initialization guarantee on the compare functor.
template <class Compare_> struct do_avl_tree_key_compare_
{
    Compare_ m_key_compare_;

    do_avl_tree_key_compare_ (const Compare_ &comp_) : m_key_compare_ (comp_) {}
    do_avl_tree_key_compare_ (do_avl_tree_key_compare_<Compare_> &&other)
    {
        m_key_compare_ = std::move (other.m_key_compare_);
    }

    template <typename Key_> bool operator() (const Key_ &k1_, const Key_ &k2_)
    {
        return m_key_compare_ (k1_, k2_);
    }
};

// Helper type to manage deafault initialization of node count and header.
template <typename Val_> struct do_avl_tree_header_
{
    using node_ptr_   = typename do_avl_tree_node_<Val_>::node_ptr_;
    using owning_ptr_ = typename do_avl_tree_node_<Val_>::owning_ptr_;
    using node_       = do_avl_tree_node_<Val_>;

    owning_ptr_ m_header_  = nullptr;
    node_ptr_ m_leftmost_  = nullptr;
    node_ptr_ m_rightmost_ = nullptr;

    do_avl_tree_header_ ()
    {
        m_header_ = std::make_unique<node_> ();
        m_reset_ ();
    }

    void m_reset_ () noexcept
    {
        m_header_->m_parent_ = nullptr;
        m_leftmost_          = nullptr;
        m_rightmost_         = nullptr;
        /* delete tree of unique pointers if any*/
        m_header_->m_left_.release ();
    }
};

//=================================dynamic_order_avl_tree_=======================================
template <typename Key_, class Compare_ = std::less<Key_>> struct dynamic_order_avl_tree_
{
    using key_compare_ = do_avl_tree_key_compare_<Compare_>;
    using header_      = do_avl_tree_header_<Key_>;
    using self_        = dynamic_order_avl_tree_<Key_, Compare_>;

    using node_ptr_   = typename do_avl_tree_node_<Key_>::node_ptr_;
    using owning_ptr_ = typename do_avl_tree_node_<Key_>::owning_ptr_;

    using node_ = do_avl_tree_node_<Key_>;

    key_compare_ m_compare_struct_;
    header_ m_header_struct_;

    struct do_avl_tree_iterator_
    {
        using value_type = Key_;
        using reference  = Key_ &;
        using pointer    = Key_ *;

        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;

        using self_ = do_avl_tree_iterator_;

        do_avl_tree_iterator_ () noexcept : m_node_ (), m_tree_ () {}

        explicit do_avl_tree_iterator_ (node_ptr_ x_) noexcept : m_node_ (x_) {}

        do_avl_tree_iterator_ (node_ptr_ x_, const dynamic_order_avl_tree_ *tree_) noexcept
            : m_node_ (x_), m_tree_ (tree_) {};

        reference operator* () const { return static_cast<node_ptr_> (m_node_)->m_key_; }

        pointer get () { return &(static_cast<node_ptr_> (m_node_)->m_key_); }

        pointer operator->() { return get (); }

        self_ &operator++ () noexcept   // pre-increment
        {
            m_node_ = m_node_->dynamic_order_avl_tree_increment_ ();
            return *this;
        }

        self_ operator++ (int) noexcept   // post-increment
        {
            self_ tmp_ = *this;
            m_node_    = m_node_->dynamic_order_avl_tree_increment_ ();
            return tmp_;
        }

        self_ &operator-- () noexcept   // pre-decrement
        {
            m_node_ =
                (m_node_ ? m_node_->dynamic_order_avl_tree_decrement_ () : m_tree_->m_end_ ());
            return *this;
        }

        self_ operator-- (int) noexcept   // post-decrement
        {
            self_ tmp_ = *this;
            m_node_ =
                (m_node_ ? m_node_->dynamic_order_avl_tree_decrement_ () : m_tree_->m_end_ ());
            return tmp_;
        }

        bool operator== (const self_ &other_) const noexcept { return m_node_ == other_.m_node_; }

        bool operator!= (const self_ &other_) const noexcept { return m_node_ != other_.m_node_; }

        node_ptr_ m_node_;
        const dynamic_order_avl_tree_ *m_tree_;
    };

  public:
    using value_type = Key_;
    using pointer    = value_type *;
    using reference  = value_type &;

    using iterator         = do_avl_tree_iterator_;
    using reverse_iterator = std::reverse_iterator<iterator>;

    using size_type     = typename node_::size_type;
    using height_diff_t = typename node_::height_diff_t;

  private:
    node_ptr_ m_root_ () const noexcept { return m_header_struct_.m_header_->m_left (); }

    node_ptr_ &m_begin_ () noexcept { return m_header_struct_.m_leftmost_; }

    node_ptr_ m_begin_ () const noexcept { return m_header_struct_.m_leftmost_; }

    node_ptr_ &m_end_ () noexcept { return m_header_struct_.m_rightmost_; }

    node_ptr_ m_end_ () const noexcept { return m_header_struct_.m_rightmost_; }

    iterator m_lower_bound_ (node_ptr_ x_, node_ptr_ y_, const value_type &k_);

    iterator m_upper_bound_ (node_ptr_ x_, node_ptr_ y_, const value_type &k_);

    static value_type &s_key_ (node_ptr_ node_) { return static_cast<node_ptr_> (node_)->m_key_; }

  public:
    dynamic_order_avl_tree_ () : m_compare_struct_ (Compare_ {}), m_header_struct_ () {}
    dynamic_order_avl_tree_ (const Compare_ &comp_) : m_compare_struct_ (comp_), m_header_struct_ ()
    {
    }

    dynamic_order_avl_tree_ (const self_ &other) = delete;
    self_ &operator= (const self_ &other)        = delete;

    dynamic_order_avl_tree_ (self_ &&other) noexcept
        : m_compare_struct_ (std::move (other.m_compare_struct_.m_key_compare_))
    {
        m_header_struct_.m_header_ = std::move (other.m_header_struct_.m_header_);
        std::swap (m_header_struct_.m_leftmost_, other.m_header_struct_.m_leftmost_);
        std::swap (m_header_struct_.m_rightmost_, other.m_header_struct_.m_rightmost_);
    }

    // Accessors.

    iterator begin () const noexcept { return iterator (m_header_struct_.m_leftmost_, this); }

    iterator end () const noexcept { return iterator (nullptr, this); }

    reverse_iterator rbegin () noexcept { return reverse_iterator (end ()); }

    reverse_iterator rend () noexcept { return reverse_iterator (begin ()); }

    size_type size () const noexcept
    {
        auto root_ = m_root_ ();
        return (root_ ? node_::size (root_) : 0);
    }

    bool empty () const noexcept { return (size () == 0); }

    template <typename F>
    std::tuple<node_ptr_, node_ptr_, bool> m_trav_bin_search_ (value_type key_, F step_);

    // Insert/erase.

  private:
    // Insert node in AVL tree without rebalancing.
    node_ptr_ m_insert_node_ (owning_ptr_ to_insert_);

    // create node, insert and rebalance tree
    iterator m_insert_ (const value_type &key_)
    {
        auto to_insert_             = new node_ (key_);
        auto to_insert_base_unique_ = owning_ptr_ (static_cast<node_ptr_> (to_insert_));

        auto res = m_insert_node_ (std::move (to_insert_base_unique_));
        m_rebalance_after_insert_ (res);

        return iterator (res, this);
    }

    // Rebalance subtree after insert.
    void m_rebalance_after_insert_ (node_ptr_ leaf_);

    node_ptr_ m_erase_pos_ (iterator to_erase_pos_) { return m_erase_pos_impl_ (to_erase_pos_); }

    // Rebalance tree for erase.
    void m_rebalance_for_erase_ (node_ptr_ node_);

    // erase node from the container.
    node_ptr_ m_erase_pos_impl_ (iterator pos_);

  public:
    iterator find (const value_type &key_)
    {
        auto [found, prev, prev_greater] = m_trav_bin_search_ (key_, [] (node_ptr_ &) {});
        return iterator (found, this);
    }

    iterator m_find_for_erase_ (const value_type &key_)
    {
        auto [found, prev, prev_greater] =
            m_trav_bin_search_ (key_, [] (node_ptr_ &node_) { node_->m_size_--; });
        auto node_ = found;

        if ( node_ )
            return iterator (node_, this);

        m_trav_bin_search_ (key_, [] (node_ptr_ &node_) { node_->m_size_++; });
        throw std::out_of_range ("No element with requested key for erase.");
    }

    iterator insert (const value_type &key_) { return m_insert_ (key_); }

    bool erase (const value_type &key_)
    {
        auto to_erase_pos_ = m_find_for_erase_ (key_);
        return m_erase_pos_ (to_erase_pos_);
    }

    void erase (iterator pos_)   // ???
    {
        if ( pos_ != end () )
        {
            m_trav_bin_search_ (*pos_, [] (node_ptr_ &node_) { node_->m_size_--; });
            m_erase_pos_ (pos_);
        }
    }

    void clear () noexcept { m_header_struct_.m_reset_ (); }

    // Set operations.
    iterator lower_bound (const value_type &k_) { return m_lower_bound_ (m_root_ (), nullptr, k_); }

    iterator upper_bound (const value_type &k_) { return m_upper_bound_ (m_root_ (), nullptr, k_); }

    // return key value of ith smallest element in AVL-tree
    value_type m_os_select_ (size_type i);

    // return the rank of the node with matching key_
    size_type m_get_rank_of_ (iterator pos_);

    // Return number of elements with the key less then the given one.
    size_type m_get_number_less_then_ (value_type key_)
    {
        if ( empty () )
            return 0;

        auto min_key_ = s_key_ (m_begin_ ());

        if ( this->m_compare_struct_.m_key_compare_ (key_, min_key_) || key_ == min_key_ )
            return 0;

        /* Previous element exists. */
        auto closest_left_ = --upper_bound (key_);

        auto rank_ = m_get_rank_of_ (closest_left_);

        return (*closest_left_ == key_ ? rank_ - 1 : rank_);
    }

  public:
    bool operator== (const dynamic_order_avl_tree_ &other_) const
    {
        return size () == other_.size () && std::equal (begin (), end (), other_.begin ());
    }

    bool operator!= (const dynamic_order_avl_tree_ &other_) const { return !(*this == other_); }

    value_type os_select (size_type i) { return m_os_select_ (i); }

    size_type get_number_less_then (value_type key_) { return m_get_number_less_then_ (key_); }

    void dump (std::string filename) const
    {
        std::ofstream p_stream {filename};
        assert (p_stream);
        p_stream << "digraph {\nrankdir = TB\n";
        for ( auto pos = begin (); pos != end (); pos++ )
        {
            p_stream << "\tnode_" << pos.m_node_ << "[label = \"{" << *pos << " | "
                     << pos.m_node_->m_bf_ << "} | " << pos.m_node_->m_size_
                     << "\", shape=record, style=filled, fillcolor=palegreen];\n";

            if ( pos.m_node_->m_left_ )
                p_stream << "\tnode_" << pos.m_node_ << " -> node_" << pos.m_node_->m_left ()
                         << ";\n";
            else
            {
                p_stream << "\tnode_" << pos.m_node_ << " -> node_0_l_" << *pos << ";\n";
                p_stream << "\tnode_0_l_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black ];\n";
            }

            if ( pos.m_node_->m_right_ )
                p_stream << "\tnode_" << pos.m_node_ << " -> node_" << pos.m_node_->m_right ()
                         << ";\n";
            else
            {
                p_stream << "\tnode_" << pos.m_node_ << " -> node_0_r_" << *pos << ";\n";
                p_stream << "\tnode_0_r_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
            }
        }
        p_stream << "}\n";
    }
};

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_
do_avl_tree_node_<Val_>::m_predecessor_for_erase_ () noexcept
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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_
do_avl_tree_node_<Val_>::m_successor_for_erase_ () noexcept
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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_
do_avl_tree_node_<Val_>::dynamic_order_avl_tree_increment_ () noexcept
{
    auto curr_ = this;
    if ( curr_->m_right_ )

        return curr_->m_right_->m_minimum_ ();

    node_ptr_ prev_ = curr_->m_parent_;

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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_
do_avl_tree_node_<Val_>::dynamic_order_avl_tree_decrement_ () noexcept
{
    auto curr_ = this;

    if ( curr_->m_left_ )
        return curr_->m_left_->m_maximum_ ();

    node_ptr_ prev_ = curr_->m_parent_;
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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_ do_avl_tree_node_<Val_>::m_fix_left_imbalance_insert_ ()
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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_
do_avl_tree_node_<Val_>::m_fix_right_imbalance_insert_ ()
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
template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_ do_avl_tree_node_<Val_>::m_fix_right_imbalance_erase_ ()
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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_ do_avl_tree_node_<Val_>::m_fix_left_imbalance_erase_ ()
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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_ do_avl_tree_node_<Val_>::rotate_left_ ()
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

template <typename Val_>
typename do_avl_tree_node_<Val_>::node_ptr_ do_avl_tree_node_<Val_>::rotate_right_ ()
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

template <typename Key_, typename Comp_>
typename dynamic_order_avl_tree_<Key_, Comp_>::value_type
dynamic_order_avl_tree_<Key_, Comp_>::m_os_select_ (size_type i)
{
    if ( i > size () || !i )
        throw std::out_of_range ("i is greater then the size of the tree or zero.");

    auto curr_ = m_root_ ();

    /* The rank of node is the size of left subtree plus 1. */
    size_t rank_ = node_::size (curr_->m_left ()) + 1;

    while ( rank_ != i )
    {
        if ( i < rank_ )
            curr_ = curr_->m_left ();
        else
        {
            curr_ = curr_->m_right ();

            /* Reduce i, cause we've already passed rank_ smallest nodes. */
            i -= rank_;
        }
        rank_ = node_::size (curr_->m_left ()) + 1;
    }

    return s_key_ (curr_);
}
template <typename Key_, typename Comp_>
typename dynamic_order_avl_tree_<Key_, Comp_>::size_type
dynamic_order_avl_tree_<Key_, Comp_>::m_get_rank_of_ (iterator pos_)
{
    if ( pos_ == end () )
        throw std::out_of_range ("Element with the given key is not inserted.");

    auto node_ = pos_.m_node_;

    /* The rank of node is the size of left subtree plus 1. */
    size_type rank_ = node_::size (node_->m_left ()) + 1;

    while ( node_ != m_root_ () )
    {
        if ( !node_->is_left_child_ () )
            rank_ += node_::size (node_->m_parent_->m_left ()) + 1;
        node_ = node_->m_parent_;
    }

    return rank_;
}

template <typename Key_, typename Comp_>
template <typename F>
std::tuple<typename dynamic_order_avl_tree_<Key_, Comp_>::node_ptr_,
           typename dynamic_order_avl_tree_<Key_, Comp_>::node_ptr_, bool>
dynamic_order_avl_tree_<Key_, Comp_>::m_trav_bin_search_ (value_type key_, F step_)
{
    using res_ = std::tuple<node_ptr_, node_ptr_, bool>;

    auto prev_ = m_root_ ();
    auto curr_ = prev_;
    if ( !curr_ )
        return res_ (nullptr, nullptr, false);

    bool key_less_ {};

    while ( curr_ && s_key_ (curr_) != key_ )
    {
        key_less_ = this->m_compare_struct_.m_key_compare_ (key_, s_key_ (curr_));
        step_ (curr_);
        prev_ = curr_;
        if ( key_less_ )
            curr_ = curr_->m_left ();
        else
            curr_ = curr_->m_right ();
    }

    if ( curr_ == m_root_ () )
        step_ (curr_);

    return res_ (curr_, prev_, key_less_);
}

template <typename Key_, typename Comp_>
typename dynamic_order_avl_tree_<Key_, Comp_>::node_ptr_
dynamic_order_avl_tree_<Key_, Comp_>::m_insert_node_ (owning_ptr_ to_insert_)
{
    auto to_insert_ptr_ = to_insert_.get ();
    if ( empty () )
    {
        m_header_struct_.m_header_->m_left_ = std::move (to_insert_);
        to_insert_ptr_->m_parent_           = m_header_struct_.m_header_.get ();

        m_header_struct_.m_leftmost_  = to_insert_ptr_;
        m_header_struct_.m_rightmost_ = to_insert_ptr_;

        return to_insert_ptr_;
    }

    /* Find right position in the tree */
    auto [found, prev, prev_greater] =
        m_trav_bin_search_ (s_key_ (to_insert_ptr_), [] (node_ptr_ &node_) { node_->m_size_++; });

    if ( found )
    {
        m_trav_bin_search_ (s_key_ (to_insert_ptr_), [] (node_ptr_ &node_) { node_->m_size_--; });
        throw std::out_of_range ("Element already inserted");
    }
    to_insert_->m_parent_ = prev;

    if ( prev == m_header_struct_.m_header_.get () || prev_greater )
    {
        prev->m_left_ = std::move (to_insert_);
        if ( prev == m_header_struct_.m_leftmost_ )
            m_header_struct_.m_leftmost_ = to_insert_ptr_;
    }
    else
    {
        prev->m_right_ = std::move (to_insert_);
        if ( prev == m_header_struct_.m_rightmost_ )
            m_header_struct_.m_rightmost_ = to_insert_ptr_;
    }

    return to_insert_ptr_;
}

template <typename Key_, typename Comp_>
typename dynamic_order_avl_tree_<Key_, Comp_>::node_ptr_
dynamic_order_avl_tree_<Key_, Comp_>::m_erase_pos_impl_ (iterator pos_)
{
    auto to_erase_    = pos_.m_node_;
    node_ptr_ target_ = nullptr;

    /* choose node's in-order successor if it has two children */
    if ( !to_erase_->m_left_ || !to_erase_->m_right_ )
    {
        target_ = to_erase_;

        /* Change leftmost or rightmost if needed */
        if ( m_begin_ () == target_ )
            m_begin_ () = target_->m_successor_for_erase_ ();
        if ( m_end_ () == target_ )
            m_end_ () = target_->m_predecessor_for_erase_ ();
    }
    else
    {
        target_ =
            to_erase_->m_successor_for_erase_ (); /* to_erase_->m_right_ exist, thus move down */
        std::swap (s_key_ (target_), s_key_ (to_erase_));
    }

    target_->m_size_--;

    /* rebalancing target subtree */
    m_rebalance_for_erase_ (target_);

    auto child_u_ptr_ = std::move (target_->m_left_ ? target_->m_left_ : target_->m_right_);

    if ( child_u_ptr_ )
        child_u_ptr_->m_parent_ = target_->m_parent_;

    auto t_parent_ = target_->m_parent_;

    if ( target_->is_left_child_ () )
        t_parent_->m_left_ = std::move (child_u_ptr_);
    else
        t_parent_->m_right_ = std::move (child_u_ptr_);

    return target_;
}

template <typename Key_, typename Comp_>
void dynamic_order_avl_tree_<Key_, Comp_>::m_rebalance_after_insert_ (node_ptr_ node_)
{

    /*
     * 1. update the balance factor of parent node;
     * 2. rebalance if the balance factor of parent node temporarily becomes +2 or -2;
     * 3. terminate if the height of that parent subtree remains unchanged.
     */

    auto curr_   = node_;
    auto parent_ = curr_->m_parent_;

    while ( curr_ != m_root_ () )
    {
        int &bf_ = parent_->m_bf_;
        if ( curr_->is_left_child_ () ) /* The height of left subtree of parent subtree increases */
        {
            if ( bf_ == 1 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                bf_ = 0;
                break;
            }
            else if ( bf_ == 0 )
            {
                /*
                 * The height of parent subtree increases by one, thus backtracking continue.
                 * Left-heavy.
                 */
                bf_ = -1;
            }
            else if ( bf_ == -1 )
            {
                /* The balance factor becomes -2, thus need to fix imbalance. */
                parent_->m_fix_left_imbalance_insert_ ();
                break;
            }
            else
                throw std::out_of_range ("Unexpected value of bf.");
        }
        else /* The height of right subtree of parent subtree increases */
        {
            if ( bf_ == -1 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                bf_ = 0;
                break;
            }
            else if ( bf_ == 0 )
            {
                /*
                 * Height of parent subtree increases by one, thus backtracking continue.
                 * Right-heavy.
                 */
                bf_ = 1;
            }
            else if ( bf_ == 1 )
            {
                /*
                 * The balance factor becomes 2, thus need to fix imbalance.
                 * After fixing parent tree has the same height, thus backtracking terminate.
                 */
                parent_->m_fix_right_imbalance_insert_ ();
                break;
            }
            else
                throw std::out_of_range ("Unexpected value of bf.");
        }
        curr_   = parent_;
        parent_ = curr_->m_parent_;
    }
}

template <typename Key_, typename Comp_>
void dynamic_order_avl_tree_<Key_, Comp_>::m_rebalance_for_erase_ (node_ptr_ node_)
{

    /*
     * Backtracking algo:
     * 1. update the balance factor of parent node;
     * 2. rebalance if the balance factor of parent node temporarily becomes +2 or -2;
     * 3. terminate if the height of that parent subtree remains unchanged.
     */

    auto curr_   = node_;
    auto parent_ = node_->m_parent_;

    while ( curr_ != m_root_ () )
    {
        auto &parent_bf_ = parent_->m_bf_;

        if ( curr_
                 ->is_left_child_ () ) /* The height of left subtree of parent subtree decreases. */
        {
            if ( parent_bf_ == -1 )
                /* The height of parent subtree decreases by one, thus continue backtracking. */
                parent_bf_ = 0;
            else if ( parent_bf_ == 0 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                parent_bf_ = 1;
                break;
            }
            else if ( parent_bf_ == 1 )
            {
                /* The balance factor becomes 2, thus need to fix imbalance. */
                parent_->m_fix_right_imbalance_erase_ ();
                break;
            }
            else
                throw std::out_of_range ("Unexpected value of bf.");
        }
        else /* The height of right subtree of parent subtree decreases. */
        {
            if ( parent_bf_ == 1 )
                /* The height of parent subtree decreases by one, thus continue backtracking. */
                parent_bf_ = 0;
            else if ( parent_bf_ == 0 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                parent_bf_ = -1;
                break;
            }
            else if ( parent_bf_ == -1 )
            {
                /* The balance factor becomes -2, thus need to fix imbalance. */
                parent_->m_fix_left_imbalance_erase_ ();
                break;
            }
        }

        curr_   = parent_;
        parent_ = curr_->m_parent_;
    }
}

// Accessors.
template <typename Key_, typename Comp_>
typename dynamic_order_avl_tree_<Key_, Comp_>::iterator
dynamic_order_avl_tree_<Key_, Comp_>::m_lower_bound_ (node_ptr_ x_, node_ptr_ y_,
                                                      const value_type &k_)
{
    while ( x_ )
    {
        bool key_bigger_ = this->m_compare_struct_.m_key_compare_ (s_key_ (x_), k_);
        if ( !key_bigger_ )
        {
            y_ = x_;
            x_ = x_->m_left ();
        }
        else
            x_ = x_->m_right ();
    }
    return iterator (y_, this);
}

template <typename Key_, typename Comp_>
typename dynamic_order_avl_tree_<Key_, Comp_>::iterator
dynamic_order_avl_tree_<Key_, Comp_>::m_upper_bound_ (node_ptr_ x_, node_ptr_ y_,
                                                      const value_type &k_)
{
    while ( x_ )
    {
        bool key_less_ = this->m_compare_struct_.m_key_compare_ (k_, s_key_ (x_));
        if ( key_less_ )
        {
            y_ = x_;
            x_ = x_->m_left ();
        }
        else
            x_ = x_->m_right ();
    }
    return iterator (y_, this);
}

}   // namespace rethinking_stl