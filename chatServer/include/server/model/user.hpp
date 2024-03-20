#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

// User表的ORM类
class User
{
public:
    User(int id = -1, string name = "", string password = "", string state = "offline") : _id(id), _name(name), _password(password), _state(state){};

    void setId(int id) { this->_id = id; }
    void setName(string name) { this->_name = name; }
    void setPasswd(string pwd) { this->_password = pwd; }
    void setState(string state) { this->_state = state; }

    int getId() { return this->_id; }
    string getName() { return this->_name; }
    string getPasswd() { return this->_password; }
    string getState() { return this->_state; }

protected:
    int _id;
    string _name;
    string _password;
    string _state;
};

#endif