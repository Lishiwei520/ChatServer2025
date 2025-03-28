#ifndef CHATSERVER_H //防止重复引用
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
    public:
    //初始化聊天服务器对象
    ChatServer(EventLoop* loop,//事件循环
        const InetAddress& listenAddr,//IP+Port
        const string &nameArg);//服务器名字
    //启动服务
    void start();
    private:
        //上报链接相关信息的回调函数
        //专门处理用户的连接创建和断开 epoll listenfd accept
        void onConnection(const TcpConnectionPtr &conn);
        //上报读写事件相关信息的回调函数
        void onMessage(const TcpConnectionPtr& conn,//连接
                        Buffer *buffer,//缓冲区
                        Timestamp time);//接收到数据的时间信息
        TcpServer _server;//组合的muduo库，实现服务器功能的类对象
        EventLoop *_loop;//指向循环事件的指针

};

#endif