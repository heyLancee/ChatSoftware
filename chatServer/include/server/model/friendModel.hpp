#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "userModel.hpp"
#include "vector"

using namespace std;

class FriendModel
{
public:
    bool insert(int userId, int friendId); // 添加好友
    vector<User> query(int userId);        // 返回好友用户列表
};

#endif