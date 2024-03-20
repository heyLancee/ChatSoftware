#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include "common.h"
#include "widget.h"
#include <QDialog>

namespace Ui {
class AddFriendDialog;
}

class AddFriendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFriendDialog(QWidget *parent = nullptr);
    ~AddFriendDialog();
public:
    UserInfo getAddFriendInfo(){
        return m_addFriendInfo;
    }
private slots:
    void on_pushButton_find_clicked();
    void on_pushButton_addFriend_clicked();
    void queryUserAckHandler(QString str);
    void addFriendAckHandler(QString str);
signals:
    void signal_findUserInfo(int account);
private:
    Ui::AddFriendDialog *ui;
    UserInfo m_addFriendInfo;
};

#endif // ADDFRIENDDIALOG_H
