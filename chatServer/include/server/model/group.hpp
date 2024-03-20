#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>
#include "user.hpp"

using namespace std;

#define GROUP_ADMINI "creator"
#define GROUP_MEMBER "normal"

class GroupUser : public User
{
public:
    GroupUser(int id = -1, string name = "", string password = "", string state = "offline", string role = GROUP_MEMBER) : User(id, name, password, state), _role(role) {}
    void setRole(string role) { this->_role = role; }
    string getRole() { return this->_role; }

private:
    string _role;
};

class Group
{
public:
    Group(int id = -1, string name = "", string desc = "") : _id(id), _name(name), _desc(desc) {}

    void setId(int id) { this->_id = id; }
    void setName(string name) { this->_name = name; }
    void setDesc(string desc) { this->_desc = desc; }

    int getId() { return this->_id; }
    string getName() { return this->_name; }
    string getDesc() { return this->_desc; }

    vector<GroupUser> &getUsers() { return this->_users; }

private:
    int _id;
    string _name;
    string _desc;
    vector<GroupUser> _users;
};

#endif