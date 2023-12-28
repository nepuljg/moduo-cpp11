#pragma once 

#include "Poller.h"
#include<vector>
#include<sys/epoll.h>

/*
* epoll 的使用
* epoll_create
* epoll_ctl  add/mod/del
* epoll_wait
**/
class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller() override;
    
    Timestamp poll(int timeoutMs);
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;
private:
    static const int kInitEventListSize = 16;
    using EventList = std::vector<epoll_event>;
    
    //填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    //更新channel通道
    void update(int operation , Channel *channel) ;
    int epollfd_;
    EventList events_;
};