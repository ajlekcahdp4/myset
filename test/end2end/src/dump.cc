#include "avl_tree.hpp"
int main ()
{
    my::avl_tree_<int> tree;

    for ( int i = 6; i <= 10; i++ )
        tree.insert (i);
    for ( int i = 1; i <= 5; i++ )
        tree.insert (i);
    tree.dump ();
}