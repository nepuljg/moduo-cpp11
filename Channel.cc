#include "Channel.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop , int fd)
    : loop_(loop)
    , fd_(fd)
    , events_(0)
    , revents_(0)
    , index_(-1)
    , tied_(false)
    , eventHandling_(false)
    , addedToLoop_(false)
{

}

Channel::~Channel()
{

}

//Channel的tie方法什么时候调用过
void Channel::tie(const std::shared_ptr<void>&obj)
{
    tie_ = obj;
    tied_ = true;
}

//当改变channel所表示fd的events事件后，update负责在poller里面更改fd相应的事件epoll_ctl
void Channel::update()
{
     
}
//在channel所属的EventLoop中，把当前的channel删除掉
void Channel::remove()
{
     
}
void Channel::handleEvent(Timestamp receiveTime)
{
    if(tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else 
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    
    LOG_FATAL("channel handleEvent revents:%d\n" , revents_);
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if(closeCallback_)
        {
            closeCallback_();
        }
    }
    
    if(revents_ & EPOLLERR )
    {
        if(errorCallback_)
        {
            errorCallback_();
        }
    }
    if(revents_ &  EPOLLIN)
    {
        if(readCallback_)
        {
            readCallback_(receiveTime);
        }
    }
    if(revents_ & EPOLLOUT)
    {
        if(writeCallback_)
        {
            writeCallback_();
        }
    }
    
}