#ifndef CHARSERVICE_H
#define CHARSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <string.h>

#include <mutex>

#include "json.hpp"
#include "public.hpp"
#include "unordered_map"
#include "userModel.hpp"
#include "offlineMessModel.hpp"
#include "friendModel.hpp"
#include "groupModel.hpp"

using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace placeholders;

using json = nlohmann::json;

typedef std::function<void(const TcpConnectionPtr &, json &, Timestamp)>
    _msgHandler;

// 聊天服务器业务类-单例模式
class ChatService
{
public:
  static ChatService &getInstance();

  // 业务处理函数
  void login(const TcpConnectionPtr &conn, json &js, Timestamp ts);     // 登录
  void regist(const TcpConnectionPtr &conn, json &js, Timestamp ts);    // 注册
  void queryUser(const TcpConnectionPtr &conn, json &js, Timestamp ts); // 查询用户信息
  void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp ts);   // 小窗聊天
  void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp ts); // 添加好友

  void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp ts); // 创建群组
  void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp ts);    // 加入群组
  void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp ts);   // 群组聊天

  // 服务器异常退出 业务重置操作
  void reset();
  void clientCloseException(const TcpConnectionPtr &conn);

  _msgHandler getMsgHandler(enMsgType type);

private:
  ChatService();
  ~ChatService();

  // 存储消息id和对应的业务处理方法
  unordered_map<enMsgType, _msgHandler> _msgHandlerMap;

  // 记录连接服务器的用户的TCP连接对象
  unordered_map<int, TcpConnectionPtr> _userConnMap;

  // 互斥量，保证_userConnMap的线程安全
  mutex _connMutex;

  // 数据库user表操作类对象
  UserModel _userModel;

  // 数据库offlinemessage表操作类对象
  OfflineMessModel _offlineMessModel;

  // 数据库friend表操作类对象
  FriendModel _friendModel;

  // 数据库allgroup和groupuser表操作类对象
  GroupModel _groupModel;
};

#endif