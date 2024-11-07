
#include "EdgeHost.hpp"

using namespace std; 

EdgeHost::EdgeHost(std::string name, int mem_limit)
{
    host_name = name;
    host_mem_limit = mem_limit;
}

void EdgeHost::add_task_in_cache(const std::string & task_name, int timeslot)
{
    cache[task_name]  = timeslot;
}

void EdgeHost::delete_task_from_cache(const std::string & task_name)
{
    cache.erase(task_name);
}

void EdgeHost::expire_tasks(int timeslot)
{
    for (auto it = cache.cbegin(); it != cache.cend() /* not hoisted */; /* no increment */)
    {
        // Second is the value of the hashmap, considering the Time to Live
        if ( it->second < timeslot)
        {
            cache.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            ++it;
        }
    }
}


bool EdgeHost::has_free_memory_in_cache()
{
    return cache.size() < host_mem_limit;
}


bool EdgeHost::validate_task_expiration(std::string task_name, int timeslot)
{
    if(cache[task_name] < timeslot)
    {
        cache.erase(task_name);
        return false;
    }
    return true;
}

