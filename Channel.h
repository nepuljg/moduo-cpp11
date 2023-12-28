#pragma once 
#include<memory>
#include<functional>
#include "Timestamp.h"
#include "noncopyable.h"
#include "Logger.h"
class EventLoop;
 /*
 *Channel 理解为通道，封装了sockfd和其感兴趣的event,如EPOLLIN,EPOLLOUT事件
 */
class Channel : noncopyable
{
public:
   // typedef std::function<void()>EventCallback;
    using ReadEventCallback = std::function<void(Timestamp)>;
    using EventCallback = std::function<void()>;

    Channel(EventLoop * loop, int fd);
    ~Channel();

    //fd得到poller通知以后处理事件的
    void handleEvent(Timestamp receiveTime);
    
    //设置回调函数对象
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb);};
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb);}
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb);}
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb);}

    //防止当Channel被手动remove时 ，Channel还在执行回调函数
    int fd() const { return fd_;}
    int events() const { return events_;}
    void set_revents(int revt) { revents_ = revt;}
    
    //设置fd相应的事件状态
    void enableReading() { events_ |= kReadEvent; update();}
    void disableReading() {events_ &= ~kReadEvent; update();}
    void enableWriting() { events_ |= kWriteEvent;update();}
    void disableWriting() { events_ &= ~kWriteEvent;update();}
    void disableAll() {events_ = kNoneEvent; update();}

    //返回fd当前的事件状态
    bool isNoneEvent() const {return events_ == kNoneEvent;}
    bool isWriting() const { return events_ & kWriteEvent;}
    bool isReading() const { return events_ & kReadEvent;}
    
    int index() { return index_;}
    void set_index(int idx) { index_ = idx;}

    //one loop per thread
    EventLoop * ownerLoop() { return loop_;}
    void remove();
    void tie(const std::shared_ptr<void>&obj);
private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);
    
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; //事件循环
    const int fd_; //Poller监听的对象
    int events_; //注册fd感兴趣的事件
    int revents_; //poller返回的具体发生的事件
    int index_; //

    std::weak_ptr<void> tie_;
    bool tied_;
    bool eventHandling_;
    bool addedToLoop_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};