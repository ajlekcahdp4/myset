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
template <typename value_type = int> class set
{
  public:
    class iterator;
    //=============================avl_tree_node_t====================================
    class avl_tree_node_t
    {
      public:
        using size_type  = std::size_t;
        using state_type = int;
        using node_ptr_  = avl_tree_node_t *;

        node_ptr_ parent_ = nullptr;
        node_ptr_ left_   = nullptr;
        node_ptr_ right_  = nullptr;
        state_type diff_  = 0;
        size_type size_   = 0;
        value_type val_;
        iterator pos;

        avl_tree_node_t (value_type val) : val_ (val), pos (this) {}

        bool insert_right_child (node_ptr_ new_node)
        {
            if ( !new_node )
                throw std::invalid_argument ("insert_right_child()");

            if ( !right_ )
            {
                right_            = new_node;
                new_node->parent_ = this;
                return true;
            }
            return false;
        }

        bool insert_left_child (node_ptr_ new_node)
        {
            if ( !new_node )
                throw std::invalid_argument ("insert_left_child()");

            if ( !left_ )
            {
                left_             = new_node;
                new_node->parent_ = this;
                return true;
            }
            return false;
        }
    };

    using node_ptr_t = typename avl_tree_node_t::node_ptr_;
    using size_type  = typename avl_tree_node_t::size_type;
    using node_t     = avl_tree_node_t;

    //=============================set::iterator=====================================
    class iterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type *;
        using reference         = value_type &;

        iterator (node_ptr_t ptr) : m_ptr_ (ptr) {}

        reference operator* () const { return m_ptr_->val_; }
        pointer operator-> () { return &m_ptr_->val_; }

        iterator &operator++ ()   // prefix increment
        {
            m_ptr_++;
            return *this;
        }
        iterator &operator++ (int)   // postfix increment
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator &operator-- ()   // prefix decrement
        {
            m_ptr_--;
            return *this;
        }
        iterator &operator-- (int)   // postfix decrement
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        friend bool operator== (const iterator &a, const iterator &b) { return a.m_ptr_ == b.m_ptr_; }
        friend bool operator!= (const iterator &a, const iterator &b) { return a.m_ptr_ != b.m_ptr_; }

      private:
        node_ptr_t m_ptr_;
        node_ptr_t m_prev_;
        node_ptr_t m_next_;
    };

  private:
    node_ptr_t head_ = nullptr;
    size_type size_  = 0;
    node_ptr_t minimum_;
    node_ptr_t maximum_;

  public:
    node_ptr_t head () noexcept { return head_; };
    size_type size () const noexcept { return size_; }
    bool empty () const noexcept { return !head_; }

    iterator begin () { return iterator {&minimum_->val_}; }
    iterator end () { return iterator {&maximum_->val_}; }

    std::pair<iterator, bool> insert (value_type &val)   // copying
    {
        auto inserted_node_ptr = new avl_tree_node_t (val);
        if ( !head_ )
        {
            head_ = inserted_node_ptr;
            size_++;
            return std::pair<iterator, bool> (head_, true);
        }

        auto cur_node = head_;
        for ( ;; )
        {
            if ( val == cur_node->val_ )   // case =
            {
                size_++;
                return std::pair<iterator, bool> (cur_node, false);
            }
            if ( val > cur_node->val_ )   // case >
            {
                if ( cur_node->right_ )
                    cur_node = cur_node->right_;
                else
                {
                    cur_node->insert_right_child (inserted_node_ptr);
                    size_++;
                    return std::pair<iterator, bool> (inserted_node_ptr, true);
                }
            }
            else   // case <
            {
                if ( cur_node->left_ )
                    cur_node = cur_node->left_;
                else
                {
                    size_++;
                    cur_node->insert_right_child (inserted_node_ptr);
                    return std::pair<iterator, bool> (inserted_node_ptr, true);
                }
            }
        }
    }
    void emplace (value_type &&val);   // move
    bool lookup (value_type &val);

  private:
};
}   // namespace my