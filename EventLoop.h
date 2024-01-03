#pragma once 
#include "noncopyable.h"
#include<functional>
#include<mutex>
#include<atomic>
#include<memory>
#include"CurrentThread.h"
#include "Timestamp.h"
//时间循环类 抓哟包含两个大模块，一个是channel Poller(epoll的抽象P)
class Channel;
class Poller;
class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();
    //开启事件循环
    void loop();
    //退出事件循环
    void quit();
    
    Timestamp pollReturnTime() const { return pollReturnTime_;}
    //在当前loop中执行
    void runInLoop(Functor cb);
    //把cb放入到队列中
    void queueInLoop(Functor cb);
    void wakeup();
    
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void hasChannel(Channel* channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid();}
    
private:
    void handleRead();
    void doPendingFunctors();
    using ChannelList = std::vector<Channel*>;
    
    std::atomic_bool looping_; //原子操作的bool值
    std::atomic_bool quit_; //推出loop循环
    std::atomic_bool callingPendingFunctors_; ///表示当前执行的回调函数
    const pid_t threadId_;
     //记录当前loop当前线程Id
    Timestamp pollReturnTime_;//poller返回发生事件
    std::unique_ptr<Poller>poller_;

    int wakeupfd_;//
    std::unique_ptr<Channel>wakeupChannel_;
    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    std::vector<Functor>pending_;
    std::mutex mutex_;  
};