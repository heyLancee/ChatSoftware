#include "userModel.hpp"
#include "db.hpp"
#include <muduo/base/Logging.h>

bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s','%s','%s')",
            user.getName().c_str(), user.getPasswd().c_str(), user.getState().c_str());

    MySql sqlConn;
    if (sqlConn.connect())
    {
        if (sqlConn.update(sql))
        {
            // 返回插入成功的用户的主键id
            user.setId(mysql_insert_id(sqlConn.getConnection()));
            return true;
        }
    }
    LOG_INFO << "添加失败";
    return false;
}

User UserModel::query(int id)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);

    MySql sqlConn;
    if (sqlConn.connect())
    {
        MYSQL_RES *res = sqlConn.query(sql);

        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user(atoi(row[0]), row[1], row[2], row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    LOG_INFO << "查询失败";
    return User();
}

bool UserModel::updateState(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySql sqlConn;
    if (sqlConn.connect())
    {
        if (sqlConn.update(sql))
        {
            return true;
        }
    }
    LOG_INFO << "状态更新失败";
    return false;
}

bool UserModel::resetState()
{
    // 组装sql语句
    char sql[1024] = {"update user set state = 'offline' where state = 'online'"};

    MySql sqlConn;
    if (sqlConn.connect())
    {
        if (sqlConn.update(sql))
        {
            return true;
        }
    }
    LOG_INFO << "状态重置失败";
    return false;
}
