#include "CurrentThread.h"
#include<unistd.h>
#include<sys/syscall.h>

namespace CurrentThread
{
    __thread int t_catchTid = 0;
    void cacheTid()
    {
        if(t_catchTid == 0)
        {
            //通过系统调用获取当前的线程tid值
            t_catchTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }
}