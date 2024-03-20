#ifndef WIDGET_H
#define WIDGET_H

#include "addfrienddialog.h"
#include "common.h"
#include "custom/chatmessage.h"
#include "logindlg.h"
#include "net/netpublic.hpp"
#include <QListWidget>
#include <QWidget>
#include <map>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Widget;
}
QT_END_NAMESPACE

typedef enum
{
    TYPE_GROUP_CHAT,
    TYPE_PRIVATE_CHAT
} EChatType;

struct chatWidgetInfo
{
    int m_account;    // 聊天窗口对应的聊天号(私聊时为好友id，群聊时为群组id)
    EChatType m_type; // 聊天窗口对应的类型 群聊/私聊
};

typedef std::map<int, QListWidget *> mapChatWidget; // account->
typedef std::vector<chatWidgetInfo *> vectorChatWidgetInfo;

class Widget : public QWidget
{
    Q_OBJECT

  public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

  protected:
    // Event handlers
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event); // 双击全屏操作

  public:
    QPoint mouseWindowTopLeft; // 鼠标相对窗口左上角的坐标 在mousePressEvent 得到
    QPoint mouseDeskTopLeft;   // 鼠标相对于桌面左上角坐标 在mouseMoveEvent实时获取
    QPoint windowDeskTopLeft;  // 窗口左上角相对于桌面左上角坐标
                               // 在mouseMoveEvent实时计算(矢量)获得

  private slots:
    void on_pushBtn_send_clicked();             // 发送(s)按钮单击
    void SlotUserGroupButtonClicked(int index); // 这个是组按钮按下后响应槽函数

  private slots:
    int handleMsg(QString string);
    void on_pushButton_addFriend_clicked();
    void on_pushBtn_close_clicked();
    void on_pushBtn_hide_clicked();
    void on_pushBtn_max_clicked();

  private:
    void Init();
    void InitUI();
    void Init_Group_Info(GroupChatInfo *groupInfo);
    void Init_Friend_Info(FriendInfo *friendInfo);

    bool messShow(json js);

  public:
    int getLoginStatus()
    {
        return m_isLogin;
    }
#if 0
    GroupUserInfo* findUserInfo(int account);
#endif
  private:
    int handleLogin(json js);
    void handleLogout(); //  server quit

  private:
    void dealMessage(QListWidget *listWidget, ChatMessage *messageW, QListWidgetItem *item, QString text, QString time,
                     QString name, ChatMessage::User_Type type); // 用户发送文本
    void dealMessageTime(QListWidget *listWidget,
                         QString curMsgTime); // 处理时间
  private:
    Ui::Widget *ui;

    UserInfo m_userInfo;

    bool m_isLogin; // 是否登录
    bool m_isfull;  // 显示状态是否全屏

    QRect m_rect;

    mapChatWidget m_chatWigetMap;              // 聊天号->聊天框Map
    vectorChatWidgetInfo m_chatWidgetInfoList; // 存储聊天号和对应的类型
    int m_curCheckedUser;
};
#endif // WIDGET_H
