#include "chatService.hpp"
#include "muduo/base/Logging.h"
#include <iostream>

ChatService::ChatService() {
  // 注册消息以及对应的Handler操作
  _msgHandlerMap.emplace(make_pair(
      enMsgType::LOGIN_ACK, std::bind(&ChatService::loginAck, this, _1, _2, _3)));
  _msgHandlerMap.emplace(
      make_pair(enMsgType::REGIST_ACK,
                std::bind(&ChatService::registAck, this, _1, _2, _3)));
  _msgHandlerMap.emplace(
      make_pair(enMsgType::USER_INFO_QUERY_ACK,
                std::bind(&ChatService::userInfoQueryAck, this, _1, _2, _3)));
   _msgHandlerMap.emplace(
       make_pair(enMsgType::ONE_CHAT_MSG,
                 std::bind(&ChatService::oneChat, this, _1, _2, _3)));
   _msgHandlerMap.emplace(
       make_pair(enMsgType::ADD_FRIEND_MSG_ACK,
                 std::bind(&ChatService::addFriendAck, this, _1, _2, _3)));
   _msgHandlerMap.emplace(
       make_pair(enMsgType::CREATE_GROUP_MSG,
                 std::bind(&ChatService::createGroup, this, _1, _2, _3)));
   _msgHandlerMap.emplace(
       make_pair(enMsgType::ADD_GROUP_MSG,
                 std::bind(&ChatService::addGroup, this, _1, _2, _3)));
   _msgHandlerMap.emplace(
       make_pair(enMsgType::GROUP_CHAT_MSG,
                 std::bind(&ChatService::groupChat, this, _1, _2, _3)));
}

ChatService::~ChatService() {}

ChatService &ChatService::getInstance() {
  // 局部静态变量版本的单例模式，保证了线程安全，是不同实现方法中的最优选择
  static ChatService instance;
  return instance;
}

_msgHandler ChatService::getMsgHandler(enMsgType type) {
  auto iter = _msgHandlerMap.find(type);
  if (iter == _msgHandlerMap.end()) {
    return [=](const TcpConnectionPtr &, json &, Timestamp) {
      LOG_ERROR << "enMsgType: " << type << " can not find handler";
    };
  }
  return _msgHandlerMap[type];
}

// 登录业务处理函数 ORM
void ChatService::loginAck(const TcpConnectionPtr &conn, json &js,
                           Timestamp ts) {
  emit NetSignal::getInstance().signalLogin(QString::fromStdString(js.dump()));
}

void ChatService::registAck(const TcpConnectionPtr &conn, json &js,
                            Timestamp ts) {
    emit NetSignal::getInstance().signalRegist(QString::fromStdString(js.dump()));
}

void ChatService::serverCloseException(const TcpConnectionPtr &conn) {
    emit NetSignal::getInstance().signalServerLogout();
}

void ChatService::userInfoQueryAck(const TcpConnectionPtr &conn, json &js,
                          Timestamp ts) {
    emit NetSignal::getInstance().signalQueryUserAck(QString::fromStdString(js.dump()));
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js,
                          Timestamp ts) {}

void ChatService::addFriendAck(const TcpConnectionPtr &conn, json &js,
                            Timestamp ts) {
    emit NetSignal::getInstance().signalAddFriendAck(QString::fromStdString(js.dump()));
}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js,
                              Timestamp ts) {}

void ChatService::addGroup(const TcpConnectionPtr &conn, json &js,
                           Timestamp ts) {}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js,
                            Timestamp ts) {}
