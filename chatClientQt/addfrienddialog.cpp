#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"

AddFriendDialog::AddFriendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFriendDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("账号查找");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint); // 最小化按钮
    setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint); // 帮助按钮

    // bind signal slot
    connect(&NetSignal::getInstance(), &NetSignal::signalQueryUserAck, this, &AddFriendDialog::queryUserAckHandler);
    connect(&NetSignal::getInstance(), &NetSignal::signalAddFriendAck, this, &AddFriendDialog::addFriendAckHandler);
}

AddFriendDialog::~AddFriendDialog()
{
    delete ui;
}

void AddFriendDialog::on_pushButton_find_clicked()
{
    ChatLogInfo()<<"on_pushButton_find_clicked in..";
    m_addFriendInfo.m_account = ui->lineEdit_account->text().toInt();

    // send find message to the server
    json loginInfo;
    loginInfo["msgid"] = enMsgType::USER_INFO_QUERY;
    loginInfo["id"] = m_addFriendInfo.m_account;
    // 发送注册消息给服务器
    if (client.connection() == nullptr) {
      return;
    }
    client.connection()->send(loginInfo.dump());
}

void AddFriendDialog::queryUserAckHandler(QString str)
{
    json js = json::parse(str.toStdString());
    if (js["errno"] != 0)
        return;

    ui->lineEdit_friendName->setText(QString::fromStdString(js["name"]));
    ui->lineEdit_friend_account->setText(QString::number(js["id"].get<int>()));
    ui->lineEdit_friendState->setText(QString::fromStdString(js["state"]));
}

void AddFriendDialog::on_pushButton_addFriend_clicked()
{
    m_addFriendInfo.m_account = ui->lineEdit_account->text().toInt();

    // send find message to the server
    json loginInfo;
    loginInfo["msgid"] = enMsgType::ADD_FRIEND_MSG;
    loginInfo["id"] = m_addFriendInfo.m_account;
    // 发送注册消息给服务器
    if (client.connection() == nullptr) {
      return;
    }
    client.connection()->send(loginInfo.dump());
}

void AddFriendDialog::addFriendAckHandler(QString str)
{
    json js = json::parse(str.toStdString());
    if (js["errno"].get<int>() == 0){
        m_addFriendInfo.m_userName = js["name"];
        return accept();
    }
    return reject();
}
