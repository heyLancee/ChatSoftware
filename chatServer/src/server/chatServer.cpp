#include "chatServer.hpp"
#include "chatService.hpp"
#include "iostream"
#include "json.hpp"
#include "muduo/base/Logging.h"

using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
  _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));

  // 给服务器注册用户读写事件回调
  _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

  // 设置服务器端的线程数量: 一个IO线程（只处理新用户连接），三个工作线程
  _server.setThreadNum(4);
}

void ChatServer::_start() { _server.start(); }

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
  if (conn->connected())
  {
    cout << conn->peerAddress().toIpPort() << "->"
         << conn->localAddress().toIpPort() << " state: online" << endl;
  }
  else
  {
    ChatService::getInstance().clientCloseException(conn);
    cout << conn->peerAddress().toIpPort() << "->"
         << conn->localAddress().toIpPort() << " state: offline" << endl;
    conn->shutdown();
    // _loop->quit();
  }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf,
                           Timestamp time)
{
  string buffer = buf->retrieveAllAsString();
	
  json js = json::parse(buffer, nullptr, false);
  if (js.is_discarded())
  {
    LOG_ERROR << "js parse fail";
    return;
  }
  // 解析出js的key，为每一个key分配一个业务句柄函数，将网络代码和业务代码区分开
  _msgHandler handler = ChatService::getInstance().getMsgHandler(
      enMsgType(js["msgid"].get<int>()));
  // 回调消息绑定的事件处理器，来执行业务处理
  handler(conn, js, time);
}
