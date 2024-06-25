
#include "EdgeHost.hpp"

using namespace std; 


// Removes the items that have expired
void EdgeHost::expire_cache(double time)
{
    for (auto it = cache.cbegin(); it != cache.cend() /* not hoisted */; /* no increment */)
    {
        // Second is the value of the hashmap, considering the Time to Live
        if ( it->second < time)
        {
            cache.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            ++it;
        }
    }
}