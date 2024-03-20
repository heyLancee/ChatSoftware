#ifndef PUBLIC_H
#define PUBLIC_H

#include <QObject>
#include <string>

using namespace std;

class NetSignal : public QObject {
  Q_OBJECT
 public:
  static NetSignal& getInstance();

 signals:
  void signalRegist(QString string);  // 注册用户 公共信号
  void signalLogin(QString string);   // 用户登录 公共信号

  void signalQueryUserAck(QString string);
  void signalAddFriendAck(QString string);

  void signalServerLogout();

 private:
  NetSignal();
  ~NetSignal();

  //禁止拷贝构造
  NetSignal(const NetSignal&) = delete;
  NetSignal(const NetSignal&&) = delete;
  //禁止重载=号
  NetSignal& operator=(const NetSignal&) = delete;
};

extern NetSignal netSignal;

enum enMsgType {
  LOGIN,           // 登陆消息
  LOGIN_ACK,       //
  REGIST,          // 注册消息
  REGIST_ACK,      //
  ONE_CHAT_MSG,    // 小窗聊天消息
  USER_INFO_QUERY, // 查询用户信息
  USER_INFO_QUERY_ACK,
  ADD_FRIEND_MSG,  // 添加好友
  ADD_FRIEND_MSG_ACK,

  CREATE_GROUP_MSG,  // 创建群组
  CREATE_GROUP_MSG_ACK,
  ADD_GROUP_MSG,  // 加入群组
  ADD_GROUP_MSG_ACK,
  GROUP_CHAT_MSG  // 群组聊天消息
};

#endif
