#include "widget.h"

#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QGuiApplication>
#include <QMainWindow>
#include <QScreen>
#include <vector>
#include <QMessageBox>

#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget), m_curCheckedUser(0)
{
    ui->setupUi(this);
    InitUI();
    Init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    mouseWindowTopLeft = event->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    // 窗口移动
    if (event->buttons() & Qt::LeftButton)
    {
        mouseDeskTopLeft = event->globalPos();
        windowDeskTopLeft = mouseDeskTopLeft - mouseWindowTopLeft; // 矢量计算
        this->move(windowDeskTopLeft);                             // 移动到目的地
    }
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (m_isfull)
    {
        // 取消全屏
        m_isfull = false;
        ui->centerWidget->setGeometry(m_rect);

        ui->centerWidget->move(QApplication::desktop()->screen()->rect().center() - ui->centerWidget->rect().center());
    }
    else
    {
        m_isfull = true;
        m_rect = ui->centerWidget->rect();
        setGeometry(QGuiApplication::primaryScreen()->availableGeometry()); // 不包含windows任务栏区域
        ui->centerWidget->setGeometry(this->rect());
    }
}

void Widget::on_pushBtn_max_clicked()
{
    //    this->showFullScreen(); //全屏
    if (m_isfull)
    {
        // 取消全屏
        m_isfull = false;
        ui->centerWidget->setGeometry(640, 480, m_rect.width(), m_rect.height());
        ui->centerWidget->move(QApplication::desktop()->screen()->rect().center() - ui->centerWidget->rect().center());
    }
    else
    {
        m_isfull = true;
        m_rect = ui->centerWidget->rect();
        setGeometry(QGuiApplication::primaryScreen()->availableGeometry()); // 不包含windows任务栏区域
        ui->centerWidget->setGeometry(this->rect());
    }
}

void Widget::InitUI()
{
    this->setWindowTitle("WeChat");
    ui->centerWidget->setStyleSheet("QWidget#centerWidget{ border-radius:4px; "
                                    "background:rgba(255,255,255,1); }");

    this->setWindowFlags(Qt::FramelessWindowHint); // 去掉标题栏无边框
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    // 实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    // 设置阴影距离
    shadow->setOffset(0, 0);
    // 设置阴影颜色
    shadow->setColor(QColor(39, 40, 43, 100));
    // 设置阴影圆角
    shadow->setBlurRadius(10);
    // 给嵌套QWidget设置阴影
    ui->centerWidget->setGraphicsEffect(shadow);

    m_isfull = false;
}

void Widget::Init()
{
    connect(ui->userListWidget, SIGNAL(SignalUserGroupButtonClicked(int)), this, SLOT(SlotUserGroupButtonClicked(int)));

    connect(&NetSignal::getInstance(), &NetSignal::signalLogin, this, &Widget::handleMsg);
    connect(&NetSignal::getInstance(), &NetSignal::signalServerLogout, this, &Widget::handleLogout);

    LoginDlg loginDlg; // 这个loginDlog必须是父窗口
    do
    {
        loginDlg.show();
        int status = loginDlg.exec();
        if (status == QDialog::Accepted)
        {
            UserInfo *userInfo = loginDlg.getUserInfo();
            m_userInfo.m_account = userInfo->m_account;
            m_userInfo.m_userName = userInfo->m_userName;
            m_userInfo.m_password = userInfo->m_password;
            this->m_isLogin = true;
        }
        else if (status == QDialog::Rejected)
        {
            this->m_isLogin = false;
            return;
        }
    } while (this->m_isLogin == false);
}

static int stackWidgetIndex = 0;

void Widget::Init_Group_Info(GroupChatInfo *groupInfo)
{
    ui->userListWidget->AddWXWidget(QString::fromStdString(groupInfo->name), ":/src/头像/bianchenxuexijidi.jpg");
    ui->stackedWidget_Msg->setCurrentIndex(stackWidgetIndex++);
    if (stackWidgetIndex >= ui->stackedWidget_Msg->count())
    {
        QWidget *page = new QWidget();
        page->setObjectName(QString("page%1").arg(stackWidgetIndex));
        ui->stackedWidget_Msg->addWidget(page);
        ChatLogInfo() << "stackedWidget_Msg account:" << ui->stackedWidget_Msg->count()
                      << ",stackWidgetIndex:" << stackWidgetIndex;
    }
    QListWidget *listWidget = new QListWidget;
    QGridLayout *mainLayout = new QGridLayout;
    ui->stackedWidget_Msg->currentWidget()->setLayout(mainLayout);
    listWidget->setStyleSheet("QListWidget{"
                              "background-color: rgb(247,247,247);"
                              "border-style: none;"
                              "}");
    mainLayout->addWidget(listWidget);
    mainLayout->setMargin(0);  // 设置外边距
    mainLayout->setSpacing(0); // 设置内边距

    m_chatWigetMap.insert(std::make_pair(groupInfo->m_account, listWidget));
    chatWidgetInfo *chatInfo = new chatWidgetInfo;
    chatInfo->m_account = groupInfo->m_account;
    chatInfo->m_type = TYPE_GROUP_CHAT;

    m_chatWidgetInfoList.push_back(chatInfo);
    m_curCheckedUser = stackWidgetIndex - 1;
}

void Widget::Init_Friend_Info(FriendInfo *friendInfo)
{
    ui->userListWidget->AddWXWidget(QString::fromStdString(friendInfo->name), ":/src/头像/icebear.jpg");

    ui->stackedWidget_Msg->setCurrentIndex(stackWidgetIndex++);
    if (stackWidgetIndex >= ui->stackedWidget_Msg->count())
    {
        QWidget *page = new QWidget();
        page->setObjectName(QString("page%1").arg(stackWidgetIndex));
        ui->stackedWidget_Msg->addWidget(page);
        ChatLogInfo() << "stackedWidget_Msg account:" << ui->stackedWidget_Msg->count()
                      << ",stackWidgetIndex:" << stackWidgetIndex;
    }
    QListWidget *listWidget = new QListWidget;
    QGridLayout *mainLayout = new QGridLayout;
    ui->stackedWidget_Msg->currentWidget()->setLayout(mainLayout);
    listWidget->setStyleSheet("QListWidget{"
                              "background-color: rgb(247,247,247);"
                              "border-style: none;"
                              "border-top:1px solid #D6D6D6;"
                              "}");
    mainLayout->addWidget(listWidget);
    mainLayout->setMargin(0);  // 设置外边距
    mainLayout->setSpacing(0); // 设置内边距

    m_chatWigetMap.insert(std::make_pair(friendInfo->m_account, listWidget));
    chatWidgetInfo *chatInfo = new chatWidgetInfo;
    chatInfo->m_account = friendInfo->m_account;
    chatInfo->m_type = TYPE_PRIVATE_CHAT;

    m_chatWidgetInfoList.push_back(chatInfo);
    m_curCheckedUser = stackWidgetIndex - 1;
}

bool Widget::messShow(json js){
    int senderid = js["senderid"].get<int>();
    EChatType type;
    if (js["type"] == "onechat")
        type= TYPE_PRIVATE_CHAT;
    else
        type = TYPE_GROUP_CHAT;

    QListWidget *chatWidget = NULL;
    mapChatWidget::iterator iter = m_chatWigetMap.find(senderid);
    if (iter == m_chatWigetMap.end())
        return false;

    chatWidget = iter->second;

    QString str = QString::fromStdString(js["msg"]);
    cout << str.toStdString() << endl;

    // 显示在对应聊天框
    QString time = QString::number(QDateTime::currentDateTimeUtc().toTime_t());
    ChatMessage *message = new ChatMessage(chatWidget);
    QListWidgetItem *item = new QListWidgetItem();
    dealMessageTime(chatWidget, time);
    dealMessage(chatWidget, message, item, str, time, QString::fromStdString(""),
                ChatMessage::User_She);
    // 设置消息发送成功
    message->setTextSuccess();
    return true;
}

int Widget::handleLogin(json js)
{
    m_isLogin = 1; // 更新登录状态

    // 解析好友列表
    if (js["friends"] != nullptr)
    {
        vector<string> friendStrVec = js["friends"];
        for (string f : friendStrVec)
        {
            json friendJs = json::parse(f);
            FriendInfo *friInfo = new FriendInfo;
            friInfo->m_account = friendJs["id"];
            friInfo->name += friendJs["name"];
            friInfo->state += friendJs["state"];
            Init_Friend_Info(friInfo);
        }
    }

    // 解析群组列表
    if (js["groups"] != nullptr)
    {
        vector<string> groupStrVec = js["groups"];
        for (string g : groupStrVec)
        {
            json groupJs = json::parse(g);
            GroupChatInfo *groupInfo = new GroupChatInfo;
            groupInfo->m_account = groupJs["id"];
            groupInfo->name += groupJs["name"];
            groupInfo->desc += groupJs["desc"];
            Init_Group_Info(groupInfo);
        }
    }

    // 解析离线消息
    if (js["offlineMess"] != nullptr)
    {
        vector<string> messVec = js["offlineMess"];
        for (string mess : messVec)
        {
            json messJs = json::parse(mess);
            if (js.is_discarded())
            {
                cout << "js parse fail" << endl;;
                continue;
            }

            QString str = QString::fromStdString(messJs["msg"]);
            if (str.toStdString().size() > MAX_SEND_LENGTH)
            {
                ChatLogInfo() << "超出最大发送大小..";
                continue;
            }

            messShow(messJs);
        }
    }
    return 0;
}

void Widget::handleLogout()
{
    exit(0);
}

void Widget::dealMessage(QListWidget *listWidget, ChatMessage *messageW, QListWidgetItem *item, QString text,
                         QString time, QString name,
                         ChatMessage::User_Type type) // 用户发送文本
{
    listWidget->addItem(item);
    messageW->setFixedWidth(listWidget->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, name, type);
    listWidget->setItemWidget(item, messageW);
    return;
}
void Widget::dealMessageTime(QListWidget *listWidget,
                             QString curMsgTime) // 处理时间
{
    bool isShowTime = false;
    cout << listWidget->count() << endl;
    if (listWidget->count() > 0)
    {
        QListWidgetItem *lastItem = listWidget->item(listWidget->count() - 1);
        ChatMessage *messageW = (ChatMessage *)listWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
                                                  //        isShowTime = true;
    }
    else
    {
        isShowTime = true;
    }
    if (isShowTime)
    {
        QListWidgetItem *itemTime = new QListWidgetItem();
        listWidget->addItem(itemTime);
        QSize size = QSize(listWidget->width(), 40);
        itemTime->setSizeHint(size);

        ChatMessage *messageTime = new ChatMessage(listWidget);
        messageTime->resize(size);
        messageTime->setText(curMsgTime, curMsgTime, size);
        listWidget->setItemWidget(itemTime, messageTime);
    }
    return;
}

void Widget::on_pushBtn_send_clicked()
{
    if (ui->textEdit->toPlainText().isEmpty())
        return;

    if (this->m_curCheckedUser < 0)
    {
        ChatLogInfo() << "未选择聊天窗口";
        return;
    }

    chatWidgetInfo *chatInfo = m_chatWidgetInfoList[this->m_curCheckedUser];

    QListWidget *chatWidget = NULL;
    mapChatWidget::iterator iter = m_chatWigetMap.find(chatInfo->m_account);
    if (iter != m_chatWigetMap.end())
    {
        chatWidget = iter->second;
    }
    else
    {
        ChatLogInfo() << "聊天窗口未找到";
        return;
    }

    QString str = ui->textEdit->toPlainText();
    if (str.toStdString().size() > MAX_SEND_LENGTH)
    {
        ChatLogInfo() << "超出最大发送大小..";
        return;
    }
    ui->textEdit->clear();

    // 用户自己发送的消息
    QString time = QString::number(QDateTime::currentDateTimeUtc().toTime_t());
    ChatMessage *message = new ChatMessage(chatWidget);
    QListWidgetItem *item = new QListWidgetItem();
    dealMessageTime(chatWidget, time);
    dealMessage(chatWidget, message, item, str, time, QString::fromStdString(m_userInfo.m_userName),
                ChatMessage::User_Me);
    // 设置消息发送成功
    message->setTextSuccess();

    // 发送消息
     if (chatInfo->m_type == TYPE_GROUP_CHAT)
     {
         json loginInfo;
         loginInfo["msgid"] = enMsgType::GROUP_CHAT_MSG;
         loginInfo["groupID"] = chatInfo->m_account;
         loginInfo["msg"] = str.toStdString();
         // 发送注册消息给服务器
         if (client.connection() == nullptr) {
           QMessageBox::information(NULL, "登录", "连接服务器失败",
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes);
           return;
         }
         client.connection()->send(loginInfo.dump());
     }
     else if (chatInfo->m_type == TYPE_PRIVATE_CHAT)
     {
         json loginInfo;
         loginInfo["msgid"] = enMsgType::ONE_CHAT_MSG;
         loginInfo["to"] = chatInfo->m_account;
         loginInfo["msg"] = str.toStdString();
         // 发送注册消息给服务器
         if (client.connection() == nullptr) {
           QMessageBox::information(NULL, "登录", "连接服务器失败",
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes);
           return;
         }
         client.connection()->send(loginInfo.dump());
     }
}

void Widget::SlotUserGroupButtonClicked(int index)
{
    m_curCheckedUser = index;
    qDebug() << "checked index:" << index;
    ui->stackedWidget_Msg->setCurrentIndex(m_curCheckedUser);
    ChatLogInfo() << "current Row chicked.." << m_curCheckedUser;
}

int Widget::handleMsg(QString string)
{
    json js = json::parse(string.toStdString());
    int msgid = js["msgid"];
    if (msgid < 0)
        return -1;

    // 根据msgid执行不同业务
    switch (msgid)
    {
    case LOGIN_ACK: // 登录事件，解析显示离线消息和好友群组
        this->handleLogin(js);
        break;
    case GROUP_CHAT_MSG:
         messShow(js);
        break;
    case ONE_CHAT_MSG: // 小窗聊天消息
        messShow(js);
        break;
    }
    return 0;
}

void Widget::on_pushButton_addFriend_clicked()
{
    AddFriendDialog addFriendDlg;

    addFriendDlg.show();
    if (addFriendDlg.exec() == QDialog::Accepted)
    {
        UserInfo addFriendInfo = addFriendDlg.getAddFriendInfo();

        // add addFriendInfoReq.m_account to the chatList
        FriendInfo *friInfo = new FriendInfo;
        friInfo->m_account = addFriendInfo.m_account;
        friInfo->name += addFriendInfo.m_userName;
        Init_Friend_Info(friInfo);
    }
}

void Widget::on_pushBtn_close_clicked()
{
    this->close();
}

void Widget::on_pushBtn_hide_clicked()
{
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
        pWindow->showMinimized();
}
