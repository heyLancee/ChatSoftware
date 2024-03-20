#ifndef CHARSERVICE_H
#define CHARSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <string.h>
#include <mutex>
#include <unordered_map>

#include "json.hpp"
#include "netpublic.hpp"

using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace placeholders;

using json = nlohmann::json;

typedef std::function<void(const TcpConnectionPtr &, json &, Timestamp)>
    _msgHandler;

// 聊天服务器业务类-单例模式
class ChatService {
 public:
  static ChatService &getInstance();

  // 业务处理函数
  void loginAck(const TcpConnectionPtr &conn, json &js, Timestamp ts);  // 登录
  void registAck(const TcpConnectionPtr &conn, json &js, Timestamp ts);  // 注册
  void userInfoQueryAck(const TcpConnectionPtr &conn, json &js, Timestamp ts);
  void oneChat(const TcpConnectionPtr &conn, json &js,
               Timestamp ts);  // 小窗聊天
  void addFriendAck(const TcpConnectionPtr &conn, json &js,
                 Timestamp ts);  // 添加好友

  void createGroup(const TcpConnectionPtr &conn, json &js,
                   Timestamp ts);  // 创建群组
  void addGroup(const TcpConnectionPtr &conn, json &js,
                Timestamp ts);  // 加入群组
  void groupChat(const TcpConnectionPtr &conn, json &js,
                 Timestamp ts);  // 群组聊天

  // 服务器异常退出 业务重置操作
  void reset();
  void serverCloseException(const TcpConnectionPtr &conn);

  _msgHandler getMsgHandler(enMsgType type);

 private:
  ChatService();
  ~ChatService();

  // 存储消息id和对应的业务处理方法
  unordered_map<enMsgType, _msgHandler> _msgHandlerMap;
};

#endif
