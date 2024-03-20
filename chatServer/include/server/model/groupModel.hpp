#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <string>
#include <vector>
#include "group.hpp"

class GroupModel
{
public:
    bool insert(Group &group);                            // 创建群组
    bool addGroup(int userid, int groupid, string role);  // 加入群组
    vector<Group> query(int userid);                      // 查询用户所在群组
    vector<int> queryGroupUsers(int userid, int groupid); // 查询所在群组id内的其余成员的id
};

#endif