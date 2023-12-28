#pragma once 
namespace  CurrentThread
{
    extern __thread int t_catchTid;
    void cacheTid();

    inline int tid()
    {
        if(__builtin_expect(t_catchTid == 0 , 0))
        {
            cacheTid();
        }
        return t_catchTid;
    }

};