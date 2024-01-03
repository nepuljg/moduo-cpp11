#include "Channel.h"
#include "EventLoop.h"
#include "Poller.h"
#include "EPollPoller.h"
#include "Logger.h"

#include<unistd.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<sys/eventfd.h>
//防止一个线程创建多个EventLoop thread_local

__thread EventLoop * t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

//创建wakefd , 用来通知subRector通知新来的
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0)
    {
        LOG_FATAL("eventfd()  is failed.");        
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , callingPendingFunctors_(false)
    , threadId_(CurrentThread::tid())
    , poller_(Poller::newDefaultPoller(this))
    , wakeupfd_(createEventfd())
    , wakeupChannel_(new Channel(this , wakeupfd_))
    , currentActiveChannel_(nullptr)
{
    LOG_DEBUG("EventLoop created %p" in thread %d \n, this, threadId_ );
    if(t_loopInThisThread)
    {
        LOG_FATAL("Anothre Eventloop %p exists in  this thread %d\n", t_loopInThisThread, threadId_);
    }
    else 
    {
        t_loopInThisThread = this;
    }
    //设置wankeupfd时间类型以及发生事件后的回调操作
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    //每一个eventloop都将监听wwakeupchannel
    wakeupChannel_->enableReading();
    
}
EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();

}
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakeupfd_, &one , sizeof one );
    
}

void EventLoop::loop()
{
     looping_ = true;
     quit_ = false;

     LOG_INFO("EventLoop %p start looping \n", this);
     
     while(!quit_)
     {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel * channel : activeChannels_)
        {
            //poller监听哪些channel发生事件了,然后上报给Eventloop f
            channel->handleEvent(pollReturnTime_);
        }
        //执行当前Eventloop事件循环需要处理的回调操作
        doPendingFunctors();
     }
     LOG_INFO("EventLoop %p stop looping \n", this);
     looping_ = false;
}
//退出事件循环
void EventLoop::quit()
{
    quit_ = true;

    if(!isInLoopThread()) // 如果在其他线程中，调用quit_, 
    {
        wakeup();
    }
}
//在当前loop执行cb
void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread())
    {
        cb();
    }
    else 
    {
        queueInLoop(cb);
    }
}
//将cb放入队列中，唤醒loop所在的线程
void EventLoop::queueInLoop(Functor cb)
{
     {
        std::unique_lock<std::mutex>lock(mutex_);
        pending_.emplace_back(cb);
     }
     //唤醒相应的，需要执行上面回调操作的loop的线程
     if (!isInLoopThread() || callingPendingFunctors_)
     {
        wakeup(); //唤醒loop所在线程
     }
}