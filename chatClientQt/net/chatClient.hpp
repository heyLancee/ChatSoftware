#ifndef CHATCLIENT_HPP
#define CHATCLIENT_HPP

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <functional>
#include <iostream>
#include "json.hpp"

using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace placeholders;

using json = nlohmann::json;

class ChatClient {
 public:
  ChatClient(EventLoop *loop, const InetAddress &listenAddr,
             const string &nameArg);

  // 连接事件回调
  void onConnection(const TcpConnectionPtr &conn);
  // 消息事件回调
  void onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp ts);

  void connect() { _client.connect(); }

  TcpConnectionPtr connection() { return _client.connection(); }

 private:
  TcpClient _client;
};

#endif
