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
namespace rethinking_stl
{

template <typename Key_, typename Compare_ = std::less<Key_>>
using set = dynamic_order_avl_tree_<Key_, Compare_>;

}   // namespace rethinking_stl