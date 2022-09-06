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
    using height_diff_t = int;
    using size_type     = std::size_t;
    using base_ptr_     = avl_tree_node_base_ *;
    using self_         = avl_tree_node_base_;
    using owning_ptr_   = typename std::unique_ptr<self_>;

    height_diff_t m_bf_  = 0;
    size_type m_size_    = 1;
    base_ptr_ m_parent_  = nullptr;
    owning_ptr_ m_left_  = nullptr;
    owning_ptr_ m_right_ = nullptr;

    static size_type size (base_ptr_ node_) { return (node_ ? node_->m_size_ : 0); }

    base_ptr_ m_left () noexcept { return m_left_.get (); }

    base_ptr_ m_right () noexcept { return m_right_.get (); }

    base_ptr_ m_minimum_ () noexcept
    {
        base_ptr_ x_ = this;
        while ( x_->m_left () )
            x_ = x_->m_left ();
        return x_;
    }

    base_ptr_ m_maximum_ () noexcept
    {
        base_ptr_ x_ = this;
        while ( x_->m_right () )
            x_ = x_->m_right ();
        return x_;
    }

    base_ptr_ m_predecessor_for_erase_ () noexcept
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

    base_ptr_ m_successor_for_erase_ () noexcept
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

    base_ptr_ avl_tree_increment_ () noexcept;
    base_ptr_ avl_tree_decrement_ () noexcept;

    // Fix left imbalance after insertion. Return the new root.
    base_ptr_ m_fix_left_imbalance_insert_ ();
    // Fix right imbalance after insertion. Return the new root.
    base_ptr_ m_fix_right_imbalance_insert_ ();

    // Fix left imbalance for erase. Return the new root.
    base_ptr_ m_fix_left_imbalance_erase_ ();

    // Fixright imbalance for erase. Return the new root.
    base_ptr_ m_fix_right_imbalance_erase_ ();

    base_ptr_ rotate_left_ ();
    base_ptr_ rotate_right_ ();

    bool is_left_child_ () const noexcept
    {
        return (m_parent_ ? this == m_parent_->m_left () : false);
    }
};

// Helper type offering value initialization guarantee on the compare functor.
template <class key_compare_> struct avl_tree_key_compare_
{
    key_compare_ m_key_compare_;

    avl_tree_key_compare_ (const key_compare_ &comp_) : m_key_compare_ (comp_) {}

    template <typename Key_> bool operator() (const Key_ &k1_, const Key_ &k2_)
    {
        return m_key_compare_ (k1_, k2_);
    }
};

// Node type.
template <typename val_> struct avl_tree_node_ : public avl_tree_node_base_
{
    using link_type_ = avl_tree_node_<val_>;
    using node_ptr_  = link_type_ *;

    avl_tree_node_ (val_ x_) : avl_tree_node_base_ (), m_key_ (x_) {}

    val_ m_key_;
};

// Helper type to manage deafault initialization of node count and header.
struct avl_tree_header_
{
    using base_ptr_   = typename avl_tree_node_base_::base_ptr_;
    using owning_ptr_ = typename avl_tree_node_base_::owning_ptr_;
    using base_node_  = avl_tree_node_base_;
    using size_type   = typename avl_tree_node_base_::size_type;

    owning_ptr_ m_header_  = nullptr;
    base_ptr_ m_leftmost_  = nullptr;
    base_ptr_ m_rightmost_ = nullptr;

    avl_tree_header_ () noexcept
    {
        m_header_ = std::make_unique<base_node_> ();
        m_reset_ ();
    }

    void m_move_data_ (avl_tree_header_ &from_) noexcept
    {
        m_header_->m_bf_     = from_.m_header_->m_bf_;
        m_header_->m_parent_ = from_.m_header_->m_parent_;
        m_header_->m_left_   = std::move (from_.m_header_->m_left_);
        m_header_->m_right_  = std::move (from_.m_header_->m_right_);

        from_.m_reset_ ();
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

//================================avl_tree_impl_========================================
template <typename Key_, typename Compare_>
struct avl_tree_impl_ : public avl_tree_key_compare_<Compare_>, public avl_tree_header_
{
    using base_key_compare_ = avl_tree_key_compare_<Compare_>;

    using base_ptr_     = typename avl_tree_node_base_::base_ptr_;
    using owning_ptr_   = typename avl_tree_node_base_::owning_ptr_;
    using link_type_    = avl_tree_node_<Key_> *;
    using base_node_    = avl_tree_node_base_;
    using node_         = avl_tree_node_<Key_>;
    using key_type      = Key_;
    using size_type     = typename avl_tree_node_base_::size_type;
    using height_diff_t = typename avl_tree_node_base_::height_diff_t;

    avl_tree_impl_ (const avl_tree_impl_ &x_)
        : base_key_compare_ (x_.m_key_compare_), avl_tree_header_ ()
    {
    }

    avl_tree_impl_ (const Compare_ &comp_) : base_key_compare_ (comp_), avl_tree_header_ () {}
};

//=================================avl_tree_=======================================
template <typename Key_, class Compare_ = std::less<Key_>>
struct avl_tree_ : public avl_tree_impl_<Key_, Compare_>
{
    using m_impl_ = avl_tree_impl_<Key_, Compare_>;

    using base_ptr_   = typename m_impl_::base_ptr_;
    using base_node_  = typename m_impl_::base_node_;
    using link_type_  = typename m_impl_::link_type_;
    using owning_ptr_ = typename m_impl_::owning_ptr_;

    using node_ = typename m_impl_::node_;

    struct avl_tree_iterator_
    {
        using value_type = Key_;
        using reference  = Key_ &;
        using pointer    = Key_ *;

        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;

        using self_ = avl_tree_iterator_;

        avl_tree_iterator_ () noexcept : m_node_ (), m_tree_ () {}

        explicit avl_tree_iterator_ (base_ptr_ x_) noexcept : m_node_ (x_) {}

        avl_tree_iterator_ (base_ptr_ x_, const avl_tree_ *tree_) noexcept
            : m_node_ (x_), m_tree_ (tree_) {};

        reference operator* () const { return static_cast<link_type_> (m_node_)->m_key_; }

        pointer get () { return &(static_cast<link_type_> (m_node_)->m_key_); }

        pointer operator-> () { return get (); }

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
            m_node_ = (m_node_ ? m_node_->avl_tree_decrement_ () : m_tree_->m_end_ ());
            return *this;
        }

        self_ operator-- (int) noexcept   // post-decrement
        {
            self_ tmp_ = *this;
            m_node_    = (m_node_ ? m_node_->avl_tree_decrement_ () : m_tree_->m_end_ ());
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
    using key_type      = Key_;
    using value_type    = Key_;
    using pointer       = value_type *;
    using reference     = value_type &;
    using size_type     = typename m_impl_::size_type;
    using height_diff_t = typename m_impl_::height_diff_t;

    using iterator         = avl_tree_iterator_;
    using reverse_iterator = std::reverse_iterator<iterator>;

  private:
    base_ptr_ m_root_ () noexcept { return m_impl_::m_header_->m_left (); }

    const base_ptr_ m_root_ () const noexcept { return m_impl_::m_header_->m_left (); }

    base_ptr_ &m_begin_ () noexcept { return m_impl_::m_leftmost_; }

    base_ptr_ m_begin_ () const noexcept { return m_impl_::m_leftmost_; }

    base_ptr_ &m_end_ () noexcept { return m_impl_::m_rightmost_; }

    base_ptr_ m_end_ () const noexcept { return m_impl_::m_rightmost_; }

    link_type_ m_copy_ (const avl_tree_ &tree_);

    iterator m_lower_bound_ (base_ptr_ x_, base_ptr_ y_, const key_type &k_);

    iterator m_upper_bound_ (base_ptr_ x_, base_ptr_ y_, const key_type &k_);

    static key_type &s_key_ (base_ptr_ node_) { return static_cast<link_type_> (node_)->m_key_; }

  public:
    avl_tree_ () : m_impl_ (Compare_ {}) {}
    avl_tree_ (const Compare_ &comp_) : m_impl_ (comp_) {}

    avl_tree_ (const avl_tree_ &tree_) { m_root_ () = m_copy (tree_); }

    // default move ctor

    ~avl_tree_ () noexcept {}

    avl_tree_ &operator= (const avl_tree_ &tree_);

    base_ptr_ m_leftmost_ () { return m_impl_::m_leftmost_; }
    base_ptr_ m_rightmost_ () { return m_impl_::m_rightmost_; }

    // Accessors.
    Compare_ key_comp () const { return m_impl_::base_key_compare_ (); }

    iterator begin () const noexcept { return iterator (m_impl_::m_leftmost_, this); }

    iterator end () const noexcept { return iterator (nullptr, this); }

    reverse_iterator rbegin () noexcept { return reverse_iterator (end ()); }

    reverse_iterator rend () noexcept { return reverse_iterator (begin ()); }

    size_type size () const noexcept
    {
        auto root_ = m_root_ ();
        return (root_ ? base_node_::size (root_) : 0);
    }

    bool empty () const noexcept { return (size () == 0); }

    template <typename F>
    std::tuple<base_ptr_, base_ptr_, bool> m_trav_bin_search_ (key_type key_, F step_);

    // Insert/erase.

  private:
    // Insert node in AVL tree without rebalancing.
    base_ptr_ m_insert_node_ (owning_ptr_ to_insert_);

    // create node, insert and rebalance tree
    iterator m_insert_ (const key_type &key_)
    {
        auto to_insert_             = new node_ (key_);
        auto to_insert_base_unique_ = owning_ptr_ (static_cast<base_ptr_> (to_insert_));

        auto res = m_insert_node_ (std::move (to_insert_base_unique_));
        m_rebalance_after_insert_ (res);

        return iterator (res, this);
    }

    // Rebalance subtree after insert.
    void m_rebalance_after_insert_ (base_ptr_ leaf_);

    base_ptr_ m_erase_pos_ (iterator to_erase_pos_) { return m_erase_pos_impl_ (to_erase_pos_); }

    // Rebalance tree for erase.
    void m_rebalance_for_erase_ (base_ptr_ node_);

    // erase node from the container.
    base_ptr_ m_erase_pos_impl_ (iterator pos_);

  public:
    iterator find (const key_type &key_)
    {
        auto tuple_ = m_trav_bin_search_ (key_, [] (base_ptr_ &) {});
        return iterator (std::get<0> (tuple_), this);
    }

    iterator m_find_for_erase_ (const key_type &key_)
    {
        auto tuple_ = m_trav_bin_search_ (key_, [] (base_ptr_ &node_) { node_->m_size_--; });
        auto node_  = std::get<0> (tuple_);

        if ( node_ )
            return iterator (node_, this);

        m_trav_bin_search_ (key_, [] (base_ptr_ &node_) { node_->m_size_++; });
        throw std::out_of_range ("No element with requested key for erase.");
    }

    iterator insert (const key_type &key_) { return m_insert_ (key_); }

    bool erase (const key_type &key_)
    {
        auto to_erase_pos_ = m_find_for_erase_ (key_);
        return m_erase_pos_ (to_erase_pos_);
    }

    void erase (iterator pos_)   // ???
    {
        if ( pos_ != end () )
        {
            m_trav_bin_search_ (*pos_, [] (base_ptr_ &node_) { node_->m_size_--; });
            m_erase_pos_ (pos_);
        }
    }

    void clear () noexcept { m_impl_::m_reset_ (); }

    // Set operations.
    iterator lower_bound (const key_type &k_) { return m_lower_bound_ (m_root_ (), nullptr, k_); }

    iterator upper_bound (const key_type &k_) { return m_upper_bound_ (m_root_ (), nullptr, k_); }

    // return key value of ith smallest element in AVL-tree
    key_type m_os_select_ (size_t i);

    // return the rank of the node with matching key_
    size_type m_get_rank_of_ (iterator pos_);

    // Return number of elements with the key less then the given one.
    size_type m_get_number_less_then_ (key_type key_)
    {
        if ( empty () )
            return 0;

        auto min_key_ = s_key_ (m_begin_ ());

        std::cerr << min_key_ << " " << key_ << std::endl;

        if ( m_impl_::m_key_compare_ (key_, min_key_) || key_ == min_key_ )
            return 0;

        /* Previous element exists. */
        auto closest_left_ = --upper_bound (key_);

        auto rank_ = m_get_rank_of_ (closest_left_);

        return (*closest_left_ == key_ ? rank_ - 1 : rank_);
    }

  private:
    // Move elements from container.

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

template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::key_type avl_tree_<Key_, Comp_>::m_os_select_ (size_t i)
{
    if ( i > size () || !i )
        throw std::out_of_range ("i is greater then the size of the tree or zero.");

    auto curr_ = m_root_ ();

    /* The rank of node is the size of left subtree plus 1. */
    size_t rank_ = base_node_::size (curr_->m_left ()) + 1;

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
        rank_ = base_node_::size (curr_->m_left ()) + 1;
    }

    return s_key_ (curr_);
}
template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::size_type avl_tree_<Key_, Comp_>::m_get_rank_of_ (iterator pos_)
{
    if ( pos_ == end () )
        throw std::out_of_range ("Element with the given key is not inserted.");

    auto node_ = pos_.m_node_;

    /* The rank of node is the size of left subtree plus 1. */
    size_type rank_ = base_node_::size (node_->m_left ()) + 1;

    while ( node_ != m_root_ () )
    {
        if ( !node_->is_left_child_ () )
            rank_ += base_node_::size (node_->m_parent_->m_left ()) + 1;
        node_ = node_->m_parent_;
    }

    return rank_;
}

template <typename Key_, typename Comp_>
template <typename F>
std::tuple<typename avl_tree_<Key_, Comp_>::base_ptr_, typename avl_tree_<Key_, Comp_>::base_ptr_,
           bool>
avl_tree_<Key_, Comp_>::m_trav_bin_search_ (key_type key_, F step_)
{
    using res_ = std::tuple<base_ptr_, base_ptr_, bool>;

    auto prev_ = m_root_ ();
    auto curr_ = prev_;
    if ( !curr_ )
        return res_ (nullptr, nullptr, false);

    bool key_less_ {};

    while ( curr_ && s_key_ (curr_) != key_ )
    {
        key_less_ = m_impl_::m_key_compare_ (key_, s_key_ (curr_));
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
typename avl_tree_<Key_, Comp_>::base_ptr_
avl_tree_<Key_, Comp_>::m_insert_node_ (owning_ptr_ to_insert_)
{
    auto to_insert_ptr_ = to_insert_.get ();
    if ( empty () )
    {
        m_impl_::m_header_->m_left_ = std::move (to_insert_);
        to_insert_ptr_->m_parent_   = m_impl_::m_header_.get ();

        m_impl_::m_leftmost_  = to_insert_ptr_;
        m_impl_::m_rightmost_ = to_insert_ptr_;

        return to_insert_ptr_;
    }

    /* Find right position in the tree */
    auto res =
        m_trav_bin_search_ (s_key_ (to_insert_ptr_), [] (base_ptr_ &node_) { node_->m_size_++; });
    auto found        = std::get<0> (res);
    auto prev         = std::get<1> (res);
    auto prev_greater = std::get<2> (res);

    if ( found )
    {
        m_trav_bin_search_ (s_key_ (to_insert_ptr_), [] (base_ptr_ &node_) { node_->m_size_--; });
        throw std::out_of_range ("Element already inserted");
    }
    to_insert_->m_parent_ = prev;

    if ( prev_greater )
    {
        prev->m_left_ = std::move (to_insert_);
        if ( prev == m_impl_::m_leftmost_ )
            m_impl_::m_leftmost_ = to_insert_ptr_;
    }
    else
    {
        prev->m_right_ = std::move (to_insert_);
        if ( prev == m_impl_::m_rightmost_ )
            m_impl_::m_rightmost_ = to_insert_ptr_;
    }

    return to_insert_ptr_;
}

template <typename Key_, typename Comp_>
typename avl_tree_<Key_, Comp_>::base_ptr_ avl_tree_<Key_, Comp_>::m_erase_pos_impl_ (iterator pos_)
{
    auto to_erase_    = pos_.m_node_;
    base_ptr_ target_ = nullptr;

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
void avl_tree_<Key_, Comp_>::m_rebalance_after_insert_ (base_ptr_ node_)
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
void avl_tree_<Key_, Comp_>::m_rebalance_for_erase_ (base_ptr_ node_)
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
typename avl_tree_<Key_, Comp_>::iterator
avl_tree_<Key_, Comp_>::m_lower_bound_ (base_ptr_ x_, base_ptr_ y_, const key_type &k_)
{
    while ( x_ )
    {
        bool key_bigger_ = m_impl_::m_key_compare_ (s_key_ (x_), k_);
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
typename avl_tree_<Key_, Comp_>::iterator
avl_tree_<Key_, Comp_>::m_upper_bound_ (base_ptr_ x_, base_ptr_ y_, const key_type &k_)
{
    while ( x_ )
    {
        bool key_less_ = m_impl_::m_key_compare_ (k_, s_key_ (x_));
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

}   // namespace my