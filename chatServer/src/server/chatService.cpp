#include "chatService.hpp"
#include "muduo/base/Logging.h"
#include <iostream>
using namespace std;

ChatService::ChatService()
{
    // 注册消息以及对应的Handler操作
    _msgHandlerMap.emplace(make_pair(
        enMsgType::LOGIN, std::bind(&ChatService::login, this, _1, _2, _3)));
    _msgHandlerMap.emplace(make_pair(
        enMsgType::REGIST, std::bind(&ChatService::regist, this, _1, _2, _3)));
    _msgHandlerMap.emplace(make_pair(
        enMsgType::ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)));
    _msgHandlerMap.emplace(make_pair(
        enMsgType::USER_INFO_QUERY, std::bind(&ChatService::queryUser, this, _1, _2, _3)));
    _msgHandlerMap.emplace(make_pair(
        enMsgType::ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)));
    _msgHandlerMap.emplace(make_pair(
        enMsgType::CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)));
    _msgHandlerMap.emplace(make_pair(
        enMsgType::ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)));
    _msgHandlerMap.emplace(make_pair(
        enMsgType::GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)));
}

ChatService::~ChatService() {}

ChatService &ChatService::getInstance()
{
    // 局部静态变量版本的单例模式，保证了线程安全，是不同实现方法中的最优选择
    static ChatService instance;
    return instance;
}

// 登录业务处理函数 ORM
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{
    // 抽取id和pwd
    int id = js["id"];
    string passwd = js["password"];
    json response;

    User user = _userModel.query(id);
    if (user.getId() != -1 && user.getPasswd() == passwd)
    {
        if (user.getState() == "online")
        {
            // 用户已经登陆
            response["msgid"] = LOGIN_ACK;
            response["errno"] = 1;
            response["errmsg"] = "该账号已经登录";
        }
        else
        {
            {
                // 线程安全，退出当前大括号后自动调用_connMutex的释放（类的析构中）
                lock_guard<mutex> lock(_connMutex);
                // 登录成功，保存连接信息
                _userConnMap.emplace(user.getId(), conn);
            }

            // 登录成功,更新用户状态
            LOG_INFO << "log success, user id: " << user.getId();
            user.setState("online");
            _userModel.updateState(user);

            // 查询该用户是否有离线消息
            vector<string> messVec = _offlineMessModel.query(user.getId());
            if (!messVec.empty())
            {
                response["offlineMess"] = messVec;
                // _offlineMessModel.remove(user.getId());

                vector<string> temp = messVec;
                for (string each : temp){
                    json js = json::parse(each);
                }
            }

            // 查询用户好友信息并返回
            vector<User> friendVec = _friendModel.query(user.getId());
            if (!friendVec.empty())
            {
                vector<string> friendStrVec;
                for (User &user : friendVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    friendStrVec.push_back(js.dump());
                }
                response["friends"] = friendStrVec;
            }

            // 查询用户群组信息并返回
            vector<Group> groupVec = _groupModel.query(user.getId());
            if (!groupVec.empty())
            {
              vector<string> groupStrVec;
              for (Group &group : groupVec)
              {
                json js;
                js["id"] = group.getId();
                js["name"] = group.getName();
                js["desc"] = group.getDesc();
                groupStrVec.push_back(js.dump());
              }
              response["groups"] = groupStrVec;
            }

            response["msgid"] = LOGIN_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
        }
    }
    else if (user.getId() == -1)
    {
        response["msgid"] = LOGIN_ACK;
        response["errno"] = 3;
        response["errmsg"] = "服务器连接失败";
    }else{
        // 用户不存在或密码错误
        response["msgid"] = LOGIN_ACK;
        response["errno"] = 2;
        response["errmsg"] = "用户名或密码错误";
    }
    conn->send(response.dump());
}

void ChatService::regist(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{
    // 从json数据里里抽取所需字段
    User user;
    user.setName(js["name"]);
    user.setPasswd(js["password"]);

    json response;
    if (_userModel.insert(user))
    {
        // 注册成功
        LOG_INFO << "user table insert success, id: " << user.getId();

        response["msgid"] = REGIST_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
    }
    else
    {
        response["msgid"] = REGIST_ACK;
        response["errno"] = -1;
    }
    conn->send(response.dump());
}

_msgHandler ChatService::getMsgHandler(enMsgType type)
{
    auto iter = _msgHandlerMap.find(type);
    if (iter == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &, json &, Timestamp)
        {
            LOG_ERROR << "enMsgType: " << type << " can not find handler";
        };
    }
    return _msgHandlerMap[type];
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                // 从_userConnMap中删除conn
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 更新用户状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{
    // 从_userConnMap查找请求创建群组的用户
    int userId = -1;
    for (auto userConn : _userConnMap)
    {
        if (userConn.second == conn)
        {
            userId = userConn.first;
        }
    }

    if (userId == -1)
    {
        LOG_INFO << "请先登录";
        return;
    }
        
    int toid = js["to"].get<int>();
    js["senderid"] = userId;
    js["type"] = "onechat";

    {
        // 可以优化成向数据库查询toid状态
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid用户在线 服务器转发消息
            it->second->send(js.dump());
            return;
        }
    }

    // 用户离线，存储消息
    _offlineMessModel.insert(toid, js["msg"], js["senderid"], js["type"]);
}

void ChatService::reset()
{
    _userModel.resetState();
}

void ChatService::queryUser(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{      
    int id = js["id"].get<int>();

    User user = _userModel.query(id);

    json response;
    if (user.getId() == -1)
    {
        response["msgid"] = USER_INFO_QUERY_ACK;
        response["errno"] = -1;
        conn->send(response.dump());
        return;
    }
    response["msgid"] = USER_INFO_QUERY_ACK;
    response["errno"] = 0;
    response["name"] = user.getName();
    response["id"] = user.getId();
    response["state"] = user.getState();

    conn->send(response.dump());
    return;
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{
    // 从_userConnMap查找发送好友请求方的id
    int userId = -1;
    int friendId = js["id"].get<int>();
    for (auto userConn : _userConnMap)
    {
        if (userConn.second == conn)
        {
            userId = userConn.first;
        }
    }

    json response;
    
    if (userId == -1)
    {
        LOG_INFO << "请先登录";
        response["msgid"] = ADD_FRIEND_MSG_ACK;
        response["errno"] = -1;
        conn->send(response.dump());
        return;
    }

    // 查询用户信息
    User user = _userModel.query(friendId);

    if (user.getId() == -1)
    {
	    response["msgid"] = ADD_FRIEND_MSG_ACK;
        response["errno"] = -1;
        conn->send(response.dump());
        return;
    }
    
    // 添加到friend表
    if (_friendModel.insert(userId, friendId))
    {	
    	LOG_INFO << "添加好友id= " << friendId << " 成功";
	    response["msgid"] = ADD_FRIEND_MSG_ACK;
    	response["errno"] = 0;
    	response["name"] = user.getName();
    	conn->send(response.dump());
    }else
    {
	    response["msgid"] = ADD_FRIEND_MSG_ACK;
        response["errno"] = -1;
	    conn->send(response.dump());
    }

}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{
    // 从_userConnMap查找请求创建群组的用户
    int userId = -1;
    for (auto userConn : _userConnMap)
    {
        if (userConn.second == conn)
        {
            userId = userConn.first;
        }
    }

    if (userId == -1)
    {
        LOG_INFO << "请先登录";
        return;
    }

    Group group(-1, js["name"], js["desc"]);
    if (_groupModel.insert(group))
    {
        _groupModel.addGroup(userId, group.getId(), GROUP_ADMINI);
    }
}

void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{
    int userId = -1;
    for (auto userConn : _userConnMap)
    {
        if (userConn.second == conn)
        {
            userId = userConn.first;
        }
    }

    if (userId == -1)
    {
        LOG_INFO << "请先登录";
        return;
    }

    _groupModel.addGroup(userId, js["groupid"], GROUP_MEMBER);

    json response;
    response["msgid"] = ADD_GROUP_MSG_ACK;
    response["errno"] = 0;
    conn->send(response.dump());
}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp ts)
{
    // 从_userConnMap查找发送好友请求方的id
    int userId = -1;
    int groupId = js["groupID"].get<int>();
    for (auto userConn : _userConnMap)
    {
        if (userConn.second == conn)
        {
            userId = userConn.first;
        }
    }

    if (userId == -1)
    {
        LOG_INFO << "请先登录";
        return;
    }

    js["senderid"] = groupId;
    js["type"] = "groupchat";

    // 查询组内其余成员
    vector<int> groupUsersId = _groupModel.queryGroupUsers(userId, groupId);
    for (int id : groupUsersId)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 在线
            it->second->send(js.dump());
        }
        else
        {
            // 离线
            _offlineMessModel.insert(id, js["msg"], js["senderid"], js["type"]);
        }
    }
}
