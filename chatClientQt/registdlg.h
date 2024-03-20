#ifndef REGISTDLG_H
#define REGISTDLG_H

#include <QDialog>
#include <QMouseEvent>
#include <QString>
#include <net/chatClient.hpp>
#include <net/chatService.hpp>
#include "common.h"
#include "string.h"

extern ChatClient client;

namespace Ui {
class RegistDlg;
}

class RegistDlg : public QDialog {
  Q_OBJECT

 public:
  explicit RegistDlg(QWidget *parent = nullptr);
  ~RegistDlg();

 protected:
  // Event handlers
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);

 public:
  QPoint mouseWindowTopLeft;  //鼠标相对窗口左上角的坐标 在mousePressEvent 得到
  QPoint mouseDeskTopLeft;  //鼠标相对于桌面左上角坐标 在mouseMoveEvent实时获取
  QPoint windowDeskTopLeft;  //窗口左上角相对于桌面左上角坐标
                             //在mouseMoveEvent实时计算(矢量)获得

 public:
  UserInfo getUserInfo() { return userInfo; }
  bool getStatus() { return m_status; }

 private slots:
  void registSlot(QString string);
  void on_pushBtn_regist_clicked();
  void on_pushBtn_hide_clicked();
  void on_pushBtn_close_clicked();

 private:
  void Init();

 private:
  Ui::RegistDlg *ui;
  QTcpSocket *socket;
  UserInfo userInfo;
  bool m_status;  //注册成功或者失败
};

#endif  // REGISTDLG_H
