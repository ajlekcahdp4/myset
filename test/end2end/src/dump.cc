#include "avl_tree.hpp"
#include <iostream>
#include <sstream>

int main ()
{
    my::avl_tree_<int> tree;
    for ( int i = 0; i < 10; i++ )
    {
        tree.insert (i);
        std::stringstream ss;
        ss << "dump" << i;
        tree.dump (ss.str ());
    }

    for ( int i = 0; i < 5; i++ )
    {
        tree.erase (i);
        std::stringstream ss;
        ss << "dump_erase" << i;
        tree.dump (ss.str ());
    }

    std::cout << "::" << tree.m_os_select_ (1) << " " << tree.m_os_select_ (2) << " "
              << tree.m_os_select_ (3) << " " << tree.m_os_select_ (4);
}