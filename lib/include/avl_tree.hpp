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

namespace my
{

//===============================avl_tree_node_base===============================
struct avl_tree_node_base_
{
    using height_t        = int;
    using base_ptr_       = avl_tree_node_base_ *;
    using const_base_ptr_ = const avl_tree_node_base_ *;

    height_t m_bf_      = 0;
    base_ptr_ m_parent_ = nullptr;
    base_ptr_ m_left_   = nullptr;
    base_ptr_ m_right_  = nullptr;

    base_ptr_ m_minimum_ () noexcept
    {
        base_ptr_ x_ = this;
        while ( x_->m_left_ )
            x_ = x_->m_left_;
        return x_;
    }

    base_ptr_ m_maximum_ () noexcept
    {
        base_ptr_ x_ = this;
        while ( x_->m_right_ )
            x_ = x_->m_right_;
        return x_;
    }

    base_ptr_ m_predecessor_ () noexcept
    {
        if ( m_left_ )
            return m_left_->m_maximum_ ();
        return nullptr;
    }

    base_ptr_ m_successor_ () noexcept
    {
        if ( m_right_ )
            return m_right_->m_minimum_ ();
        return nullptr;
    }
    base_ptr_ avl_tree_increment_ () noexcept;
    base_ptr_ avl_tree_decrement_ () noexcept;

    // Fix left imbalance after insertion. Return the new root.
    base_ptr_ m_fix_left_imbalance_insert_ ();
    // Fix right imbalance after insertion. Return the new root.
    base_ptr_ m_fix_right_imbalance_insert_ ();

    base_ptr_ rotate_left_ ();
    base_ptr_ rotate_right_ ();
    void rotate_with_parent_ (base_ptr_ node_);

    bool is_left_child_ () const noexcept
    {
        return (m_parent_ ? this == m_parent_->m_left_ : false);
    }
    bool is_right_child_ () const noexcept
    {
        return (m_parent_ ? this == m_parent_->m_left_ : false);
    }
};

// Helper type offering value initialization guarantee on the compare functor.
template <class key_compare_> struct avl_tree_key_compare_
{
    key_compare_ m_key_compare_;

    avl_tree_key_compare_ () : m_key_compare_ () {}

    avl_tree_key_compare_ (const key_compare_ &comp_) : m_key_compare_ (comp_) {}

    avl_tree_key_compare_ (avl_tree_key_compare_ &&x_) : m_key_compare_ (x_.m_key_compare_) {}

    template <typename Key_> bool operator() (const Key_ &k1_, const Key_ &k2_)
    {
        return m_key_compare_ (k1_, k2_);
    }
};

// Node type.
template <typename val_> struct avl_tree_node_ : public avl_tree_node_base_
{
    using link_type_      = avl_tree_node_<val_>;
    using node_ptr_       = link_type_ *;
    using const_node_ptr_ = const link_type_ *;

    avl_tree_node_ () : avl_tree_node_base_ () {}

    avl_tree_node_ (val_ x_) : avl_tree_node_base_ (), m_key_ (x_) {}

    val_ m_key_;
};

// Helper type to manage deafault initialization of node count and header.
struct avl_tree_header_
{
    avl_tree_node_base_ *m_header_    = nullptr;
    avl_tree_node_base_ *m_leftmost_  = nullptr;
    avl_tree_node_base_ *m_rightmost_ = nullptr;
    std::size_t m_node_count_         = 0;   // keeps track of size of tree

    avl_tree_header_ () noexcept
    {
        m_header_ = new avl_tree_node_base_;
        m_reset_ ();
    }

    void m_move_data_ (avl_tree_header_ &from_) noexcept
    {
        m_header_->m_bf_     = from_.m_header_->m_bf_;
        m_header_->m_parent_ = from_.m_header_->m_parent_;
        m_header_->m_left_   = from_.m_header_->m_left_;
        m_header_->m_right_  = from_.m_header_->m_right_;
        m_node_count_        = from_.m_node_count_;

        from_.m_reset_ ();
    }

    void m_reset_ () noexcept
    {
        m_header_->m_parent_ = nullptr;
        m_leftmost_          = nullptr;
        m_rightmost_         = nullptr;
        m_header_->m_left_   = nullptr;
        m_node_count_        = 0;
    }
};

avl_tree_node_base_ *avl_tree_rebalance_for_erase (avl_tree_node_base_ *const erased_,
                                                   avl_tree_node_base_ &header_) noexcept;

//================================avl_tree_impl_========================================
template <typename Key_, typename Compare_>
struct avl_tree_impl_ : public avl_tree_key_compare_<Compare_>, public avl_tree_header_
{
    using base_key_compare_ = avl_tree_key_compare_<Compare_>;

    using base_ptr_        = avl_tree_node_base_ *;
    using const_base_ptr_  = const avl_tree_node_base_ *;
    using link_type_       = avl_tree_node_<Key_> *;
    using const_link_type_ = const avl_tree_node_<Key_> *;
    using key_type         = Key_;

    avl_tree_impl_ (const avl_tree_impl_ &x_)
        : base_key_compare_ (x_.m_key_compare_), avl_tree_header_ ()
    {
    }

    avl_tree_impl_ () : base_key_compare_ (Compare_ {}), avl_tree_header_ () {}

    avl_tree_impl_ (const Compare_ &comp_) : base_key_compare_ (comp_), avl_tree_header_ () {}

    avl_tree_impl_ (avl_tree_impl_ &&x_) noexcept
        : base_key_compare_ (std::move (x_)), avl_tree_header_ (std::move (x_))
    {
    }
};

//=================================avl_tree_=======================================
template <typename Key_, class Compare_ = std::less<Key_>>
struct avl_tree_ : public avl_tree_impl_<Key_, Compare_>
{
    using base_ptr_        = avl_tree_node_base_ *;
    using const_base_ptr_  = const avl_tree_node_base_ *;
    using link_type_       = avl_tree_node_<Key_> *;
    using const_link_type_ = const avl_tree_node_<Key_> *;

    struct avl_tree_iterator_
    {
        using value_type = Key_;
        using reference  = Key_ &;
        using pointer    = Key_ *;

        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;

        using self_      = avl_tree_iterator_;
        using base_ptr_  = avl_tree_node_base_ *;
        using link_type_ = avl_tree_node_<Key_> *;

        avl_tree_iterator_ () noexcept : m_node_ (), m_tree_ () {}

        explicit avl_tree_iterator_ (base_ptr_ x_) noexcept : m_node_ (x_) {}

        avl_tree_iterator_ (base_ptr_ x_, const avl_tree_ *tree_) noexcept
            : m_node_ (x_), m_tree_ (tree_) {};

        reference operator* () const noexcept { return static_cast<link_type_> (m_node_)->m_key_; }

        pointer operator-> () const noexcept { return &static_cast<link_type_> (m_node_)->m_key_; }

        self_ &operator++ () noexcept   // pre-increment
        {
            m_node_ = m_node_->avl_tree_increment_ ();
            return *this;
        }

        self_ operator++ (int) noexcept   // post-increment
        {
            self_ tmp_ = *this;
            m_node_    = m_node_->avl_tree_increment_ ();
            return tmp_;
        }

        self_ &operator-- () noexcept   // pre-decrement
        {
            m_node_ = (m_node_ ? m_node_->avl_tree_decrement_ () : m_tree_->m_rightmost_ ());
            return *this;
        }

        self_ operator-- (int) noexcept   // post-decrement
        {
            self_ tmp_ = *this;
            m_node_    = (m_node_ ? m_node_->avl_tree_decrement_ () : m_tree_->m_rightmost_ ());
            return tmp_;
        }

        friend bool operator== (const self_ &x_, const self_ &y_) noexcept
        {
            return x_.m_node_ == y_.m_node_;
        }

        friend bool operator!= (const self_ &x_, const self_ &y_) noexcept
        {
            return x_.m_node_ != y_.m_node_;
        }

        base_ptr_ m_node_;
        const avl_tree_ *m_tree_;
    };

  public:
    using key_type        = Key_;
    using value_type      = Key_;
    using pointer         = value_type *;
    using const_pointer   = const value_type *;
    using reference       = value_type &;
    using const_reference = const value_type &;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = avl_tree_iterator_;

    using reverse_iterator = std::reverse_iterator<iterator>;

    using m_impl_ = avl_tree_impl_<Key_, Compare_>;

  private:
    base_ptr_ &m_root_ () noexcept { return m_impl_::m_header_->m_left_; }

    const_base_ptr_ m_root_ () const noexcept { return m_impl_::m_header_->m_left_; }

    link_type_ m_begin_ () noexcept { return m_impl_::m_leftmost_; }

    base_ptr_ m_end_ () noexcept { return m_impl_::m_rightmost_; }

    link_type_ m_copy_ (const avl_tree_ &tree_);

    void m_erase_ (link_type_ x_);

    iterator m_lower_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_);

    iterator m_upper_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_);

  public:
    avl_tree_ () : m_impl_ (Compare_ {}) {}
    avl_tree_ (const Compare_ &comp_) : m_impl_ (comp_) {}

    avl_tree_ (const avl_tree_ &tree_) { m_root_ () = m_copy (tree_); }

    // default move ctor

    ~avl_tree_ () noexcept {}   // m_erase_ (m_begin_ ()); }

    avl_tree_ &operator= (const avl_tree_ &tree_);

    base_ptr_ m_leftmost_ () { return m_impl_::m_leftmost_; }
    base_ptr_ m_rightmost_ () { return m_impl_::m_rightmost_; }

    // Accessors.
    Compare_ key_comp () const { return m_impl_::base_key_compare_ (); }

    iterator begin () const noexcept { return iterator (m_impl_::m_leftmost_, this); }

    iterator end () const noexcept { return iterator (nullptr, this); }

    reverse_iterator rbegin () noexcept { return reverse_iterator (end (), this); }

    reverse_iterator rend () noexcept { return reverse_iterator (begin (), this); }

    size_type size () const noexcept { return m_impl_::m_node_count_; }

    void swap (avl_tree_ &t_);

    bool empty () const noexcept { return m_impl_::m_node_count_ == 0; }

    template <typename F>
    std::tuple<base_ptr_, base_ptr_, bool> m_trav_bin_search_ (key_type key_, F step_)
    {
        using res_      = std::tuple<avl_tree_node_base_ *, avl_tree_node_base_ *, bool>;
        using base_ptr_ = avl_tree_node_base_ *;

        base_ptr_ prev_ = m_impl_::m_header_;

        if ( !prev_ )
            return res_ (nullptr, nullptr, false);

        base_ptr_ curr_ = m_root_ ();

        bool key_less_ {};

        while ( curr_ && static_cast<link_type_> (curr_)->m_key_ != key_ )
        {
            key_less_ = m_impl_::m_key_compare_ (key_, static_cast<link_type_> (curr_)->m_key_);
            step_ (curr_);
            prev_ = curr_;
            if ( key_less_ )
                curr_ = curr_->m_left_;
            else
                curr_ = curr_->m_right_;
        }

        return res_ (curr_, prev_, key_less_);
    }

    // Insert/erase.
  public:
    // Insert node in AVL tree without rebalancing.
    base_ptr_ m_insert_node_ (link_type_ to_insert_)
    {
        if ( empty () )
        {
            m_root_ ()            = to_insert_;
            to_insert_->m_parent_ = m_impl_::m_header_;
            m_impl_::m_leftmost_  = to_insert_;
            m_impl_::m_rightmost_ = to_insert_;

            return to_insert_;
        }

        auto res          = m_trav_bin_search_ (to_insert_->m_key_, [] (base_ptr_ &) {});
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
        {
            prev->m_left_ = to_insert_;
            if ( prev == m_impl_::m_leftmost_ )
                m_impl_::m_leftmost_ = to_insert_;
        }
        else
        {
            prev->m_right_ = to_insert_;
            if ( prev == m_impl_::m_rightmost_ )
                m_impl_::m_rightmost_ = to_insert_;
        }

        return to_insert_;
    }
    // create node, insert and rebalance tree
    iterator m_insert_ (const key_type &key_)
    {
        auto to_insert_ = new avl_tree_node_<Key_> (key_);

        auto res = m_insert_node_ (to_insert_);
        m_rebalance_after_insert_ (to_insert_);
        m_impl_::m_node_count_ = 1;

        return iterator (res, this);
    }

    // Rebalance subtree after insert.
    void m_rebalance_after_insert_ (base_ptr_ leaf_);

  public:
    iterator erase (iterator pos_)
    {
        assert (pos_ != end ());
        iterator res_ = pos_;
        res_++;
        m_erase_aux_ (pos_);
        return iterator (res_, this);
    }

    iterator insert (const key_type &key_) { return m_insert_ (key_); }

    void clear () noexcept
    {
        m_erase_ (m_begin_ ());
        m_impl_::m_reset_ ();
    }

    // Set operations.
    iterator find (const key_type &k_);
    size_type count (const key_type &k_) const;

    iterator lower_bound (const key_type &k_)
    {
        return m_lower_bound_ (m_begin_ (), m_end_ (), k_);
    }

    iterator upper_bound (const key_type &k_)
    {
        return m_upper_bound_ (m_begin_ (), m_end_ (), k_);
    }

    const key_type closest_left (const key_type &k_)
    {
        base_ptr_ curr_  = m_impl_::m_header_;
        base_ptr_ bound_ = nullptr;

        while ( curr_ )
        {
            bool key_less = m_key_compare_ (k_, static_cast<link_type_> (curr_)->m_key_);
            if ( !key_less )
            {
                bound_ = curr_;
                curr_  = curr_->m_right_;
            }
            else
                curr_ = curr_->m_left_;
        }

        if ( !bound_ )
            throw std::out_of_range ("Leftmost element has no predeccessor");
        return static_cast<link_type_> (bound_)->m_key_;
    }

    const key_type closest_right (const key_type &k_)
    {
        base_ptr_ curr_  = m_impl_::m_header_;
        base_ptr_ bound_ = nullptr;

        while ( curr_ )
        {
            bool key_less = avl_tree_key_compare (k_, static_cast<link_type_> (curr_)->m_key_);
            if ( !key_less )
            {
                curr_ = curr_->m_right_;
            }
            else
            {
                bound_ = curr_;
                curr_  = curr_->m_left_;
            }
        }

        if ( !bound_ )
            throw std::out_of_range ("Rightmost element has no successor");
        return static_cast<link_type_> (bound_)->m_key_;
    }

  private:
    // Move elements from container.
    void m_move_data_ (avl_tree_ &tree_);

    friend bool operator== (const avl_tree_ &x_, const avl_tree_ &y_)
    {
        return x_.size () == y_.size () && std::equal (x_.begin (), x_.end (), y_.begin ());
    }

  public:
    void dump (std::string filename) const
    {
        std::ofstream p_stream {filename};
        assert (p_stream);
        p_stream << "digraph {\nrankdir = TB\n";
        for ( auto pos = begin (); pos != end (); pos++ )
        {
            p_stream << "\tnode_" << pos.m_node_ << "[label = \"" << *pos
                     << "\", shape=record, style=filled, fillcolor=palegreen];\n";

            if ( pos.m_node_->m_left_ )
                p_stream << "\tnode_" << pos.m_node_ << " -> node_" << pos.m_node_->m_left_
                         << ";\n";
            else
            {
                p_stream << "\tnode_" << pos.m_node_ << " -> node_0_l_" << *pos << ";\n";
                p_stream << "\tnode_0_l_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black ];\n";
            }

            if ( pos.m_node_->m_right_ )
                p_stream << "\tnode_" << pos.m_node_ << " -> node_" << pos.m_node_->m_right_
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

template <typename Key_, typename Val_, typename Comp_>
void swap (avl_tree_<Key_, Comp_> &x_, avl_tree_<Key_, Comp_> &y_)
{
    x_.swap (y_);
}

template <typename Key_, typename Comp_>
void avl_tree_<Key_, Comp_>::m_rebalance_after_insert_ (base_ptr_ node_)
{
    auto curr_   = node_;
    auto parent_ = curr_->m_parent_;

    while ( curr_ != m_root_ () )
    {
        int &bf_ = parent_->m_bf_;
        if ( curr_->is_left_child_ () ) /* The height of left subtree of parent subtree increases */
        {
            if ( bf_ == 1 )
            {
                bf_ = 0;
                break;
            }
            else if ( bf_ == 0 )
            {
                bf_ = -1;
            }
            else if ( bf_ == -1 )
            {
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
                bf_ = 0;
                break;
            }
            else if ( bf_ == 0 )
            {
                bf_ = 1;
            }
            else if ( bf_ == 1 )
            {
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

// Accessors.
template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::iterator
avl_tree_<Key_, Comp_>::m_lower_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_)
{
    while ( x_ )
    {
        bool key_bigger_ = m_impl_::m_key_compare_ (s_key_ (x_), k_);
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
typename avl_tree_<Key_, Comp_>::iterator
avl_tree_<Key_, Comp_>::m_upper_bound_ (link_type_ x_, base_ptr_ y_, const key_type &k_)
{
    while ( x_ )
    {
        bool key_less_ = m_impl_::key_compare_ (k_, s_key_ (x_));
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

}   // namespace my