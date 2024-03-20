#include "logindlg.h"
#include "ui_logindlg.h"

#include <QBitmap>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QPainter>

LoginDlg::LoginDlg(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDlg) {
  ui->setupUi(this);
  Init();
}

LoginDlg::~LoginDlg() { delete ui; }

void LoginDlg::Init() {
  this->setWindowTitle("登录");
  setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);  // 最小化按钮
  setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint);  // 帮助按钮

#if 0
    QWidget *container;
    container=new QWidget(this);
    container->setGeometry(5,5,width,height);
    container->setStyleSheet("QWidget{border-radius:4px;background:rgba(255,255,255,1);}");

    this->setWindowFlags(Qt::FramelessWindowHint);      //去掉标题栏无边框
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor(39,40,43,100));
    //设置阴影圆角
    shadow->setBlurRadius(10);
    //给嵌套QWidget设置阴影
    container->setGraphicsEffect(shadow);
#else
  int width = this->width() - 10;
  int height = this->height() - 10;
  ui->centerWidget->setGeometry(5, 5, width, height);
  ui->centerWidget->setStyleSheet(
      "QWidget{border-radius:4px;background:rgba(255,255,255,1);}");  //设置圆角

  this->setWindowFlags(Qt::FramelessWindowHint);  //去掉标题栏无边框
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  //实例阴影shadow
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
  //设置阴影距离
  shadow->setOffset(0, 0);
  //设置阴影颜色
  shadow->setColor(QColor(39, 40, 43, 100));
  //设置阴影圆角
  shadow->setBlurRadius(10);
  //给嵌套QWidget设置阴影
  ui->centerWidget->setGraphicsEffect(shadow);
#endif

  // 设置信号槽绑定函数
  connect(&NetSignal::getInstance(), &NetSignal::signalLogin, this,
          &LoginDlg::loginSlot);
}

void LoginDlg::mousePressEvent(QMouseEvent *event) {
  mouseWindowTopLeft = event->pos();
}

void LoginDlg::mouseMoveEvent(QMouseEvent *event) {
  //窗口移动
  if (event->buttons() & Qt::LeftButton) {
    mouseDeskTopLeft = event->globalPos();
    windowDeskTopLeft = mouseDeskTopLeft - mouseWindowTopLeft;  //矢量计算
    this->move(windowDeskTopLeft);  //移动到目的地
  }
}

void LoginDlg::loginSlot(QString string) {
  json js = js.parse(string.toStdString());

  QString mess = "登录";
  if (js["errno"] != 0) {
    mess.append("失败, 错误码: ");
    mess.append(QString::number(js["errno"].get<int>()));
    mess.append("(");
    mess.append(QString::fromStdString(js["errmsg"]));
    mess.append(")");
    QMessageBox::information(
        NULL, "登录", mess, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    return reject();
  } else {
    m_userInfo.m_userName = js["name"];
    mess.append("成功, 用户名: ");
    mess.append(QString::fromStdString((js["name"])));
    QMessageBox::information(
        NULL, "登录", mess, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    return accept();
  }
}

void LoginDlg::on_pushbtn_regist_clicked() {
  RegistDlg registDlg;
  registDlg.show();
  if (registDlg.exec() == QDialog::Accepted) {
    if (!registDlg.getStatus()) return;
    UserInfo info = registDlg.getUserInfo();
    ui->lineEdit_account->setText(QString::number(info.m_account));
    m_userInfo.m_account = info.m_account;
  }
}

void LoginDlg::on_pushButton_login_clicked() {
  m_userInfo.m_account = ui->lineEdit_account->text().toInt();
  m_userInfo.m_password = ui->lineEdit_password->text().toStdString();

  ChatLogInfo() << "get account:" << m_userInfo.m_account;
  ChatLogInfo() << "get password:" << m_userInfo.m_password.c_str();

  json loginInfo;
  loginInfo["msgid"] = 0;
  loginInfo["id"] = m_userInfo.m_account;
  loginInfo["password"] = m_userInfo.m_password;
  // 发送注册消息给服务器
  if (client.connection() == nullptr) {
    QMessageBox::information(NULL, "登录", "连接服务器失败",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
    return;
  }
  client.connection()->send(loginInfo.dump());
}

void LoginDlg::on_pushBtn_hide_clicked() {
  QWidget *pWindow = this->window();
  if (pWindow->isTopLevel()) pWindow->showMinimized();
}

void LoginDlg::on_pushBtn_close_clicked() { this->close(); }
