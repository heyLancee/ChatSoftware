#include "groupModel.hpp"
#include "db.hpp"
#include <muduo/base/Logging.h>

bool GroupModel::insert(Group &group)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup (groupname, groupdesc) values('%s','%s')",
            group.getName().c_str(), group.getDesc().c_str());

    MySql sqlConn;
    if (sqlConn.connect())
    {
        if (sqlConn.update(sql))
        {
            // 返回插入成功的用户的主键id
            group.setId(mysql_insert_id(sqlConn.getConnection()));
            return true;
        }
    }
    LOG_INFO << "创建失败";
    return false;
}

bool GroupModel::addGroup(int userid, int groupid, string role)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser(groupid, userid, grouprole) values(%d, %d, '%s')",
            groupid, userid, role.c_str());

    MySql sqlConn;
    if (sqlConn.connect())
    {
        if (sqlConn.update(sql))
        {
            return true;
        }
    }
    LOG_INFO << "加入群组失败";
    return false;
}

vector<Group> GroupModel::query(int userid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select id, groupname, groupdesc from allgroup inner join \
            groupuser on allgroup.id = groupuser.groupid where groupuser.userid = %d",
            userid);

    vector<Group> groups;
    MySql sqlConn;
    if (sqlConn.connect())
    {
        MYSQL_RES *res = sqlConn.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group(atoi(row[0]), row[1], row[2]);
                groups.push_back(group);
            }
            mysql_free_result(res);
            return groups;
        }
    }
    LOG_INFO << "查询失败";
    return groups;
}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select groupuser.userid from groupuser where groupuser.userid!=%d and groupuser.groupid=%d",
            userid, groupid);

    vector<int> groupUsers;
    MySql sqlConn;
    if (sqlConn.connect())
    {
        MYSQL_RES *res = sqlConn.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                groupUsers.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
            return groupUsers;
        }
    }
    LOG_INFO << "查询失败";
    return groupUsers;
}