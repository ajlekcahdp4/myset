#include "myset.hpp"
#include <iostream>
#include <sstream>

template class my::set<int>;

int main ()
{
    my::set<int> set_ {};
    bool not_end = true;

    while ( not_end )
    {
        char query_type {};
        int key;

        if ( !(std::cin >> query_type >> key) )
            break;
        switch ( query_type )
        {
        case 'k':
            set_.insert (key);
            break;
        case 'm':
            std::cout << set_.os_select (key) << " ";
            break;
        case 'n':
            std::cout << set_.get_number_less_then (key) << " ";
            break;
        default:
            std::cerr << "Invalid query." << std::endl;
            not_end = false;
        }
    }

    std::cout << std::endl;
}
