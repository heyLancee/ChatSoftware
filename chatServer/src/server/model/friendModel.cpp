#include "friendModel.hpp"
#include "db.hpp"
#include <muduo/base/Logging.h>

bool FriendModel::insert(int userId, int friendId)
{
    // 组装sql语句
    char sql1[1024] = {0};
    sprintf(sql1, "select exists(select id from user where id = %d)", friendId);
    char sql2[1024] = {0};
    sprintf(sql2, "insert into friend(userid, friendid) values(%d,%d), (%d,%d)", 
            userId, friendId, friendId, userId);

    MySql sqlConn;
    if (sqlConn.connect())
    {
        MYSQL_RES *res = sqlConn.query(sql1);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row == nullptr || strncasecmp(row[0], "0", 1) == 0)
            {
                LOG_INFO << "添加的好友并未注册";
                return false;
            }
        }
        mysql_free_result(res);
	
	    LOG_INFO << sql2;
	
        if (sqlConn.update(sql2))
        {
            return true;
        }

        LOG_INFO << "请勿重复添加";
        return false;
    }
    LOG_INFO << "添加失败";
    return false;
}

vector<User> FriendModel::query(int userId)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select id, name, state from user inner join friend on friend.friendid = user.id where friend.userid = %d", userId);

    vector<User> friends;
    MySql sqlConn;
    if (sqlConn.connect())
    {
        MYSQL_RES *res = sqlConn.query(sql);

        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                friends.push_back(user);
            }
            mysql_free_result(res);
            return friends;
        }
    }
    LOG_INFO << "查询失败";
    return friends;
}
