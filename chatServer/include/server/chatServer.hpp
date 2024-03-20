#ifndef CHARSERVER_H
#define CHARSERVER_H

#include <functional>
#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/TcpServer.h>
#include <string.h>

using namespace muduo;
using namespace muduo::net;
using namespace std;
using namespace placeholders;

class ChatServer {
public:
  ChatServer(EventLoop *loop, const InetAddress &listenAddr,
             const string &nameArg);

  // 开启事件循环
  void _start();

private:
  void onConnection(const TcpConnectionPtr &conn);

  void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time);

  TcpServer _server;
  EventLoop *_loop;
};

#endif