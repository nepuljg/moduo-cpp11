#pragma once 
#include "noncopyable.h"
#include "Timestamp.h"
#include<vector>
#include<unordered_map>
class EventLoop;
class Channel;
//moduo 库中多路事件分发器的核心IO复用模块
class Poller 
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller();

    //给所有的IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs , ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    //判断参数channel是否在当前poller当中
    bool hasChannel(Channel *channel) const;
    //EventLoop可以通过接口获取默认的IO复用的具体实现
    static Poller* newDefaultPoller(EventLoop *loop);
protected:
    // key ： sockfd   value: channel
    using ChannelMap = std::unordered_map<int , Channel*>;
    ChannelMap channels_;
private:
    EventLoop * ownerLoop_;
};