#include "friendmodel.hpp"
#include "db.hpp"

//添加好友关系
void FriendModel::insert(int userid,int friendid)
{
     //1.组装sql语句
     char sql[1024]={0};
     sprintf(sql,"insert into friend values('%d','%d')", userid,friendid);
 
     MySQL mysql;
     if(mysql.connect()){
         mysql.update(sql);
     }
 
}

//返回用户好友列表
vector<User> FriendModel::query(int userid)
{
    //1.组装sql语句
    //select u.id,u.name u.state from user u inner join friend f on u.id= f.friendid where f.userid=%d;
    char sql[1024]={0};
    sprintf(sql,"select u.id,u.name,u.state from user u inner join friend f on u.id= f.friendid where f.userid=%d;", userid);

    vector<User> vec;
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res=mysql.query(sql);
        if(res !=nullptr)
        {
            //资源拿到行
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            //把userid用户的所有离线消息放入vec中返回
            
        }
    }
    return vec;
}