#include "avl_tree.hpp"
#include <iostream>
#include <sstream>

int main ()
{
    my::avl_tree_<int> tree;
    for ( int i = 6; i <= 10; i++ )
    {
        tree.insert (i);
        std::stringstream ss;
        ss << "dump" << i;
        tree.dump (ss.str ());
    }
    for ( int i = 1; i <= 5; i++ )
    {
        tree.insert (i);
        std::stringstream ss;
        ss << "dump" << i;
        tree.dump (ss.str ());
    }
}