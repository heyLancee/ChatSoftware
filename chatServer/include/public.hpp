#ifndef PUBLIC_H
#define PUBLIC_H

enum enMsgType {
  LOGIN,           // 登陆消息
  LOGIN_ACK,       //
  REGIST,          // 注册消息
  REGIST_ACK,      //
  ONE_CHAT_MSG,    // 小窗聊天消息
  USER_INFO_QUERY,
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
