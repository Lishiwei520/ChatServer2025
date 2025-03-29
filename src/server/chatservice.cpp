#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <iostream>
using namespace muduo;


//注册消息以及对应的Handler回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});
}

//获取单例对象的接口函数
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

//获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    //记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if(it ==_msgHandlerMap.end())//没找到
    {
        //返回一个空参数
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp){
            LOG_ERROR << "msgid:"<<msgid<<"can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

//处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);
    if(user.getId()!=-1&&user.getPwd()==pwd)
    {
        if(user.getState()==("online"))
        {
            //用户已经登录，不允许重复
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errnp"] = 1;
            response["errmsg"] = "该账户已经登录，不能重复登录";
            conn->send(response.dump());
        }
        else{
            //登录成功，记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id,conn});
            }

            //登陆成功，更新用户状态信息 state offline =>online
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errnp"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            conn->send(response.dump());
        }
    }
    else{
        //用户名或密码错误，登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errnp"] = 1;
        response["errmsg"] = "用户名或者密码错误";
        conn->send(response.dump());
    }
}
//处理注册业务 name password
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if(state)
    {
        //注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errnp"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        //注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errnp"] = 1;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
}

//处理客户端异常退出
void ChatService::clientCloseExcepetion(const TcpConnectionPtr &conn)
{
    User user;
    {
        //删除_userConnMap中的conn
        lock_guard<mutex> lock(_connMutex);
        for(auto m:_userConnMap)
        {
            if(m.second == conn)
            {
                //从map表删除用户的连接信息
                user.setId(m.first);
                _userConnMap.erase(m.first);
                break;
            }
        }
    }
    //将状态改为offline
    if(user.getId()!=-1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

//一对一聊天服务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it !=_userConnMap.end())
        {
            //接收方在线，转发消息,服务器做中转操作，主动发送消息给toid
            it->second->send(js.dump());
            return;
        }
        else
        {
            //接收方不在线，存储离线消息
        }
    }
}