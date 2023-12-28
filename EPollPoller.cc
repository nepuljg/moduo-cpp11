#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller :: EPollPoller(EventLoop *loop)
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{
    if(epollfd_ < 0)
    {
        LOG_FATAL("epoll_create error:%d\n", errno);
    }
}
EPollPoller::~EPollPoller()
{
    ::close(epollfd_);   
}
// channel update remove => EventLoop 
void updateChannel(Channel *channel)
{
     const int index = channel->index();
     LOG_INFO("fd=%d events=%d index=%d \n",channel->fd() , channel->events(), index);
     
     if (index == kNew || index == kDeleted)
     {
        if (index == kNew)
        {
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
     }
     else //channel已经在poller上注册过了
     {
        int fd = channel->fd();
        if(channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else 
        {
            update(EPOLL_CTL_MOD, channel);
        }
     }
}
void removeChannel(Channel *channel)
{
     
}
void EPollPoller::update(int operation, Channel* channel)
{
     epoll_event event;
     memset(&event , 0 , sizeof event);
     int fd = channel->fd();

     event.events = channel->events();
     event.data.fd = fd;
     event.data.ptr = channel;
     

     if(::epoll_ctl(epollfd_ , operation , fd , &event) < 0)
     {
        if(operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        }
        else 
        {
            LOG_FATAL("epoll_ctl add/mod/del:%d\n ", errno);
        }

     }
     
} 