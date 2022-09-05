#include "avl_tree.hpp"
#include <iostream>
#include <sstream>

int main ()
{
    my::avl_tree_<int> tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.dump ("dump1");
    tree.erase (1);
    tree.dump ("dump2");
    tree.erase (7);
    tree.dump ("dump3");
    tree.erase (4);
    tree.dump ("dump4");

    std::cout << tree.m_get_rank_of_ (10);
}