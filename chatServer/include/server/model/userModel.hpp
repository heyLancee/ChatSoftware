#ifndef USERMODEL_H
#define USERMODEL_H

#include <string>
#include "user.hpp"

using namespace std;

class UserModel
{
public:
    bool insert(User &user);      // 向user表中插入用户
    User query(int id);           // 查询id对应的用户信息
    bool updateState(User &user); // 更新用户状态
    bool resetState();            // 重置所有用户状态为offline
};

#endif