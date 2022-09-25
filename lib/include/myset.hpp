/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// set implementation header

#pragma once

#include "avl_tree.hpp"
namespace rethinking_std
{
template <typename Key_, typename Compare_ = std::less<Key_>>
class set : public avl_tree_<Key_, Compare_>
{
  private:
    using tree_impl_ = avl_tree_<Key_, Compare_>;

  public:
    using value_type       = typename tree_impl_::value_type;
    using reference        = typename tree_impl_::reference;
    using pointer          = typename tree_impl_::pointer;
    using size_type        = typename tree_impl_::size_type;
    using iterator         = typename tree_impl_::iterator;
    using reverse_iterator = typename tree_impl_::reverse_iterator;
};

}   // namespace rethinking_std