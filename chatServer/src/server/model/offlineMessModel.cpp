#include "offlineMessModel.hpp"
#include <muduo/base/Logging.h>
#include "db.hpp"

// 向offlinemessage数据库插入离线待发送数据
bool OfflineMessModel::insert(int id, string message, int senderid, string type)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage(userid, message, senderid, type) values(%d,'%s',%d,'%s')", 
            id, message.c_str(), senderid, type.c_str());

    MySql sqlConn;
    if (sqlConn.connect())
    {
        if (sqlConn.update(sql))
        {
            // 返回插入成功的用户的主键id
            return true;
        }
        return false;
    }
    LOG_INFO << "添加失败";
    return false;
}

// 从offlinemessage数据库中删除离线待发送数据
bool OfflineMessModel::remove(int id)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", id);

    MySql sqlConn;
    if (sqlConn.connect())
    {
        if (sqlConn.update(sql))
        {
            // 返回插入成功的用户的主键id
            return true;
        }
        return false;
    }
    LOG_INFO << "删除失败";
    return false;
}

// 从offlinemessage数据库中查询离线待发送数据
vector<string> OfflineMessModel::query(int id)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message, senderid, type from offlinemessage where userid = %d", id);

    MySql sqlConn;
    vector<string> vec;
    if (sqlConn.connect())
    {
        MYSQL_RES *res = sqlConn.query(sql);

        if (res != nullptr)
        {
            MYSQL_ROW row;

            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                json js;
                js["msg"] = row[0];
                js["senderid"] = atoi(row[1]);
                js["type"] = row[2];
                vec.push_back(js.dump());
            }
            mysql_free_result(res);
        }
        return vec;
    }
    LOG_INFO << "查询失败";
    return vec;
}
