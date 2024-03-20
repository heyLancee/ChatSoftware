#include "chatClient.hpp"
#include "chatService.hpp"

ChatClient::ChatClient(EventLoop *loop, const InetAddress &listenAddr,
                       const string &nameArg)
    : _client(loop, listenAddr, nameArg) {
  // 连接事件回调函数
  _client.setConnectionCallback(std::bind(&ChatClient::onConnection, this, _1));

  // 服务器
  _client.setMessageCallback(
      std::bind(&ChatClient::onMessage, this, _1, _2, _3));
}

// 连接事件回调
void ChatClient::onConnection(const TcpConnectionPtr &conn) {
  if (conn->connected()) {
    cout << conn->localAddress().toIpPort() << "->"
         << conn->peerAddress().toIpPort() << "state: online" << endl;
  } else {
      ChatService::getInstance().serverCloseException(conn);
    cout << conn->localAddress().toIpPort() << "->"
         << conn->peerAddress().toIpPort() << " state: offline" << endl;
    conn->shutdown();
  }
}
// 消息事件回调
void ChatClient::onMessage(const TcpConnectionPtr &conn, Buffer *buff,
                           Timestamp ts) {

  string buffer = buff->retrieveAllAsString();

  json js = json::parse(buffer);
  if (js.is_discarded()) {
    LOG_ERROR << "js parse fail";
    return;
  }

  // 解析出js的key，为每一个key分配一个业务句柄函数，将网络代码和业务代码区分开
  _msgHandler handler = ChatService::getInstance().getMsgHandler(
      enMsgType(js["msgid"].get<int>()));

  // 回调消息绑定的事件处理器，来执行业务处理
  handler(conn, js, ts);
}
