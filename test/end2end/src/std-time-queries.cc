#include <set>
#include <iostream>
#include <cassert>
#include <iterator>


int os_select (std::set<int> set_, const int &key_)
{
    assert(key_ > 0);
    auto l_key = key_ - 1;
    auto it = set_.begin();
    return *(std::next(it, l_key));
}

int get_number_less_then (std::set<int> set_, const int &key_)
{
    auto it = set_.begin();
    int cnt {};
    if (key_ < *it)
        return 0;
    while (it != set_.end() && *it != key_)
    {
        it++;
        cnt++;
    }
    return cnt;
}


int main ()
{
    std::set<int> set_ {};
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
            os_select (set_, key);
            break;
        case 'n':
            get_number_less_then (set_, key);
            break;
        default:
            std::cerr << "Invalid query." << std::endl;
            not_end = false;
        }
    }
}
